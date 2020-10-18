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

#include "cont.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _player_stats 
{
	int countries_won;		/**< number of countries won */
	int armies_killed;		/**< number of armies killed */
	int continents_turn[CONT_CANT];	/**< number of turns that the conq was defended */
	int players_killed;		/**< quantity of players killed */

	int score;		/**< score, based on the stats */
} PLAYER_STATS, *PPLAYER_STATS;


/*! initialize the scores */
void stats_init(PPLAYER_STATS s);

/*! calculate the score */
void stats_score(PPLAYER_STATS s, CONT const* continents);

#ifdef __cplusplus
}
#endif
