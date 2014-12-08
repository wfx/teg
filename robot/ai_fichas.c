/*	$Id: ai_fichas.c,v 1.7 2006/03/13 22:55:51 nordi Exp $	*/
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
 * @file ai_fichas.c
 * AI for placing armies
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <unistd.h>

#include "client.h"
#include "ai.h"
#include "ai_fichas.h"

/**
 * @fn TEG_STATUS ai_fichas_calc_puntaje_conquer( int country )
 * Strategy: Try to conquer continent
 */
TEG_STATUS ai_fichas_calc_puntaje_conquer( int country )
{
	int i;
	int pc;			/* countries del continente */
	int pe=0;		/* countries enemigos */
	int ee=0;		/* ejercitos enemigos */
	int pm=0;		/* countries mios */
	int em=0;		/* ejercitos mios */
	int ple=0;		/* countries limitrofes enemigos */


	if( ai_own_continent( g_countries[country].continente ))
		return TEG_STATUS_SUCCESS;

	pc = g_conts[ g_countries[country].continente ].cant_countries;

	for(i=0;i<COUNTRIES_CANT;i++) {
		if( g_countries[country].continente == g_countries[i].continente ) {
			if( g_countries[i].numjug == WHOAMI() ) {
				pm++;
				em += g_countries[i].ejercitos;
			} else {
				pe++;
				ee += g_countries[i].ejercitos;
				if( countries_eslimitrofe(country,i)) {
					ple++;
				}
			}
		}
	}

	/* si tengo mas del xx% de los countries del cont suma puntos */
#if 0
	i = pm*100/pc;
	if( i < 99 ) {
		if( i >= 50 )
			ai_puntaje[country] += 3;
		if( i >= 65 )
			ai_puntaje[country] += 4;
		if( i >= 80 )
			ai_puntaje[country] += 5;
		ai_puntaje[country] += ple;
	}
#else
	if( pc != pm ) {
		/* ASIA */
		if( pc >= 12 ) {
			if( pm >= 10 ) ai_puntaje[country] += 1;
			if( pm >= 11 ) ai_puntaje[country] += 2;
			if( pm >= 12 ) ai_puntaje[country] += 7;
			if( pm < 7 ) ai_puntaje[country] -= 5;
		}
		/* EUROPE, NORTH AMERICA */
		else if( pc >= 8 ) {
			if( pm >= 5 ) ai_puntaje[country] += 1;
			if( pm >= 7 ) ai_puntaje[country] += 2;
			if( pm >= 8 ) ai_puntaje[country] += 7;
		}
		/* SOUTH AMERICA, AFRICA */
		else if( pc >= 6 ) {
			if( pm >= 4 ) ai_puntaje[country] += 2;
			if( pm >= 5 ) ai_puntaje[country] += 7;
		}
		/* OCEANIA */
		if( pc >= 4 ) {
			if( pm >= 2 ) ai_puntaje[country] += 2;
			if( pm >= 3 ) ai_puntaje[country] += 3;
		}
		ai_puntaje[country] += ple;
		if( em > ee )
			ai_puntaje[country] += 3;

		if(ple == 0)
			ai_puntaje[country] -= 50;
	}
#endif
	return TEG_STATUS_SUCCESS;
}

/**
 * @fn TEG_STATUS ai_fichas_calc_puntaje_defense( int country )
 * Strategy: Try to defend continent
 */
TEG_STATUS ai_fichas_calc_puntaje_defense( int country )
{
	int i;
	int c = g_countries[country].continente;
	int suma=0;
	int f=0;		/* fronteras que tiene */

	if( !ai_own_continent(c) )
		return TEG_STATUS_SUCCESS;

	for(i=0;i< COUNTRIES_CANT;i++) {
		if( countries_eslimitrofe(country,i) && g_countries[i].continente != c ) {
			if (g_countries[i].numjug != WHOAMI() ) {

				suma += 5;

				/* increase points if the enemy has more armies */
				if( g_countries[i].ejercitos > g_countries[country].ejercitos )
					suma += 6;
			}
			else
				suma += 1;

			/* increase points if I have a few armies */
			if( g_countries[country].ejercitos < 4 )
				suma += 3; 

			f++;
		}
	}

	/* if it is no border, dont place on him */
	if( f == 0 )
		ai_puntaje[country] -= 50;

	ai_puntaje[country] += suma + (f/2);

	return TEG_STATUS_SUCCESS;
}


/**
 * @fn TEG_STATUS ai_fichas_calc_puntaje_fichas( int country )
 * Strategy: Try to conquer other countries
 */
TEG_STATUS ai_fichas_calc_puntaje_fichas( int country )
{
	int i;
	int p1=0;
	int p2=0;
	int suma=0;

	int cant_country = g_conts[g_countries[country].continente].cant_countries;

	/* suma las fichas de los countries limitrofes enemigos */
	for(i=0;i< COUNTRIES_CANT;i++) {
		if( g_countries[i].numjug != WHOAMI() && countries_eslimitrofe( country, i )) {

			/* si es un enemigo ya tiene punto */
			suma += 3;

			if( g_countries[country].ejercitos >= g_countries[i].ejercitos )
				p2 += 2 + g_countries[country].ejercitos - g_countries[i].ejercitos;
			else
				p1 +=g_countries[i].ejercitos - g_countries[country].ejercitos;
		}
	}

	if( suma == 0 )
		return TEG_STATUS_SUCCESS;

	suma += p1/2 + p2/2;

	/* ASIA */
	if( cant_country >= 12)
		suma -= 1;

	/* EUROPE, NORTH AMERICA */
	else if( cant_country >= 8 )
		suma += 1;

	/* SOUTH AMERICA, AFRICA */
	else if( cant_country >= 6)
		suma += 3;

	/* OCENIA */
	if( cant_country >= 4)
		suma += 3;

	ai_puntaje[country] += suma;

	return TEG_STATUS_SUCCESS;
}


/**
 * @fn TEG_STATUS ai_fichas_calc_puntaje( int p )
 * Calls all the strategies of the robots to place armies
 */
TEG_STATUS ai_fichas_calc_puntaje( int p )
{
	ai_fichas_calc_puntaje_conquer( p );
	ai_fichas_calc_puntaje_fichas( p );
	ai_fichas_calc_puntaje_defense( p );

	return TEG_STATUS_SUCCESS;
}


/**
 * @fn TEG_STATUS __ai_fichas( int cant )
 * Pone fichas en los countries.
 */
TEG_STATUS __ai_fichas( int cant )
{
	int i;

	/* all unavailble countries are -10000 */
	ai_puntaje_clean();

	for(i=0;i< COUNTRIES_CANT;i++ ) {
		if( g_countries[i].numjug == WHOAMI() ) {
			ai_puntaje[i]=0;	
			ai_fichas_calc_puntaje(i);
		}
	}

	if ( ai_puntaje_sort( cant ) != TEG_STATUS_SUCCESS ) {
		textmsg(M_ERR,_("Error in ai_puntaje_sort, terminating the robot. Variable cant was %d"),cant );
		return TEG_STATUS_ERROR;
	}

	for(i=0;i<cant;i++) {
		if( fichas_add( &g_countries[ ai_sorted[i] ]) != TEG_STATUS_SUCCESS ) {
			textmsg(M_ERR,_("Failed to fichas_add(%s)"),g_countries[ai_sorted[i]].name );
			return TEG_STATUS_ERROR;
		}
	}
	return TEG_STATUS_SUCCESS;
}
/**
 * @fn TEG_STATUS ai_fichas( int cant )
 */
TEG_STATUS ai_fichas( int cant )
{
	ai_puntaje_clean();

	if( __ai_fichas( cant ) != TEG_STATUS_SUCCESS )
		return TEG_STATUS_ERROR;

	if( fichas_out() != TEG_STATUS_SUCCESS )
		return TEG_STATUS_ERROR;

	return TEG_STATUS_SUCCESS;
}


/**
 * @fn TEG_STATUS ai_fichas_en_cont( int cont )
 * Place armies in the continents
 */
TEG_STATUS ai_fichas_en_cont( int cont )
{
	int cant = g_conts[cont].fichas_x_cont;
	int i;

	/* all unavailble countries are -10000 */
	ai_puntaje_clean();

	for(i=0;i<COUNTRIES_CANT;i++) {
		if( g_countries[i].continente == g_conts[cont].id ) {
			ai_puntaje[i] = 0;
			ai_fichas_calc_puntaje_defense( i );
		}
	}

	ai_puntaje_sort( cant );

	for(i=0;i<cant;i++) {
#ifdef DEBUG
		printf("%s: %s\n",g_conts[cont].name, g_countries[ai_sorted[i]].name);
#endif

		if( fichas_add( &g_countries[ ai_sorted[i] ]) != TEG_STATUS_SUCCESS ) {
			textmsg(M_ERR,_("Failed to fichas_add(%s)"),g_countries[ai_sorted[i]].name);
			return TEG_STATUS_ERROR;
		}
	}

	return TEG_STATUS_SUCCESS;
}

/**
 * @fn TEG_STATUS ai_fichasc( int cant, int conts)
 * pone fichas de continente y de eso.
 */
TEG_STATUS ai_fichasc( int cant, int conts)
{
	int i;
	int conts_tmp;

	ai_puntaje_clean();

	/* pongo las fichas del continente */
	conts_tmp = conts;
	for(i=0;i<CONT_CANT;i++) {
		if( conts_tmp & 1 ) {
			ai_fichas_en_cont( i );
		}
		conts_tmp >>= 1;
	}

	ai_puntaje_clean();

	/* y ahora las fichas donde creo conveniente */
	if( __ai_fichas( cant ) != TEG_STATUS_SUCCESS ) {
		textmsg(M_ERR,_("Failed to __ai_fichas( %d )"),cant );
		return TEG_STATUS_ERROR;
	}

	if( fichas_out() != TEG_STATUS_SUCCESS ) {
		textmsg(M_ERR,_("Failed to fichas_out()" ));
		return TEG_STATUS_ERROR;
	}

	return TEG_STATUS_SUCCESS;
}
