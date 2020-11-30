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

#include "client.h"
#include "../common/common.h"

namespace teg::client
{

/** \addtogroup TransmitProtocoll
 *
 * All functions in this group use g_game.fd as the file descriptor to send the
 * data to the server.
 * \{
 */

/// \brief Send message \p msg as chat message to all players
TEG_STATUS out_mensaje(char *msg);

/** \brief Tell the server who I am
 *
 * This function sends the name, the observer and the human proprty to the
 * server.
 */
TEG_STATUS out_id();

/// \brief Tell the server to close the connection
TEG_STATUS out_exit();

/// ask server the countries' status
TEG_STATUS out_countries();

/// Request all players scores
TEG_STATUS out_status();

/// Request the server to start the game
TEG_STATUS out_start();

/** Tell the server the ammount of armies (\p cant) I'm sending into the
 *  conquered country p dst from the country \p src. */
TEG_STATUS out_tropas(int src, int dst, int cant);

/// Tell the server I'm done with my turn
TEG_STATUS out_endturn();

/// Sends to server a request for a 'get card'
TEG_STATUS out_tarjeta();

/// request the server my secret mission
TEG_STATUS out_missions();

/// Sets the Conquer-The-World option on/off, Fog of war on/off
TEG_STATUS out_set_typeofgame(int a, int b, int c, int ar1, int ar2);

/// request the type of game
TEG_STATUS out_get_typeofgame();

/// tell the server which color I prefer
TEG_STATUS out_color(int color);

/// robot: ask server what do I have to do
TEG_STATUS out_loque();

/// tells the server that I surrender
TEG_STATUS out_surrender();

/**! request the high scores from the server */
TEG_STATUS out_scores();

/**! sends the request to enumerate all the cards i have */
TEG_STATUS out_enum_cards();

/**! tell the server to launch a robot there */
TEG_STATUS out_robot();

/**! request who started the round, and the round number */
TEG_STATUS out_new_round();

/** \} */

}
