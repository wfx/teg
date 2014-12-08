/*	$Id: countries.c,v 1.2 2007/07/07 20:31:23 nordi Exp $	*/
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
 * @file countries.c
 * Funciones para manejar countries en el servidor
 * Functions to manage countries on the server
 */
#include <stdio.h>
#include <stdlib.h>
#include "server.h"

/*
 * incluye matriz de adyacencia
 */

/* Assign the countries to the players at the beginning of the game */
void countries_repartir()
{
	int i;
	int j=0;
	int real_j;
	int resto = COUNTRIES_CANT%g_game.playing;
	PCOUNTRY p;

	for(i=0; i < COUNTRIES_CANT-resto; i++) {
		p = get_random_country( country_libre );  // get random country that is not yet owned by anyone
		player_from_indice( j, &real_j );         // get real player number via reference and...
		player_asignarcountry(real_j,p);          // ...assing the chosen country to this player
		p->ejercitos=1;	                          // give the country one army
		j = (j + 1 ) % g_game.playing;
	}

	/* toma un player al azar, y a partir de el le da un country a cada uno 
	   Give the remaining countries to the players. Start with random player so that
	   the first players do not have an advantage. */
	if( resto ) {
		j = RANDOM_MAX(0,g_game.playing-1);       // choose random player to _start_ with
		for( i=0;i<resto;i++) {                   // same reason as before
			p = get_random_country( country_libre );
			player_from_indice( j, &real_j );
			player_asignarcountry( real_j,p);
			p->ejercitos=1;	
			j = (j + 1) % g_game.playing;
		}
	}
}
