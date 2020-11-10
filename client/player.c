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

#include <string.h>
#include "client.h"

LIST_ENTRY g_list_player;

TEG_STATUS player_whois(int numjug, PCPLAYER *j)
{
	PLIST_ENTRY l = g_list_player.Flink;
	PCPLAYER pJ;

	while( !IsListEmpty( &g_list_player ) && (l != &g_list_player) ) {
		pJ = (PCPLAYER) l;
		if( pJ->numjug == numjug) {
			*j = pJ;
			return TEG_STATUS_SUCCESS;
		}
		l = LIST_NEXT(l);
	}
	return TEG_STATUS_PLAYERNOTFOUND;
}

TEG_STATUS player_update(PCPLAYER j)
{
	PCPLAYER i;

	if( player_whois( j->numjug, &i ) != TEG_STATUS_SUCCESS )
		goto error; 

	memcpy(	&j->next, &i->next, sizeof(LIST_ENTRY));
	memcpy( i,j,sizeof(CPLAYER));
	return TEG_STATUS_SUCCESS;

error:
	return TEG_STATUS_PLAYERNOTFOUND;
}

void player_ins(PCPLAYER j)
{
	PCPLAYER new = (PCPLAYER) malloc( sizeof(CPLAYER) );
	if( new==NULL)
		return;

	memmove( new, j, sizeof(CPLAYER));
	InitializeListHead( &new->next );
	InsertTailList( &g_list_player, (PLIST_ENTRY) new );

	g_game.playeres++;
}

void player_del(PCPLAYER pJ)
{
	PLIST_ENTRY l = (PLIST_ENTRY) pJ;

	l = RemoveHeadList( l->Blink );
	free( l );
	g_game.playeres--;
}

void player_flush(void)
{
	PLIST_ENTRY tmp;

	g_game.playeres=0;
	while( !IsListEmpty( &g_list_player ) ) {
		tmp = RemoveHeadList( &g_list_player );
		free( tmp );
	}
	return TEG_STATUS_SUCCESS;
}

void player_init(void)
{
	InitializeListHead(&g_list_player);
	return TEG_STATUS_SUCCESS;
}
