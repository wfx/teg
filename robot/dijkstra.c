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
 * @file dijkstra.c
 * Implementacion de de camino mas corte de Dijkstra
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "client.h"
#include "ai.h"
#include "dijkstra.h"

#define INFINITO (-1)
#define MAX_NODES (COUNTRIES_CANT)

struct _etiqueta {
	int peso;
	int origen;
	int modificado;
};

static int mat[MAX_NODES][MAX_NODES];
static struct _etiqueta et[MAX_NODES];
static int mat_src=-1;

static void dij_ponerdefaults( void )
{
	int i;
	for(i=0;i<MAX_NODES;i++) {
		et[i].origen = -1;
		if( i == mat_src ) {
			et[i].peso = 0;
			et[i].modificado = TRUE;
		} else {
			et[i].peso = INFINITO;
			et[i].modificado = FALSE;
		}
	}
}

static int dij_haymodificados( int *id )
{
	int i;
	for(i=0;i<MAX_NODES;i++) {
		if( et[i].modificado ) {
			*id = i;
			return TRUE;
		}
	}
	return FALSE;
}

static void dij_calcularpeso( int s, int d )
{
	// Si los 2 son distintos a  INFINITO
	if( mat[s][d] != INFINITO && et[s].peso != INFINITO) {

		// Si es menor pongo el nuevo valor
		if( (et[d].peso >=  et[s].peso + mat[s][d] ) 
			|| et[d].peso==INFINITO) {

			// Nuevo valor para destino
			et[d].peso = et[s].peso + mat[s][d];
			et[d].modificado = TRUE;
			et[d].origen = s;
		}
	}
	// else como alguno es infinito no tiene sentido
}

void dij_resolver( void )
{
	int s;			// source
	int d;			// destino

	dij_ponerdefaults();

	while( dij_haymodificados( &s ) ) {
		for(d=0;d<MAX_NODES;d++) {
			if(d==s) continue;	// opminizacion

			if( mat[s][d] > 0) {	// es adyacente ?
				dij_calcularpeso(s,d);
			}
		}
		et[s].modificado = FALSE;	// limpia el flag de modif
	}
}

void dij_cargar( int *m[], int src )
{
	mat_src = src;
}
