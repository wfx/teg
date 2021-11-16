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

using ClientPlayerList = std::list<Player>;

ClientPlayerList players;

TEG_STATUS player_whois(int numjug, PCPLAYER *j)
{
	TEG_STATUS result = TEG_STATUS_PLAYERNOTFOUND;
	players_map_int([&result, &j, numjug](Player& player) {
		if((result != TEG_STATUS_SUCCESS) && (player.numjug == numjug)) {
			*j = &player;
			result = TEG_STATUS_SUCCESS;
			return false;
		}
		return true;
	});
	return result;
}

TEG_STATUS player_update(PCPLAYER j)
{
	PCPLAYER i{nullptr};

	if(player_whois(j->numjug, &i) != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_PLAYERNOTFOUND;
	}

	*i = *j;
	return TEG_STATUS_SUCCESS;
}

void player_ins(PCPLAYER j)
{
	players.push_back(*j);
	g_game.playeres = players.size();
}

void player_del(PCPLAYER pJ)
{
	assert(players.size() > 0);

	auto it = std::find_if(players.begin(), players.end(),
	[pJ](Player &p) {
		return &p == pJ;
	});
	assert(it!=players.end());

	players.erase(it);

	g_game.playeres = players.size();
}

void player_flush(void)
{
	players.clear();
	g_game.playeres = players.size();
}

void player_init(void)
{
}

void players_map(PlayersCallback cb)
{
	std::for_each(players.begin(), players.end(), cb);
}

void players_map_int(InterruptablePlayersCallback cb)
{
	for(Player& p: players) {
		if(!cb(p)) {
			return;
		}
	}
}

}
