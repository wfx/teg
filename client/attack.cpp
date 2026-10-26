/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <rquesada@core-sdi.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; only version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */
/*
 * manages the client side of the attack
 */

#include <filesystem>
#include "attack.h"

#include "protocol.h"
#include "net.h"
#include "fcintl.h"
#include "client.h"

namespace teg::client
{

/* are the necesary conditions met for the attack ? */
static TEG_STATUS attack_check(ClientState& state)
{
	PLAYER_STATUS e;

	e = client_status(state);

	if(e==PLAYER_STATUS_ATAQUE || e==PLAYER_STATUS_TROPAS) {
		set_status(state, PLAYER_STATUS_ATAQUE);
		return TEG_STATUS_SUCCESS;
	} else {
		return TEG_STATUS_ERROR;
	}
}

/* reset the status of the attack */
void attack_reset(ClientState& state)
{
	if(state.attack.country_origen != -1) {
		state.game.common.countries[state.attack.country_origen].selected &= ~COUNTRY_SELECT_ATTACK;
		callbacks::gui_country_select(state, state.attack.country_origen);
		state.attack.country_origen = -1;
	}

	if(state.attack.country_destino != -1) {
		state.game.common.countries[state.attack.country_destino].selected &= ~COUNTRY_SELECT_ATTACK;
		callbacks::gui_country_select(state, state.attack.country_destino);
		state.attack.country_destino = -1;
	}
}

void attack_restore(ClientState& state)
{
	state.attack.country_origen = state.attack.country_origen_bak;
	state.attack.country_destino = state.attack.country_destino_bak;
}

void attack_backup(ClientState& state)
{
	state.attack.country_origen_bak = state.attack.country_origen;
	state.attack.country_destino_bak = state.attack.country_destino;
}

TEG_STATUS attack_init(ClientState& state)
{
	if(attack_check(state) != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_UNEXPECTED;
	}

	attack_backup(state); /// \todo This looks wrong here.
	attack_reset(state);

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS attack_click(ClientState& state, PCOUNTRY p)
{
	if(attack_check(state) != TEG_STATUS_SUCCESS) {
		textmsg(state, M_ERR, _("Error, It's not the time to attack"));
		return TEG_STATUS_UNEXPECTED;
	}

	if(state.attack.country_origen == -1) {
		if(p->numjug == who_am_i(state)) {
			if(p->ejercitos >1) {
				p->selected &= ~COUNTRY_SELECT_ATTACK_ENTER;
				p->selected |= COUNTRY_SELECT_ATTACK;
				callbacks::gui_country_select(state, p->id);
				state.attack.country_origen = p->id;
				textmsg(state, M_INF, _("Source country: '%s'. Now select the destination country"), countries_get_name(state.game.common, p->id));
			} else {
				textmsg(state, M_ERR, _("Error, '%s' must have at least 2 armies"), countries_get_name(state.game.common, p->id));
				return TEG_STATUS_UNEXPECTED;
			}
		} else {
			textmsg(state, M_ERR, _("Error, '%s' isnt one of your countries"), countries_get_name(state.game.common, p->id));
			return TEG_STATUS_UNEXPECTED;
		}
	} else if(state.attack.country_destino == -1) {
		if(state.attack.country_origen == p->id) {
			textmsg(state, M_INF, _("Source country is the same as the destination. Resetting the attack..."));
			attack_reset(state);
			return TEG_STATUS_SUCCESS;
		}

		if(p->numjug != who_am_i(state)) {
			if(countries_eslimitrofe(state.attack.country_origen, p->id)) {
				p->selected &= ~COUNTRY_SELECT_ATTACK_ENTER;
				p->selected |= COUNTRY_SELECT_ATTACK;
				callbacks::gui_country_select(state, p->id);
				state.attack.country_destino = p->id;
				textmsg(state, M_INF, _("Destination country: '%s'. Attacking..."), countries_get_name(state.game.common, p->id));
				attack_out(state);
			} else {
				textmsg(state, M_ERR, _("Error, '%s' isnt frontier with '%s'"), countries_get_name(state.game.common, p->id), countries_get_name(state.game.common, state.attack.country_origen));
				attack_reset(state);
				return TEG_STATUS_UNEXPECTED;
			}
		} else {
			textmsg(state, M_ERR, _("Error, you cant attack your own countries ('%s')"), countries_get_name(state.game.common, p->id));
			attack_reset(state);
			return TEG_STATUS_UNEXPECTED;
		}
	} else {
		attack_reset(state);
		textmsg(state, M_ERR, _("Error, unexpected error in attack_click(). Report this bug!"));
		return TEG_STATUS_UNEXPECTED;
	}

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS attack_finish(ClientState& state, int *ori, int *dst)
{
	if(attack_check(state) != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_UNEXPECTED;
	}

	if(state.attack.country_destino == -1 || state.attack.country_origen == -1) {
		return TEG_STATUS_ERROR;
	}

	*ori = state.attack.country_origen;
	*dst = state.attack.country_destino;

	return TEG_STATUS_SUCCESS;
}


TEG_STATUS attack_pre_reset(ClientState& state)
{
	if(attack_check(state) != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_ERROR;
	} else {
		attack_reset(state);
		return TEG_STATUS_SUCCESS;
	}
}


/* Sends to server the attack message */
TEG_STATUS attack_out(ClientState& state)
{
	int src;
	int dst;

	if(attack_finish(state, &src, &dst) != TEG_STATUS_SUCCESS)  {
		textmsg(state, M_ERR, _("Error, make sure to select the countries first."));
		attack_init(state);
		return TEG_STATUS_ERROR;
	}

	net_printf(state.game.fd, TOKEN_ATAQUE"=%d,%d\n", src, dst);
	attack_backup(state);
	return TEG_STATUS_SUCCESS;
}

/* Sends to server the attack message */
TEG_STATUS attackre_out(ClientState& state)
{
	attack_restore(state);
	return attack_out(state);
}

/* the mouse is over a country */
TEG_STATUS attack_enter(ClientState& state, PCOUNTRY p)
{
	if(attack_check(state) != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_UNEXPECTED;
	}

	if(state.attack.country_origen == -1) {
		if(p->numjug == who_am_i(state)) {
			if(p->ejercitos >1) {
				if(!(p->selected & COUNTRY_SELECT_ATTACK_ENTER)) {
					p->selected |= COUNTRY_SELECT_ATTACK_ENTER;
					callbacks::gui_country_select(state, p->id);
				}
			}
		}
	} else if(state.attack.country_destino == -1) {
		if(p->numjug != who_am_i(state)) {
			if(countries_eslimitrofe(state.attack.country_origen, p->id)) {
				if(!(p->selected & COUNTRY_SELECT_ATTACK_ENTER)) {
					p->selected |= COUNTRY_SELECT_ATTACK_ENTER;
					callbacks::gui_country_select(state, p->id);
				}
			}
		}
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS attack_leave(ClientState& state, PCOUNTRY p)
{
	if(attack_check(state) != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_UNEXPECTED;
	}
	if(p->selected & COUNTRY_SELECT_ATTACK_ENTER) {
		p->selected &= ~COUNTRY_SELECT_ATTACK_ENTER;
		callbacks::gui_country_select(state, p->id);
	}
	return TEG_STATUS_SUCCESS;
}

/* unshow the attack shown with attack_show() */
void attack_unshow(ClientState& state)
{

	if(state.attack.show_src != -1) {
		state.game.common.countries[state.attack.show_src].selected &= ~COUNTRY_SELECT_ATTACK_SRC;
		callbacks::gui_country_select(state, state.attack.show_src);
		state.attack.show_src = -1;
	}

	if(state.attack.show_dst != -1) {
		state.game.common.countries[state.attack.show_dst].selected &= ~COUNTRY_SELECT_ATTACK_DST;
		callbacks::gui_country_select(state, state.attack.show_dst);
		state.attack.show_dst = -1;
	}
}

/* shows attack an attack from srt to dst */
void attack_show(ClientState& state, int src, int dst)
{
	attack_unshow(state);

	if(src >= 0 && src < static_cast<int>(COUNTRIES_CANT)) {
		state.attack.show_src = src;
		state.game.common.countries[src].selected |= COUNTRY_SELECT_ATTACK_SRC;
		callbacks::gui_country_select(state, src);
	}

	if(dst >= 0 && dst < static_cast<int>(COUNTRIES_CANT)) {
		state.attack.show_dst = dst;
		state.game.common.countries[dst].selected |= COUNTRY_SELECT_ATTACK_DST;
		callbacks::gui_country_select(state, dst);
	}
}

}
