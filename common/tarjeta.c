/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <rquesada@core-sdi.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; only version 2 of the License.
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
 * @file tarjeta.c
 * funciones para manejar a las tarjetas
 */

#include "all.h"

/**
 * @fn BOOLEAN tarjeta_puedocanje( int numjug, int t1, int t2, int t3 )
 * Dice si es correcto el canje con las tarjetas t1,t2 y t3
 */
BOOLEAN tarjeta_puedocanje( int numjug, int t1, int t2, int t3 )
{
	int result;

	/* chequear que las tarjetas existan */
	if( !(countrynumber_is_valid(t1) &&
	      countrynumber_is_valid(t2) &&
	      countrynumber_is_valid(t3)))
		return FALSE;

	/* chequear que las tarjetas sean del jugador */
	if(!( g_countries[t1].tarjeta.numjug == numjug &&
		g_countries[t2].tarjeta.numjug == numjug &&
		g_countries[t3].tarjeta.numjug == numjug ))
		return FALSE;

	result = g_countries[t1].tarjeta.tarjeta + g_countries[t2].tarjeta.tarjeta + g_countries[t3].tarjeta.tarjeta ;

	return ( result > TARJ_COMODIN || 
		result==TARJ_CANION + TARJ_GLOBO + TARJ_GALEON ||
		result==TARJ_CANION * 3 ||
		result==TARJ_GLOBO * 3 ||
		result==TARJ_GALEON * 3 );
}

void tarjeta_init( void )
{
}

/**
 * @fn void tarjeta_usar( PTARJETA pT )
 */
void tarjeta_usar( PTARJETA pT )
{
	pT->usada = TRUE;
}

/**
 * @fn void tarjeta_desusar( PTARJETA pT )
 */
void tarjeta_desusar( PTARJETA pT )
{
	pT->usada = FALSE;
}

/**
 * @fn void tarjeta_inittarj( PTARJETA t )
 */
void tarjeta_inittarj( PTARJETA t )
{
	t->usada = FALSE;
	t->numjug = -1;
}

/**
 * @fn void tarjeta_poner( PTARJETA t )
 */
void tarjeta_poner( PTARJETA t )
{
	tarjeta_inittarj( t );
}

/**
 * @fn void tarjeta_sacar( PTARJETA t, int numjug )
 */
void tarjeta_sacar( PTARJETA t, int numjug )
{
	t->numjug = numjug;
}

/**
 * @fn int tarjeta_es_libre( int i )
 * funcion auxiliar de token_tarjeta
 * @param i Pais que contiene a la tarjeta
 * @return TRUE si la tarjeta esta libre
 */
int tarjeta_es_libre( int i )
{
	return( g_countries[i].tarjeta.numjug == -1 );
}

/**
 * @fn BOOLEAN tarjeta_es_usada( PTARJETA pT )
 */
BOOLEAN tarjeta_es_usada( PTARJETA pT )
{
	return ( pT->usada == TRUE );
}

