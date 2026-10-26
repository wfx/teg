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
/**
 * @file reagrue.c
 * Contiene algunas funciones auxiliares para el manejo del estado 'ESTADO_REAGRUPE'
 */

#include <filesystem>
#include "../common/net.h"

#include "fcintl.h"
#include "client.h"
#include "protocol.h"

namespace teg::client
{

TEG_STATUS reagrupe_check(ClientState& state)
{
	PLAYER_STATUS e = client_status(state);

	if(e>=PLAYER_STATUS_ATAQUE && e<=PLAYER_STATUS_REAGRUPE) {
		set_status(state, PLAYER_STATUS_REAGRUPE);
		return TEG_STATUS_SUCCESS;
	} else {
		return TEG_STATUS_ERROR;
	}
}

TEG_STATUS reagrupe_click(ClientState& state, PCOUNTRY p)
{
	if(reagrupe_check(state) != TEG_STATUS_SUCCESS) {
		textmsg(state, M_ERR, _("Error, It's not the time to regroup"));
		return TEG_STATUS_UNEXPECTED;
	}

	if(state.reagrupe.country_origen == -1) {
		if(p->numjug == who_am_i(state)) {
			if(p->ejercitos - p->ejer_reagrupe > 1) {
				p->selected &= ~COUNTRY_SELECT_REGROUP_ENTER;
				p->selected |= COUNTRY_SELECT_REGROUP;
				callbacks::gui_country_select(state, p->id);
				state.reagrupe.country_origen = p->id;
				textmsg(state, M_INF, _("Source country: '%s'. Now select the destination country"), countries_get_name(state.game.common, p->id));
			} else {
				textmsg(state, M_ERR, _("Error, '%s' doesnt have any avalaible armies to move"), countries_get_name(state.game.common, p->id));
				return TEG_STATUS_UNEXPECTED;
			}
		} else {
			textmsg(state, M_ERR, _("Error, '%s' isnt one of your countries"), countries_get_name(state.game.common, p->id));
			return TEG_STATUS_UNEXPECTED;
		}
	} else if(state.reagrupe.country_destino == -1) {
		if(state.reagrupe.country_origen == p->id) {
			textmsg(state, M_INF, _("Source country is the same as the destination. Resetting the regroup..."));
			reagrupe_reset(state);
			return TEG_STATUS_SUCCESS;
		}

		if(p->numjug == who_am_i(state)) {
			if(countries_eslimitrofe(state.reagrupe.country_origen, p->id)) {
				p->selected &= ~COUNTRY_SELECT_REGROUP_ENTER;
				p->selected |= COUNTRY_SELECT_REGROUP;
				callbacks::gui_country_select(state, p->id);
				state.reagrupe.country_destino = p->id;
				textmsg(state, M_INF, _("Destination country: '%s'. Now select the quantity of armies to move"), countries_get_name(state.game.common, p->id));
				callbacks::gui_reagrupe(state, state.reagrupe.country_origen, state.reagrupe.country_destino, state.game.common.countries[state.reagrupe.country_origen].ejercitos - state.game.common.countries[state.reagrupe.country_origen].ejer_reagrupe - 1);
			} else {
				textmsg(state, M_ERR, _("Error, '%s' isnt frontier with '%s'"), countries_get_name(state.game.common, p->id), countries_get_name(state.game.common, state.reagrupe.country_origen));
				return TEG_STATUS_UNEXPECTED;
			}
		} else {
			textmsg(state, M_ERR, _("Error, '%s' isnt one of your countries"), countries_get_name(state.game.common, p->id));
			reagrupe_reset(state);
			return TEG_STATUS_UNEXPECTED;
		}
	} else {
		textmsg(state, M_ERR, _("Error, unexpected error in reagrupe_click(). Report this bug!"));
		reagrupe_reset(state);
		return TEG_STATUS_UNEXPECTED;
	}

	return TEG_STATUS_SUCCESS;
}

void reagrupe_reset(ClientState& state)
{
	if(state.reagrupe.country_origen != -1) {
		state.game.common.countries[state.reagrupe.country_origen].selected &= ~COUNTRY_SELECT_REGROUP;
		callbacks::gui_country_select(state, state.reagrupe.country_origen);
		state.reagrupe.country_origen = -1;
	}

	if(state.reagrupe.country_destino != -1) {
		state.game.common.countries[state.reagrupe.country_destino].selected &= ~COUNTRY_SELECT_REGROUP;
		callbacks::gui_country_select(state, state.reagrupe.country_origen);
		state.reagrupe.country_destino = -1;
	}
}

void reagrupe_bigreset(ClientState& state)
{
	int i;

	reagrupe_reset(state);

	for(i=0; i<static_cast<int>(COUNTRIES_CANT); i++) {
		state.game.common.countries[i].ejer_reagrupe = 0;
	}
	state.reagrupe.last_origen = -1;
	state.reagrupe.last_destino = -1;
	state.reagrupe.last_cant = 0;
}

TEG_STATUS reagrupe_init(ClientState& state)
{
	attack_reset(state);

	if(reagrupe_check(state) != TEG_STATUS_SUCCESS) {
		textmsg(state, M_ERR, _("Error, you can't regroup your armies now"));
		return TEG_STATUS_ERROR;
	}
	reagrupe_reset(state);
	return TEG_STATUS_SUCCESS;
}

void reagrupe_set_and_save(ClientState& state, int src, int dst, int cant)
{
	state.reagrupe.last_origen = src;
	state.reagrupe.last_destino = dst;
	state.reagrupe.last_cant = cant;
	state.game.common.countries[ dst ].ejer_reagrupe += cant;
}

TEG_STATUS reagrupe_restore_from_error(ClientState& state)
{
	if(state.reagrupe.last_origen != -1 && state.reagrupe.last_destino != -1 && state.reagrupe.last_cant >= 0) {
		state.game.common.countries[ state.reagrupe.last_destino ].ejer_reagrupe -= state.reagrupe.last_cant;
		return TEG_STATUS_SUCCESS;
	} else {
		return TEG_STATUS_ERROR;
	}

}

TEG_STATUS reagrupe_out(ClientState& state, int src, int dst, int cant)
{
	PLAYER_STATUS e;

	e = client_status(state);
	if(e==PLAYER_STATUS_REAGRUPE) {
		reagrupe_reset(state);
		reagrupe_set_and_save(state, src, dst, cant);
		state.connection->printf(TOKEN_REAGRUPE"=%d,%d,%d\n", src, dst, cant);
	} else {
		textmsg(state, M_ERR, _("Error, you cant regroup now."));
		return TEG_STATUS_ERROR;
	}

	return TEG_STATUS_SUCCESS;
}

/**
 * @fn TEG_STATUS reagrupe_enter( PCOUNTRY p )
 * Cuando se esta reagrupando resalta los countries que se pueden reagrupar
 * @param p Pais a resaltar
 */
TEG_STATUS reagrupe_enter(ClientState& state, PCOUNTRY p)
{
	if(reagrupe_check(state) != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_UNEXPECTED;
	}

	if(state.reagrupe.country_origen == -1) {
		if(p->numjug == who_am_i(state)) {
			if(p->ejercitos - p->ejer_reagrupe > 1) {
				p->selected |= COUNTRY_SELECT_REGROUP_ENTER;
				callbacks::gui_country_select(state, p->id);
			}
		}
	} else if(state.reagrupe.country_destino == -1) {
		if(p->numjug == who_am_i(state)) {
			if(countries_eslimitrofe(state.reagrupe.country_origen, p->id)) {
				p->selected |= COUNTRY_SELECT_REGROUP_ENTER;
				callbacks::gui_country_select(state, p->id);
			}
		}
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS reagrupe_leave(ClientState& state, PCOUNTRY p)
{
	if(reagrupe_check(state) != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_UNEXPECTED;
	}
	if(p->selected & COUNTRY_SELECT_REGROUP_ENTER) {
		p->selected &= ~COUNTRY_SELECT_REGROUP_ENTER;
		callbacks::gui_country_select(state, p->id);
	}
	return TEG_STATUS_SUCCESS;
}

}
