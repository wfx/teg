/*	$Id: ai_misc.c,v 1.10 2002/09/09 03:52:07 riq Exp $	*/
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
/**
 * @file ai_misc.c
 * Funciones que mucho no tienen que ver con el robot, pero ayudan
 */

#include <stdlib.h>
#include <stdio.h>

#include "client.h"

/**
 * @fn char *ai_fetch_a_name()
 * Devuelve el name de un player al azar
 */
char *ai_fetch_a_name()
{
	int i=0;
	PCPLAYER pJ;
	PLIST_ENTRY l = g_list_player.Flink;
	int n;

	if( g_game.playeres < 2 ) {
		return NULL;
	}

	n = RANDOM_MAX(0, g_game.playeres -1 );

	while( !IsListEmpty( &g_list_player ) && (l != &g_list_player) ) {
		if( (i++) == n ) {
			pJ = (PCPLAYER) l;
			if( pJ->numjug == WHOAMI() )
				n++;
			else
				return pJ->name;
		}
		l = LIST_NEXT(l);
	}

	return NULL;
}
