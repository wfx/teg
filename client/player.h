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

#include <functional>

#include "../common/common.h"

namespace teg::client
{

typedef struct _player {
	LIST_ENTRY next;
	char name[max_playername_length]; ///< Player name
	char addr[inet_addr_len]; ///< server address
	int color;
	int score; ///< final score
	int numjug; ///< player number
	PLAYER_STATUS estado; ///< current game state
	int tot_countries; ///< \todo find out what the meaning of this field is
	int tot_armies; ///< \todo find out what the meaning of this field is
	int tot_cards; ///< \todo find out what the meaning of this field is
	int empezo_turno; ///< did this player start the turn?
	int human; ///< is this a human?
} CPLAYER, *PCPLAYER;


/// Client game state
extern LIST_ENTRY g_list_player;

/**
 * \brief Fill in the details for the player with number \p numjug into the
 * structure \p j.
 *
 * \return TEG_STATUS_SUCCESS if successfull
 *         TEG_STATUS_PLAYERNOTFOUND if the player number is not assigned.
 */
TEG_STATUS player_whois(int numjug, PCPLAYER *j);

/// \todo: find out if this function does something usefull
TEG_STATUS player_update(PCPLAYER j);

/**
 * Insert the player \p j into the list of all players
 */
void player_ins(PCPLAYER j);

/**
 * Removes the player \p j from the player list.
 *
 * \warning The \p j storage space is freed, so don't access the value
 *          afterwards.
 */
void player_del(PCPLAYER j);

/// Remove all players from the player list
void player_flush(void);

/** Initializes the player list
 *
 * \warning When you call this function with an active player list, you get a
 *          ressource leak since the old list items become unfreeable
 */
void player_init(void);

/// \brief Callback function to process a single player entry
using PlayersCallback = std::function<void(CPLAYER&)>;

/// \brief Map function \p cb over the active player list
void players_map(PlayersCallback cb);

/// \brief Callback function for an interruptable mapping function
using InterruptablePlayersCallback = std::function<bool(CPLAYER&)>;

/**
 * \brief Maps over the player list with the possibility to abort.
 *
 * This function calls \p cb with each active player. When the function returns
 * true, the mapping continutes, a return value of false stops the mapping.
 */
void players_map_int(InterruptablePlayersCallback cb);

}
