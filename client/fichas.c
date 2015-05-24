/*	$Id: fichas.c,v 1.22 2006/03/12 16:24:34 nordi Exp $	*/
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
 * @file fichas.c
 * Contiene algunas funciones auxiliares para el manejo del estado 'ESTADO_FICHAS'
 */

#include <stdio.h>
#include <string.h>
#include "client.h"

static int	aFichas[COUNTRIES_CANT];	/**< array de los countries */
static int	aConts[CONT_CANT];	/**< array de los continentes */
static int	fichas_tot;		/**< cantidad de fichas que hay en el array */
static int	wanted_tot;		/**< cantidad de fichas pedidas para enviar */
static int	wanted_conts;		/**< continentes pedidos */


/**
 * @fn static TEG_STATUS fichas_check()
 * Dice si se esta en condiciones de hacer algo con fichas
 */
TEG_STATUS fichas_check()
{
	PLAYER_STATUS e;

	e = ESTADO_GET();

	if( e==PLAYER_STATUS_FICHAS || e==PLAYER_STATUS_FICHAS2 || e==PLAYER_STATUS_FICHASC )
		return TEG_STATUS_SUCCESS;
	else
		return TEG_STATUS_ERROR;
}

/**
 * @fn TEG_STATUS add_fichas( PCOUNTRY p )
 * llamada desde la 'gui' y recuerda los countries que se fueron agregando
 * @params p country que se esta agregando
 */
TEG_STATUS fichas_add( PCOUNTRY p )
{
	if( fichas_check() != TEG_STATUS_SUCCESS ) {
		textmsg(M_ERR,_("Error, you cant add armies now"));
		return TEG_STATUS_UNEXPECTED;
	}

	if(p->numjug == WHOAMI() ) {
		if( fichas_tot < wanted_tot ) {
			fichas_tot++;
			p->ejercitos++;
			aFichas[p->id]++;
			aConts[p->continente]++;
			return TEG_STATUS_SUCCESS;
		} else {
			textmsg(M_ERR,_("Error, you cant put more than %d armies"),wanted_tot);
			fichas_enter( p );
			return TEG_STATUS_ERROR;
		}
	} else {
		textmsg(M_ERR,_("Error, '%s' isnt one of your countries"),countries_get_name(p->id));
		return TEG_STATUS_UNEXPECTED;
	}
}

/**
 * @fn TEG_STATUS sub_fichas( PCOUNTRY p )
 * llamada desde la 'gui' y recuerda los countries que se fueron agregando
 * @params p country que se esta agregando
 */
TEG_STATUS fichas_sub( PCOUNTRY p )
{
	if( fichas_check() != TEG_STATUS_SUCCESS ) {
		textmsg(M_ERR,_("Error, you cant sub armies now"));
		return TEG_STATUS_UNEXPECTED;
	}

	if(p->numjug == WHOAMI() ) {
		if( aFichas[p->id] ) {
			fichas_tot--;
			p->ejercitos--;
			aFichas[p->id]--;
			aConts[p->continente]--;
			fichas_enter( p );
			return TEG_STATUS_SUCCESS;
		} else return TEG_STATUS_UNEXPECTED;
	} else {
		textmsg(M_ERR,_("Error, '%s' isnt one of your countries"),countries_get_name(p->id));
		return TEG_STATUS_UNEXPECTED;
	}
}

/**
 * @fn TEG_STATUS finish_fichas( int cant, int **ptr )
 * Funcion llamada cuando uno 'cree' que ya puso todas las fichas
 * @params cant Cantidad de fichas que tiene que haber
 * @params ptr Puntero al array
 * @return TEG_STATUS_SUCCESS si las fichas estan bien puestas
 */
TEG_STATUS fichas_finish( int **ptr )
{
	int i,c;
	if( fichas_check() != TEG_STATUS_SUCCESS )
		return TEG_STATUS_UNEXPECTED;

	if( wanted_tot != fichas_tot )
		return TEG_STATUS_ERROR;

	c = wanted_conts;
	for(i=0;i<CONT_CANT;i++) {
		if( (c & 1) && (aConts[i] < g_conts[i].fichas_x_cont) )
			return TEG_STATUS_ERROR;
		c >>= 1;
	}

	*ptr = aFichas;
	return TEG_STATUS_SUCCESS;
}

/**
 * @fn TEG_STATUS init_fichas()
 * Llamada antes de empezar a poner fichas
 * @return TEG_STATUS_SUCCESS (siempre)
 */
TEG_STATUS fichas_init(int cant, int conts)
{
	int i;
	for(i=0;i<COUNTRIES_CANT;i++)
		aFichas[i]=0;
	for(i=0;i<CONT_CANT;i++)
		aConts[i]=0;
	fichas_tot = 0;
	wanted_tot = cant;
	wanted_conts = conts;

	return TEG_STATUS_SUCCESS;
}

/**
 * @fn void fichas_add_wanted( int i )
 * Si hace un canje, entonces se van a poder poner mas fichas
 */
void fichas_add_wanted( int i )
{
	wanted_tot += i ;
}

/**
 * @fn TEG_STATUS reset_fichas()
 * Si hay un error, se llama esta funcion que hace que empiece todo de nuevo
 * @return TEG_STATUS_SUCCESS (siempre)
 */
TEG_STATUS fichas_reset()
{
	int i;

	for( i=0;i<COUNTRIES_CANT;i++) {
		if( aFichas[i] ) {
			g_countries[i].ejercitos -= aFichas[i];
			aFichas[i] = 0;
		}
	}
	for( i=0;i<CONT_CANT;i++)
		aConts[i] = 0;

	fichas_tot = 0;
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS fichas_restore_from_error()
{
	ESTADO_SET( PLAYER_STATUS_FICHAS );
	fichas_reset();
	aux_draw_all_countries();
	gui_sensi();
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS fichas2_restore_from_error()
{
	ESTADO_SET( PLAYER_STATUS_FICHAS2 );
	fichas_reset();
	aux_draw_all_countries();
	gui_sensi();
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS fichasc_restore_from_error()
{
	ESTADO_SET( PLAYER_STATUS_FICHASC );
	fichas_reset();
	aux_draw_all_countries();
	gui_sensi();
	return TEG_STATUS_SUCCESS;
}

/**
 * @fn TEG_STATUS fichas_out()
 * Envia las fichas al server. Usada por FICHAS, FICHAS2 y FICHASC
 */
TEG_STATUS fichas_out()
{
	char buf[2000];
	char tmp[100];
	int *ptr;
	int i;
	int first_time;
	PLAYER_STATUS e;

	if( fichas_check() != TEG_STATUS_SUCCESS ) {
		textmsg( M_ERR,_("Error, its not the time to send armies"));
		return TEG_STATUS_ERROR;
	}

	if( fichas_finish( &ptr ) != TEG_STATUS_SUCCESS )  {
		fichas_reset();
		textmsg( M_ERR,_("Error, put the correct number of armies"));
		return TEG_STATUS_ERROR;
	}

	buf[0]=0;
	tmp[0]=0;

	first_time = 1;

	for(i=0;i<COUNTRIES_CANT;i++) {
		if( ptr[i]>0 ) {
			if( first_time )
				sprintf(tmp,"%d:%d",i,ptr[i]);
			else
				sprintf(tmp,",%d:%d",i,ptr[i]);
			strcat(buf,tmp);
			first_time = 0;
		}
	}

	e = ESTADO_GET();
	switch(e) {
	case PLAYER_STATUS_FICHAS:
		net_printf(g_game.fd,TOKEN_FICHAS"=%s\n",buf);
		ESTADO_SET( PLAYER_STATUS_POSTFICHAS);
		break;
	case PLAYER_STATUS_FICHAS2:
		net_printf(g_game.fd,TOKEN_FICHAS2"=%s\n",buf);
		ESTADO_SET( PLAYER_STATUS_POSTFICHAS2);
		break;
	case PLAYER_STATUS_FICHASC:
		net_printf(g_game.fd,TOKEN_FICHASC"=%s\n",buf);
		ESTADO_SET( PLAYER_STATUS_POSTFICHASC);
		break;
	default:
		textmsg( M_ERR,_("Error, its not the moment to send your armies"));
		return TEG_STATUS_ERROR;
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS fichas_enter( PCOUNTRY p )
{
	if( fichas_check() != TEG_STATUS_SUCCESS ) {
		return TEG_STATUS_ERROR;
	}

	if( p->numjug == WHOAMI() ) {
		if( fichas_tot >= wanted_tot )  {
			if(!( p->selected & COUNTRY_SELECT_FICHAS_OUT )) {
				p->selected &= ~COUNTRY_SELECT_FICHAS_IN;
				p->selected |= COUNTRY_SELECT_FICHAS_OUT;
				gui_country_select( p->id );
			}
		} else {
			if(!( p->selected & COUNTRY_SELECT_FICHAS_IN )) {
				p->selected &= ~COUNTRY_SELECT_FICHAS_OUT;
				p->selected |= COUNTRY_SELECT_FICHAS_IN;
				gui_country_select( p->id );
			}
		}
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS fichas_leave( PCOUNTRY p )
{
	if( fichas_check() != TEG_STATUS_SUCCESS ) {
		return TEG_STATUS_ERROR;
	}

	if( p->numjug == WHOAMI() ) {
		p->selected &= ~COUNTRY_SELECT_FICHAS_IN;
		p->selected &= ~COUNTRY_SELECT_FICHAS_OUT;
		gui_country_select( p->id );
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS fichas_get_wanted( int *cant, int *conts )
{
	*cant = wanted_tot - cont_tot(wanted_conts);
	*conts = wanted_conts;
	return TEG_STATUS_SUCCESS;
}
