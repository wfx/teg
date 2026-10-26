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

TEG_STATUS out_mensaje(ClientState& state, char *msg)
{
	if(status_below(state, PLAYER_STATUS_CONNECTED)) {
		return TEG_STATUS_NOTCONNECTED;
	}

	strip_invalid_msg(msg);
	state.connection->printf(TOKEN_MESSAGE"=\"%s\"\n", msg);
	return TEG_STATUS_SUCCESS;
}

/* tells the server who am i */
TEG_STATUS out_id(ClientState& state)
{
	if(status_at_least(state, PLAYER_STATUS_CONNECTED)) {
		return TEG_STATUS_ERROR;
	}

	strip_invalid(state.game.myname);
	state.connection->printf(TOKEN_PLAYERID"=%s,%d,%d\n", state.game.myname, !state.game.observer, state.game.human);
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS out_color(ClientState& state, int color)
{
	if(status_at_least(state, PLAYER_STATUS_HABILITADO)) {
		return TEG_STATUS_ERROR;
	}

	state.connection->printf(TOKEN_COLOR"=%d\n", color);
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS out_exit(ClientState& state)
{
	if(status_below(state, PLAYER_STATUS_CONNECTED)) {
		return TEG_STATUS_NOTCONNECTED;
	}

	state.connection->printf(TOKEN_EXIT"\n");
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS out_countries(ClientState& state)
{
	if(status_below(state, PLAYER_STATUS_CONNECTED)) {
		textmsg(state, M_ERR, _("Error, you must be connected"));
		return TEG_STATUS_NOTCONNECTED;
	}

	state.connection->printf(TOKEN_COUNTRIES"=-1\n");

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS out_enum_cards(ClientState& state)
{
	if(status_below(state, PLAYER_STATUS_CONNECTED)) {
		textmsg(state, M_ERR, _("Error, you must be connected"));
		return TEG_STATUS_NOTCONNECTED;
	}

	state.connection->printf(TOKEN_ENUM_CARDS"\n");

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS out_new_round(ClientState& state)
{
	if(status_below(state, PLAYER_STATUS_CONNECTED)) {
		textmsg(state, M_ERR, _("Error, you must be connected"));
		return TEG_STATUS_NOTCONNECTED;
	}

	state.connection->printf(TOKEN_NEW_ROUND"\n");

	return TEG_STATUS_SUCCESS;
}

/* request the server the type of game */
TEG_STATUS out_status(ClientState& state)
{
	if(status_below(state, PLAYER_STATUS_CONNECTED)) {
		return TEG_STATUS_NOTCONNECTED;
	}
	state.connection->printf(TOKEN_STATUS"\n");
	return TEG_STATUS_SUCCESS;
}

/* start the game */
TEG_STATUS out_start(ClientState& state)
{
	if(is_status(state, PLAYER_STATUS_HABILITADO)) {
		state.connection->printf(TOKEN_START"\n");
		return TEG_STATUS_SUCCESS;
	}
	return TEG_STATUS_NOTCONNECTED;
}

TEG_STATUS out_tarjeta(ClientState& state)
{
	if(status_below(state, PLAYER_STATUS_ATAQUE) || status_at_least(state, PLAYER_STATUS_TARJETA)) {
		textmsg(state, M_ERR, _("Error, it's not the moment to get a card"));
		return TEG_STATUS_ERROR;
	}

	state.connection->printf(TOKEN_TARJETA"\n");
	return TEG_STATUS_SUCCESS;
}

/* send the server the armies I'm moving after conquering a country */
TEG_STATUS out_tropas(ClientState& state, int src, int dst, int cant)
{
	PLAYER_STATUS e;

	e = client_status(state);
	if(e==PLAYER_STATUS_TROPAS || e==PLAYER_STATUS_ATAQUE) {
		state.connection->printf(TOKEN_TROPAS"=%d,%d,%d\n", src, dst, cant);
	} else {
		return TEG_STATUS_ERROR;
	}

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS out_endturn(ClientState& state)
{
	PLAYER_STATUS e;

	e = client_status(state);
	if(e > PLAYER_STATUS_TURNOSTART && e < PLAYER_STATUS_TURNOEND) {
		attack_reset(state);
		reagrupe_reset(state);
		set_status(state, PLAYER_STATUS_IDLE);
		state.connection->printf(TOKEN_TURNO"\n");
	} else {
		textmsg(state, M_ERR, _("Error, it's not your turn."));
		return TEG_STATUS_ERROR;
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS out_surrender(ClientState& state)
{
	PLAYER_STATUS e;

	e = client_status(state);
	if(e >= PLAYER_STATUS_START) {
		state.connection->printf(TOKEN_SURRENDER"\n");
		return TEG_STATUS_SUCCESS;
	}
	return TEG_STATUS_ERROR;
}

TEG_STATUS out_missions(ClientState& state)
{
	PLAYER_STATUS e;

	e = client_status(state);
	if(e >= PLAYER_STATUS_START) {
		state.connection->printf(TOKEN_MISSION"\n");
	} else {
		textmsg(state, M_ERR, _("Error, the game is not started"));
		return TEG_STATUS_ERROR;
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS out_get_typeofgame(ClientState& state)
{
	PLAYER_STATUS e;

	e = client_status(state);
	if(e >= PLAYER_STATUS_START) {
		state.connection->printf(TOKEN_MODALIDAD"\n");
	} else {
		textmsg(state, M_ERR, _("Error, the game is not started"));
		return TEG_STATUS_ERROR;
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS out_set_typeofgame(ClientState& state, int conqworld, int fog_of_war, int with_common, int armies1, int armies2)
{
	PLAYER_STATUS e;

	e = client_status(state);
	if(e >= PLAYER_STATUS_START) {
		textmsg(state, M_ERR, _("Error, the game is started so you cant change the type of game"));
		return TEG_STATUS_ERROR;
	} else {
		state.connection->printf("%s=%s=%d;%s=%s=%d;%s=%s=%d;%s=%s=%d,%d\n",
		                         TOKEN_SET, OPTION_CONQWORLD, conqworld,
		                         TOKEN_SET, OPTION_FOG_OF_WAR, fog_of_war,
		                         TOKEN_SET, OPTION_CMISSION, with_common,
		                         TOKEN_SET, OPTION_ARMIES, armies1, armies2
		                        );

	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS out_loque(ClientState& state)
{
	PLAYER_STATUS e;

	e = client_status(state);
	if(e >= PLAYER_STATUS_CONNECTED) {
		state.connection->printf(TOKEN_LOQUE"\n");
		return TEG_STATUS_SUCCESS;
	}
	return TEG_STATUS_ERROR;
}

/* ask for scores */
TEG_STATUS out_scores(ClientState& state)
{
	PLAYER_STATUS e = client_status(state);

	if(e >= PLAYER_STATUS_CONNECTED &&  client_fd(state) > 0) {
		state.connection->printf(TOKEN_SCORES"\n");
		return TEG_STATUS_SUCCESS;
	}
	return TEG_STATUS_ERROR;
}

/* request a robot */
TEG_STATUS out_robot(ClientState& state)
{
	PLAYER_STATUS e = client_status(state);

	if(e >= PLAYER_STATUS_CONNECTED && e < PLAYER_STATUS_START && client_fd(state) > 0) {
		state.connection->printf(TOKEN_ROBOT"\n");
		return TEG_STATUS_SUCCESS;
	}
	return TEG_STATUS_ERROR;
}

}
