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
/**
 * @file stats.c
 * Statistics for the players
 */

#include "stats.h"
#include "common.h"

#include <string.h>

void stats_init(PPLAYER_STATS s)
{
	memset( s, 0, sizeof(*s) );
}

void stats_score(PPLAYER_STATS s, CONT const* continents)
{
	int i;

	/* points for armies, countries */
	s->score =  s->countries_won * 5 +
		s->armies_killed  * 2;

	/* points for defending continents */
	for(i=0;i<CONT_CANT;i++)
		s->score += s->continents_turn[i] * continents[i].fichas_x_cont;
}
