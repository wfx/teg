/*	$Id: stats.h,v 1.3 2002/05/25 18:38:14 riq Exp $	*/
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
 * @file stats.h
 * Statistics for the players
 */
#ifndef __TEG_STATS_H
#define __TEG_STATS_H

#include "common.h"
#include "cont.h"

typedef struct _player_stats 
{
	int countries_won;		/**< number of countries won */
<<<<<<< HEAD
	int countries_lost;		/**< number of countries lost */
	int armies_killed;		/**< number of armies killed */
	int armies_lost;		/**< number of armies lost */
=======
	int armies_killed;		/**< number of armies killed */
>>>>>>> b8e1f4d5000e931e81e397369fa5ec789dffe26c
	int continents_turn[CONT_CANT];	/**< number of turns that the conq was defended */
	int players_killed;		/**< quantity of players killed */

	int score;		/**< score, based on the stats */
} PLAYER_STATS, *PPLAYER_STATS;


/* prototypes */
/*! initialize the scores */
TEG_STATUS stats_init( PPLAYER_STATS s );
/*! calculate the score */
TEG_STATUS stats_score( PPLAYER_STATS s );

#endif /* __TEG_STATS_H */
