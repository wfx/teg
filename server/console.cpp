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
static TEG_STATUS rl_return = TEG_STATUS_SUCCESS;
#endif /* HAVE_LIBREADLINE */

/* prototipos */
STATIC TEG_STATUS con_start(int, char*);
STATIC TEG_STATUS con_status(int, char*);
STATIC TEG_STATUS con_message(int, char*);
STATIC TEG_STATUS con_help(int, char*);
STATIC TEG_STATUS con_test(int, char*);
STATIC TEG_STATUS con_exit(int, char*);
STATIC TEG_STATUS con_save(int, char*);
STATIC TEG_STATUS con_set(int, char*);
STATIC TEG_STATUS con_kick(int, char*);
STATIC TEG_STATUS con_view(int, char *);
STATIC TEG_STATUS con_scores(int, char *);
STATIC TEG_STATUS con_stats(int, char *);

struct {
	char const *label;
	TEG_STATUS(*func)(int, char*);
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

STATIC TEG_STATUS con_exit(int unused, char* unused2)
{
	player_flush();
#ifdef HAVE_LIBREADLINE
	rl_return = TEG_STATUS_GAMEOVER;
#endif /* HAVE_LIBREADLINE */
	return TEG_STATUS_GAMEOVER;
}

/*shows player's statistics */
STATIC void con_stats_show(PSPLAYER pJ)
{
	stats_score(&pJ->player_stats, g_conts);
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
}
STATIC TEG_STATUS con_stats(int unused, char* unused2)
{
	printf(_("Number Score - [Countries: Won  Lost] - [Armies: Won  Lost]  Name Human\n"));
	player_map(con_stats_show);
	return TEG_STATUS_SUCCESS;
}


/* shows all the scores */
STATIC void con_scores_show(PSCORES pS, void* unused)
{
	(void) unused;
	int color;
	color = ((pS->color >= maximum_player_count || pS->color < 0) ? maximum_player_count : pS->color);
	printf("  %4d   %s   %-15s   %-8s %s\n",
	       pS->score,
	       pS->date,
	       pS->name,
	       _(g_colores[color]),
	       pS->human ? _("yes") : _("no")
	      );
}


STATIC TEG_STATUS con_scores(int unused, char* unused2)
{
	printf(_("  score  date       time    name              color    human\n"));
	scores_map(con_scores_show, NULL);
	return TEG_STATUS_SUCCESS;
}

STATIC TEG_STATUS con_kick(int fd, char *name)
{
	if(name && strlen(name)) {
		PSPLAYER pJ;
		if(player_findbyname(name, &pJ) == TEG_STATUS_SUCCESS) {
			player_del_hard(pJ);

			con_text_out_wop(M_ERR, _("Player %s was kicked from the game\n"), name);
			netall_printf(TOKEN_KICK"=%s\n", name);
		} else {
			con_text_out_wop(M_ERR, _("Player %s was not found\n"), name);
		}
	}
	return TEG_STATUS_SUCCESS;
}

STATIC TEG_STATUS con_save(int fd, char *unused)
{
	con_text_out_wop(M_INF, _("Not yet implemented\n"));
	return TEG_STATUS_SUCCESS;
}

STATIC TEG_STATUS con_set(int fd, char*str)
{
	return option_parse(fd, str);
}

STATIC TEG_STATUS con_view(int fd, char*str)
{
	return option_view(fd, str);
}

STATIC TEG_STATUS con_message(int fd, char *msg)
{
	if(strlen(msg) !=0) {
		strip_invalid_msg(msg);
		netall_printf(TOKEN_MESSAGE"=(r00t),-1,%s\n", msg);
	}
	return TEG_STATUS_SUCCESS;
}

STATIC TEG_STATUS con_status(int fd, char*unused)
{
	net_printf(fd, _("players:%d, connections:%d, game number:%d, round:%d, mission:%s\n"),
	           g_game.players,
	           g_game.connections,
	           g_game.gamenumber,
	           g_game.round_number,
	           (g_game.mission?_("TRUE"):_("FALSE"))
	          );
	net_printf(fd, _("fd, number, countries, armies, cards, exch, name, human, color, status, address\n"));

	player_map([fd](PSPLAYER pJ) {
		int color;

		color = (pJ->color==-1) ? maximum_player_count : pJ->color;
		if(pJ->is_player) {
			net_printf(fd, "%-3d %d  %-3u  %-3u  %d  %d  %-15s  %s  %s  %s  %s\n",
			           pJ->fd,
			           pJ->numjug,
			           pJ->tot_countries,
			           pJ->tot_armies,
			           pJ->tot_cards,
			           pJ->tot_exchanges,
			           pJ->name,
			           pJ->human ? _("yes") : _("no"),
			           _(g_colores[color]),
			           _(g_estados[pJ->estado]),
			           pJ->addr
			          );
		} else {
			net_printf(fd, "%-3d %d  %-3d  %-3d  %d  %d  %-15s  %s  %s  %s  %s\n",
			           pJ->fd,
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

STATIC TEG_STATUS con_start(int fd, char*unused)
{
	char ch = 0;
	return token_start(fd, &ch);
}

STATIC TEG_STATUS con_help(int fd, char*unused)
{
	for(unsigned i=0; i<CONSOLE_TOKENS; i++) {
		if(con_tokens[i].func)
			net_printf(fd, "'%s' %s\n",
			           con_tokens[i].label, _(con_tokens[i].help));
	}
	return TEG_STATUS_SUCCESS;
}

static void player_dump(PSPLAYER pJ)
{
	printf("Nombre: %s\n", pJ->name);
	printf("fd: %d\n", pJ->fd);
}

STATIC TEG_STATUS con_test(int fd, char *str)
{
	player_map(player_dump);
	return TEG_STATUS_SUCCESS;
}


/*
 * interpreter
 */
STATIC TEG_STATUS console_lookup(int fd, PARSER *p)
{
	for(unsigned i=0; i<CONSOLE_TOKENS; i++) {
		if(strcmp(p->token, con_tokens[i].label)==0) {
			if(con_tokens[i].func) {
				return((con_tokens[i].func)(fd, p->value));
			}
			return TEG_STATUS_TOKENNULL;
		}
	}
	printf(_("Command '%s' not recongnized\n"), p->token);
	printf(_("Type '%s' for help\n"), TOKEN_HELP);
	return TEG_STATUS_TOKENNOTFOUND;
}

void con_show_prompt()
{
	if(g_server.with_console) {
		fprintf(stdout, "> ");
		fflush(stdout);
	}
}

TEG_STATUS console_parse(int fd, char *str)
{
	PARSER p{str, '=', ';'};

	do {
		if(!p.parse()) {
			break;
		}
		TEG_STATUS const ts = console_lookup(fd, &p);
		if(ts != TEG_STATUS_SUCCESS) {
			return ts;
		}
	} while(p.can_continue());
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS console_handle(int fd)
{
#ifdef HAVE_LIBREADLINE
#else
	int j;
	TEG_STATUS ts = TEG_STATUS_SUCCESS;
	char str[PROT_MAX_LEN];
#endif

#ifdef HAVE_LIBREADLINE
	rl_callback_read_char();
	return rl_return;
#else
	str[0]=0;
	j=net_readline(fd, str, PROT_MAX_LEN);

	if(j<1) {
		return TEG_STATUS_CONNCLOSED;
	}

	if(j > 1) {
		ts = console_parse(fd, str);
		con_show_prompt();
		return ts;
	}
	return TEG_STATUS_SUCCESS;
#endif /* !HAVE_LIBREADLINE */
}


void con_text_out(int level, char const *format, ...)
{
	va_list args;
	char buf[PROT_MAX_LEN];

	if(g_server.with_console) {
		va_start(args, format);
		vsnprintf(buf, sizeof(buf) -1, format, args);
		buf[ sizeof(buf) -1 ] = 0;
		va_end(args);

		fprintf(stdout, "%s", buf);
		con_show_prompt();
	}
}

TEG_STATUS con_text_out_wop(int level, char const *format, ...)
{
	va_list args;
	char buf[PROT_MAX_LEN];

	if(g_server.with_console) {
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
			console_parse(CONSOLE_FD, line);
		}
		free(line);
	}
}
#endif /* HAVE_LIBREADLINE */

TEG_STATUS console_init(void)
{
#ifdef HAVE_LIBREADLINE
	if(g_server.with_console) {
		rl_initialize();
		rl_callback_handler_install("> ", con_readline_input_callback);
	}
#endif /* HAVE_LIBREADLINE */
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS console_quit(void)
{
#ifdef HAVE_LIBREADLINE
	if(g_server.with_console) {
		rl_callback_handler_remove();
	}
#endif /* HAVE_LIBREADLINE */
	return TEG_STATUS_SUCCESS;
}

}
