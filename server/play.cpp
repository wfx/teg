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
/*
 * Controls all the messages that receives the server
 */

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include "../common/net.h"

#include "server.h"
#include "tegdebug.h"
#include "xmlscores.h"
#include "fow.h"
#include "fcintl.h"
#include "parser.h"
#include "missions.h"

#undef DEBUG_PLAY
#define PLAY_DEBUG PDEBUG

#ifdef DEBUG_PLAY
# define STATIC
#else
# define STATIC static
#endif


namespace teg::server
{

STATIC TEG_STATUS token_status(ServerState&, int, char *);
STATIC TEG_STATUS token_test(ServerState&, int, char *);
STATIC TEG_STATUS token_sversion(ServerState&, int, char *);
STATIC TEG_STATUS token_cversion(ServerState&, int, char *);
STATIC TEG_STATUS token_pversion(ServerState&, int, char *);
STATIC TEG_STATUS token_playerid(ServerState&, int, char *);
STATIC TEG_STATUS token_help(ServerState&, int, char *);
STATIC TEG_STATUS token_countries(ServerState&, int, char *);
STATIC TEG_STATUS token_fichas(ServerState&, int, char*);
STATIC TEG_STATUS token_fichas2(ServerState&, int, char *);
STATIC TEG_STATUS token_fichasc(ServerState&, int, char *);
STATIC TEG_STATUS token_attack(ServerState&, int, char *);
STATIC TEG_STATUS token_qumm(ServerState& state, int fd, char *);
STATIC TEG_STATUS token_rem(ServerState& state, int fd, char *);
STATIC TEG_STATUS token_message(ServerState& state, int fd, char *);
STATIC TEG_STATUS token_tropas(ServerState& state, int fd, char *str);
STATIC TEG_STATUS token_turn(ServerState& state, int fd, char *str);
STATIC TEG_STATUS token_card(ServerState& state, int fd, char *str);
STATIC TEG_STATUS token_regroup(ServerState&, int, char *);
STATIC TEG_STATUS token_canje(ServerState&, int, char*);
STATIC TEG_STATUS token_ejer2(ServerState&, int, char *);
STATIC TEG_STATUS token_mission(ServerState&, int, char *);
STATIC TEG_STATUS token_color(ServerState&, int, char*);
STATIC TEG_STATUS token_loque(ServerState&, int, char*);
STATIC TEG_STATUS token_surrender(ServerState&, int, char*);
STATIC TEG_STATUS token_set(ServerState&, int, char*);
STATIC TEG_STATUS token_scores(ServerState&, int, char*);
STATIC TEG_STATUS token_enum_cards(ServerState&, int, char*);
STATIC TEG_STATUS token_robot(ServerState&, int, char*);
STATIC TEG_STATUS token_new_round(ServerState& state, int fd, char *str);
STATIC TEG_STATUS token_typeofgame(ServerState& state, int fd, char *str);

struct {
	char const *label;
	TEG_STATUS(*func)(ServerState&, int, char*);
	char const *help;
} tokens[] = {
	{ TOKEN_START,		token_start,	N_("to start the game") },
	{ TOKEN_STATUS,		token_status,	N_("shows the status of the players") },
	{ TOKEN_MESSAGE,	token_message,	N_("to send a message") },
	{ TOKEN_EXIT,		token_exit,	N_("to exit the game") },
	{ TOKEN_TEST,		token_test,	N_("internal use. Dont use it.") },
	{ TOKEN_CVERSION,	token_cversion,	N_("client version") },
	{ TOKEN_SVERSION,	token_sversion,	N_("server version") },
	{ TOKEN_PVERSION,	token_pversion,	N_("protocol version") },
	{ TOKEN_PLAYERID,	token_playerid,	N_("to register as a player") },
	{ TOKEN_HELP,		token_help,	N_("to ask for help") },
	{ TOKEN_REM,		token_rem,	N_("to comment a command") },
	{ TOKEN_QUMM,		token_qumm,	N_("It makes you more happy") },
	{ TOKEN_COUNTRIES,		token_countries, N_("It shows info about the countries") },
	{ TOKEN_FICHAS,		token_fichas,	N_("to place the initials 5 armies") },
	{ TOKEN_FICHAS2,	token_fichas2,	N_("to place the initials 3 armies") },
	{ TOKEN_FICHASC,	token_fichasc,	N_("to place the armies after a turn have finished") },
	{ TOKEN_ATAQUE,		token_attack,	N_("to attack a country") },
	{ TOKEN_TROPAS,		token_tropas,	N_("to send armies to a conquered country") },
	{ TOKEN_TARJETA,	token_card,	N_("to pick up a country-card") },
	{ TOKEN_REAGRUPE,	token_regroup,	N_("to reorder your armies") },
	{ TOKEN_TURNO,		token_turn,	N_("to finish your turn") },
	{ TOKEN_CANJE,		token_canje,	N_("to exchange your cards for armies") },
	{ TOKEN_EJER2,		token_ejer2,	N_("to place 2 armies in a country. You must have that card") },
	{ TOKEN_MISSION,	token_mission,	N_("request a mission") },
	{ TOKEN_COLOR,		token_color,	N_("to select a color") },
	{ TOKEN_LOQUE,		token_loque,	N_("to remind me what to do") },
	{ TOKEN_SURRENDER,	token_surrender, N_("to surrender") },
	{ TOKEN_SET,		token_set,	N_("to set options") },
	{ TOKEN_SCORES,		token_scores,	N_("to show the highscores") },
	{ TOKEN_ENUM_CARDS,	token_enum_cards, N_("to show the cards a player has") },
	{ TOKEN_ROBOT,		token_robot,	N_("to play with a robot") },
	{ TOKEN_NEW_ROUND,	token_new_round, N_("to know who started the round, and the round number") },
	{ TOKEN_MODALIDAD,	token_typeofgame, N_("to know the type of game it is being played") },
};
#define	NTOKENS  (sizeof(tokens)/sizeof(tokens[0]))


/* Sets a server option */
STATIC TEG_STATUS token_set(ServerState& state, int fd, char *str)
{
	if(strlen(str)==0) {
		goto error;
	}

	if(option_parse(state, fd, str) == TEG_STATUS_SUCCESS) {
		return TEG_STATUS_SUCCESS;
	}

error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_SET "\n");
	return TEG_STATUS_PARSEERROR;
}

/* Sends the player the actions he must do. Usefull as callback */
STATIC TEG_STATUS token_loque(ServerState& state, int fd, char *unused)
{
	PSPLAYER pJ;
	PLAY_DEBUG("token_loque()");
	if(player_whoisfd(state, fd, &pJ) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	switch(pJ->estado) {
	case PLAYER_STATUS_FICHAS:
		net_printf(fd, "%s=%d,%d\n", TOKEN_FICHAS, state.game.turno->numjug, state.game.fichas);
		break;
	case PLAYER_STATUS_FICHAS2:
		net_printf(fd, "%s=%d,%d\n", TOKEN_FICHAS2, state.game.turno->numjug, state.game.fichas);
		break;
	case PLAYER_STATUS_FICHASC:
	case PLAYER_STATUS_CANJE:
		aux_token_fichasc(state, state.game.turno);
		break;
	case PLAYER_STATUS_ATAQUE:
	case PLAYER_STATUS_TROPAS:
		net_printf(fd, "%s=%d\n", TOKEN_TURNO, state.game.turno->numjug);
		break;
	default:
		break;
	}

	return TEG_STATUS_SUCCESS;
error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_LOQUE "\n");
	return TEG_STATUS_PARSEERROR;
}

/* Assigns the player a color */
STATIC TEG_STATUS token_color(ServerState& state, int fd, char *str)
{
	int color;
	int a;
	PSPLAYER pJ;
	PLAY_DEBUG("token_color()");

	if(player_whoisfd(state, fd, &pJ) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	if(strlen(str)==0) {
		goto error;
	}

	if(pJ->estado != PLAYER_STATUS_CONNECTED) {
		goto error;
	}

	if(pJ->is_player == false) {
		goto error;
	}

	a = atoi(str);
	if(a < 0 ||  static_cast<unsigned>(a) >= maximum_player_count) {
		goto error;
	}

	color = a;
	if(color_libre(state, &color)  == false) {
		goto error;
	}

	pJ->estado = PLAYER_STATUS_HABILITADO;
	pJ->color = color;
	con_text_out(state, M_INF, _("Player %s(%d) has color %s\n"), pJ->name, pJ->numjug, _(color_name(color)));

	netall_printf(state, "%s=%s,%d,%d\n", TOKEN_NEWPLAYER, pJ->name, pJ->numjug, pJ->color);
	return TEG_STATUS_SUCCESS;
error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_COLOR "\n");
	return TEG_STATUS_PARSEERROR;
}


/* The player is asking a mission */
STATIC TEG_STATUS token_mission(ServerState& state, int fd, char *unused)
{
	PSPLAYER pJ;
	PLAY_DEBUG("token_mission()");

	if(player_whoisfd(state, fd, &pJ) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	if(pJ->estado < PLAYER_STATUS_START) {
		goto error;
	}

	if(mission_asignar(state, pJ) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	net_printf(fd, "%s=%d\n", TOKEN_MISSION, pJ->mission);
	return TEG_STATUS_SUCCESS;
error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_MISSION "\n");
	return TEG_STATUS_PARSEERROR;
}

/* Called when a player finish his turn */
STATIC TEG_STATUS token_turn(ServerState& state, int fd, char *unused)
{
	PSPLAYER pJ;
	PLAY_DEBUG("token_turn()");

	if(player_whoisfd(state, fd, &pJ) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	if(pJ->estado < PLAYER_STATUS_TURNOSTART || pJ->estado > PLAYER_STATUS_TURNOEND) {
		goto error;
	}

	if(pJ != state.game.turno) {
		con_text_out(state, M_ERR, _("BUG: The server believes that player `%s' does not have the turn"), pJ->name);
		goto error;
	}

	pJ->estado = PLAYER_STATUS_IDLE;

	if(turno_end(state, pJ) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	/* give turn to the next player */
	if(turno_next(state) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	return TEG_STATUS_SUCCESS;
error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_TURNO "\n");
	return TEG_STATUS_PARSEERROR;
}

/* Sends all the player a message. An observer may use this function */
STATIC TEG_STATUS token_message(ServerState& state, int fd, char *msg)
{
	PSPLAYER j;
	PLAY_DEBUG("token_message()\n");

	if(player_whoisfd(state, fd, &j)!=TEG_STATUS_SUCCESS || strlen(msg)==0) {
		goto error;
	}

	strip_invalid_msg(msg);

	if(j->is_player) {
		netall_printf(state, "%s=%s,%d,\"%s\"\n", TOKEN_MESSAGE, j->name, j->numjug, msg);
	} else {
		netall_printf(state, "%s=observer-%s,%d,\"%s\"\n", TOKEN_MESSAGE, j->name, j->numjug, msg);
	}
	return TEG_STATUS_SUCCESS;
error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_MESSAGE "\n");
	return TEG_STATUS_PARSEERROR;
}

/* mostly-Useless function */
STATIC TEG_STATUS token_rem(ServerState& state, int fd, char *unused)
{
	PLAY_DEBUG("token_rem()\n");

	if(!SPLAYER_HABILITADO(state, fd)) {
		goto error;
	}

	net_printf(fd, "%s=%s\n", TOKEN_REM, _("Para que me envias un rem?"));
	return TEG_STATUS_SUCCESS;
error:
	return TEG_STATUS_PARSEERROR;
}

/* I want a better world "Quiero Un Mundo Mejor" */
STATIC TEG_STATUS token_qumm(ServerState& state, int fd, char *str)
{
	PLAY_DEBUG("token_qumm()\n");

	if(!SPLAYER_HABILITADO(state, fd)) {
		goto error;
	}

	net_printf(fd, "%s=%s\n", TOKEN_REM, _("Yo tambien quiero un mundo mejor!"));
	return TEG_STATUS_SUCCESS;
error:
	return TEG_STATUS_PARSEERROR;
}

/* Creates a Player */
STATIC TEG_STATUS token_playerid(ServerState& state, int fd, char *str)
{
	PARSER p{str};
	SPLAYER j, *pJ;
	char c[maximum_player_count];
	char colores[100];
	std::size_t i;
	int reconnect = false;

	PLAY_DEBUG("token_playerid( fd=%d)\n", fd);

	/* si existe entonces da error, porque no tiene que existir */
	if(player_whoisfd(state, fd, &pJ) == TEG_STATUS_SUCCESS) {
		goto error;
	}

	if(strlen(str)==0) {
		goto error;
	}

	memset(&j, 0, sizeof(SPLAYER));

	/* averigua el name */
	if(p.parse_fragment()) {
		player_fillname(state, &j, p.token);
	} else {
		goto error;
	}

	if(p.parse_fragment()) {
		j.is_player = atoi(p.token);
	} else {
		goto error;
	}

	if(p.parse_everything()) {
		j.human = atoi(p.token);
	} else {
		goto error;
	}


	if(j.is_player) {
		if(game_started(state)) {
			if(!(reconnect = player_is_disconnected(state, &j))) {
				net_print(fd, TOKEN_GAMEINPROGRESS"\n");
				fd_remove(state, fd);
				return TEG_STATUS_CONNCLOSED;
			}
		}
		if(reconnect) {
			pJ = player_return_disconnected(state, &j);
		} else {
			pJ = player_ins_player(state, &j);
		}
	} else {
		pJ = player_ins_ro(state, &j);
	}


	if(pJ == NULL) {
		net_print(fd, TOKEN_SERVERFULL "\n");
		fd_remove(state, fd);
		return TEG_STATUS_CONNCLOSED;
	}

	pJ->fd = fd;

	aux_find_inaddr(state, pJ);

	if(reconnect) {

		pJ->estado = pJ->status_before_discon;

		net_printf(fd, "%s=%s,%d,%d\n", TOKEN_RECONNECT, pJ->name, pJ->numjug, pJ->color);
		con_text_out(state, M_INF, _("Player %s(%d) is re-connected from %s\n"), pJ->name, pJ->numjug, pJ->addr);

	} else {
		colores_libres(state, c);
		memset(colores, 0, sizeof(colores));

		size_t used = 0;
		for(i=0; i<maximum_player_count; i++) {
			char buf[100];
			snprintf(buf, sizeof(buf), ",%d", c[i]);
			size_t const len = strlen(buf);
			if(used + len < sizeof(colores)) {
				memcpy(&colores[used], buf, len + 1);
				used += len;
			}
		}

		net_printf(fd, "%s=%s,%d%s\n", TOKEN_PLAYERID, pJ->name, pJ->numjug, colores);

		con_text_out(state, M_INF, _("Player %s(%d) is connected from %s\n"), pJ->name, pJ->numjug, pJ->addr);
	}
	return TEG_STATUS_SUCCESS;
error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_PLAYERID "\n");
	return TEG_STATUS_PARSEERROR;
}

STATIC TEG_STATUS token_cversion(ServerState& state, int fd, char *str)
{
	con_text_out(state, M_INF, _("Using client version: %s\n"), str);
	return TEG_STATUS_SUCCESS;
}

/* Parses the armies the player must place for the 1st time */
STATIC TEG_STATUS token_fichas(ServerState& state, int fd, char *str)
{
	PLAY_DEBUG("token_fichas()\n");

	if(!SPLAYER_FICHAS(state, fd)) {
		goto error;
	}

	if(aux_token_fichas(state, fd, str, state.game.fichas, 0) == TEG_STATUS_SUCCESS) {

		state.game.turno->estado = PLAYER_STATUS_POSTFICHAS;

		fichas_next(state);
		return TEG_STATUS_SUCCESS;
	}

error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_FICHAS "\n");
	return TEG_STATUS_PARSEERROR;
}

/* Parses the armies the player must place for the 2nd time */
STATIC TEG_STATUS token_fichas2(ServerState& state, int fd, char *str)
{
	PLAY_DEBUG("token_fichas2()\n");

	if(!SPLAYER_FICHAS2(state, fd)) {
		goto error;
	}

	if(aux_token_fichas(state, fd, str, state.game.fichas2, 0) == TEG_STATUS_SUCCESS) {

		state.game.turno->estado = PLAYER_STATUS_POSTFICHAS2;

		fichas2_next(state);
		return TEG_STATUS_SUCCESS;
	}
error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_FICHAS2 "\n");
	return TEG_STATUS_PARSEERROR;
}

/* Parses the armies the player must place the rest of the times */
STATIC TEG_STATUS token_fichasc(ServerState& state, int fd, char *str)
{
	PSPLAYER pJ;
	int total_armies;
	PLAY_DEBUG("token_fichasc()\n");

	if(player_whoisfd(state, fd, &pJ) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	if(pJ->estado != PLAYER_STATUS_FICHASC && pJ->estado != PLAYER_STATUS_CANJE) {
		goto error;
	}

	total_armies = pJ->fichasc_armies + cont_tot(state.game.common, pJ->fichasc_conts);

	if(pJ->hizo_canje) {
		total_armies += cards_for_this_exchange(pJ->tot_exchanges);
	}

	if(aux_token_fichas(state, fd, str, total_armies, pJ->fichasc_conts) == TEG_STATUS_SUCCESS) {

		pJ->estado = PLAYER_STATUS_IDLE;

		fichasc_next(state);

		pJ->hizo_canje = false;
		pJ->fichasc_armies = 0;
		pJ->fichasc_conts = 0;

		return TEG_STATUS_SUCCESS;
	}

error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_FICHASC "\n");
	return TEG_STATUS_PARSEERROR;
}

/* a player request to play with a robot. the robot will be launched in the server-side */
STATIC TEG_STATUS token_robot(ServerState& state, int fd, char *str)
{
	PSPLAYER pJ;
	int newfd;

	PLAY_DEBUG("token_robot()\n");

	if(player_whoisfd(state, fd, &pJ) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	if(! pJ->is_player) {
		goto error;
	}

	if(game_started(state)) {
		goto error;
	}

	if(launch_robot(state, &newfd, "--connected") != TEG_STATUS_SUCCESS) {
		goto error;
	}

	fd_add(state, newfd);

	return TEG_STATUS_SUCCESS;

error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_ROBOT "\n");
	return TEG_STATUS_PARSEERROR;
}


/* A player is attacking from src to dst */
STATIC TEG_STATUS token_attack(ServerState& state, int fd, char *str)
{
	PARSER p{str};
	int src, dst, src_lost, dst_lost;
	char d_src[3], d_dst[3];
	PSPLAYER pJ_src, pJ_dst;
	int conq = 0;
	int tropas = 0;
	char buffer[4096];

	PLAY_DEBUG("token_attack()\n");

	if(strlen(str)==0) {
		goto error;
	}

	if(!SPLAYER_ATAQUE_P(state, fd, &pJ_src)) {
		if(SPLAYER_TROPAS_P(state, fd, &pJ_src)) {
			pJ_src->estado=PLAYER_STATUS_ATAQUE;
			pJ_src->country_src = pJ_src->country_dst = -1;
		} else {
			goto error;
		}
	}

	if(p.parse_fragment()) {
		src = atoi(p.token);
	} else {
		goto error;
	}

	if(p.parse_everything()) {
		dst = atoi(p.token);
	} else {
		goto error;
	}

	if(src >= static_cast<int>(COUNTRIES_CANT) || src < 0 || dst >= static_cast<int>(COUNTRIES_CANT) || dst < 0) {
		goto error;
	}

	if(pJ_src->numjug != state.game.common.countries[src].numjug || pJ_src->numjug == state.game.common.countries[dst].numjug)  {
		goto error;
	}

	if(state.game.common.countries[src].ejercitos < 2 || !countries_eslimitrofe(src, dst)) {
		goto error;
	}

	if(player_whois(state, state.game.common.countries[dst].numjug, &pJ_dst) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	/* aviso a todos que hay un attack */
	if(! state.game.fog_of_war) {
		netall_printf(state, "%s=%d,%d\n", TOKEN_ATAQUE, src, dst);
	} else {
		fow_2_netall_printf(state, src, dst, "%s=%s,%s\n", TOKEN_ATAQUE, "%d", "%d");
	}

	/* so far, attack... */
	aux_token_attack(state.game.common.countries[src].ejercitos, state.game.common.countries[dst].ejercitos, &src_lost, &dst_lost, d_src, d_dst);

	state.game.common.countries[src].ejercitos -= src_lost;
	state.game.common.countries[dst].ejercitos -= dst_lost;
	pJ_src->tot_armies -= src_lost;
	pJ_dst->tot_armies -= dst_lost;

	/* updated statistics */
	pJ_src->player_stats.armies_killed += dst_lost;
	pJ_dst->player_stats.armies_killed += src_lost;

	/* conquisto el country */
	if(state.game.common.countries[dst].ejercitos == 0) {
		conq = 1;

		pJ_src->turno_conq++;
		pJ_src->tot_countries++;


		state.game.common.countries[dst].numjug = pJ_src->numjug;

		state.game.common.countries[dst].ejercitos++;		/* se pasa automaticamente */
		state.game.common.countries[src].ejercitos--;		/* un ejercito */

		tropas = state.game.common.countries[src].ejercitos - 1;	/* cantidad que se pueden pasar */
		if(tropas > 2) {		/* En verdad son 3, pero ya se le paso 1 */
			tropas =2;
		}

		pJ_src->estado = PLAYER_STATUS_TROPAS;
		pJ_src->country_src = src;
		pJ_src->country_dst = dst;

		pJ_dst->tot_countries--;

		state.game.common.countries[dst].numjug = pJ_src->numjug;

		/* updated statistics */
		pJ_src->player_stats.countries_won ++;
	}

	/* update the scores */
	stats_score(&pJ_src->player_stats, state.game.common.conts.data());
	stats_score(&pJ_dst->player_stats, state.game.common.conts.data());

	/* tell everybody the result of the attack */

	memset(buffer, 0, sizeof(buffer));

	if(! state.game.fog_of_war) {
		netall_printf(state, "%s=%d,%d,%d,%d,%d,%d,%d,%d\n", TOKEN_DADOS,
		              src, d_src[0], d_src[1], d_src[2], dst, d_dst[0], d_dst[1], d_dst[2]);
	} else {
		fow_2_netall_printf(state, src, dst, "%s=%s,%d,%d,%d,%s,%d,%d,%d\n"
		                    , TOKEN_DADOS
		                    , "%d", d_src[0], d_src[1], d_src[2]
		                    , "%d", d_dst[0], d_dst[1], d_dst[2]);
	}

	if(! state.game.fog_of_war) {
		netall_printf(state, "%s=%d,%d,%d;%s=%d,%d,%d\n",
		              TOKEN_COUNTRY, src, state.game.common.countries[src].numjug, state.game.common.countries[src].ejercitos,
		              TOKEN_COUNTRY, dst, state.game.common.countries[dst].numjug, state.game.common.countries[dst].ejercitos
		             );
	} else {
		fow_netall_printf(state, src, "%s=%d,%d,%d\n", TOKEN_COUNTRY,
		                  src, state.game.common.countries[src].numjug, state.game.common.countries[src].ejercitos);

		fow_netall_printf(state, dst, "%s=%d,%d,%d\n", TOKEN_COUNTRY,
		                  dst, state.game.common.countries[dst].numjug, state.game.common.countries[dst].ejercitos);
	}

	if(conq == 1) {

		/* Did 'dst' player lose the game ? */
		if(player_is_lost(pJ_dst)) {
			con_text_out(state, M_INF, _("Player %s(%d) lost the game\n"), pJ_dst->name, pJ_dst->numjug);
			netall_printf(state, "%s=%d\n", TOKEN_LOST, pJ_dst->numjug);
			player_poner_perdio(state, pJ_dst);
		}

		/* Did 'src' player win the game ? */
		if(mission_chequear(state, pJ_src) == TEG_STATUS_GAMEOVER || game_is_finished(state)) {
			con_text_out(state, M_INF, _("Player %s(%d) is the winner! Game Over\n"), pJ_src->name, pJ_src->numjug);
			pJ_src->estado = PLAYER_STATUS_GAMEOVER;
			game_end(state, pJ_src);
			return TEG_STATUS_SUCCESS;
		}

		net_printf(fd, "%s=%d,%d,%d\n", TOKEN_TROPAS, src, dst, tropas);


		/* in FOW show the boundaries countries */
		if(state.game.fog_of_war) {
			char buffer[2048];

			memset(buffer, 0, sizeof(buffer));
			if(fow_fill_with_boundaries(state, dst, buffer, sizeof(buffer)) == TEG_STATUS_SUCCESS) {
				net_printf(fd, "%s\n", buffer);
			}
		}
	}

	return TEG_STATUS_SUCCESS;
error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_ATAQUE "\n");
	return TEG_STATUS_PARSEERROR;
}

/* Place armies in the conquered country */
STATIC TEG_STATUS token_tropas(ServerState& state, int fd, char *str)
{
	PARSER p{str};
	int src, dst, cant;
	PSPLAYER pJ;

	PLAY_DEBUG("token_tropas()\n");

	if(!SPLAYER_TROPAS_P(state, fd, &pJ) || strlen(str)==0) {
		goto error;
	}

	if(p.parse_fragment()) {
		src = atoi(p.token);
	} else {
		goto error;
	}

	if(p.parse_fragment()) {
		dst = atoi(p.token);
	} else {
		goto error;
	}

	if(p.parse_everything()) {
		cant = atoi(p.token);
	} else {
		goto error;
	}

	if(src >= static_cast<int>(COUNTRIES_CANT) || src < 0 || dst >= static_cast<int>(COUNTRIES_CANT) || dst < 0) {
		goto error;
	}

	if(src != pJ->country_src || dst != pJ->country_dst) {
		goto error;
	}

	if(cant > state.game.common.countries[src].ejercitos-1 || cant > 3) {
		goto error;
	}

	if(cant > 0) {

		state.game.common.countries[src].ejercitos -= cant;
		state.game.common.countries[dst].ejercitos += cant;

		pJ->estado=PLAYER_STATUS_ATAQUE;
		pJ->country_src = pJ->country_dst = -1;

		if(! state.game.fog_of_war)
			netall_printf(state, "%s=%d,%d,%d;%s=%d,%d,%d\n",
			              TOKEN_COUNTRY, src, state.game.common.countries[src].numjug, state.game.common.countries[src].ejercitos,
			              TOKEN_COUNTRY, dst, state.game.common.countries[dst].numjug, state.game.common.countries[dst].ejercitos
			             );
		else {
			fow_netall_printf(state, src, "%s=%d,%d,%d\n",
			                  TOKEN_COUNTRY, src, state.game.common.countries[src].numjug, state.game.common.countries[src].ejercitos);
			fow_netall_printf(state, dst, "%s=%d,%d,%d\n",
			                  TOKEN_COUNTRY, dst, state.game.common.countries[dst].numjug, state.game.common.countries[dst].ejercitos);
		}
	}

	return TEG_STATUS_SUCCESS;
error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_TROPAS "\n");
	return TEG_STATUS_PARSEERROR;
}

/* Ask for a card after the attack */
STATIC TEG_STATUS token_card(ServerState& state, int fd, char *str)
{
	PSPLAYER pJ;
	PCOUNTRY pP;

	PLAY_DEBUG("token_card()\n");


	/* Veo si puede sacar una tarjeta... */
	if(player_whoisfd(state, fd, &pJ) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	if(pJ->estado < PLAYER_STATUS_TURNOSTART || pJ->estado >= PLAYER_STATUS_TARJETA) {
		goto error;
	}

	if(pJ->tot_cards >= static_cast<int>(maximum_country_cards)) {
		goto error;
	}

	if(pJ->turno_conq < 1) {
		goto error;
	}

	if(pJ->tot_exchanges > 3 && pJ->turno_conq < 2) {
		goto error;
	}


	/* Puede sacar tarjeta */
	pJ->tot_cards++;
	pP = get_random_country(state, [&state](int i) {
		return tarjeta_es_libre(state.game.common, i);
	});

	pJ->estado = PLAYER_STATUS_TARJETA;

	tarjeta_sacar(&pP->tarjeta, pJ->numjug);

	/*
	 * Me fijo si el player es dueño del país que dice la tarjeta. Si es así
	 * le agrego 2 fichas automaticamente como dice el reglamento.
	 */
	if(pP->numjug == pJ->numjug) {
		pP->ejercitos += 2;
		pJ->tot_armies += 2;
		tarjeta_usar(&pP->tarjeta);
		if(! state.game.fog_of_war) {
			netall_printf(state, "%s=%d,%d,%d\n", TOKEN_COUNTRY, pP->id, pP->numjug, pP->ejercitos);
		} else {
			fow_netall_printf(state, pP->id, "%s=%d,%d,%d\n", TOKEN_COUNTRY, pP->id, pP->numjug, pP->ejercitos);
		}
	}
	net_printf(fd, "%s=%d,%d\n", TOKEN_TARJETA, pP->id, pP->tarjeta.usada);

	return TEG_STATUS_SUCCESS;
error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_TARJETA "\n");
	return TEG_STATUS_ERROR;
}

/* request which cards do the players has */
STATIC TEG_STATUS token_enum_cards(ServerState& state, int fd, char *str)
{
	PSPLAYER pJ;
	int i, first;
	char buffer[1024];

	PLAY_DEBUG("token_enum_cards()\n");


	if(player_whoisfd(state, fd, &pJ) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	memset(buffer, 0, sizeof(buffer));

	first=1;
	for(i=0; i<static_cast<int>(COUNTRIES_CANT); i++) {
		if(state.game.common.countries[i].tarjeta.numjug == pJ->numjug) {
			char buffy[512];

			memset(buffy, 0, sizeof(buffy));

			if(first) {
				snprintf(buffy, sizeof(buffy)-1, "%d:%d", i, state.game.common.countries[i].tarjeta.usada);
				strncat(buffer, buffy, sizeof(buffer)-1);
				first = 0;
			} else {
				snprintf(buffy, sizeof(buffy)-1, ",%d:%d", i, state.game.common.countries[i].tarjeta.usada);
				strncat(buffer, buffy, sizeof(buffer)-1);
			}
		}
	}

	net_printf(fd, "%s=%s\n", TOKEN_ENUM_CARDS, buffer);

	return TEG_STATUS_SUCCESS;
error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_ENUM_CARDS "\n");
	return TEG_STATUS_ERROR;
}

/* request who started the round, and which round number */
STATIC TEG_STATUS token_new_round(ServerState& state, int fd, char *str)
{
	PSPLAYER pJ;

	PLAY_DEBUG("token_new_round()\n");

	if(!game_started(state)) {
		goto error;
	}

	if(player_whoisfd(state, fd, &pJ) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	net_printf(fd, "%s=%d,%d\n", TOKEN_NEW_ROUND
	           , state.game.turno->numjug 	/* who starts the new turn */
	           , state.game.round_number	/* the round number */
	          );

	return TEG_STATUS_SUCCESS;
error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_NEW_ROUND "\n");
	return TEG_STATUS_ERROR;
}

/* Place 2 armies in the card's country. The player must own the country */
STATIC TEG_STATUS token_ejer2(ServerState& state, int fd, char *str)
{
	PARSER p{str};
	int country;
	PSPLAYER j;

	PLAY_DEBUG("token_ejer2()\n");

	if(strlen(str)==0) {
		goto error;
	}

	if(player_whoisfd(state, fd, &j) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	if(j->estado != PLAYER_STATUS_TARJETA) {
		goto error;
	}

	if(p.parse_everything()) {
		country = atoi(p.token);
	} else {
		goto error;
	}

	if(country >= static_cast<int>(COUNTRIES_CANT) || country < 0) {
		goto error;
	}

	if(tarjeta_es_usada(&state.game.common.countries[ country ].tarjeta)) {
		goto error;
	}

	if(state.game.common.countries[ country ].numjug == j->numjug && state.game.common.countries[ country ].tarjeta.numjug == j->numjug) {
		state.game.common.countries[ country ].ejercitos += 2;
		j->tot_armies += 2;
		tarjeta_usar(&state.game.common.countries[ country ].tarjeta);

		if(! state.game.fog_of_war)
			netall_printf(state, "%s=%d,%d,%d\n", TOKEN_COUNTRY
			              , country, state.game.common.countries[ country ].numjug, state.game.common.countries[ country ].ejercitos);
		else
			fow_netall_printf(state, country, "%s=%d,%d,%d\n", TOKEN_COUNTRY
			                  , country, state.game.common.countries[ country ].numjug, state.game.common.countries[ country ].ejercitos);
	}

	return TEG_STATUS_SUCCESS;
error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_EJER2 "\n");
	return TEG_STATUS_PARSEERROR;
}


/* To exchange cards for armies */
STATIC TEG_STATUS token_canje(ServerState& state, int fd, char *str)
{
	PARSER p{str};
	PSPLAYER pJ;
	int t1, t2, t3;
	int canj_ejer;

	PLAY_DEBUG("token_canje()\n");

	if(player_whoisfd(state, fd, &pJ) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	if(pJ->estado != PLAYER_STATUS_FICHASC) {
		goto error;
	}

	if(p.parse_fragment()) {
		t1 = atoi(p.token);
	} else {
		goto error;
	}

	if(p.parse_fragment()) {
		t2 = atoi(p.token);
	} else {
		goto error;
	}

	if(p.parse_everything()) {
		t3 = atoi(p.token);
	} else {
		goto error;
	}

	/* se puede hacer el canje ? */
	if(!tarjeta_puedocanje(state.game.common, pJ->numjug, t1, t2, t3)) {
		goto error;
	}

	pJ->estado = PLAYER_STATUS_CANJE;

	pJ->hizo_canje = true;
	pJ->tot_exchanges++;
	pJ->tot_cards -= 3;

	canj_ejer = cards_for_this_exchange(pJ->tot_exchanges);

	/* quitarle las tarjetas al player | mark the cards as unused*/
	tarjeta_poner(&state.game.common.countries[t1].tarjeta);
	tarjeta_poner(&state.game.common.countries[t2].tarjeta);
	tarjeta_poner(&state.game.common.countries[t3].tarjeta);


	netall_printf(state, "%s=%d,%d,%d,%d,%d\n", TOKEN_CANJE,
	              pJ->numjug, canj_ejer, t1, t2, t3);
	return TEG_STATUS_SUCCESS;
error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_CANJE "\n");
	return TEG_STATUS_PARSEERROR;
}


/* Player is regrouping its armies */
STATIC TEG_STATUS token_regroup(ServerState& state, int fd, char *str)
{
	PARSER p{str};
	int src, dst, cant;
	int ejer_disp;
	PSPLAYER pJ;

	PLAY_DEBUG("token_regroup()\n");


	if(strlen(str)==0) {
		goto error;
	}

	if(player_whoisfd(state, fd, &pJ) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	if(pJ->estado < PLAYER_STATUS_TURNOSTART || pJ->estado > PLAYER_STATUS_REAGRUPE) {
		goto error;
	}

	if(p.parse_fragment()) {
		src = atoi(p.token);
	} else {
		goto error;
	}

	if(p.parse_fragment()) {
		dst = atoi(p.token);
	} else {
		goto error;
	}

	if(p.parse_everything()) {
		cant = atoi(p.token);
	} else {
		goto error;
	}

	if(src >= static_cast<int>(COUNTRIES_CANT) || src < 0 || dst >= static_cast<int>(COUNTRIES_CANT) || dst < 0 || cant <= 0) {
		goto error;
	}

	if(state.game.common.countries[src].numjug != pJ->numjug || state.game.common.countries[dst].numjug != pJ->numjug) {
		goto error;
	}

	if(!countries_eslimitrofe(src, dst)) {
		goto error;
	}

	ejer_disp = state.game.common.countries[src].ejercitos - state.game.common.countries[src].ejer_reagrupe - 1;
	if(cant > ejer_disp) {
		goto error;
	}

	pJ->estado = PLAYER_STATUS_REAGRUPE;

	state.game.common.countries[dst].ejercitos += cant;
	state.game.common.countries[dst].ejer_reagrupe += cant;
	state.game.common.countries[src].ejercitos -= cant;

	if(! state.game.fog_of_war) {
		netall_printf(state, "%s=%d,%d,%d;%s=%d,%d,%d\n",
		              TOKEN_COUNTRY, src, state.game.common.countries[src].numjug, state.game.common.countries[src].ejercitos,
		              TOKEN_COUNTRY, dst, state.game.common.countries[dst].numjug, state.game.common.countries[dst].ejercitos);
	} else {
		fow_netall_printf(state, src, "%s=%d,%d,%d\n",
		                  TOKEN_COUNTRY, src, state.game.common.countries[src].numjug, state.game.common.countries[src].ejercitos);
		fow_netall_printf(state, dst, "%s=%d,%d,%d\n",
		                  TOKEN_COUNTRY, dst, state.game.common.countries[dst].numjug, state.game.common.countries[dst].ejercitos);
	}

	return TEG_STATUS_SUCCESS;
error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_REAGRUPE "\n");
	return TEG_STATUS_PARSEERROR;
}

/* Show the status of all the players */
STATIC TEG_STATUS token_status(ServerState& state, int fd, char *unused)
{
	char strout[PROT_MAX_LEN + max_playername_length * maximum_player_count + 200];

	PLAY_DEBUG("token_status()\n");

	if(!SPLAYER_CONNECTED(state, fd)) {
		goto error;
	}

	if(aux_token_stasta(state, strout, sizeof(strout) -1) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	net_printf(fd, "%s=%s\n", TOKEN_STATUS, strout);

	return TEG_STATUS_SUCCESS;

error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_STATUS "\n");
	return TEG_STATUS_PARSEERROR;
}

/* send the hi-scores to the player */
STATIC TEG_STATUS token_scores(ServerState& state, int fd, char *unused)
{
	char strout[PROT_MAX_LEN + max_playername_length * maximum_player_count + 200];

	PLAY_DEBUG("token_scores()\n");

	if(!SPLAYER_CONNECTED(state, fd)) {
		goto error;
	}

	if(scores_dump(state, strout, sizeof(strout) -1) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	net_printf(fd, "%s=%s\n", TOKEN_SCORES, strout);

	return TEG_STATUS_SUCCESS;

error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_SCORES "\n");
	return TEG_STATUS_PARSEERROR;
}


/* Enums the players'countries , or of all players if player is -1 */
STATIC TEG_STATUS token_countries(ServerState& state, int fd, char *str)
{
	PARSER p{str};
	int i;
	PSPLAYER pJ;
	char strout[PROT_MAX_LEN];

	PLAY_DEBUG("token_countries()\n");

	if(!game_started(state)) {
		goto error;
	}

	if(strlen(str)==0) {
		goto error;
	}

	if(p.parse_everything()) {
		i = atoi(p.token);
	} else {
		goto error;
	}

	if(i != -1 && player_whois(state, i, &pJ) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	if(i!=-1) {
		/* ask just for 1 player */
		aux_token_countries(state, pJ, strout, sizeof(strout)-1);
		net_printf(fd, "%s=%d/%s\n", TOKEN_COUNTRIES, pJ->numjug, strout);
	} else  {
		/* ask for all the players */

		player_map(state, [&state, &strout, fd](PSPLAYER pJ) {
			aux_token_countries(state, pJ, strout, sizeof(strout)-1);
			net_printf(fd, "%s=%d/%s\n", TOKEN_COUNTRIES, pJ->numjug, strout);
		});

		if(state.game.fog_of_war) {
			SPLAYER j;

			j.numjug = -1;

			aux_token_countries(state, &j, strout, sizeof(strout)-1);
			net_printf(fd, "%s=%d/%s\n", TOKEN_COUNTRIES, j.numjug, strout);
		}
	}
	return TEG_STATUS_SUCCESS;

error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_COUNTRIES "\n");
	return TEG_STATUS_PARSEERROR;
}

/* To start the game */
TEG_STATUS token_start(ServerState& state, int fd, char*)
{
	char strout[PROT_MAX_LEN + max_playername_length  * maximum_player_count + 200];
	PSPLAYER pJ;
	PLAY_DEBUG("token_start()\n");

	if(game_started(state) || state.game.players < 2) {
		goto error;
	}

	if(state.server.with_console && fd != CONSOLE_FD) {
		if(!SPLAYER_HABILITADO_P(state, fd, &pJ) || !pJ->is_player) {
			goto error;
		}
	}

	state.game.estado = JUEGO_ESTADO_EMPEZADO;

	state.game.playing = state.game.players;

	con_text_out(state, M_INF, _("Starting game number: %d with seed: %u\n"), state.game.gamenumber, state.game.seed);

	player_all_set_status(state, PLAYER_STATUS_START);
	countries_repartir(state);

	if(turno_init(state) != TEG_STATUS_SUCCESS) {
		con_text_out(state, M_ERR, _("Error, cant initilize a new turn\n"));
		goto error;
	}

	state.game.estado = JUEGO_ESTADO_FICHAS;

	state.game.turno->estado = PLAYER_STATUS_FICHAS;

	aux_token_stasta(state, strout, sizeof(strout) -1);

	netall_printf(state, "%s=%s;%s=%d,%d;%s=%d,%d,%d,%d;%s=%d,%d\n"
	              , TOKEN_START
	              , strout			/* available players */
	              , TOKEN_NEW_ROUND
	              , state.game.turno->numjug 	/* who starts the new turn */
	              , state.game.round_number	/* the round number */
	              , TOKEN_MODALIDAD
	              , state.game.mission	/* play with missions ? */
	              , state.game.cmission	/* play with common mission */
	              , state.game.fog_of_war	/* play with fog of war */
	              , state.game.reglas		/* which rules ? */
	              , TOKEN_FICHAS
	              , state.game.turno->numjug,	/* who starts ? */
	              state.game.fichas);	/* how many armies to place */
	return TEG_STATUS_SUCCESS;
error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_START "\n");
	return TEG_STATUS_PARSEERROR;
}

/* sends a player the type of game it is being played */
TEG_STATUS token_typeofgame(ServerState& state, int fd, char *str)
{
	PSPLAYER pJ;
	PLAY_DEBUG("token_typeofgame()\n");

	if(game_started(state)) {
		goto error;
	}

	if(!SPLAYER_HABILITADO_P(state, fd, &pJ) || !pJ->is_player) {
		goto error;
	}

	netall_printf(state, "%s=%d,%d,%d,%d;\n"
	              , TOKEN_MODALIDAD
	              , state.game.mission	/* play with missions ? */
	              , state.game.cmission	/* play with common mission */
	              , state.game.fog_of_war	/* play with fog of war */
	              , state.game.reglas		/* which rules ? */
	             );
	return TEG_STATUS_SUCCESS;
error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_MODALIDAD "\n");
	return TEG_STATUS_PARSEERROR;
}

/* For testing only */
STATIC TEG_STATUS token_test(ServerState& state, int fd, char *unused)
{
	PLAY_DEBUG("token_test()\n");

	return TEG_STATUS_SUCCESS;
}

/* Server version */
STATIC TEG_STATUS token_sversion(ServerState& state, int fd, char *unused)
{
	PLAY_DEBUG("token_sversion()\n");

	net_printf(fd, "%s=%s %s\n", TOKEN_SVERSION, _("TEG server version "), VERSION);
	return TEG_STATUS_SUCCESS;
}

/* Protocol version */
STATIC TEG_STATUS token_pversion(ServerState& state, int fd, char *str)
{
	PSPLAYER pJ;
	PARSER p{str};
	int hi;

	PLAY_DEBUG("token_pversion()\n");


	if(strlen(str)==0) {
		goto error;
	}

	if(p.parse_fragment()) {
		hi = atoi(p.token);
	} else {
		goto error;
	}

	if(p.parse_everything()) {
		// We don't use the this integer value
	} else {
		goto error;
	}

	net_printf(fd, "%s=%i,%i\n", TOKEN_PVERSION, PROTOCOL_HIVER, PROTOCOL_LOVER);

	if(hi != PROTOCOL_HIVER) {
		con_text_out(state, M_ERR, _("Client with incompatible protocol version (server:%d , client:%d)\n"), PROTOCOL_HIVER, hi);

		if(player_whoisfd(state, fd, &pJ) == TEG_STATUS_SUCCESS) {
			player_del_hard(state, pJ);
		} else {
			fd_remove(state, fd);
		}
		return TEG_STATUS_CONNCLOSED;
	}

	return TEG_STATUS_SUCCESS;

error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_PVERSION "\n");
	return TEG_STATUS_PARSEERROR;
}

TEG_STATUS token_exit(ServerState& state, int fd, char*)
{
	PSPLAYER pJ;
	PLAY_DEBUG("token_exit\n");

	if(player_whoisfd(state, fd, &pJ) == TEG_STATUS_SUCCESS) {
		player_del_hard(state, pJ);
	} else {
		fd_remove(state, fd);
	}

	return TEG_STATUS_CONNCLOSED;
}

/* Puts the player in Game Over state */
TEG_STATUS token_surrender(ServerState& state, int fd, char *unused)
{
	PSPLAYER pJ;
	PLAY_DEBUG("token_surrender\n");

	if(player_whoisfd(state, fd, &pJ) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	if(!pJ->is_player) {
		goto error;
	}

	if(pJ->estado < PLAYER_STATUS_HABILITADO) {
		goto error;
	}

	con_text_out(state, M_INF, _("Player %s(%d) abandoned the game\n"), pJ->name, pJ->numjug);

	netall_printf(state, "%s=%d\n", TOKEN_SURRENDER, pJ->numjug);

	player_del_soft(state, pJ);

	return TEG_STATUS_SUCCESS;
error:
	net_print(fd, TOKEN_ERROR "=" TOKEN_SURRENDER "\n");
	return TEG_STATUS_PARSEERROR;
}

/* Ask for help */
STATIC TEG_STATUS token_help(ServerState& state, int fd, char *unused)
{
	for(unsigned i=0; i<NTOKENS; i++) {
		if(tokens[i].func)
			net_printf(fd, "%s='%s' %s\n", TOKEN_REM,
			           tokens[i].label, _(tokens[i].help));
	}
	return TEG_STATUS_SUCCESS;
}

/* Parses the tokens */
STATIC TEG_STATUS token_lookup(ServerState& state, int fd, PARSER *p)
{
	for(unsigned i=0; i<NTOKENS; i++) {
		if(strcasecmp(p->token, tokens[i].label)==0) {
			if(tokens[i].func) {
				return((tokens[i].func)(state, fd, p->value));
			}
			return TEG_STATUS_TOKENNULL;
		}
	}
	PLAY_DEBUG("Token '%s' no encontrado\n", p->token);
	return TEG_STATUS_TOKENNOTFOUND;
}

/* Read the file descriptor and call the apropiate function */
TEG_STATUS play_teg(ServerState& state, int fd)
{
	if(state.game.fog_of_war) {
		state.game.player_fow = NULL;
	}

	char str[PROT_MAX_LEN];
	str[0]=0;

	if(net_readline(fd, str, PROT_MAX_LEN)<1) {
		PSPLAYER pJ;
		if(player_whoisfd(state, fd, &pJ) == TEG_STATUS_SUCCESS) {
			player_del_hard(state, pJ);
		} else {
			fd_remove(state, fd);
		}

		return TEG_STATUS_CONNCLOSED;
	}

	if(state.game.fog_of_war) {
		if(player_whoisfd(state, fd, &state.game.player_fow) != TEG_STATUS_SUCCESS) {
			state.game.player_fow = NULL;
		}
	}

	PARSER p{str, '=', ';'};

	do {
		if(!p.parse()) {
			break;
		}

		if(token_lookup(state, fd, &p) == TEG_STATUS_CONNCLOSED) {
			return TEG_STATUS_CONNCLOSED;
		}
	} while(p.can_continue());

	return TEG_STATUS_SUCCESS;
}

}
