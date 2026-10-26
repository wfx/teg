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
 * "GUI" of the robot.
 * The robot has no GUI at all, but overriding this funtions, the robot
 * can know the status of the game
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "../common/share.h"
#include "fcintl.h"
#include "client.h"
#include "ai.h"
#include "ai_fichas.h"
#include "ai_msg.h"
#include "ai_misc.h"
#include "state.h"


namespace teg::client::callbacks
{

namespace r = ::teg::robot;

/*
 * private functions
 */
void tolowerstr(char const *n, char *converted)
{
	int i;

	if(n==NULL || converted==NULL) {
		return;
	}

	for(i=0; n[i] != '\0' ; i++) {
		converted[i] = n[i] | 0x20;
	}
}

TEG_STATUS gui_mission(ClientState& state)
{
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_textplayermsg(ClientState& state, char const *n, int num, char const *msg)
{
	char converted[sizeof(state.game.myname)+1];
	char msg_lower[strlen(msg)+1];

	memset(converted, 0, sizeof(converted));

	tolowerstr(msg, msg_lower);
	tolowerstr(_(state.game.myname), converted);

	/* a player, not a robot, sends me a message i respond */
	if(strstr(msg_lower, converted) && r::ai_findname(n) != TEG_STATUS_SUCCESS) {
		r::ai_msg(state, r::AI_MSG_ANSWER, n);
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_habilitado(ClientState& state, int numjug)
{
	client::PCPLAYER pJ;
	if(numjug != who_am_i(state)) {
		if(player_whois(state, numjug, &pJ) == TEG_STATUS_SUCCESS) {
			r::ai_msg(state, r::AI_MSG_HI, pJ->name);
		}
	} else {
		out_status(state);
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_winner(ClientState& state, int numjug, int mission)
{
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_lost(ClientState& state, int numjug)
{
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_surrender(ClientState& state, int numjug)
{
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_init(ClientState& state, int argc, char **argv)
{

	init_nls();

	r::robot_state().seed = get_int_from_dev_random();
	srand(r::robot_state().seed);

	if(!state.game.serport) {
		state.game.serport = default_server_port;
	}

	if(!strlen(state.game.sername)) {
		strcpy(state.game.sername, "localhost");
	}

	if(!strlen(state.game.myname)) {
		snprintf(state.game.myname, sizeof(state.game.myname), "%s", r::ai_name());
	}
	textmsg(state, M_IMP, _("Robot name: %s"), state.game.myname);

	r::robot_state().timeout = 1;

	r::ai_init(state);
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_main(ClientState& state)
{
	fd_set readfds;
	struct timeval tout;
	int r;

	if(teg_connect(state) != TEG_STATUS_SUCCESS) {
		textmsg(state, M_ERR, _("Robot: Error while trying to connect to server"));
		return TEG_STATUS_ERROR;
	}

	out_id(state);

	while(1) {
		tout.tv_sec =  r::robot_state().timeout;
		tout.tv_usec = 0;
		FD_ZERO(&readfds);
		FD_SET(state.game.fd, &readfds);
		r = select(state.game.fd+1, &readfds, NULL, NULL, &tout);

		/* fd input */
		if(r > 0) {
			if(client_recv(state, state.game.fd) == TEG_STATUS_CONNCLOSED) {
				return TEG_STATUS_CONNCLOSED;
			}
		}

		/* timeout */
		else if(r == 0) {
			out_loque(state);
			r::ai_msg(state, r::AI_MSG_MISC, r::ai_fetch_a_name(state));
		}

		/* error */
		else {
			perror("select()");
		}
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_disconnect(ClientState& state)
{
	exit(1);
	return TEG_STATUS_CONNCLOSED;
}

TEG_STATUS gui_connected(ClientState& state, int const *_ignored)
{
	int s = random_between(0, maximum_player_count-1);
	out_color(state, s);
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_start(ClientState& state)
{
	r::robot_state().timeout = 1;
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_status(ClientState& state)
{
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_fichas(ClientState& state, int cant, int conts)
{
	int e;

	e = client_status(state);

	r::robot_state().timeout = 1;

	switch(e) {
	case PLAYER_STATUS_FICHAS:
	case PLAYER_STATUS_FICHAS2:
	case PLAYER_STATUS_FICHASC: {

		/* averiguo el status para saber quien empezo */
		out_status(state);

		if(!r::robot_state().hubo_loque) {
			out_loque(state);
			r::robot_state().hubo_loque =1;
		}

		/* por las dudas, este chequeo no esta demas */
		if(state.game.common.countries[0].ejercitos == 0) {
			out_loque(state);
			break;
		}

		if(e == PLAYER_STATUS_FICHASC) {
			int p1, p2, p3;
			if(r::ai_puedocanje(state, &p1, &p2, &p3) == TEG_STATUS_SUCCESS) {
				if(canje_out(state, p1, p2, p3) == TEG_STATUS_SUCCESS) {
					return TEG_STATUS_SUCCESS;
				} else {
					textmsg(state, M_ERR, "Robot: Unexpected error in canje_out()");
				}
				/* else, fall through, y no hago canje */
			}
			if(r::ai_fichasc(state, cant, conts) != TEG_STATUS_SUCCESS) {
				out_countries(state);
				printf("Error in ai_fichasc(%d,%d)\n", cant, conts);
				textmsg(state, M_ERR, "Robot: Abnormal error in ai_fichasc()");
				return TEG_STATUS_ERROR;
			}
		} else if(r::ai_fichas(state, cant) != TEG_STATUS_SUCCESS) {
			out_countries(state);
			textmsg(state, M_ERR, "Robot: Abnormal error in ai_fichas()");
			return TEG_STATUS_ERROR;
		}
		r::robot_state().hubo_loque = 0;
		break;
	}
	default:
		textmsg(state, M_ERR, "Robot: Error in gui_fichas()");
		return TEG_STATUS_ERROR;
	}

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_turn(ClientState& state, PCPLAYER pJ)
{
	if(pJ->numjug == who_am_i(state)) {
		r::robot_state().timeout = 1;

		if(r::ai_turno(state) != TEG_STATUS_SUCCESS) {
			r::ai_reagrupe(state);
			out_tarjeta(state);
			out_endturn(state);
			r::robot_state().timeout = 5;
		}
	}

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_country(ClientState& state, int p)
{
	return TEG_STATUS_SUCCESS;
}


TEG_STATUS gui_tropas(ClientState& state, int src, int dst, int cant)
{
	r::ai_tropas(state, src, dst, cant);
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_reagrupe(ClientState& state, int src, int dst, int cant)
{
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_tarjeta(ClientState& state, int country_id_unused)
{
	countries_map(state.game.common, [&state](COUNTRY& country) {
		if(country.tarjeta.numjug == who_am_i(state)) {

			if(!country.tarjeta.usada && country.numjug==who_am_i(state)) {
				ejer2_out(state, country.id);
			}
		}
	});
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_dados(ClientState& state)
{
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_sensi(ClientState& state)
{
	return TEG_STATUS_SUCCESS;
}


TEG_STATUS gui_canje(ClientState& state, int cant, int p1, int p2, int p3)
{
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_attack(ClientState& state, int src, int dst)
{
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_textmsg(ClientState& state, char *astring)
{
	printf("%s\n", astring);
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_scores(ClientState& state)
{
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_country_select(ClientState& state, int country)
{
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_reconnected(ClientState& state)
{
	return TEG_STATUS_SUCCESS;
}

}
