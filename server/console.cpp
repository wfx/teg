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
 * Maneja los mensajes que llegan por la consola
 * Manages the console's messages
 */

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#ifdef HAVE_LIBREADLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif /* HAVE_LIBREADLINE */

#include "../common/common.h"
#include "../common/net.h"

#include "server.h"
#include "fcintl.h"
#include "scores.h"
#include "parser.h"

namespace teg::server
{

#undef DEBUG_CONSOLE

#define CONSOLE_DEBUG PDEBUG

#ifdef DEBUG_CONSOLE
# define STATIC
#else
# define STATIC static
#endif

#ifdef HAVE_LIBREADLINE
/* Readline invokes its callback without user data, so the state of the
 * console has to be remembered here. This is the only place where the
 * readline API forces us to keep a pointer around. */
static ServerState *readline_console_state = nullptr;
#endif /* HAVE_LIBREADLINE */

/* prototipos */
STATIC TEG_STATUS con_start(ServerState&, std::shared_ptr<Transport> const&, char*);
STATIC TEG_STATUS con_status(ServerState&, std::shared_ptr<Transport> const&, char*);
STATIC TEG_STATUS con_message(ServerState&, std::shared_ptr<Transport> const&, char*);
STATIC TEG_STATUS con_help(ServerState&, std::shared_ptr<Transport> const&, char*);
STATIC TEG_STATUS con_test(ServerState&, std::shared_ptr<Transport> const&, char*);
STATIC TEG_STATUS con_exit(ServerState&, std::shared_ptr<Transport> const&, char*);
STATIC TEG_STATUS con_save(ServerState&, std::shared_ptr<Transport> const&, char*);
STATIC TEG_STATUS con_set(ServerState&, std::shared_ptr<Transport> const&, char*);
STATIC TEG_STATUS con_kick(ServerState&, std::shared_ptr<Transport> const&, char*);
STATIC TEG_STATUS con_view(ServerState&, std::shared_ptr<Transport> const&, char *);
STATIC TEG_STATUS con_scores(ServerState&, std::shared_ptr<Transport> const&, char *);
STATIC TEG_STATUS con_stats(ServerState&, std::shared_ptr<Transport> const&, char *);

struct {
	char const *label;
	TEG_STATUS(*func)(ServerState&, std::shared_ptr<Transport> const&, char*);
	char const *help;
} con_tokens[] = {
	{ TOKEN_START,		con_start,	N_("to start playing") },
	{ TOKEN_STATUS,		con_status,	N_("shows status of players") },
	{ TOKEN_MESSAGE,	con_message,	N_("sends a message to all the players") },
	{ TOKEN_HELP,		con_help,	N_("shows help") },
	{ TOKEN_TEST,		con_test,	N_("internal use. Dont use it") },
	{ TOKEN_EXIT,		con_exit,	N_("exits the game") },
	{ TOKEN_SAVE,		con_save,	N_("save the game") },
	{ TOKEN_SET,		con_set,	N_("sets options") },
	{ TOKEN_VIEW,		con_view,	N_("view options") },
	{ TOKEN_KICK,		con_kick,	N_("kick player from the game") },
	{ TOKEN_STATS,		con_stats,	N_("show players statistics") },
	{ TOKEN_SCORES,		con_scores,	N_("show all-time high scores") },
};
#define	CONSOLE_TOKENS  (sizeof(con_tokens)/sizeof(con_tokens[0]))

STATIC TEG_STATUS con_exit(ServerState& state, std::shared_ptr<Transport> const& connection, char* unused2)
{
	player_flush(state);
#ifdef HAVE_LIBREADLINE
	state.readline_status = TEG_STATUS_GAMEOVER;
#endif /* HAVE_LIBREADLINE */
	return TEG_STATUS_GAMEOVER;
}

STATIC TEG_STATUS con_stats(ServerState& state, std::shared_ptr<Transport> const& connection, char* unused2)
{
	printf(_("Number Score - [Countries: Won  Lost] - [Armies: Won  Lost]  Name Human\n"));
	player_map(state, [&state](PSPLAYER pJ) {
		stats_score(&pJ->player_stats, state.game.common.conts.data());
		printf(" %i   %-4i  [ %-3u   %-3u ] - [ %-3u  %-3u ]  %-15s %s\n",
		       pJ->numjug,
		       pJ->player_stats.score,
		       pJ->player_stats.countries_won,
		       0,
		       pJ->player_stats.armies_killed,
		       0,
		       pJ->name,
		       pJ->human ? _("yes") : _("no")
		      );
	});
	return TEG_STATUS_SUCCESS;
}


/* shows all the scores */
STATIC void con_scores_show(PSCORES pS, void* unused)
{
	(void) unused;
	int color;
	color = ((pS->color >= static_cast<int>(maximum_player_count) || pS->color < 0) ? static_cast<int>(maximum_player_count) : pS->color);
	printf("  %4d   %s   %-15s   %-8s %s\n",
	       pS->score,
	       pS->date,
	       pS->name,
	       _(color_name(color)),
	       pS->human ? _("yes") : _("no")
	      );
}


STATIC TEG_STATUS con_scores(ServerState& state, std::shared_ptr<Transport> const& connection, char* unused2)
{
	printf(_("  score  date       time    name              color    human\n"));
	scores_map(state.game.common.highscores, con_scores_show, NULL);
	return TEG_STATUS_SUCCESS;
}

STATIC TEG_STATUS con_kick(ServerState& state, std::shared_ptr<Transport> const& connection, char *name)
{
	if(name && strlen(name)) {
		PSPLAYER pJ;
		if(player_findbyname(state, name, &pJ) == TEG_STATUS_SUCCESS) {
			player_del_hard(state, pJ);

			con_text_out_wop(state, M_ERR, _("Player %s was kicked from the game\n"), name);
			netall_printf(state, TOKEN_KICK"=%s\n", name);
		} else {
			con_text_out_wop(state, M_ERR, _("Player %s was not found\n"), name);
		}
	}
	return TEG_STATUS_SUCCESS;
}

STATIC TEG_STATUS con_save(ServerState& state, std::shared_ptr<Transport> const& connection, char *unused)
{
	con_text_out_wop(state, M_INF, _("Not yet implemented\n"));
	return TEG_STATUS_SUCCESS;
}

STATIC TEG_STATUS con_set(ServerState& state, std::shared_ptr<Transport> const& connection, char*str)
{
	return option_parse(state, connection, str);
}

STATIC TEG_STATUS con_view(ServerState& state, std::shared_ptr<Transport> const& connection, char*str)
{
	return option_view(state, connection, str);
}

STATIC TEG_STATUS con_message(ServerState& state, std::shared_ptr<Transport> const& connection, char *msg)
{
	if(strlen(msg) !=0) {
		strip_invalid_msg(msg);
		netall_printf(state, TOKEN_MESSAGE"=(r00t),-1,%s\n", msg);
	}
	return TEG_STATUS_SUCCESS;
}

STATIC TEG_STATUS con_status(ServerState& state, std::shared_ptr<Transport> const& connection, char*unused)
{
	connection->printf(_("players:%d, connections:%d, game number:%d, round:%d, mission:%s\n"),
	                   state.game.players,
	                   state.game.connections,
	                   state.game.gamenumber,
	                   state.game.round_number,
	                   (state.game.mission?_("TRUE"):_("FALSE"))
	                  );
	connection->printf(_("fd, number, countries, armies, cards, exch, name, human, color, status, address\n"));

	player_map(state, [&connection](PSPLAYER pJ) {
		int color;

		color = (pJ->color==-1) ? maximum_player_count : pJ->color;
		if(pJ->is_player) {
			connection->printf("%-3d %d  %-3u  %-3u  %d  %d  %-15s  %s  %s  %s  %s\n",
			                   player_fd(*pJ),
			                   pJ->numjug,
			                   pJ->tot_countries,
			                   pJ->tot_armies,
			                   pJ->tot_cards,
			                   pJ->tot_exchanges,
			                   pJ->name,
			                   pJ->human ? _("yes") : _("no"),
			                   _(color_name(color)),
			                   _(player_status_name(pJ->estado)),
			                   pJ->addr
			                  );
		} else {
			connection->printf("%-3d %d  %-3d  %-3d  %d  %d  %-15s  %s  %s  %s  %s\n",
			                   player_fd(*pJ),
			                   -1,
			                   -1,
			                   -1,
			                   -1,
			                   -1,
			                   pJ->name,
			                   pJ->human ? _("yes") : _("no"),
			                   _("n/a"),
			                   _("observer"),
			                   pJ->addr
			                  );
		}
	}, PlayerMapPolicy::everyone);
	return TEG_STATUS_SUCCESS;
}

STATIC TEG_STATUS con_start(ServerState& state, std::shared_ptr<Transport> const& connection, char*unused)
{
	char ch = 0;
	return token_start(state, connection, &ch);
}

STATIC TEG_STATUS con_help(ServerState& state, std::shared_ptr<Transport> const& connection, char*unused)
{
	for(unsigned i=0; i<CONSOLE_TOKENS; i++) {
		if(con_tokens[i].func)
			connection->printf("'%s' %s\n",
			                   con_tokens[i].label, _(con_tokens[i].help));
	}
	return TEG_STATUS_SUCCESS;
}

static void player_dump(PSPLAYER pJ)
{
	printf("Nombre: %s\n", pJ->name);
	printf("fd: %d\n", player_fd(*pJ));
}

STATIC TEG_STATUS con_test(ServerState& state, std::shared_ptr<Transport> const& connection, char *str)
{
	player_map(state, player_dump);
	return TEG_STATUS_SUCCESS;
}


/*
 * interpreter
 */
STATIC TEG_STATUS console_lookup(ServerState& state, std::shared_ptr<Transport> const& connection, PARSER *p)
{
	for(unsigned i=0; i<CONSOLE_TOKENS; i++) {
		if(strcmp(p->token, con_tokens[i].label)==0) {
			if(con_tokens[i].func) {
				return((con_tokens[i].func)(state, connection, p->value));
			}
			return TEG_STATUS_TOKENNULL;
		}
	}
	printf(_("Command '%s' not recongnized\n"), p->token);
	printf(_("Type '%s' for help\n"), TOKEN_HELP);
	return TEG_STATUS_TOKENNOTFOUND;
}

void con_show_prompt(ServerState& state)
{
	if(state.server.with_console) {
		fprintf(stdout, "> ");
		fflush(stdout);
	}
}

TEG_STATUS console_parse(ServerState& state, std::shared_ptr<Transport> const& connection, char *str)
{
	PARSER p{str, '=', ';'};

	do {
		if(!p.parse()) {
			break;
		}
		TEG_STATUS const ts = console_lookup(state, connection, &p);
		if(ts != TEG_STATUS_SUCCESS) {
			return ts;
		}
	} while(p.can_continue());
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS console_handle(ServerState& state)
{
#ifdef HAVE_LIBREADLINE
#else
	int j;
	TEG_STATUS ts = TEG_STATUS_SUCCESS;
	char str[PROT_MAX_LEN];
#endif

#ifdef HAVE_LIBREADLINE
	rl_callback_read_char();
	return state.readline_status;
#else
	str[0]=0;
	j=state.console->read_line(str, PROT_MAX_LEN);

	if(j<1) {
		return TEG_STATUS_CONNCLOSED;
	}

	if(j > 1) {
		ts = console_parse(state, state.console, str);
		con_show_prompt(state);
		return ts;
	}
	return TEG_STATUS_SUCCESS;
#endif /* !HAVE_LIBREADLINE */
}


void con_text_out(ServerState& state, int level, char const *format, ...)
{
	va_list args;
	char buf[PROT_MAX_LEN];

	if(state.server.with_console) {
		va_start(args, format);
		vsnprintf(buf, sizeof(buf) -1, format, args);
		buf[ sizeof(buf) -1 ] = 0;
		va_end(args);

		fprintf(stdout, "%s", buf);
		con_show_prompt(state);
	}
}

TEG_STATUS con_text_out_wop(ServerState& state, int level, char const *format, ...)
{
	va_list args;
	char buf[PROT_MAX_LEN];

	if(state.server.with_console) {
		va_start(args, format);
		vsnprintf(buf, sizeof(buf) -1, format, args);
		buf[ sizeof(buf) -1 ] = 0;
		va_end(args);

		fprintf(stdout, "%s", buf);
	}
	return TEG_STATUS_SUCCESS;
}

#ifdef HAVE_LIBREADLINE
void con_readline_input_callback(char *line)
{
	if(line) {
		if(*line) {
			add_history(line);
			if(readline_console_state) {
				console_parse(*readline_console_state, readline_console_state->console, line);
			}
		}
		free(line);
	}
}
#endif /* HAVE_LIBREADLINE */

TEG_STATUS console_init(ServerState& state)
{
	if(state.server.with_console) {
		state.console = std::make_shared<SocketTransport>(CONSOLE_FD, false);
	}
#ifdef HAVE_LIBREADLINE
	if(state.server.with_console) {
		readline_console_state = &state;
		rl_initialize();
		rl_callback_handler_install("> ", con_readline_input_callback);
	}
#endif /* HAVE_LIBREADLINE */
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS console_quit(ServerState& state)
{
#ifdef HAVE_LIBREADLINE
	if(state.server.with_console) {
		rl_callback_handler_remove();
		readline_console_state = nullptr;
	}
#endif /* HAVE_LIBREADLINE */
	return TEG_STATUS_SUCCESS;
}

}
