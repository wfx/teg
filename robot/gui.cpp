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


namespace teg::client::callbacks
{

namespace r = ::teg::robot;

int robot_seed = 0;			/* variable global del robot */
int robot_timeout = 1;			/* variable que dice los segundos que espera */

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

TEG_STATUS gui_mission()
{
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_textplayermsg(char const *n, int num, char const *msg)
{
	char converted[sizeof(client::g_game.myname)+1];
	char msg_lower[strlen(msg)+1];

	memset(converted, 0, sizeof(converted));

	tolowerstr(msg, msg_lower);
	tolowerstr(_(g_game.myname), converted);

	/* a player, not a robot, sends me a message i respond */
	if(strstr(msg_lower, converted) && r::ai_findname(n) != TEG_STATUS_SUCCESS) {
		r::ai_msg(r::AI_MSG_ANSWER, n);
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_habilitado(int numjug)
{
	client::PCPLAYER pJ;
	if(numjug != WHOAMI()) {
		if(player_whois(numjug, &pJ) == TEG_STATUS_SUCCESS) {
			r::ai_msg(r::AI_MSG_HI, pJ->name);
		}
	} else {
		out_status();
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_winner(int numjug, int mission)
{
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_lost(int numjug)
{
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_surrender(int numjug)
{
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_init(int argc, char **argv)
{

	init_nls();

	robot_seed = get_int_from_dev_random();
	srand(robot_seed);

	if(!g_game.serport) {
		g_game.serport = default_server_port;
	}

	if(!strlen(g_game.sername)) {
		strcpy(g_game.sername, "localhost");
	}

	if(!strlen(g_game.myname)) {
		strncpy(g_game.myname, r::ai_name(), max_playername_length);
	}
	textmsg(M_IMP, _("Robot name: %s"), g_game.myname);

	robot_timeout = 1;

	r::ai_init();
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_main(void)
{
	fd_set readfds;
	struct timeval tout;
	int r;

	if(teg_connect() != TEG_STATUS_SUCCESS) {
		textmsg(M_ERR, _("Robot: Error while trying to connect to server"));
		return TEG_STATUS_ERROR;
	}

	out_id();

	while(1) {
		tout.tv_sec =  robot_timeout;
		tout.tv_usec = 0;
		FD_ZERO(&readfds);
		FD_SET(g_game.fd, &readfds);
		r = select(g_game.fd+1, &readfds, NULL, NULL, &tout);

		/* fd input */
		if(r > 0) {
			if(client_recv(g_game.fd) == TEG_STATUS_CONNCLOSED) {
				return TEG_STATUS_CONNCLOSED;
			}
		}

		/* timeout */
		else if(r == 0) {
			out_loque();
			r::ai_msg(r::AI_MSG_MISC, r::ai_fetch_a_name());
		}

		/* error */
		else {
			perror("select()");
		}
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_disconnect(void)
{
	exit(1);
	return TEG_STATUS_CONNCLOSED;
}

TEG_STATUS gui_connected(int const *_ignored)
{
	int s = random_between(0, maximum_player_count-1);
	out_color(s);
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_start(void)
{
	robot_timeout = 1;
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_status()
{
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_fichas(int cant, int conts)
{
	int e;

	e = ESTADO_GET();

	robot_timeout = 1;

	switch(e) {
	case PLAYER_STATUS_FICHAS:
	case PLAYER_STATUS_FICHAS2:
	case PLAYER_STATUS_FICHASC: {
		static int hubo_loque = 0;

		/* averiguo el status para saber quien empezo */
		out_status();

		if(!hubo_loque) {
			out_loque();
			hubo_loque =1;
		}

		/* por las dudas, este chequeo no esta demas */
		if(g_countries[0].ejercitos == 0) {
			out_loque();
			break;
		}

		if(e == PLAYER_STATUS_FICHASC) {
			int p1, p2, p3;
			if(r::ai_puedocanje(&p1, &p2, &p3) == TEG_STATUS_SUCCESS) {
				if(canje_out(p1, p2, p3) == TEG_STATUS_SUCCESS) {
					return TEG_STATUS_SUCCESS;
				} else {
					textmsg(M_ERR, "Robot: Unexpected error in canje_out()");
				}
				/* else, fall through, y no hago canje */
			}
			if(r::ai_fichasc(cant, conts) != TEG_STATUS_SUCCESS) {
				out_countries();
				printf("Error in ai_fichasc(%d,%d)\n", cant, conts);
				textmsg(M_ERR, "Robot: Abnormal error in ai_fichasc()");
				return TEG_STATUS_ERROR;
			}
		} else if(r::ai_fichas(cant) != TEG_STATUS_SUCCESS) {
			out_countries();
			textmsg(M_ERR, "Robot: Abnormal error in ai_fichas()");
			return TEG_STATUS_ERROR;
		}
		hubo_loque = 0;
		break;
	}
	default:
		textmsg(M_ERR, "Robot: Error in gui_fichas()");
		return TEG_STATUS_ERROR;
	}

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_turn(PCPLAYER pJ)
{
	if(pJ->numjug == WHOAMI()) {
		robot_timeout = 1;

		if(r::ai_turno() != TEG_STATUS_SUCCESS) {
			r::ai_reagrupe();
			out_tarjeta();
			out_endturn();
			robot_timeout = 5;
		}
	}

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_country(int p)
{
	return TEG_STATUS_SUCCESS;
}


TEG_STATUS gui_tropas(int src, int dst, int cant)
{
	r::ai_tropas(src, dst, cant);
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_reagrupe(int src, int dst, int cant)
{
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_tarjeta(int country_id_unused)
{
	countries_map([](COUNTRY& country) {
		if(country.tarjeta.numjug == WHOAMI()) {

			if(!country.tarjeta.usada && country.numjug==WHOAMI()) {
				ejer2_out(country.id);
			}
		}
	});
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_dados()
{
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_sensi()
{
	return TEG_STATUS_SUCCESS;
}


TEG_STATUS gui_canje(int cant, int p1, int p2, int p3)
{
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_attack(int src, int dst)
{
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_textmsg(char *astring)
{
	printf("%s\n", astring);
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_scores(void)
{
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_country_select(int country)
{
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_reconnected()
{
	return TEG_STATUS_SUCCESS;
}

}
