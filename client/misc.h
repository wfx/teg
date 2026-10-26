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

#include "state.h"

namespace teg::client
{

/// \brief initialize the client game dataset
void game_init(ClientState& state);

/// Reset the country and client game datasets. Call this after the game end.
void game_finalize(ClientState& state);

/** \brief Try to connect to the game server
 *
 * The connection details are taken from the client state.
 */
TEG_STATUS teg_connect(ClientState& state);

/// disconnect from the server, and reset game datastructures and GUI
void teg_disconnect(ClientState& state);

/// \brief re-initializes a game. Called every time a new game will start
TEG_STATUS game_reinit(ClientState& state);


/** \brief Callback for the "error:player_id" protocoll error handler.
 *
 * This function terminates the connection and always returns TEG_STATUS_SUCCESS.
 * Since it is a callback, the return value can't be changed.
 */
TEG_STATUS playerid_restore_from_error(ClientState& state);

/// \brief Tries to start a server at the specified \p port.
TEG_STATUS launch_server(ClientState& state, int port);

/// \brief Tries to start a robot
TEG_STATUS launch_robot(ClientState& state);

/// Log-like function to display messages above \p level in the GUI
TEG_STATUS textmsg(ClientState& state, int level, const char *format, ...);

/// Create the .teg directory structure in the users home directory
TEG_STATUS dirs_create();

/// \brief The current player status of this client
inline PLAYER_STATUS client_status(ClientState const& state)
{
	return state.game.estado;
}

/// \brief Is the client in exactly this status?
inline bool is_status(ClientState const& state, PLAYER_STATUS wanted)
{
	return state.game.estado == wanted;
}

/// \brief Is the client in a status before \p wanted?
inline bool status_below(ClientState const& state, PLAYER_STATUS wanted)
{
	return state.game.estado < wanted;
}

/// \brief Is the client in \p wanted or a later status?
inline bool status_at_least(ClientState const& state, PLAYER_STATUS wanted)
{
	return state.game.estado >= wanted;
}

/// \brief Change the client status
inline void set_status(ClientState& state, PLAYER_STATUS wanted)
{
	state.game.estado = wanted;
}

/// \brief The player number of this client
inline int who_am_i(ClientState const& state)
{
	return state.game.numjug;
}

}
