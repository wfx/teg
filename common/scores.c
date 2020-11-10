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

#include "scores.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>

#include "common.h"


static struct HighScores highscores;

void insert_score(SCORES const* score)
{
	insert_highscore(&highscores, score);
}

void insert_highscore(struct HighScores* hs, SCORES const* score)
{
	size_t insert_pos;
	for(insert_pos = 0;
	    /* *** WARNING *** this loop can walk one element past the array.
		 *
		 * The code in the loop jumps out if the test index goes behind the
		 * valid array elements. When you change this code, make sure to not
		 * access invalid elements (index >= count) */
	    (insert_pos <= hs->count);
	    insert_pos++)
	{
		if (insert_pos >= SCORES_MAX) {
			// the new high score is lower (or equal) to each existing one
			return;
		}

		if((insert_pos >= hs->count) // behind the last existing element

		   // the existing element has a lower score
		   || (score->score > hs->highscores[insert_pos].score)) {
			// we found the insertion point
			break;
		}
	}

	if(hs->count < (SCORES_MAX)) {
		// The highscore list is not full, so expand it
		hs->count++;
	}

	// do we need to move existing elements?
	if(insert_pos < (hs->count-1)) {
		assert(hs->count > 1);
		for(size_t i=(hs->count-1); i>insert_pos; i--)
		{
			assert(i >= 1);
			hs->highscores[i] = hs->highscores[i-1];
		}
	}

	// put the new highscore into its place
	hs->highscores[insert_pos] = *score;
}

void scores_map(scores_map_func func, void* user)
{
	for(size_t i=0; i < highscores.count; i++) {
		func(highscores.highscores+i, user);
	}
}

void scores_init()
{
	highscores.count = 0;
}
