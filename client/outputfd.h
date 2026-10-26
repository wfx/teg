/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <riq@corest.com>
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

#pragma once

#include "../common/common.h"

namespace teg::client
{

struct ClientState;

/** \addtogroup TransmitProtocoll
 *
 * All functions in this group use the file descriptor of the client state to
 * send the data to the server.
 * \{
 */

/// \brief Send message \p msg as chat message to all players
TEG_STATUS out_mensaje(ClientState& state, char *msg);

/** \brief Tell the server who I am
 *
 * This function sends the name, the observer and the human proprty to the
 * server.
 */
TEG_STATUS out_id(ClientState& state);

/// Tell the server to close the connection
TEG_STATUS out_exit(ClientState& state);

/// ask server the countries' status
TEG_STATUS out_countries(ClientState& state);

/// Request all players scores
TEG_STATUS out_status(ClientState& state);

/// Request the server to start the game
TEG_STATUS out_start(ClientState& state);

/** Tell the server the ammount of armies (\p cant) I'm sending into the
 *  conquered country p dst from the country \p src. */
TEG_STATUS out_tropas(ClientState& state, int src, int dst, int cant);

/// Tell the server I'm done with my turn
TEG_STATUS out_endturn(ClientState& state);

/// Sends to server a request for a 'get card'
TEG_STATUS out_tarjeta(ClientState& state);

/// request the server my secret mission
TEG_STATUS out_missions(ClientState& state);

/// Sets the Conquer-The-World option on/off, Fog of war on/off
TEG_STATUS out_set_typeofgame(ClientState& state, int a, int b, int c, int ar1, int ar2);

/// request the type of game
TEG_STATUS out_get_typeofgame(ClientState& state);

/// tell the server which color I prefer
TEG_STATUS out_color(ClientState& state, int color);

/// robot: ask server what do I have to do
TEG_STATUS out_loque(ClientState& state);

/// tells the server that I surrender
TEG_STATUS out_surrender(ClientState& state);

/**! request the high scores from the server */
TEG_STATUS out_scores(ClientState& state);

/**! sends the request to enumerate all the cards i have */
TEG_STATUS out_enum_cards(ClientState& state);

/**! tell the server to launch a robot there */
TEG_STATUS out_robot(ClientState& state);

/**! request who started the round, and the round number */
TEG_STATUS out_new_round(ClientState& state);

/** \} */

}
