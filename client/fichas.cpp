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

#include <filesystem>
#include <stdio.h>
#include <string.h>

#include "../common/net.h"
#include "fcintl.h"
#include "protocol.h"
#include "client.h"

namespace teg::client
{

/**
 * @fn static TEG_STATUS fichas_check()
 * Dice si se esta en condiciones de hacer algo con fichas
 */
TEG_STATUS fichas_check(ClientState& state)
{
	PLAYER_STATUS e;

	e = client_status(state);

	if(e==PLAYER_STATUS_FICHAS || e==PLAYER_STATUS_FICHAS2 || e==PLAYER_STATUS_FICHASC) {
		return TEG_STATUS_SUCCESS;
	} else {
		return TEG_STATUS_ERROR;
	}
}

TEG_STATUS fichas_add(ClientState& state, PCOUNTRY p)
{
	if(fichas_check(state) != TEG_STATUS_SUCCESS) {
		textmsg(state, M_ERR, _("Error, you cant add armies now"));
		return TEG_STATUS_UNEXPECTED;
	}

	if(p->numjug == who_am_i(state)) {
		if(state.fichas.total < state.fichas.wanted) {
			state.fichas.total++;
			p->ejercitos++;
			state.fichas.countries[p->id]++;
			state.fichas.continents[p->continente]++;
			return TEG_STATUS_SUCCESS;
		} else {
			textmsg(state, M_ERR, _("Error, you cant put more than %d armies"), state.fichas.wanted);
			fichas_enter(state, p);
			return TEG_STATUS_ERROR;
		}
	} else {
		textmsg(state, M_ERR, _("Error, '%s' isnt one of your countries"), countries_get_name(state.game.common, p->id));
		return TEG_STATUS_UNEXPECTED;
	}
}

TEG_STATUS fichas_sub(ClientState& state, PCOUNTRY p)
{
	if(fichas_check(state) != TEG_STATUS_SUCCESS) {
		textmsg(state, M_ERR, _("Error, you cant sub armies now"));
		return TEG_STATUS_UNEXPECTED;
	}

	if(p->numjug == who_am_i(state)) {
		if(state.fichas.countries[p->id]) {
			state.fichas.total--;
			p->ejercitos--;
			state.fichas.countries[p->id]--;
			state.fichas.continents[p->continente]--;
			fichas_enter(state, p);
			return TEG_STATUS_SUCCESS;
		} else {
			return TEG_STATUS_UNEXPECTED;
		}
	} else {
		textmsg(state, M_ERR, _("Error, '%s' isnt one of your countries"), countries_get_name(state.game.common, p->id));
		return TEG_STATUS_UNEXPECTED;
	}
}

TEG_STATUS fichas_finish(ClientState& state, int **ptr)
{
	int i, c;
	if(fichas_check(state) != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_UNEXPECTED;
	}

	if(state.fichas.wanted != state.fichas.total) {
		return TEG_STATUS_ERROR;
	}

	c = state.fichas.wanted_continents;
	for(i=0; i<CONT_CANT; i++) {
		if((c & 1) && (state.fichas.continents[i] < state.game.common.conts[i].fichas_x_cont)) {
			return TEG_STATUS_ERROR;
		}
		c >>= 1;
	}

	*ptr = state.fichas.countries.data();
	return TEG_STATUS_SUCCESS;
}

void fichas_init(ClientState& state, int cant, int conts)
{
	int i;
	for(i=0; i<static_cast<int>(COUNTRIES_CANT); i++) {
		state.fichas.countries[i]=0;
	}
	for(i=0; i<CONT_CANT; i++) {
		state.fichas.continents[i]=0;
	}
	state.fichas.total = 0;
	state.fichas.wanted = cant;
	state.fichas.wanted_continents = conts;
}

void fichas_add_wanted(ClientState& state, int i)
{
	state.fichas.wanted += i ;
}

void fichas_reset(ClientState& state)
{
	int i;

	for(i=0; i<static_cast<int>(COUNTRIES_CANT); i++) {
		if(state.fichas.countries[i]) {
			state.game.common.countries[i].ejercitos -= state.fichas.countries[i];
			state.fichas.countries[i] = 0;
		}
	}
	for(i=0; i<CONT_CANT; i++) {
		state.fichas.continents[i] = 0;
	}

	state.fichas.total = 0;
}

TEG_STATUS fichas_restore_from_error(ClientState& state)
{
	set_status(state, PLAYER_STATUS_FICHAS);
	fichas_reset(state);
	aux_draw_all_countries(state);
	callbacks::gui_sensi(state);
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS fichas2_restore_from_error(ClientState& state)
{
	set_status(state, PLAYER_STATUS_FICHAS2);
	fichas_reset(state);
	aux_draw_all_countries(state);
	callbacks::gui_sensi(state);
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS fichasc_restore_from_error(ClientState& state)
{
	set_status(state, PLAYER_STATUS_FICHASC);
	fichas_reset(state);
	aux_draw_all_countries(state);
	callbacks::gui_sensi(state);
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS fichas_out(ClientState& state)
{
	char buf[2000];
	char tmp[100];
	int *ptr;
	int i;
	int first_time;
	PLAYER_STATUS e;

	if(fichas_check(state) != TEG_STATUS_SUCCESS) {
		textmsg(state, M_ERR, _("Error, its not the time to send armies"));
		return TEG_STATUS_ERROR;
	}

	if(fichas_finish(state, &ptr) != TEG_STATUS_SUCCESS)  {
		fichas_reset(state);
		textmsg(state, M_ERR, _("Error, put the correct number of armies"));
		return TEG_STATUS_ERROR;
	}

	buf[0]=0;
	tmp[0]=0;

	first_time = 1;

	for(i=0; i<static_cast<int>(COUNTRIES_CANT); i++) {
		if(ptr[i]>0) {
			if(first_time) {
				sprintf(tmp, "%d:%d", i, ptr[i]);
			} else {
				sprintf(tmp, ",%d:%d", i, ptr[i]);
			}
			strcat(buf, tmp);
			first_time = 0;
		}
	}

	e = client_status(state);
	switch(e) {
	case PLAYER_STATUS_FICHAS:
		state.connection->printf(TOKEN_FICHAS"=%s\n", buf);
		set_status(state, PLAYER_STATUS_POSTFICHAS);
		break;
	case PLAYER_STATUS_FICHAS2:
		state.connection->printf(TOKEN_FICHAS2"=%s\n", buf);
		set_status(state, PLAYER_STATUS_POSTFICHAS2);
		break;
	case PLAYER_STATUS_FICHASC:
		state.connection->printf(TOKEN_FICHASC"=%s\n", buf);
		set_status(state, PLAYER_STATUS_POSTFICHASC);
		break;
	default:
		textmsg(state, M_ERR, _("Error, its not the moment to send your armies"));
		return TEG_STATUS_ERROR;
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS fichas_enter(ClientState& state, PCOUNTRY p)
{
	if(fichas_check(state) != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_ERROR;
	}

	if(p->numjug == who_am_i(state)) {
		if(state.fichas.total >= state.fichas.wanted)  {
			if(!(p->selected & COUNTRY_SELECT_FICHAS_OUT)) {
				p->selected &= ~COUNTRY_SELECT_FICHAS_IN;
				p->selected |= COUNTRY_SELECT_FICHAS_OUT;
				callbacks::gui_country_select(state, p->id);
			}
		} else {
			if(!(p->selected & COUNTRY_SELECT_FICHAS_IN)) {
				p->selected &= ~COUNTRY_SELECT_FICHAS_OUT;
				p->selected |= COUNTRY_SELECT_FICHAS_IN;
				callbacks::gui_country_select(state, p->id);
			}
		}
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS fichas_leave(ClientState& state, PCOUNTRY p)
{
	if(fichas_check(state) != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_ERROR;
	}

	if(p->numjug == who_am_i(state)) {
		p->selected &= ~COUNTRY_SELECT_FICHAS_IN;
		p->selected &= ~COUNTRY_SELECT_FICHAS_OUT;
		callbacks::gui_country_select(state, p->id);
	}
	return TEG_STATUS_SUCCESS;
}

void fichas_get_wanted(ClientState& state, int *cant, int *conts)
{
	*cant = state.fichas.wanted - cont_tot(state.game.common, state.fichas.wanted_continents);
	*conts = state.fichas.wanted_continents;
}

}
