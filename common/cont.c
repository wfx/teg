/*	$Id: cont.c,v 1.10 2002/09/09 03:52:06 riq Exp $	*/
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
 * @file cont.c
 */

#include "all.h"

CONT g_conts[] = {
	{ N_("South America"), CONTINENTE_AMERICASUR, CONT_AMERICASUR_CANT, 3 },
	{ N_("North America"), CONTINENTE_AMERICANORTE, CONT_AMERICANORTE_CANT, 5 },
	{ N_("Africa"), CONTINENTE_AFRICA, CONT_AFRICA_CANT, 3},
	{ N_("Oceania"), CONTINENTE_OCEANIA, CONT_OCEANIA_CANT, 2 },
	{ N_("Europe"), CONTINENTE_EUROPA, CONT_EUROPA_CANT, 5 },
	{ N_("Asia"), CONTINENTE_ASIA, CONT_ASIA_CANT, 7 },
};
#define NRCONTS ( sizeof(g_conts) /sizeof(g_conts[0]) )

/**
 * @fn int cont_tot( unsigned long  conts )
 * return the total armies that one must place in a continent
 * @param conts Continenet bit-or-wised
 * @return cantidad de fichas que hay que poner
 */
int cont_tot( unsigned long conts )
{
	int max;
	int i;

	max = 0;
	for(i=0;i<NRCONTS;i++) {
		if( conts & 1 ) {
			max += g_conts[i].fichas_x_cont;
		}
		conts >>= 1;
	}

	return max;
}

/* returns the name of a country */
char * cont_get_name( int i )
{
	static char *_unknown = N_("Unknown");

	if( i >= 0 && i < NRCONTS )
		return _( g_conts[i].name );
	else
		return _( _unknown );
}
