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

#pragma once

#include "../common/common.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \brief initialize the client game dataset
void game_init();

/// Reset the country and client game datasets. Call this after the game end.
void game_finalize();

/** \brief Try to connect to the game server
 *
 * The connection details are taken from g_game.
 */
TEG_STATUS teg_connect();

/// disconnect from the server, and reset game datastructures and GUI
void teg_disconnect();


/** \brief Callback for the "error:player_id" protocoll error handler.
 *
 * This function terminates the connection and always returns TEG_STATUS_SUCCESS.
 * Since this is a callback, the return value can't be changed.
 */
TEG_STATUS playerid_restore_from_error(void);

/// \brief Tries to start a server at the specified \p port.
TEG_STATUS launch_server(int port);

/// \brief Tries to start a robot
TEG_STATUS launch_robot(void);

/// Log-like function to display messages above \p level in the GUI
TEG_STATUS textmsg(int level, char *format, ...);

/// Create the .teg directory structure in the users home directory
TEG_STATUS dirs_create();

#define ESTADO_ES(a) (g_game.estado==(a))
#define ESTADO_MENOR(a) (g_game.estado<(a))
#define ESTADO_MAYOR_IGUAL(a) (g_game.estado>=(a))

#define WHOAMI() g_game.numjug
#define ESTADO_GET() g_game.estado
#define ESTADO_SET(a) (g_game.estado=(a))

#ifdef __cplusplus
}
#endif
