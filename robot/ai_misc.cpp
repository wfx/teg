/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <riq@core-sdi.com>
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

#include "ai_misc.h"

#include <stdlib.h>
#include <stdio.h>

#include "client.h"

namespace teg::robot
{

char *ai_fetch_a_name(teg::client::ClientState& state)
{
	namespace c = ::teg::client;

	if(state.game.playeres < 2) {
		return NULL;
	}

	int n = random_between(0, state.game.playeres -1);
	int i=0;

	char *result{nullptr};
	c::players_map_int(state, [&state, &i, &n, &result](c::Player &player) {
		if((i++) == n) {
			if(player.numjug == c::who_am_i(state)) {
				n++;
			} else {
				result = player.name;
			}
		}
		return nullptr == result;
	});

	return result;
}

}
