/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <riq@corest.com>
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
/* functions that sends tokens to the server */
#include <string.h>
#include <stdio.h>

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif /* HAVE_CONFIG_H */

#include "fcintl.h"
#include "net.h"
#include "client.h"
#include "protocol.h"

namespace teg::client
{

TEG_STATUS out_mensaje(char *msg)
{
	if(ESTADO_MENOR(PLAYER_STATUS_CONNECTED)) {
		return TEG_STATUS_NOTCONNECTED;
	}

	strip_invalid_msg(msg);
	net_printf(g_game.fd, TOKEN_MESSAGE"=\"%s\"\n", msg);
	return TEG_STATUS_SUCCESS;
}

/* tells the server who am i */
TEG_STATUS out_id()
{
	if(ESTADO_MAYOR_IGUAL(PLAYER_STATUS_CONNECTED)) {
		return TEG_STATUS_ERROR;
	}

	strip_invalid(g_game.myname);
	net_printf(g_game.fd, TOKEN_PLAYERID"=%s,%d,%d\n", g_game.myname, !g_game.observer, g_game.human);
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS out_color(int color)
{
	if(ESTADO_MAYOR_IGUAL(PLAYER_STATUS_HABILITADO)) {
		return TEG_STATUS_ERROR;
	}

	net_printf(g_game.fd, TOKEN_COLOR"=%d\n", color);
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS out_exit()
{
	if(ESTADO_MENOR(PLAYER_STATUS_CONNECTED)) {
		return TEG_STATUS_NOTCONNECTED;
	}

	net_printf(g_game.fd, TOKEN_EXIT"\n");
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS out_countries()
{
	if(ESTADO_MENOR(PLAYER_STATUS_CONNECTED)) {
		textmsg(M_ERR, _("Error, you must be connected"));
		return TEG_STATUS_NOTCONNECTED;
	}

	net_printf(g_game.fd, TOKEN_COUNTRIES"=-1\n");

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS out_enum_cards()
{
	if(ESTADO_MENOR(PLAYER_STATUS_CONNECTED)) {
		textmsg(M_ERR, _("Error, you must be connected"));
		return TEG_STATUS_NOTCONNECTED;
	}

	net_printf(g_game.fd, TOKEN_ENUM_CARDS"\n");

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS out_new_round()
{
	if(ESTADO_MENOR(PLAYER_STATUS_CONNECTED)) {
		textmsg(M_ERR, _("Error, you must be connected"));
		return TEG_STATUS_NOTCONNECTED;
	}

	net_printf(g_game.fd, TOKEN_NEW_ROUND"\n");

	return TEG_STATUS_SUCCESS;
}

/* request the server the type of game */
TEG_STATUS out_status()
{
	if(ESTADO_MENOR(PLAYER_STATUS_CONNECTED)) {
		return TEG_STATUS_NOTCONNECTED;
	}
	net_printf(g_game.fd, TOKEN_STATUS"\n");
	return TEG_STATUS_SUCCESS;
}

/* start the game */
TEG_STATUS out_start()
{
	if(ESTADO_ES(PLAYER_STATUS_HABILITADO)) {
		net_printf(g_game.fd, TOKEN_START"\n");
		return TEG_STATUS_SUCCESS;
	}
	return TEG_STATUS_NOTCONNECTED;
}

TEG_STATUS out_tarjeta()
{
	if(ESTADO_MENOR(PLAYER_STATUS_ATAQUE) || ESTADO_MAYOR_IGUAL(PLAYER_STATUS_TARJETA)) {
		textmsg(M_ERR, _("Error, it's not the moment to get a card"));
		return TEG_STATUS_ERROR;
	}

	net_printf(g_game.fd, TOKEN_TARJETA"\n");
	return TEG_STATUS_SUCCESS;
}

/* send the server the armies I'm moving after conquering a country */
TEG_STATUS out_tropas(int src, int dst, int cant)
{
	PLAYER_STATUS e;

	e = ESTADO_GET();
	if(e==PLAYER_STATUS_TROPAS || e==PLAYER_STATUS_ATAQUE) {
		net_printf(g_game.fd, TOKEN_TROPAS"=%d,%d,%d\n", src, dst, cant);
	} else {
		return TEG_STATUS_ERROR;
	}

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS out_endturn()
{
	PLAYER_STATUS e;

	e = ESTADO_GET();
	if(e > PLAYER_STATUS_TURNOSTART && e < PLAYER_STATUS_TURNOEND) {
		attack_reset();
		reagrupe_reset();
		ESTADO_SET(PLAYER_STATUS_IDLE);
		net_printf(g_game.fd, TOKEN_TURNO"\n");
	} else {
		textmsg(M_ERR, _("Error, it's not your turn."));
		return TEG_STATUS_ERROR;
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS out_surrender()
{
	PLAYER_STATUS e;

	e = ESTADO_GET();
	if(e >= PLAYER_STATUS_START) {
		net_printf(g_game.fd, TOKEN_SURRENDER"\n");
		return TEG_STATUS_SUCCESS;
	}
	return TEG_STATUS_ERROR;
}

TEG_STATUS out_missions()
{
	PLAYER_STATUS e;

	e = ESTADO_GET();
	if(e >= PLAYER_STATUS_START) {
		net_printf(g_game.fd, TOKEN_MISSION"\n");
	} else {
		textmsg(M_ERR, _("Error, the game is not started"));
		return TEG_STATUS_ERROR;
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS out_get_typeofgame()
{
	PLAYER_STATUS e;

	e = ESTADO_GET();
	if(e >= PLAYER_STATUS_START) {
		net_printf(g_game.fd, TOKEN_MODALIDAD"\n");
	} else {
		textmsg(M_ERR, _("Error, the game is not started"));
		return TEG_STATUS_ERROR;
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS out_set_typeofgame(int conqworld, int fog_of_war, int with_common, int armies1, int armies2)
{
	PLAYER_STATUS e;

	e = ESTADO_GET();
	if(e >= PLAYER_STATUS_START) {
		textmsg(M_ERR, _("Error, the game is started so you cant change the type of game"));
		return TEG_STATUS_ERROR;
	} else {
		net_printf(g_game.fd, "%s=%s=%d;%s=%s=%d;%s=%s=%d;%s=%s=%d,%d\n",
		           TOKEN_SET, OPTION_CONQWORLD, conqworld,
		           TOKEN_SET, OPTION_FOG_OF_WAR, fog_of_war,
		           TOKEN_SET, OPTION_CMISSION, with_common,
		           TOKEN_SET, OPTION_ARMIES, armies1, armies2
		          );

	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS out_loque(void)
{
	PLAYER_STATUS e;

	e = ESTADO_GET();
	if(e >= PLAYER_STATUS_CONNECTED) {
		net_printf(g_game.fd, TOKEN_LOQUE"\n");
		return TEG_STATUS_SUCCESS;
	}
	return TEG_STATUS_ERROR;
}

/* ask for scores */
TEG_STATUS out_scores()
{
	PLAYER_STATUS e = ESTADO_GET();

	if(e >= PLAYER_STATUS_CONNECTED &&  g_game.fd > 0) {
		net_printf(g_game.fd, TOKEN_SCORES"\n");
		return TEG_STATUS_SUCCESS;
	}
	return TEG_STATUS_ERROR;
}

/* request a robot */
TEG_STATUS out_robot()
{
	PLAYER_STATUS e = ESTADO_GET();

	if(e >= PLAYER_STATUS_CONNECTED && e < PLAYER_STATUS_START && g_game.fd > 0) {
		net_printf(g_game.fd, TOKEN_ROBOT"\n");
		return TEG_STATUS_SUCCESS;
	}
	return TEG_STATUS_ERROR;
}

}
