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
 * @file reagrue.c
 * Contiene algunas funciones auxiliares para el manejo del estado 'ESTADO_REAGRUPE'
 */

#include "../common/net.h"

#include "fcintl.h"
#include "client.h"
#include "protocol.h"

static int country_origen = -1;
static int country_destino = -1;

/* valores que guardan lo ultimo enviado, y en caso de error se borra el ejer_reagrupe */
static int last_origen = -1;
static int last_destino = -1;
static int last_cant = -1;

TEG_STATUS reagrupe_check(void)
{
	PLAYER_STATUS e = ESTADO_GET();

	if(e>=PLAYER_STATUS_ATAQUE && e<=PLAYER_STATUS_REAGRUPE) {
		ESTADO_SET(PLAYER_STATUS_REAGRUPE);
		return TEG_STATUS_SUCCESS;
	} else {
		return TEG_STATUS_ERROR;
	}
}

TEG_STATUS reagrupe_click(PCOUNTRY p)
{
	if(reagrupe_check() != TEG_STATUS_SUCCESS) {
		textmsg(M_ERR, _("Error, It's not the time to regroup"));
		return TEG_STATUS_UNEXPECTED;
	}

	if(country_origen == -1) {
		if(p->numjug == WHOAMI()) {
			if(p->ejercitos - p->ejer_reagrupe > 1) {
				p->selected &= ~COUNTRY_SELECT_REGROUP_ENTER;
				p->selected |= COUNTRY_SELECT_REGROUP;
				gui_country_select(p->id);
				country_origen = p->id;
				textmsg(M_INF, _("Source country: '%s'. Now select the destination country"), countries_get_name(p->id));
			} else {
				textmsg(M_ERR, _("Error, '%s' doesnt have any avalaible armies to move"), countries_get_name(p->id));
				return TEG_STATUS_UNEXPECTED;
			}
		} else {
			textmsg(M_ERR, _("Error, '%s' isnt one of your countries"), countries_get_name(p->id));
			return TEG_STATUS_UNEXPECTED;
		}
	} else if(country_destino == -1) {
		if(country_origen == p->id) {
			textmsg(M_INF, _("Source country is the same as the destination. Resetting the regroup..."));
			reagrupe_reset();
			return TEG_STATUS_SUCCESS;
		}

		if(p->numjug == WHOAMI()) {
			if(countries_eslimitrofe(country_origen, p->id)) {
				p->selected &= ~COUNTRY_SELECT_REGROUP_ENTER;
				p->selected |= COUNTRY_SELECT_REGROUP;
				gui_country_select(p->id);
				country_destino = p->id;
				textmsg(M_INF, _("Destination country: '%s'. Now select the quantity of armies to move"), countries_get_name(p->id));
				gui_reagrupe(country_origen, country_destino, g_countries[country_origen].ejercitos - g_countries[country_origen].ejer_reagrupe - 1);
			} else {
				textmsg(M_ERR, _("Error, '%s' isnt frontier with '%s'"), countries_get_name(p->id), countries_get_name(country_origen));
				return TEG_STATUS_UNEXPECTED;
			}
		} else {
			textmsg(M_ERR, _("Error, '%s' isnt one of your countries"), countries_get_name(p->id));
			reagrupe_reset();
			return TEG_STATUS_UNEXPECTED;
		}
	} else {
		textmsg(M_ERR, _("Error, unexpected error in reagrupe_click(). Report this bug!"));
		reagrupe_reset();
		return TEG_STATUS_UNEXPECTED;
	}

	return TEG_STATUS_SUCCESS;
}

void reagrupe_reset(void)
{
	if(country_origen != -1) {
		g_countries[country_origen].selected &= ~COUNTRY_SELECT_REGROUP;
		gui_country_select(country_origen);
		country_origen = -1;
	}

	if(country_destino != -1) {
		g_countries[country_destino].selected &= ~COUNTRY_SELECT_REGROUP;
		gui_country_select(country_origen);
		country_destino = -1;
	}
}

void reagrupe_bigreset(void)
{
	int i;

	reagrupe_reset();

	for(i=0; i<COUNTRIES_CANT; i++) {
		g_countries[i].ejer_reagrupe = 0;
	}
	last_origen = -1;
	last_destino = -1;
	last_cant = 0;
}

TEG_STATUS reagrupe_init(void)
{
	attack_reset();

	if(reagrupe_check() != TEG_STATUS_SUCCESS) {
		textmsg(M_ERR, _("Error, you can't regroup your armies now"));
		return TEG_STATUS_ERROR;
	}
	reagrupe_reset();
	return TEG_STATUS_SUCCESS;
}

void reagrupe_set_and_save(int src, int dst, int cant)
{
	last_origen = src;
	last_destino = dst;
	last_cant = cant;
	g_countries[ dst ].ejer_reagrupe += cant;
}

TEG_STATUS reagrupe_restore_from_error(void)
{
	if(last_origen != -1 && last_destino != -1 && last_cant >= 0) {
		g_countries[ last_destino ].ejer_reagrupe -= last_cant;
		return TEG_STATUS_SUCCESS;
	} else {
		return TEG_STATUS_ERROR;
	}

}

TEG_STATUS reagrupe_out(int src, int dst, int cant)
{
	PLAYER_STATUS e;

	e = ESTADO_GET();
	if(e==PLAYER_STATUS_REAGRUPE) {
		reagrupe_reset();
		reagrupe_set_and_save(src, dst, cant);
		net_printf(g_game.fd, TOKEN_REAGRUPE"=%d,%d,%d\n", src, dst, cant);
	} else {
		textmsg(M_ERR, _("Error, you cant regroup now."));
		return TEG_STATUS_ERROR;
	}

	return TEG_STATUS_SUCCESS;
}

/**
 * @fn TEG_STATUS reagrupe_enter( PCOUNTRY p )
 * Cuando se esta reagrupando resalta los countries que se pueden reagrupar
 * @param p Pais a resaltar
 */
TEG_STATUS reagrupe_enter(PCOUNTRY p)
{
	if(reagrupe_check() != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_UNEXPECTED;
	}

	if(country_origen == -1) {
		if(p->numjug == WHOAMI()) {
			if(p->ejercitos - p->ejer_reagrupe > 1) {
				p->selected |= COUNTRY_SELECT_REGROUP_ENTER;
				gui_country_select(p->id);
			}
		}
	} else if(country_destino == -1) {
		if(p->numjug == WHOAMI()) {
			if(countries_eslimitrofe(country_origen, p->id)) {
				p->selected |= COUNTRY_SELECT_REGROUP_ENTER;
				gui_country_select(p->id);
			}
		}
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS reagrupe_leave(PCOUNTRY p)
{
	if(reagrupe_check() != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_UNEXPECTED;
	}
	if(p->selected & COUNTRY_SELECT_REGROUP_ENTER) {
		p->selected &= ~COUNTRY_SELECT_REGROUP_ENTER;
		gui_country_select(p->id);
	}
	return TEG_STATUS_SUCCESS;
}
