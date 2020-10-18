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
 * @file score.c
 * High Scores functions
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>

#include "common.h"
#include "scores.h"

static LIST_ENTRY g_list_scores;	/**< top 5 score list */


/* ownership is yielded */
TEG_STATUS scores_insert_score( PSCORES pS_new )
{
	int position = 0;
	PSCORES pS;
	PLIST_ENTRY l = g_list_scores.Flink;

	while( !IsListEmpty(&g_list_scores) && l != &g_list_scores && position < SCORES_MAX ) {
		pS = (PSCORES) l;

		if( pS_new->stats.score > pS->stats.score ) {
			InsertHeadList( (PLIST_ENTRY) pS->next.Blink, (PLIST_ENTRY) pS_new );
			return TEG_STATUS_SUCCESS;
		}

		position++;
		l = LIST_NEXT(l);
	}

	/* If we reach this, its because we must insert the node at the tail */
	/* or because the list is empty */

	if( position < SCORES_MAX )
		InsertTailList( &g_list_scores, (PLIST_ENTRY) pS_new );

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS scores_map(scores_map_func func, void* user)
{
	PLIST_ENTRY l = g_list_scores.Flink;
	PSCORES pS;

	assert(func);

	while( !IsListEmpty( &g_list_scores) && (l != &g_list_scores) ) {
		pS = (PSCORES) l;
		(func)(pS, user);

		l = LIST_NEXT(l);
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS scores_init()
{
	InitializeListHead( &g_list_scores);
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS scores_flush()
{
	PLIST_ENTRY tmp;

	while( !IsListEmpty( &g_list_scores) ) {

		tmp = RemoveHeadList( &g_list_scores );
		free(tmp);

	}
	return TEG_STATUS_SUCCESS;
}
