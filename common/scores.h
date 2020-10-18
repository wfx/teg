/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2001 Ricardo Quesada
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
/**
 * @file scores.h
 */

#ifndef __TEG_COMMON_SCORES_H
#define __TEG_COMMON_SCORES_H

#include "stats.h"

#define SCORES_DATE_MAX (40)

/* Top Ten only */
#define SCORES_MAX	(10)

typedef struct _scores
{
	LIST_ENTRY next;

	char name[PLAYERNAME_MAX_LEN];	/**< name of the player */
	int color;			/**< color used */
	char date[SCORES_DATE_MAX];	/**< date of the game */
	int human;			/**< human or robot */

	PLAYER_STATS stats;		/**< stats of the game */

} SCORES, * PSCORES;

/**! insert a score in the table */
TEG_STATUS scores_insert_score( PSCORES score );

/**! initialize the scores */
TEG_STATUS scores_init();

/**! a map function over the scores. (like in functional programming) */
typedef void(*scores_map_func)( PSCORES pJ);
TEG_STATUS scores_map( scores_map_func func );

/**! return the list of all the scores */
PLIST_ENTRY scores_get_list();

/*! flush the list of scores */
TEG_STATUS scores_flush();

#endif /* __TEG_COMMONG_SCORES_H */
