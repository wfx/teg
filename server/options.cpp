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
 * @file options.c
 * Manages the server options
 */

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include "../common/net.h"
#include "server.h"
#include "fow.h"
#include "parser.h"
#include "fcintl.h"
#include "missions.h"

namespace teg::server
{

TEG_STATUS option_conqworld(ServerState& state, std::shared_ptr<Transport> const& connection, char *str);
TEG_STATUS option_conqworld_view(ServerState& state);
TEG_STATUS option_cmission(ServerState& state, std::shared_ptr<Transport> const& connection, char *str);
TEG_STATUS option_cmission_view(ServerState& state);
TEG_STATUS option_fog_of_war(ServerState& state, std::shared_ptr<Transport> const& connection, char *str);
TEG_STATUS option_fog_of_war_view(ServerState& state);
TEG_STATUS option_armies(ServerState& state, std::shared_ptr<Transport> const& connection, char *str);
TEG_STATUS option_armies_view(ServerState& state);
TEG_STATUS option_seed(ServerState& state, std::shared_ptr<Transport> const& connection, char *str);
TEG_STATUS option_seed_view(ServerState& state);
TEG_STATUS option_help(ServerState& state, std::shared_ptr<Transport> const& connection, char *str);

TEG_STATUS option_lookup(ServerState& state, std::shared_ptr<Transport> const& connection, PARSER *p);

struct {
	char const *label;
	TEG_STATUS(*func)(ServerState&, std::shared_ptr<Transport> const&, char*);
	TEG_STATUS(*func_view)(ServerState&);
	char const *help;
} options[] = {
	{ OPTION_CONQWORLD, option_conqworld, option_conqworld_view, N_("Conquer the world, or play with secret missions") },
	{ OPTION_CMISSION, option_cmission, option_cmission_view, N_("Enable/Disable common secret mission") },
	{ OPTION_ARMIES, option_armies, option_armies_view, N_("Set the initial number of armies to place") },
	{ OPTION_SEED, option_seed, option_seed_view, N_("Set the seed for random") },
	{ OPTION_FOG_OF_WAR, option_fog_of_war, option_fog_of_war_view, N_("Enable/Disable the Fog of War") },
	{ OPTION_HELP, option_help, NULL, N_("Shows the set options") },
};
#define	NOPTIONS  (sizeof(options)/sizeof(options[0]))

TEG_STATUS option_conqworld_view(ServerState& state)
{
	con_text_out_wop(state, M_INF, _("'%s' variable is set to: %s\n"),
	                 OPTION_CONQWORLD,
	                 (!state.game.mission?_("TRUE"):_("FALSE"))
	                );
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS option_conqworld(ServerState& state, std::shared_ptr<Transport> const& connection, char *str)
{
	int a, b;

	if(str && strlen(str) !=0) {
		a = atoi(str);
		b = !state.game.mission;
		if(mission_set(state, !a) == TEG_STATUS_SUCCESS) {
			con_text_out_wop(state, M_INF, _("Setting %s variable to: %s -> %s\n"),
			                 OPTION_CONQWORLD,
			                 (b?_("TRUE"):_("FALSE")),
			                 (a?_("TRUE"):_("FALSE"))
			                );
		} else {
			goto error;
		}
	} else {
		option_conqworld_view(state);
	}
	return TEG_STATUS_SUCCESS;

error:
	if(connection == state.console) {
		con_text_out_wop(state, M_ERR, _("Error setting %s variable.\n"), OPTION_CONQWORLD);
	}

	return TEG_STATUS_ERROR;
}

TEG_STATUS option_cmission_view(ServerState& state)
{
	con_text_out_wop(state, M_INF, _("'%s' variable is set to: %s\n"),
	                 OPTION_CMISSION,
	                 (state.game.cmission?_("TRUE"):_("FALSE"))
	                );
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS option_cmission(ServerState& state, std::shared_ptr<Transport> const& connection, char *str)
{
	int a, b;

	if(str && strlen(str) !=0) {
		a = atoi(str);
		b = state.game.cmission;
		if(mission_common_mission(state, a) == TEG_STATUS_SUCCESS) {
			con_text_out_wop(state, M_INF, _("Setting %s variable to: %s -> %s\n"),
			                 OPTION_CMISSION,
			                 (b?_("TRUE"):_("FALSE")),
			                 (a?_("TRUE"):_("FALSE"))
			                );
		} else {
			goto error;
		}
	} else {
		option_cmission_view(state);
	}
	return TEG_STATUS_SUCCESS;

error:
	if(connection == state.console) {
		con_text_out_wop(state, M_ERR, _("Error setting %s variable.\n"), OPTION_CMISSION);
	}

	return TEG_STATUS_ERROR;
}

TEG_STATUS option_fog_of_war_view(ServerState& state)
{
	con_text_out_wop(state, M_INF, _("'%s' variable is set to: %s\n"),
	                 OPTION_FOG_OF_WAR,
	                 (state.game.fog_of_war?_("TRUE"):_("FALSE"))
	                );
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS option_fog_of_war(ServerState& state, std::shared_ptr<Transport> const& connection, char *str)
{
	int a, b;

	if(str && strlen(str) !=0) {
		a = atoi(str);
		b = state.game.fog_of_war;
		if(fow_set_mode(state, a) == TEG_STATUS_SUCCESS) {
			con_text_out_wop(state, M_INF, _("Setting %s variable to: %s -> %s\n"),
			                 OPTION_FOG_OF_WAR,
			                 (b?_("TRUE"):_("FALSE")),
			                 (a?_("TRUE"):_("FALSE"))
			                );
		} else {
			goto error;
		}
	} else {
		option_fog_of_war_view(state);
	}
	return TEG_STATUS_SUCCESS;

error:
	if(connection == state.console) {
		con_text_out_wop(state, M_ERR, _("Error setting %s variable.\n"), OPTION_CMISSION);
	}

	return TEG_STATUS_ERROR;
}

TEG_STATUS option_armies_view(ServerState& state)
{
	con_text_out_wop(state, M_INF, _("'%s' variable is set to: (%d,%d)\n"), OPTION_ARMIES, state.game.fichas, state.game.fichas2);
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS option_armies(ServerState& state, std::shared_ptr<Transport> const& connection, char *str)
{
	int armies1;
	int armies2;
	PARSER p{str, '|', ','};

	if(str && strlen(str)!=0) {

		if(game_started(state)) {
			goto error;
		}

		if(p.parse_fragment()) {
			armies1 = atoi(p.token);
		} else {
			goto error;
		}

		if(p.parse_everything()) {
			armies2 = atoi(p.token);
		} else {
			goto error;
		}

		if(armies1 && armies2) {
			state.game.fichas = armies1;
			state.game.fichas2 = armies2;
		} else {
			goto error;
		}
	}
	option_armies_view(state);
	return TEG_STATUS_SUCCESS;
error:
	return TEG_STATUS_ERROR;
}

TEG_STATUS option_seed_view(ServerState& state)
{
	con_text_out_wop(state, M_INF, _("'%s' variable is set to: %u\n"), OPTION_SEED, state.game.seed);
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS option_seed(ServerState& state, std::shared_ptr<Transport> const& connection, char *str)
{
	unsigned int seed;

	if(str && strlen(str)!=0) {

		if(connection != state.console) {
			goto error;
		}

		seed = atoi(str);
		state.game.seed = seed;
		srand(seed);
	}

	option_seed_view(state);
	return TEG_STATUS_SUCCESS;
error:
	return TEG_STATUS_ERROR;
}

TEG_STATUS option_help(ServerState&, std::shared_ptr<Transport> const& connection, char *str)
{
	for(unsigned i=0; i<NOPTIONS; i++) {
		if(options[i].func) {
			connection->printf(TOKEN_REM"='%s' %s\n", options[i].label, _(options[i].help));
		}
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS option_lookup(ServerState& state, std::shared_ptr<Transport> const& connection, PARSER *p)
{
	for(unsigned i = 0; i < NOPTIONS; i++) {
		if(strcasecmp(p->token, options[i].label)==0) {
			if(options[i].func) {
				return((options[i].func)(state, connection, p->value));
			}

			return TEG_STATUS_TOKENNULL;
		}
	}
	connection->printf(_("Unknow option. Try using `%s %s' for help\n"), TOKEN_SET, OPTION_HELP);
	return TEG_STATUS_TOKENNOTFOUND;
}

TEG_STATUS option_view(ServerState& state, std::shared_ptr<Transport> const& connection, char *str)
{
	for(unsigned i = 0; i < NOPTIONS; i++) {
		if(options[i].func_view) {
			(options[i].func_view)(state);
		}
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS option_parse(ServerState& state, std::shared_ptr<Transport> const& connection, char *str)
{
	int i;
	PARSER p{str, DELIM{'='}, DELIM{.valid=false}};

	do {
		if((i=p.parse())) {
			if(option_lookup(state, connection, &p) == TEG_STATUS_CONNCLOSED) {
				return TEG_STATUS_CONNCLOSED;
			}
		}
	} while(i && p.can_continue());

	return TEG_STATUS_SUCCESS;
}

}
