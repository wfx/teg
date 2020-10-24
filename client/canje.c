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
 * @file canje.c
 * @brief Client side card management.
 */

#include "protocol.h"
#include "fcintl.h"
#include "client.h"

TEG_STATUS canje_out( int p1, int p2, int p3 )
{
	if( !ESTADO_ES( PLAYER_STATUS_FICHASC )) {
		textmsg(M_ERR,_("Error, its not the moment to exchange the cards"));
		return TEG_STATUS_ERROR;
	}

	if( tarjeta_puedocanje( WHOAMI(), p1, p2, p3 )) {
		net_printf(g_game.fd,TOKEN_CANJE"=%d,%d,%d\n",p1,p2,p3);
		tarjeta_poner(&g_countries[p1].tarjeta);
		tarjeta_poner(&g_countries[p2].tarjeta);
		tarjeta_poner(&g_countries[p3].tarjeta);
		return TEG_STATUS_SUCCESS;
	} else {
		textmsg(M_ERR,_("Error, the cards must be all equal, or all different"));
		return TEG_STATUS_ERROR;
	}
}

/**
 * @fn TEG_STATUS canje_puedo(int *p1, int *p2, int *p3)
 * Dice si se puede hacer canje, y devuelve una posible combinacion
 * de tarjetas
 */
TEG_STATUS canje_puedo(int *p1, int *p2, int *p3)
{
	PLIST_ENTRY pL = g_game.tarjetas_list.Flink;
	int i,j,k;
	int a[TEG_MAX_TARJETAS];

	if( g_game.tarjetas_cant < 3 )
		return TEG_STATUS_ERROR;

	for( i=0;i<sizeof(a)/sizeof(a[0]);i++)
		a[i] = -1;

	i=0;
	while( !IsListEmpty( &g_game.tarjetas_list ) && (pL != &g_game.tarjetas_list )) {
		PCOUNTRY pP;
		PTARJETA pT = (PTARJETA) pL;
		pP = (PCOUNTRY ) COUNTRY_FROM_TARJETA( pT );

		a[i++] = pP->id;
		if( i >= (sizeof(a)/sizeof(a[0])) )
			break;

		pL = LIST_NEXT( pL );
	}

	for(i=0;i<g_game.tarjetas_cant;i++) {
		for(j=i+1;j<g_game.tarjetas_cant;j++) {
			for(k=j+1;k<g_game.tarjetas_cant;k++) {
				if( tarjeta_puedocanje(g_game.numjug,a[i],a[j],a[k])) {
					if(p1) *p1 = a[i];
					if(p2) *p2 = a[j];
					if(p3) *p3 = a[k];
					return TEG_STATUS_SUCCESS;
				}
			}
		}
	}

	return TEG_STATUS_ERROR;
}
