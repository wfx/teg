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
#include "net.h"
#include "protocol.h"
#include "fcintl.h"
#include "client.h"

namespace teg::client
{

TEG_STATUS ejer2_out(ClientState& state, int country)
{
	if(state.game.common.countries[ country ].numjug == who_am_i(state)) {
		if(is_status(state, PLAYER_STATUS_TARJETA)) {
			if(!tarjeta_es_usada(&state.game.common.countries[ country ].tarjeta)) {
				tarjeta_usar(&state.game.common.countries[ country ].tarjeta);
				state.ejer2.last_country = country;
				net_printf(state.game.fd, TOKEN_EJER2"=%d\n", country);
				return TEG_STATUS_SUCCESS;
			} else {
				textmsg(state, M_ERR, _("Error, the 2 armies where placed before"));
				return TEG_STATUS_ERROR;
			}
		} else {
			textmsg(state, M_ERR, _("Error, it's not the time to put 2 armies."));
			return TEG_STATUS_ERROR;
		}
	} else {
		textmsg(state, M_ERR, _("Error, '%s' isnt one of your countries"), state.game.common.countries[country].name);
		return TEG_STATUS_ERROR;
	}
	return TEG_STATUS_ERROR;
}

TEG_STATUS ejer2_restore_from_error(ClientState& state)
{
	if(state.ejer2.last_country != -1) {
		tarjeta_desusar(&state.game.common.countries[ state.ejer2.last_country ].tarjeta);
		callbacks::gui_tarjeta(state, -1);
		state.ejer2.last_country = -1;
		return TEG_STATUS_SUCCESS;
	}
	return TEG_STATUS_ERROR;
}

}
