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
 * @file canje.c
 * @brief Client side card management.
 */

#include <filesystem>
#include "../common/net.h"

#include "protocol.h"
#include "fcintl.h"
#include "client.h"

namespace teg::client
{

TEG_STATUS canje_out(ClientState& state, int p1, int p2, int p3)
{
	if(!is_status(state, PLAYER_STATUS_FICHASC)) {
		textmsg(state, M_ERR, _("Error, its not the moment to exchange the cards"));
		return TEG_STATUS_ERROR;
	}

	if(tarjeta_puedocanje(state.game.common, who_am_i(state), p1, p2, p3)) {
		state.connection->printf(TOKEN_CANJE"=%d,%d,%d\n", p1, p2, p3);
		tarjeta_poner(&state.game.common.countries[p1].tarjeta);
		tarjeta_poner(&state.game.common.countries[p2].tarjeta);
		tarjeta_poner(&state.game.common.countries[p3].tarjeta);
		return TEG_STATUS_SUCCESS;
	} else {
		textmsg(state, M_ERR, _("Error, the cards must be all equal, or all different"));
		return TEG_STATUS_ERROR;
	}
}

/**
 * @fn TEG_STATUS canje_puedo(int *p1, int *p2, int *p3)
 * Dice si se puede hacer canje, y devuelve una posible combinacion
 * de tarjetas
 */
TEG_STATUS canje_puedo(ClientState& state, int *p1, int *p2, int *p3)
{

	if(state.game.tarjetas_cant < 3) {
		return TEG_STATUS_ERROR;
	}

	int a[maximum_country_cards];
	for(int& ai: a) {
		ai = -1;
	}

	unsigned index=0;
	countries_map(state.game.common, [&state, &a, &index](COUNTRY& country) {
		if((country.tarjeta.numjug == who_am_i(state))
		        && (index < std::extent<decltype(a)>::value)) {
			a[index] = country.id;
			index++;
		}
	});

	for(int i=0; i<state.game.tarjetas_cant; i++) {
		for(int j=i+1; j<state.game.tarjetas_cant; j++) {
			for(int k=j+1; k<state.game.tarjetas_cant; k++) {
				if(tarjeta_puedocanje(state.game.common, state.game.numjug, a[i], a[j], a[k])) {
					if(p1) {
						*p1 = a[i];
					}
					if(p2) {
						*p2 = a[j];
					}
					if(p3) {
						*p3 = a[k];
					}
					return TEG_STATUS_SUCCESS;
				}
			}
		}
	}

	return TEG_STATUS_ERROR;
}

}
