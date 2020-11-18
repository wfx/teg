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

#pragma once

#include "stats.h"
#include "common.h"

#define SCORES_DATE_MAX (40)

/* Top Ten only */
#define SCORES_MAX	(10)

typedef struct _scores {
	char name[PLAYERNAME_MAX_LEN];	/**< name of the player */
	int color;			/**< color used */
	char date[SCORES_DATE_MAX];	/**< date of the game */
	int human;			/**< human or robot */
	int score;
} SCORES, * PSCORES;

struct HighScores {
	size_t count; ///< the amount of valid high scores
	SCORES highscores[SCORES_MAX]; ///< the actual highscore list
};

/**! insert a score in the table */
void insert_score(SCORES const* score);

// Helper to make the highscore code easier to test
void insert_highscore(struct HighScores* hs, SCORES const* score);

/**! initialize the scores */
void scores_init();

/**! a map function over the scores. (like in functional programming) */
typedef void(*scores_map_func)(PSCORES pJ, void* user);
void scores_map(scores_map_func func, void *user);
