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
/*
 * manages the client side of the attack
 */

#include "protocol.h"
#include "net.h"
#include "fcintl.h"
#include "client.h"

/// \internal The origin of the ongoing attack
static int country_origen = -1;

//// \internal The target of the current attack
static int country_destino = -1;

/// \internal Copy of the origin country, used for attack replay (Ctrl-R)
static int country_origen_bak = -1;

/// \internal Copy of the target country, used for attack replay
static int country_destino_bak = -1;

/// \internal Used in the gnome gui only as a helper to show ongoing attacks
static int show_src = -1;

/// \internal Used in the gnome gui only as a helper to show ongoing attacks
static int show_dst = -1;

/* are the necesary conditions met for the attack ? */
static TEG_STATUS attack_check()
{
	PLAYER_STATUS e;

	e = ESTADO_GET();

	if(e==PLAYER_STATUS_ATAQUE || e==PLAYER_STATUS_TROPAS) {
		ESTADO_SET(PLAYER_STATUS_ATAQUE);
		return TEG_STATUS_SUCCESS;
	} else {
		return TEG_STATUS_ERROR;
	}
}

/* reset the status of the attack */
void attack_reset()
{
	if(country_origen != -1) {
		g_countries[country_origen].selected &= ~COUNTRY_SELECT_ATTACK;
		gui_country_select(country_origen);
		country_origen = -1;
	}

	if(country_destino != -1) {
		g_countries[country_destino].selected &= ~COUNTRY_SELECT_ATTACK;
		gui_country_select(country_destino);
		country_destino = -1;
	}
}

void attack_restore()
{
	country_origen = country_origen_bak;
	country_destino = country_destino_bak;
}

void attack_backup()
{
	country_origen_bak = country_origen;
	country_destino_bak = country_destino;
}

TEG_STATUS attack_init()
{
	if(attack_check() != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_UNEXPECTED;
	}

	attack_backup(); /// \todo This looks wrong here.
	attack_reset();

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS attack_click(PCOUNTRY p)
{
	if(attack_check() != TEG_STATUS_SUCCESS) {
		textmsg(M_ERR, _("Error, It's not the time to attack"));
		return TEG_STATUS_UNEXPECTED;
	}

	if(country_origen == -1) {
		if(p->numjug == WHOAMI()) {
			if(p->ejercitos >1) {
				p->selected &= ~COUNTRY_SELECT_ATTACK_ENTER;
				p->selected |= COUNTRY_SELECT_ATTACK;
				gui_country_select(p->id);
				country_origen = p->id;
				textmsg(M_INF, _("Source country: '%s'. Now select the destination country"), countries_get_name(p->id));
			} else {
				textmsg(M_ERR, _("Error, '%s' must have at least 2 armies"), countries_get_name(p->id));
				return TEG_STATUS_UNEXPECTED;
			}
		} else {
			textmsg(M_ERR, _("Error, '%s' isnt one of your countries"), countries_get_name(p->id));
			return TEG_STATUS_UNEXPECTED;
		}
	} else if(country_destino == -1) {
		if(country_origen == p->id) {
			textmsg(M_INF, _("Source country is the same as the destination. Resetting the attack..."));
			attack_reset();
			return TEG_STATUS_SUCCESS;
		}

		if(p->numjug != WHOAMI()) {
			if(countries_eslimitrofe(country_origen, p->id)) {
				p->selected &= ~COUNTRY_SELECT_ATTACK_ENTER;
				p->selected |= COUNTRY_SELECT_ATTACK;
				gui_country_select(p->id);
				country_destino = p->id;
				textmsg(M_INF, _("Destination country: '%s'. Attacking..."), countries_get_name(p->id));
				attack_out();
			} else {
				textmsg(M_ERR, _("Error, '%s' isnt frontier with '%s'"), countries_get_name(p->id), countries_get_name(country_origen));
				attack_reset();
				return TEG_STATUS_UNEXPECTED;
			}
		} else {
			textmsg(M_ERR, _("Error, you cant attack your own countries ('%s')"), countries_get_name(p->id));
			attack_reset();
			return TEG_STATUS_UNEXPECTED;
		}
	} else {
		attack_reset();
		textmsg(M_ERR, _("Error, unexpected error in attack_click(). Report this bug!"));
		return TEG_STATUS_UNEXPECTED;
	}

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS attack_finish(int *ori, int *dst)
{
	if(attack_check() != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_UNEXPECTED;
	}

	if(country_destino == -1 || country_origen == -1) {
		return TEG_STATUS_ERROR;
	}

	*ori = country_origen;
	*dst = country_destino;

	return TEG_STATUS_SUCCESS;
}


TEG_STATUS attack_pre_reset()
{
	if(attack_check() != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_ERROR;
	} else {
		attack_reset();
		return TEG_STATUS_SUCCESS;
	}
}


/* Sends to server the attack message */
TEG_STATUS attack_out()
{
	int src;
	int dst;

	if(attack_finish(&src, &dst) != TEG_STATUS_SUCCESS)  {
		textmsg(M_ERR, _("Error, make sure to select the countries first."));
		attack_init();
		return TEG_STATUS_ERROR;
	}

	net_printf(g_game.fd, TOKEN_ATAQUE"=%d,%d\n", src, dst);
	attack_backup();
	return TEG_STATUS_SUCCESS;
}

/* Sends to server the attack message */
TEG_STATUS attackre_out()
{
	attack_restore();
	return attack_out();
}

/* the mouse is over a country */
TEG_STATUS attack_enter(PCOUNTRY p)
{
	if(attack_check() != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_UNEXPECTED;
	}

	if(country_origen == -1) {
		if(p->numjug == WHOAMI()) {
			if(p->ejercitos >1) {
				if(!(p->selected & COUNTRY_SELECT_ATTACK_ENTER)) {
					p->selected |= COUNTRY_SELECT_ATTACK_ENTER;
					gui_country_select(p->id);
				}
			}
		}
	} else if(country_destino == -1) {
		if(p->numjug != WHOAMI()) {
			if(countries_eslimitrofe(country_origen, p->id)) {
				if(!(p->selected & COUNTRY_SELECT_ATTACK_ENTER)) {
					p->selected |= COUNTRY_SELECT_ATTACK_ENTER;
					gui_country_select(p->id);
				}
			}
		}
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS attack_leave(PCOUNTRY p)
{
	if(attack_check() != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_UNEXPECTED;
	}
	if(p->selected & COUNTRY_SELECT_ATTACK_ENTER) {
		p->selected &= ~COUNTRY_SELECT_ATTACK_ENTER;
		gui_country_select(p->id);
	}
	return TEG_STATUS_SUCCESS;
}

/* unshow the attack shown with attack_show() */
void attack_unshow()
{

	if(show_src != -1) {
		g_countries[show_src].selected &= ~COUNTRY_SELECT_ATTACK_SRC;
		gui_country_select(show_src);
		show_src = -1;
	}

	if(show_dst != -1) {
		g_countries[show_dst].selected &= ~COUNTRY_SELECT_ATTACK_DST;
		gui_country_select(show_dst);
		show_dst = -1;
	}
}

/* shows attack an attack from srt to dst */
void attack_show(int src, int dst)
{
	attack_unshow();

	if(src >= 0 && src < COUNTRIES_CANT) {
		show_src = src;
		g_countries[src].selected |= COUNTRY_SELECT_ATTACK_SRC;
		gui_country_select(src);
	}

	if(dst >= 0 && dst < COUNTRIES_CANT) {
		show_dst = dst;
		g_countries[dst].selected |= COUNTRY_SELECT_ATTACK_DST;
		gui_country_select(dst);
	}
}
