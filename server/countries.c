/*	$Id: countries.c,v 1.1 2002/08/31 17:46:00 riq Exp $	*/
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
 */
#include <stdio.h>
#include <stdlib.h>
#include "server.h"

/*
 * incluye matriz de adyacencia
 */

void countries_repartir()
{
	int i;
	int j=0;
	int real_j;
	int resto = COUNTRIES_CANT%g_game.playing;
	PCOUNTRY p;

	for(i=0; i < COUNTRIES_CANT-resto; i++) {
		p = get_random_country( country_libre );
		player_from_indice( j, &real_j );
		player_asignarcountry(real_j,p);
		p->ejercitos=1;	
		j = (++j % g_game.playing);
	}

	/* toma un player al azar, y a partir de el le da un country a cada uno */
	if( resto ) {
		j = RANDOM_MAX(0,g_game.playing-1);
		for( i=0;i<resto;i++) {
			p = get_random_country( country_libre );
			player_from_indice( j, &real_j );
			player_asignarcountry( real_j,p);
			p->ejercitos=1;	
			j = (++j % g_game.playing);
		}
	}
}
