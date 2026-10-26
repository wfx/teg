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
 * @file inputfd.c
 * manages the 'in' tokens
 */
#include <filesystem>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <limits>

#include "../common/net.h"
#include "../common/missions.h"

#include "parser.h"
#include "fcintl.h"
#include "protocol.h"
#include "client.h"

namespace teg::client
{

TEG_STATUS clitok_rem(ClientState& state, char *str);
TEG_STATUS clitok_status(ClientState& state, char *str);
TEG_STATUS clitok_start(ClientState& state, char *str);
TEG_STATUS clitok_playerid(ClientState& state, char *str);
TEG_STATUS clitok_newplayer(ClientState& state, char *str);
TEG_STATUS clitok_message(ClientState& state, char *str);
TEG_STATUS clitok_countries(ClientState& state, char *str);
TEG_STATUS clitok_fichas(ClientState& state, char *str);
TEG_STATUS clitok_fichas2(ClientState& state, char *str);
TEG_STATUS clitok_fichasc(ClientState& state, char *str);
TEG_STATUS clitok_turno(ClientState& state, char *str);
TEG_STATUS clitok_attack(ClientState& state, char *str);
TEG_STATUS clitok_dados(ClientState& state, char *str);
TEG_STATUS clitok_country(ClientState& state, char *str);
TEG_STATUS clitok_error(ClientState& state, char *str);
TEG_STATUS clitok_ok(ClientState& state, char *str);
TEG_STATUS clitok_tropas(ClientState& state, char *str);
TEG_STATUS clitok_tarjeta(ClientState& state, char *str);
TEG_STATUS clitok_exchange(ClientState& state, char *str);
TEG_STATUS clitok_modalidad(ClientState& state, char *str);
TEG_STATUS clitok_mission(ClientState& state, char *str);
TEG_STATUS clitok_winner(ClientState& state, char *str);
TEG_STATUS clitok_lost(ClientState& state, char *str);
TEG_STATUS clitok_exit(ClientState& state, char *str);
TEG_STATUS clitok_pversion(ClientState& state, char *str);
TEG_STATUS clitok_sversion(ClientState& state, char *str);
TEG_STATUS clitok_serverfull(ClientState& state, char * str);
TEG_STATUS clitok_surrender(ClientState& state, char *str);
TEG_STATUS clitok_started(ClientState& state, char *);
TEG_STATUS clitok_kick(ClientState& state, char *str);
TEG_STATUS clitok_scores(ClientState& state, char *str);
TEG_STATUS clitok_reconnect(ClientState& state, char *str);
TEG_STATUS clitok_enum_cards(ClientState& state, char *str);
TEG_STATUS clitok_playersscores(ClientState& state, char *str);
TEG_STATUS clitok_new_round(ClientState& state, char *str);

struct {
	char const *label;
	TEG_STATUS(*func)(ClientState&, char*);
} tokens[] = {
	{ TOKEN_REM,		clitok_rem 	},
	{ TOKEN_STATUS,		clitok_status 	},
	{ TOKEN_START,		clitok_start 	},
	{ TOKEN_PLAYERID,	clitok_playerid	},
	{ TOKEN_NEWPLAYER,	clitok_newplayer},
	{ TOKEN_MESSAGE,	clitok_message	},
	{ TOKEN_COUNTRIES,		clitok_countries	},
	{ TOKEN_FICHAS,		clitok_fichas	},
	{ TOKEN_FICHAS2,	clitok_fichas2	},
	{ TOKEN_FICHASC,	clitok_fichasc	},
	{ TOKEN_TURNO,		clitok_turno	},
	{ TOKEN_ATAQUE,		clitok_attack	},
	{ TOKEN_DADOS,		clitok_dados	},
	{ TOKEN_COUNTRY,		clitok_country	},
	{ TOKEN_ERROR,		clitok_error	},
	{ TOKEN_TROPAS,		clitok_tropas	},
	{ TOKEN_TARJETA,	clitok_tarjeta	},
	{ TOKEN_CANJE,		clitok_exchange	},
	{ TOKEN_MODALIDAD,	clitok_modalidad},
	{ TOKEN_MISSION,	clitok_mission	},
	{ TOKEN_WINNER,		clitok_winner	},
	{ TOKEN_LOST,		clitok_lost	},
	{ TOKEN_EXIT,		clitok_exit	},
	{ TOKEN_PVERSION,	clitok_pversion	},
	{ TOKEN_SVERSION,	clitok_sversion	},
	{ TOKEN_SURRENDER,	clitok_surrender},
	{ TOKEN_SERVERFULL,	clitok_serverfull},
	{ TOKEN_GAMEINPROGRESS,	clitok_started	},
	{ TOKEN_KICK,		clitok_kick	},
	{ TOKEN_SCORES,		clitok_scores	},
	{ TOKEN_RECONNECT,	clitok_reconnect},
	{ TOKEN_ENUM_CARDS,	clitok_enum_cards},
	{ TOKEN_NEW_ROUND,	clitok_new_round},

};
#define	NRCLITOKENS  (sizeof(tokens)/sizeof(tokens[0]))

struct {
	char const *label;
	TEG_STATUS(*func)(ClientState&);
} err_tokens[] = {
	{ TOKEN_FICHAS,		fichas_restore_from_error },
	{ TOKEN_FICHAS2,	fichas2_restore_from_error },
	{ TOKEN_FICHASC,	fichasc_restore_from_error },
	{ TOKEN_REAGRUPE,	reagrupe_restore_from_error },
	{ TOKEN_EJER2,		ejer2_restore_from_error },
	{ TOKEN_PLAYERID,	playerid_restore_from_error },
	{ TOKEN_START,		aux_start_error },
};
#define	NRERRCLITOKENS  (sizeof(err_tokens)/sizeof(err_tokens[0]))

/* the server is full */
TEG_STATUS clitok_serverfull(ClientState& state, char *str)
{
	textmsg(state, M_ERR, _("The server is full. Try connecting as an observer"));
	teg_disconnect(state);
	return TEG_STATUS_SUCCESS;
}

/* the game is already started. you cant join it */
TEG_STATUS clitok_started(ClientState& state, char *)
{
	textmsg(state, M_ERR,
	        _("The game has already started. Try connecting as an observer."));
	teg_disconnect(state);
	return TEG_STATUS_SUCCESS;
}

/* a player has been kicked */
TEG_STATUS clitok_kick(ClientState& state, char *name)
{
	if(name && strlen(name)) {
		textmsg(state, M_IMP, _("Player %s was kicked from the game"), name);
	}
	return TEG_STATUS_SUCCESS;
}


/* Informs that a player has surrender */
TEG_STATUS clitok_surrender(ClientState& state, char *str)
{
	int numjug;
	PCPLAYER pJ;

	numjug = atoi(str);
	if(player_whois(state, numjug, &pJ) != TEG_STATUS_SUCCESS) {
		/* no lo tengo en la base */
		textmsg(state, M_IMP, _("Player %d abandoned the game"), numjug);
		return TEG_STATUS_SUCCESS;
	}

	textmsg(state, M_IMP, _("Player %s(%s) abandoned the game"),
	        pJ->name,
	        _(color_name(pJ->color))
	       );

	if(pJ->numjug == who_am_i(state)) {
		set_status(state, PLAYER_STATUS_GAMEOVER);
		callbacks::gui_sensi(state);
	}

	out_countries(state);

	callbacks::gui_surrender(state, numjug);

	return TEG_STATUS_SUCCESS;
}

/* a players lost the connection of just quit the game */
TEG_STATUS clitok_exit(ClientState& state, char *str)
{
	int numjug;
	PCPLAYER pJ;

	numjug = atoi(str);
	if(player_whois(state, numjug, &pJ) != TEG_STATUS_SUCCESS) {
		/* no lo tengo en la base */
		textmsg(state, M_IMP, _("Player %d exit the game"), numjug);
		return TEG_STATUS_SUCCESS;
	}

	textmsg(state, M_IMP, _("Player %s(%s) exit the game"),
	        pJ->name,
	        _(color_name(pJ->color))
	       );

	/* dont delete the player, I need the status */
	/* player_del( pJ ); */

	if(who_am_i(state) == numjug) {
		/* por alguna razon el server quiere que abandone el game */
		teg_disconnect(state);
	}

	return TEG_STATUS_SUCCESS;
}

/* a player won the game */
TEG_STATUS clitok_winner(ClientState& state, char *str)
{
	PARSER p{str};
	int numjug, mission;

	p >> numjug >> mission;
	if(!p.finished()) {
		goto error;
	}

	PCPLAYER pJ;

	if(player_whois(state, numjug, &pJ) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	if(mission == -1) {
		mission = 0;
	}

	if(mission < 0 || mission >= missions_cant()) {
		goto error;
	}

	textmsg(state, M_IMP, _("Player %s(%s) is the WINNER"),
	        pJ->name,
	        _(color_name(pJ->color))
	       );

	callbacks::gui_winner(state, pJ->numjug, mission);

	set_status(state, PLAYER_STATUS_HABILITADO);
	callbacks::gui_sensi(state);

	game_finalize(state);
	return TEG_STATUS_SUCCESS;
error:
	textmsg(state, M_ERR, "Error in clitok_winner()");
	return TEG_STATUS_ERROR;
}

/* a player lost the game */
TEG_STATUS clitok_lost(ClientState& state, char *str)
{
	int numjug;
	PCPLAYER pJ;

	numjug = atoi(str);
	if(player_whois(state, numjug, &pJ) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	textmsg(state, M_IMP, _("Player %s(%s) lost the game\n"),
	        pJ->name,
	        _(color_name(pJ->color))
	       );

	if(pJ->numjug == who_am_i(state)) {
		set_status(state, PLAYER_STATUS_GAMEOVER);
	}

	callbacks::gui_lost(state, pJ->numjug);

	return TEG_STATUS_SUCCESS;
error:
	textmsg(state, M_ERR, "Error in clitok_lost()");
	return TEG_STATUS_ERROR;
}

/* you can move some armies to the conquered country */
TEG_STATUS clitok_tropas(ClientState& state, char *str)
{
	PARSER p{str};
	int src, dst, cant;

	p >> src >> dst >> cant;

	if(!p.finished()) {
		textmsg(state, M_ERR, "Error in clitok_tropas()");
		return TEG_STATUS_ERROR;
	}

	set_status(state, PLAYER_STATUS_TROPAS);

	callbacks::gui_tropas(state, src, dst, cant);

	return TEG_STATUS_SUCCESS;
}

/* your last request has an error */
TEG_STATUS clitok_error(ClientState& state, char *str)
{
	if(strlen(str)==0) {
		goto error;
	}

	for(unsigned i = 0; i < NRERRCLITOKENS; i++) {
		if(strcasecmp(str, err_tokens[i].label)==0) {
			if(err_tokens[i].func) {
				return((err_tokens[i].func)(state));
			}
			textmsg(state, M_ERR, _("The server report an error in '%s'"), str);
			return TEG_STATUS_TOKENNULL;
		}
	}
	return TEG_STATUS_SUCCESS;

error:
	textmsg(state, M_ERR, "Error in clitok_error()");
	return TEG_STATUS_ERROR;
}

/* update just one country */
TEG_STATUS clitok_country(ClientState& state, char *str)
{
	int country, jug, ejer;
	PARSER p{str};
	p >> country >> jug >> ejer;

	if(!p.finished()) {
		textmsg(state, M_ERR, "Error in clitok_country()");
		return TEG_STATUS_ERROR;
	}

	state.game.common.countries[country].numjug = jug;
	state.game.common.countries[country].ejercitos = ejer;

	callbacks::gui_country(state, country);

	return TEG_STATUS_SUCCESS;
}

/* dices result */
TEG_STATUS clitok_dados(ClientState& state, char *str)
{
	PARSER p{str};

	/* src and dst country can be -1 */

	p >> Limited{state.game.dados_srccountry, 0, COUNTRIES_CANT-1, -1}
	  >> state.game.dados_src[0] >> state.game.dados_src[1] >> state.game.dados_src[2]
	  >> Limited{state.game.dados_dstcountry, 0, COUNTRIES_CANT-1, -1}
	  >> state.game.dados_dst[0] >> state.game.dados_dst[1] >> state.game.dados_dst[2];

	if(!p.finished()) {
		textmsg(state, M_ERR, "Error in clitok_dados()");
		return TEG_STATUS_ERROR;
	}


	textmsg(state, M_INF, _("Dices: %s: %d %d %d vs. %s: %d %d %d")
	        , countries_get_name(state.game.common, state.game.dados_srccountry)
	        , state.game.dados_src[0]
	        , state.game.dados_src[1]
	        , state.game.dados_src[2]
	        , countries_get_name(state.game.common, state.game.dados_dstcountry)
	        , state.game.dados_dst[0]
	        , state.game.dados_dst[1]
	        , state.game.dados_dst[2]);

	callbacks::gui_dados(state);

	return TEG_STATUS_SUCCESS;
}

/* src is attacking dst */
TEG_STATUS clitok_attack(ClientState& state, char *str)
{
	int src, dst;
	PARSER p{str};
	PCPLAYER pJsrc, pJdst;

	p >> src >> dst;
	if(!p.finished()) {
		goto error;
	}

	/* countries are allowed to be -1 (unknown in fog of war) */
	if(src<-1 || src>=static_cast<int>(COUNTRIES_CANT) || dst<-1 || dst>=static_cast<int>(COUNTRIES_CANT)) {
		goto error;
	}

	pJsrc = NULL;
	if(src >=0) {
		player_whois(state, state.game.common.countries[src].numjug, &pJsrc);
	}

	pJdst = NULL;
	if(dst >=0) {
		player_whois(state, state.game.common.countries[dst].numjug, &pJdst);
	}

	if(src >= 0 && state.game.common.countries[src].numjug == who_am_i(state)) {
		attack_reset(state);
	} else {
		attack_show(state, src, dst);
	}

	if(pJsrc && pJdst)
		textmsg(state, M_INF, _("%s(%s) is attacking %s(%s)"), countries_get_name(state.game.common, src),
		        _(color_name(pJsrc->color))
		        , countries_get_name(state.game.common, dst), _(color_name(pJdst->color)));
	else {
		textmsg(state, M_INF, _("%s is attacking %s"), countries_get_name(state.game.common, src),
		        countries_get_name(state.game.common, dst));
	}

	return TEG_STATUS_SUCCESS;
error:
	textmsg(state, M_ERR, "Error in clitok_attack()");
	return TEG_STATUS_ERROR;
}

/* it's someone turn */
TEG_STATUS clitok_turno(ClientState& state, char *str)
{
	int numjug;
	PCPLAYER pJ;

	PARSER p{str};
	p >> numjug;
	if(!p.finished()) {
		goto error;
	}

	if(player_whois(state, numjug, &pJ) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	state.game.whos_turn = numjug;

	attack_unshow(state);
	out_countries(state);

	if(numjug == state.game.numjug) {
		set_status(state, PLAYER_STATUS_ATAQUE);
		/* cosas a hacer cuando yo comienzo un nuevo turno */
		reagrupe_bigreset(state);
		attack_init(state);
		textmsg(state, M_INF, _("Its your turn to attack!!"));
	} else  {
		/* No es mi turno, entonces yo tengo que estar en idle */
		textmsg(state, M_IMP, _("Player %s(%s) has the turn to attack!"),
		        pJ->name,
		        _(color_name(pJ->color)));
		set_status(state, PLAYER_STATUS_IDLE);
	}
	callbacks::gui_turn(state, pJ);

	return TEG_STATUS_SUCCESS;
error:
	textmsg(state, M_ERR, "Error in clitok_turno()");
	return TEG_STATUS_ERROR;
}

/* someone is placing the initial armies */
TEG_STATUS clitok_fichas(ClientState& state, char *str)
{
	int numjug, cant;
	PCPLAYER j;

	PARSER p{str};
	p >> numjug >> cant;
	if(!p.finished()) {
		goto error;
	}

	if(player_whois(state, numjug, &j) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	out_countries(state);

	state.game.whos_turn = numjug;

	callbacks::gui_sensi(state);

	if(numjug == state.game.numjug) {
		set_status(state, PLAYER_STATUS_FICHAS);
		fichas_init(state, cant, 0);
		callbacks::gui_fichas(state, cant, 0);
	} else {
		textmsg(state, M_INF, _("Player %s(%s) is placing %d armies for 1st time"),
		        j->name,
		        _(color_name(j->color)),
		        cant);
	}

	return TEG_STATUS_SUCCESS;
error:
	textmsg(state, M_ERR, "Error in clitok_fichas()");
	return TEG_STATUS_ERROR;
}

/* someone is placing the initial armies (2nd time) */
TEG_STATUS clitok_fichas2(ClientState& state, char *str)
{
	int numjug, cant;
	PCPLAYER j;

	PARSER p{str};
	p >> numjug >> cant;
	if(!p.finished()) {
		goto error;
	}

	if(player_whois(state, numjug, &j) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	out_countries(state);

	state.game.whos_turn = numjug;
	callbacks::gui_sensi(state);

	if(numjug == state.game.numjug) {
		set_status(state, PLAYER_STATUS_FICHAS2);
		fichas_init(state, cant, 0);
		callbacks::gui_fichas(state, cant, 0);
	} else {
		textmsg(state, M_INF, _("Player %s(%s) is placing %d armies for 2nd time"),
		        j->name,
		        _(color_name(j->color)),
		        cant);
	}

	return TEG_STATUS_SUCCESS;
error:
	textmsg(state, M_ERR, "Error in clitok_fichas2()");
	return TEG_STATUS_ERROR;
}

/* someone is placing the continents armies */
TEG_STATUS clitok_fichasc(ClientState& state, char *str)
{
	/// \todo refactor out the common code of clitok_fichas, clitok_fichas2
	///	      and clitok_fichasc
	int numjug, cant, conts;
	PCPLAYER j;

	PARSER p{str};
	p >> numjug >> conts >> cant;
	if(!p.finished()) {
		goto error;
	}

	if(player_whois(state, numjug, &j) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	attack_unshow(state);

	state.game.whos_turn = numjug;
	callbacks::gui_sensi(state);

	out_countries(state);

	if(numjug == state.game.numjug) {
		int const tot_cant = cont_tot(state.game.common, conts) + cant;
		set_status(state, PLAYER_STATUS_FICHASC);
		fichas_init(state, tot_cant, conts);
		callbacks::gui_fichas(state, cant, conts);
	} else {
		textmsg(state, M_INF, _("Player %s(%s) is placing %d armies"),
		        j->name,
		        _(color_name(j->color)),
		        cant);
	}

	return TEG_STATUS_SUCCESS;
error:
	textmsg(state, M_ERR, "Error in clitok_fichasc()");
	return TEG_STATUS_ERROR;
}


/* all the countries that a players has */
TEG_STATUS clitok_countries(ClientState& state, char *str)
{
	int numjug;
	PCPLAYER j;
	PARSER p{str, ':', '/'};

	p >> numjug;
	if(!p.ok()) {
		goto error;
	}

	/* XXX: numjug == -1 in fog of war. Need to check if in FOW */
	if(numjug == -1 || player_whois(state, numjug, &j) == TEG_STATUS_SUCCESS) {
		return aux_countries(state, numjug, p.remainder());
	}
error:
	textmsg(state, M_ERR, "Error in clitok_countries()");
	return TEG_STATUS_ERROR;
}

/* who am i, and available colors */
TEG_STATUS clitok_playerid(ClientState& state, char *str)
{
	static_assert(maximum_player_count == 6);
	int c[maximum_player_count];

	PARSER p{str};
	p >> CString{state.game.myname} >> state.game.numjug
	  >> c[0] >> c[1] >> c[2] >> c[3] >> c[4] >> c[5];
	if(!p.finished()) {
		goto error;
	}

	set_status(state, PLAYER_STATUS_CONNECTED);

	out_status(state);
	if(state.game.observer) {
		out_countries(state);
	}

	callbacks::gui_connected(state, c);

	textmsg(state, M_IMP, _("I'm player number:%d"), state.game.numjug);
	return TEG_STATUS_SUCCESS;
error:
	textmsg(state, M_ERR, "Error in clitok_playerid()");
	return TEG_STATUS_ERROR;
}


/* I reconnected to the server */
TEG_STATUS clitok_reconnect(ClientState& state, char *str)
{
	PARSER p{str};

	p >> CString{state.game.myname} >> state.game.numjug >> state.game.mycolor;
	if(!p.finished()) {
		goto error;
	}

	set_status(state, PLAYER_STATUS_IDLE);

	textmsg(state, M_IMP,
	        _("Successful reconnection. I'm player number:%d"), state.game.numjug);

	{
		/* insert myself in the list of players */
		Player j;
		PCPLAYER pJ;

		if(player_whois(state, state.game.numjug, &pJ) != TEG_STATUS_SUCCESS) {
			memset(&j, 0, sizeof(j));
			j.color = state.game.mycolor;
			j.numjug = state.game.numjug;
			snprintf(j.name, sizeof(j.name), "%s", state.game.myname);
			player_ins(state, &j);
		}
	}

	out_status(state);
	out_countries(state);
	out_enum_cards(state);
	out_get_typeofgame(state);
	out_new_round(state);

	callbacks::gui_reconnected(state);

	return TEG_STATUS_SUCCESS;
error:
	textmsg(state, M_ERR, "Error in clitok_reconnect()");
	return TEG_STATUS_ERROR;
}

/* a new player enters the game */
TEG_STATUS clitok_newplayer(ClientState& state, char *str)
{
	char name[max_playername_length];
	int color, numjug;
	Player j;
	PCPLAYER pJ;

	PARSER p{str};
	p >> CString{name} >> numjug >> color;
	if(!p.finished()) {
		goto error;
	}

	if(player_whois(state, numjug, &pJ) != TEG_STATUS_SUCCESS) {
		memset(&j, 0, sizeof(j));
		j.color = color;
		j.numjug = numjug;
		snprintf(j.name, sizeof(j.name), "%s", name);
		player_ins(state, &j);
	} else {
		pJ->color =  color;
	}

	if(numjug == who_am_i(state)) {
		state.game.mycolor = color;
		set_status(state, PLAYER_STATUS_HABILITADO);
		textmsg(state, M_IMP, _("My color is: %s"), _(color_name(color)));
	} else {
		textmsg(state, M_IMP, _("Player[%d] '%s' is connected with color %s"), numjug,
		        name, _(color_name(color)));
	}
	callbacks::gui_habilitado(state, numjug);

	return TEG_STATUS_SUCCESS;
error:
	textmsg(state, M_ERR, "Error in clitok_newplayer()");
	return TEG_STATUS_ERROR;
}

/* a new message has arrived */
TEG_STATUS clitok_message(ClientState& state, char *str)
{
	char name[max_playername_length];
	int numjug;

	PARSER p{str};
	p >> CString{name} >> numjug;
	if(!p.ok()) {
		goto error;
	}

	if(state.game.msg_show & M_MSG) {
		callbacks::gui_textplayermsg(state, name, numjug, p.remainder());
	}
	return TEG_STATUS_SUCCESS;
error:
	textmsg(state, M_ERR, "Error in clitok_message()");
	return TEG_STATUS_ERROR;
}

/* ignore this. used for sending comments in help */
TEG_STATUS clitok_rem(ClientState& state, char *str)
{
	if(strlen(str)>0) {
		textmsg(state, M_IMP, str);
	}
	return TEG_STATUS_SUCCESS;
}

/* shows the status of the players */
TEG_STATUS clitok_status(ClientState& state, char *str)
{
	Player j, *j_tmp;
	PARSER p{str, ':', '/'};

	player_flush(state);

	if(strlen(str)==0) {
		goto ok;
	}

	do {
		if(!p.parse()) {
			break;
		}
		if(aux_status(state, &j, p.token) != TEG_STATUS_SUCCESS) {
			goto error;
		}

		if(player_whois(state, j.numjug, &j_tmp) == TEG_STATUS_SUCCESS) {
			player_update(state, &j);
		} else {
			player_ins(state, &j);
		}
	} while(p.can_continue());
ok:
	callbacks::gui_status(state);
	return TEG_STATUS_SUCCESS;
error:
	textmsg(state, M_ERR, "Error in clitok_status()");
	return TEG_STATUS_PARSEERROR;
}

/* receives the players scores */
TEG_STATUS clitok_scores(ClientState& state, char *str)
{
	SCORES score;
	PARSER p{str, DELIM{.valid=false}, DELIM{'\\'}};

	if(strlen(str)==0) {
		goto ok;
	}

	scores_init(state.game.common.highscores);

	do {
		if(!p.parse()) {
			break;
		}
		if(aux_scores(state, &score, p.token) != TEG_STATUS_SUCCESS) {
			goto error;
		}
		insert_score(state.game.common.highscores, &score);

	} while(p.can_continue());

ok:
	callbacks::gui_scores(state);
	return TEG_STATUS_SUCCESS;
error:
	textmsg(state, M_ERR, "Error in clitok_scores()");
	return TEG_STATUS_PARSEERROR;
}


/* the game have just started */
TEG_STATUS clitok_start(ClientState& state, char *str)
{
	Player j;
	PARSER p{str, ':', '/'};

	if(strlen(str)==0) {
		goto error;
	}

	player_flush(state);
	do {
		if((!p.parse())) {
			break;
		}

		if(aux_status(state, &j, p.token) != TEG_STATUS_SUCCESS) {
			goto error;
		}

		player_ins(state, &j);
	} while(p.can_continue());

	set_status(state, PLAYER_STATUS_START);

	out_countries(state);
	callbacks::gui_start(state);
	return TEG_STATUS_SUCCESS;

error:
	textmsg(state, M_ERR, "Error in clitok_start()");
	return TEG_STATUS_ERROR;
}


/* what cards do I have */
TEG_STATUS clitok_enum_cards(ClientState& state, char *str)
{
	PARSER p{str};
	int country, used;

	state.game.tarjetas_cant = 0;

	if(! str || strlen(str) == 0) {
		goto ok;
	}

	do {
		if(p.parse()) {
			country = atoi(p.token);
			used = atoi(p.value);
		} else {
			goto error;
		}

		if(country < 0 || country >= static_cast<int>(COUNTRIES_CANT)) {
			goto error;
		}

		state.game.tarjetas_cant++;

		if(used) {
			tarjeta_usar(&state.game.common.countries[ country ].tarjeta);
		}
		state.game.common.countries[ country ].tarjeta.numjug = who_am_i(state);

	} while(p.can_continue());

ok:
	return TEG_STATUS_SUCCESS;
error:
	textmsg(state, M_ERR, "Error in clitok_enum_cards()");
	return TEG_STATUS_ERROR;
}

/* someone has exchanged cards for armies. maybe its me */
TEG_STATUS clitok_exchange(ClientState& state, char *str)
{
	int p1, p2, p3, numjug, cant;
	PARSER p{str};

	p >> numjug >> cant >> p1 >> p2 >> p3;
	if(!p.finished()) {
		goto error;
	}

	PCPLAYER pJ;
	if(player_whois(state, numjug, &pJ) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	if(numjug == who_am_i(state)) {

		countries_map(state.game.common, [&state, p1, p2, p3](COUNTRY& country) {
			if(country.tarjeta.numjug != who_am_i(state)) {
				return;
			}

			if(country.id == p1 || country.id == p2 || country.id == p3) {
				state.game.common.countries[country.id].tarjeta.numjug = -1;
				state.game.tarjetas_cant--;
			}
		});

		fichas_add_wanted(state, cant);
		textmsg(state, M_IMP,
		        _("Exchanged approved. Now you can place %d more armies!"),
		        cant);
		callbacks::gui_canje(state, cant, p1, p2, p3);
	} else {
		textmsg(state, M_IMP, _("Player %s(%s) exchanged 3 cards for %d armies"),
		        pJ->name,
		        _(color_name(pJ->color)),
		        cant);
	}

	return TEG_STATUS_SUCCESS;
error:
	textmsg(state, M_ERR, "Error in clitok_tarjeta()");
	return TEG_STATUS_ERROR;
}


/* tells the rules of the game */
TEG_STATUS clitok_modalidad(ClientState& state, char *str)
{
	int with_secret_mission, with_common_mission, with_fog_of_war, _unused;
	PARSER p{str};
	p >> with_secret_mission >> with_common_mission >> with_fog_of_war
	  >> _unused;

	if(!p.finished()) {
		goto error;

	}

	state.game.with_common_mission = with_common_mission;
	state.game.with_secret_mission = with_secret_mission;
	state.game.with_fog_of_war = with_fog_of_war;

	out_missions(state);

	return TEG_STATUS_SUCCESS;
error:
	textmsg(state, M_ERR, "Error in clitok_tarjeta()");
	return TEG_STATUS_ERROR;
}

/* tells what is your secret mission */
TEG_STATUS clitok_mission(ClientState& state, char *str)
{
	int mission;
	PARSER p{str};
	p >> Limited{mission, 0, missions_cant()-1, 0};
	if(!p.finished()) {
		goto error;
	}

	state.game.secret_mission = mission;
	callbacks::gui_mission(state);
	return TEG_STATUS_SUCCESS;
error:
	textmsg(state, M_ERR, "Error in clitok_mission()");
	return TEG_STATUS_ERROR;
}

/* I'm receiving the card I've requested after finishing my turn */
TEG_STATUS clitok_tarjeta(ClientState& state, char *str)
{
	int country, used;
	PARSER p{str};
	p >> Limited{country, 0, COUNTRIES_CANT-1, 0} >> used;

	set_status(state, PLAYER_STATUS_TARJETA);

	state.game.tarjetas_cant++;

	if(used) {
		tarjeta_usar(&state.game.common.countries[ country ].tarjeta);
		textmsg(state, M_IMP,
		        _("You received card: '%s' and 2 armies where placed there"),
		        countries_get_name(state.game.common, country));
	} else {
		textmsg(state, M_IMP, _("You received card: '%s'"),
		        countries_get_name(state.game.common, country));
	}
	state.game.common.countries[ country ].tarjeta.numjug = who_am_i(state);

	callbacks::gui_tarjeta(state, country);
	return TEG_STATUS_SUCCESS;
}

/* Servers's Protocol version. HIVER MUST be equal, otherwise wont work */
TEG_STATUS clitok_pversion(ClientState& state, char *str)
{
	int hi, _low;
	PARSER p{str};
	p >> hi >> _low;

	if(!p.finished()) {
		goto error;
	}

	if(hi != PROTOCOL_HIVER) {
		textmsg(state, M_ERR,
		        _("Aborting: Different protocols version. Server:%d Client:%d"),
		        hi, PROTOCOL_HIVER);
		teg_disconnect(state);
		return TEG_STATUS_ERROR;
	}

	return TEG_STATUS_SUCCESS;
error:
	textmsg(state, M_ERR, "Error in clitok_pversion()");
	return TEG_STATUS_ERROR;
}

/* server version */
TEG_STATUS clitok_sversion(ClientState& state, char *str)
{
	textmsg(state, M_ALL, "Server version is '%s'", str);
	return TEG_STATUS_SUCCESS;
}

/* says who starts the round and the round number */
TEG_STATUS clitok_new_round(ClientState& state, char *str)
{
	int numjug, round_number;
	PARSER p{str};
	p >> Limited{numjug, 0, maximum_player_count-1, -1}
	  >> Limited{round_number, 0, std::numeric_limits<int>::max(), -1};
	if(!p.finished()) {
		goto error;
	}

	state.game.round_number = round_number;
	state.game.who_started_round = numjug;

	PCPLAYER pJ;
	if(player_whois(state, numjug, &pJ) != TEG_STATUS_SUCCESS) {
		/* no lo tengo en la base */
		textmsg(state, M_IMP, _("Player %d started round number: %d"),
		        numjug, round_number);
	} else {
		textmsg(state, M_IMP, _("Player %s(%s) started round number: %d"),
		        pJ->name,
		        _(color_name(pJ->color)),
		        round_number
		       );
	}

	callbacks::gui_sensi(state);

	return TEG_STATUS_SUCCESS;
error:
	textmsg(state, M_ERR, "Error in clitok_new_round()");
	return TEG_STATUS_ERROR;
}

/*
 * 	code that interprets which fn to call
 */
static TEG_STATUS client_lookup(ClientState& state, PARSER *p)
{
	for(unsigned i = 0; i < NRCLITOKENS; i++) {
		if(strcasecmp(p->token, tokens[i].label)==0) {
			if(tokens[i].func) {
				return((tokens[i].func)(state, p->value));
			}
			return TEG_STATUS_TOKENNULL;
		}
	}
	textmsg(state, M_ERR, _("Token '%s' not found"), p->token);
	return TEG_STATUS_TOKENNOTFOUND;
}

/* read from the fd, then parse the data */
TEG_STATUS client_recv(ClientState& state, int fd)
{
	char str[PROT_MAX_LEN];
	memset(str, 0, sizeof(str));

	if(net_readline(fd, str, PROT_MAX_LEN)<1) {
		teg_disconnect(state);
		return TEG_STATUS_CONNCLOSED;
	}

	PARSER p{str, '=', ';'};

	do {
		if(!p.parse()) {
			break;
		}
		if(client_lookup(state, &p) == TEG_STATUS_CONNCLOSED) {
			return TEG_STATUS_CONNCLOSED;
		}
	} while(p.can_continue());

	return TEG_STATUS_SUCCESS;
}

}
