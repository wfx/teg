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
 * client side player functions
 */

#include "player.h"

#include <list>
#include <algorithm>
#include <cassert>
#include <string.h>

#include "client.h"

namespace teg::client
{

TEG_STATUS player_whois(ClientState& state, int numjug, PCPLAYER *j)
{
	TEG_STATUS result = TEG_STATUS_PLAYERNOTFOUND;
	players_map_int(state, [&result, &j, numjug](Player& player) {
		if((result != TEG_STATUS_SUCCESS) && (player.numjug == numjug)) {
			*j = &player;
			result = TEG_STATUS_SUCCESS;
			return false;
		}
		return true;
	});
	return result;
}

TEG_STATUS player_update(ClientState& state, PCPLAYER j)
{
	PCPLAYER i{nullptr};

	if(player_whois(state, j->numjug, &i) != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_PLAYERNOTFOUND;
	}

	*i = *j;
	return TEG_STATUS_SUCCESS;
}

void player_ins(ClientState& state, PCPLAYER j)
{
	state.players.push_back(*j);
	state.game.playeres = state.players.size();
}

void player_del(ClientState& state, PCPLAYER pJ)
{
	assert(state.players.size() > 0);

	auto it = std::find_if(state.players.begin(), state.players.end(),
	[pJ](Player &p) {
		return &p == pJ;
	});
	assert(it!=state.players.end());

	state.players.erase(it);

	state.game.playeres = state.players.size();
}

void player_flush(ClientState& state)
{
	state.players.clear();
	state.game.playeres = state.players.size();
}

void player_init(ClientState& state)
{
}

void players_map(ClientState& state, PlayersCallback cb)
{
	std::for_each(state.players.begin(), state.players.end(), cb);
}

void players_map_int(ClientState& state, InterruptablePlayersCallback cb)
{
	for(Player& p: state.players) {
		if(!cb(p)) {
			return;
		}
	}
}

}
