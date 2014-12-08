/*	$Id: pactos.c,v 1.3 2002/08/31 17:45:59 riq Exp $	*/
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
 * @file pactos.c
 * Maneja los pactos entre los jugadores
 * Cuando se setea un pacto, es bidireccional. Esto se hace para simplificar
 * el tema de los pactos.
 */
#include "all.h"

LIST_ENTRY g_list_pactos;

/**
 * @fn TEG_STATUS pactos_attack( int src, int dst )
 * Dice si se pude atacar de src a dst
 */
TEG_STATUS pactos_attack( int src, int dst )
{
	PPACTOS pPac=NULL;

	if( pactos_find( src, dst, &pPac ) != TEG_STATUS_SUCCESS )
		return TEG_STATUS_SUCCESS;

	/* si el pacto esta seteado, entonces no se puede atacar */
	if( pPac->status == PACTO_ESTABLISHED ||
			pPac->status == PACTO_ESTABLISHED_LAST_ROUND )
		return TEG_STATUS_ERROR;
	return TEG_STATUS_SUCCESS;
}

/**
 * @fn TEG_STATUS pactos_init()
 */
TEG_STATUS pactos_init()
{
	InitializeListHead( &g_list_pactos );
	return TEG_STATUS_SUCCESS;
}

/**
 * @fn TEG_STATUS pactos_add(int src, int dst)
 */
TEG_STATUS pactos_add(int src, int dst)
{
	PPACTOS pPac=NULL;

	if( src<0 || dst<0 || src>=COUNTRIES_CANT || dst>=COUNTRIES_CANT || src==dst)
		return TEG_STATUS_ERROR;

	if( pactos_find( src, dst, NULL ) == TEG_STATUS_SUCCESS )
		return TEG_STATUS_ERROR;

	pPac = malloc(sizeof(PACTOS));
	if(!pPac)
		return TEG_STATUS_ERROR;

	pPac->src = src;
	pPac->dst = dst;
	pPac->status = PACTO_REQUEST;

	InsertTailList( &g_list_pactos, (PLIST_ENTRY) pPac );

	return TEG_STATUS_SUCCESS;
}

/**
 * @fn TEG_STATUS pactos_update(int src, int dst, int status )
 */
TEG_STATUS pactos_update(int src, int dst, int status )
{
	PPACTOS pPac=NULL;

	if( pactos_find( src, dst, &pPac ) != TEG_STATUS_SUCCESS )
		return TEG_STATUS_ERROR;

	pPac->status = status;
	return TEG_STATUS_SUCCESS;
}

/**
 * @fn TEG_STATUS pactos_del( int src, int dst )
 */
TEG_STATUS pactos_del( int src, int dst )
{
	PLIST_ENTRY pL=NULL;

	if( pactos_find( src, dst, (PPACTOS*)&pL ) != TEG_STATUS_SUCCESS )
		return TEG_STATUS_ERROR;

	pL = RemoveHeadList( pL->Blink );
	free(pL);
	return TEG_STATUS_SUCCESS;
}

/**
 * @fn TEG_STATUS pactos_find( int src, int dst, PPACTOS *p )
 */
TEG_STATUS pactos_find( int src, int dst, PPACTOS *p )
{
	PLIST_ENTRY pL = g_list_pactos.Flink;
	PPACTOS pPac;

	while( !IsListEmpty( &g_list_pactos ) && (pL != &g_list_pactos) ) {
		pPac = (PPACTOS) pL;

		if( (pPac->src==src && pPac->dst==dst) ||
			(pPac->src==dst && pPac->dst==src) ){

			if(p)
				*p = pPac;

			return TEG_STATUS_SUCCESS;
		}

		pL = LIST_NEXT(pL);
	}
	return TEG_STATUS_NOTFOUND;
}

/**
 * @fn TEG_STATUS pactos_flush()
 */
TEG_STATUS pactos_flush()
{
	PLIST_ENTRY tmp;

	while( !IsListEmpty( &g_list_pactos ) ) {
		tmp = RemoveHeadList( &g_list_pactos );
		free( tmp );
	}
	return TEG_STATUS_SUCCESS;
}
