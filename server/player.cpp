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

#include <list>
#include <algorithm>
#include <iterator>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "../common/tegdebug.h"
#include "server.h"
#include "scores.h"
#include "xmlscores.h"
#include "fcintl.h"
#include "main.h"

namespace teg::server
{

typedef struct {
	int humans;
	int robots;
} PlayerCount;

PlayerCount player_count(ServerState& state)
{
	PlayerCount result = {0, 0};

	player_map(state, [&result](PSPLAYER pJ) {
		if(pJ->human) {
			result.humans++;
		} else {
			result.robots++;
		}
	});
	return result;
}

TEG_STATUS player_whois(ServerState& state, int numjug, PSPLAYER *pJ)
{
	TEG_STATUS result=TEG_STATUS_PLAYERNOTFOUND;
	player_map(state, [numjug, &result, pJ](PSPLAYER pJ_new) {
		if(pJ_new->numjug == numjug) {
			*pJ = pJ_new;
			result = TEG_STATUS_SUCCESS;
		}
	});
	return result;
}

TEG_STATUS player_findbyname(ServerState& state, char const *name, PSPLAYER *pJ)
{
	TEG_STATUS result = TEG_STATUS_PLAYERNOTFOUND;
	player_map(state, [name, &pJ, &result](PSPLAYER pJ_new) {
		if(!strncmp(pJ_new->name, name, sizeof(pJ_new->name))) {
			*pJ = pJ_new;
			result = TEG_STATUS_SUCCESS;
		}
	});
	return result;
}

void player_erase(ServerState& state, PSPLAYER pJ)
{
	auto player_it{std::find_if(state.players.begin(), state.players.end(),
	                            [pJ](SPLAYER& p)
	{
		return pJ == &p;
	})};
	if(player_it != state.players.end()) {
		state.players.erase(player_it);
	}
}

void player_yield_color(ServerState& state, SPLAYER& player)
{
	if(player.color != -1) {
		color_del(state, player.color);
	}
}

bool player_is_to_be_deleted(SPLAYER const& player)
{
	return (PLAYER_STATUS_DESCONECTADO==player.estado)
	       || (!player.connection);
}

void player_delete_discon(ServerState& state, PSPLAYER pJ)
{
	if(player_is_to_be_deleted(*pJ)) {
		player_yield_color(state, *pJ);
		player_erase(state, pJ);
	}
}

void player_map_erasable(ServerState& state, std::function<void(PlayersList::iterator it)> fn)
{
	for(auto it=state.players.begin(); it!=state.players.end(); /*advance in body*/) {
		auto follow=std::next(it); // Make a copy of the following iterator
		fn(it);
		/* it may be now invalid, and so is (it++). This is why we already
		 * stored the following iterator */
		it=follow;
	}
}

void player_delete_all_disconnected(ServerState& state)
{
	player_map_erasable(state, [&state](PlayersList::iterator it) {
		if(player_is_to_be_deleted(*it)) {
			player_yield_color(state, *it);
			state.players.erase(it);

		}
	});
}

void player_initplayer(PSPLAYER pJ)
{
	assert(pJ);

	pJ->hizo_canje = false;
	pJ->tot_exchanges = 0;
	pJ->tot_countries = 0;
	pJ->tot_cards = 0;
	pJ->tot_armies = 0;
	pJ->turno_conq = 0;
	pJ->country_src = -1;
	pJ->country_dst = -1;
	pJ->mission = -1;
	pJ->fichasc_armies = 0;
	pJ->fichasc_conts = 0;

	stats_init(&pJ->player_stats);
}

/* main initialization */
void player_init(void)
{
}

TEG_STATUS player_numjug_libre(ServerState& state, int *libre)
{
	char jugs[maximum_player_count] = {0};
	int i;

	assert(libre);

	player_map(state, [&jugs](PSPLAYER pJ) {
		if(pJ->numjug >= 0 && pJ->numjug < static_cast<int>(maximum_player_count)) {
			jugs[pJ->numjug] = 1;
		}
	});

	for(i=0; i<static_cast<int>(maximum_player_count); i++) {
		if(jugs[i] == 0) {
			*libre = i;
			return TEG_STATUS_SUCCESS;
		}
	}

	/* server is full */
	return TEG_STATUS_ERROR;
}

/* creates a player and initialize it */
PSPLAYER player_ins(ServerState& state, PSPLAYER pJ, bool esplayer)
{
	int numjug;

	assert(pJ);

	if(esplayer && player_numjug_libre(state, &numjug) != TEG_STATUS_SUCCESS) {
		return NULL;
	}

	pJ->numjug = -1;
	pJ->color = -1;
	state.players.push_back(*pJ);

	SPLAYER &newJ{state.players.back()};
	player_initplayer(&newJ);

	newJ.is_player = esplayer;
	newJ.estado = PLAYER_STATUS_CONNECTED;

	if(esplayer) {
		newJ.numjug = numjug;
		state.game.players++;
	}

	state.game.connections++;

	return &newJ;
}

TEG_STATUS player_flush(ServerState& state)
{
	for(SPLAYER &player: state.players) {
		if(player.connection) {
			remove_connection(state, player.connection);
			player.connection.reset();
		}
		con_text_out(state, M_INF, ("Deleting %s\n"), player.name);
	}

	state.game.connections = 0;
	state.game.players = 0;
	return TEG_STATUS_SUCCESS;
}

bool player_is_playing(PSPLAYER pJ)
{
	if(! pJ->is_player) {
		return false;
	}

	if(pJ->estado < PLAYER_STATUS_START || pJ->estado >= PLAYER_STATUS_LAST) {
		return false;
	}

	return true;
}

/* release the turn, and give it to the next or prev one */
TEG_STATUS player_give_turn_away(ServerState& state, PSPLAYER pJ)
{

	PLAYER_STATUS status = pJ->estado;

	/* needed to prevent loops in xxx_next or xxx_prev algorithms */
	pJ->estado = PLAYER_STATUS_GAMEOVER;

	/* si el player tenia el turno, lo tiene que pasar al sig*/
	if(state.game.turno &&  state.game.turno->numjug == pJ->numjug) {
		if(status <= PLAYER_STATUS_POSTFICHAS) {
			fichas_next(state);
		} else if(status <= PLAYER_STATUS_POSTFICHAS2) {
			fichas2_next(state);
		} else if(status <= PLAYER_STATUS_POSTFICHASC) {
			fichasc_next(state);
		} else { /* tenia el turno */
			turno_next(state);
		}
	}

	pJ->estado = status;

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS player_del_soft(ServerState& state, PSPLAYER pJ)
{
	assert(pJ);

	if(! player_is_playing(pJ)) {
		return TEG_STATUS_ERROR;
	}

	state.game.playing--;

	/* game without players */
	if(game_started(state) && state.game.playing == 0) {

		con_text_out(state, M_INF, _("Game without players. Initializing another game.\n"));
		game_end(state, NULL);

		/* game with just one player... the winner */
	} else if(state.game.playing == 1 && game_started(state)) {

		PSPLAYER winner=NULL;
		player_map(state, [&state, pJ, &winner](PSPLAYER pJ2) {
			if(pJ2->numjug!=pJ->numjug && player_is_playing(pJ2)) {
				con_text_out(state, M_INF, _("Game with one player. Player %s(%d) is the winner\n"), pJ2->name, pJ2->numjug);
				pJ2->estado = PLAYER_STATUS_GAMEOVER;
				winner = pJ2;
			}
		});

		if(winner != nullptr) {
			game_end(state, winner);
		}

		/* game may continue normally */
	} else {
		player_give_turn_away(state, pJ);
		pJ->estado = PLAYER_STATUS_GAMEOVER;
	}

	return TEG_STATUS_SUCCESS;
}

void player_del_hard(ServerState& state, PSPLAYER pJ)
{
	assert(pJ);

	state.game.connections--;

	/* close the connection */
	if(pJ->connection) {
		remove_connection(state, pJ->connection);
		pJ->connection.reset();
	}

	if(pJ->is_player) {

		con_text_out(state, M_INF, _("Player %s(%d) quit the game\n"), pJ->name, pJ->numjug);
		netall_printf(state, TOKEN_EXIT"=%d\n", pJ->numjug);

		if(player_is_playing(pJ)) {


			player_del_soft(state, pJ);

			state.game.players--;
			pJ->status_before_discon = PLAYER_STATUS_IDLE;
			pJ->estado = PLAYER_STATUS_DESCONECTADO;
			return;

		} else if(pJ->estado == PLAYER_STATUS_GAMEOVER) {

			state.game.players--;
			pJ->status_before_discon = PLAYER_STATUS_GAMEOVER;
			pJ->estado = PLAYER_STATUS_DESCONECTADO;
			return;
		}

		color_del(state, pJ->color);
		state.game.players--;

	} else {
		con_text_out(state, M_INF, _("Observer %s(%d) quit the game\n"), pJ->name, pJ->numjug);
	}

	player_erase(state, pJ);

	return;
}

TEG_STATUS player_from_indice(ServerState& state, int j, int *real_j)
{
	TEG_STATUS result = TEG_STATUS_PLAYERNOTFOUND;
	int i=0;

	player_map(state, [&i, j, &real_j, &result](PSPLAYER pJ) {
		if(pJ->estado>=PLAYER_STATUS_HABILITADO) {
			if(j == i) {
				*real_j = pJ->numjug;
				result = TEG_STATUS_SUCCESS;
			}
			i++;
		}
	});
	return result;
}

TEG_STATUS player_asignarcountry(ServerState& state, int numjug, PCOUNTRY p)
{
	PSPLAYER pJ;

	if(player_whois(state, numjug, &pJ) != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_PLAYERNOTFOUND;
	}

	p->numjug = numjug;
	pJ->tot_countries++;
	pJ->tot_armies++;		/* cada country viene con un ejercito */
	return TEG_STATUS_SUCCESS;
}

/* given a fd, return the player who owns it */
TEG_STATUS player_whose(ServerState& state, std::shared_ptr<Transport> const& connection, PSPLAYER *j)
{
	TEG_STATUS result = TEG_STATUS_PLAYERNOTFOUND;

	player_map(state, [&connection, &j, &result](PSPLAYER i) {
		if(i->connection == connection) {
			*j = i;
			result = TEG_STATUS_SUCCESS;
		}
	}, PlayerMapPolicy::everyone);
	return result;
}

bool player_esta_xxx(ServerState& state, std::shared_ptr<Transport> const& connection, PLAYER_STATUS wanted, bool exact)
{
	PSPLAYER pJ;
	return player_esta_xxx_plus(state, connection, wanted, exact, &pJ);
}

bool player_esta_xxx_plus(ServerState& state, std::shared_ptr<Transport> const& connection, PLAYER_STATUS wanted, bool strict, PSPLAYER *j)
{
	if(player_whose(state, connection, j) == TEG_STATUS_SUCCESS) {
		if(strict) {
			return ((*j)->estado == wanted);
		} else {
			return ((*j)->estado >= wanted);
		}
	} else {
		return false;
	}
}

void player_listar_countries(ServerState& state, PSPLAYER pJ, int *countries)
{
	assert(pJ);
	assert(countries);

	countries_map(state.game.common, pJ->numjug,
	[&countries](COUNTRY &country) {
		countries[country.continente]++;
	});
}

void player_listar_conts(ServerState& state, PSPLAYER pJ, unsigned long *ret)
{
	int countries[CONT_CANT];
	int i;

	assert(pJ);
	assert(ret);

	memset(countries, 0, sizeof(countries));

	player_listar_countries(state, pJ, countries);

	*ret = 0;

	for(i=0; i< CONT_CANT; i++) {
		if(countries[i] == state.game.common.conts[i].cant_countries) {
			*ret |= 1 << i;
		}
	}
}

void player_clear_turn(ServerState& state, PSPLAYER pJ)
{
	int i;

	assert(pJ);

	pJ->turno_conq = 0;
	pJ->country_src = -1;
	pJ->country_dst = -1;
	pJ->estado =  PLAYER_STATUS_IDLE;

	/* clean all the regroups the player could have done */
	for(i=0; i<static_cast<int>(COUNTRIES_CANT); i++) {
		if(state.game.common.countries[i].numjug == pJ->numjug) {
			state.game.common.countries[i].ejer_reagrupe = 0;
		}
	}
}

int player_fichasc_cant(PSPLAYER pJ)
{
	assert(pJ);

	if(pJ->tot_countries <= 6) {
		return 3;
	} else {
		return pJ->tot_countries/2;
	}
}

void player_all_set_status(ServerState& state, PLAYER_STATUS estado)
{
	player_map(state, [estado](PSPLAYER pJ) {
		if(pJ->estado >= PLAYER_STATUS_GAMEOVER) {
			pJ->estado = estado;
		}
	});
}

void player_map(ServerState& state, jug_map_func func,
                PlayerMapPolicy policy)
{
	assert(func);

	std::for_each(state.players.begin(), state.players.end(),
	[func, policy](SPLAYER& p) {
		if(p.is_player || (policy==PlayerMapPolicy::everyone)) {
			func(&p);
		}
	});
}

bool player_is_lost(PSPLAYER pJ)
{
	assert(pJ);

	if(pJ->tot_countries > 0) {
		return 0;
	}

	return 1;
}

void player_poner_perdio(ServerState& state, PSPLAYER pJ)
{
	assert(pJ);

	if(pJ->estado == PLAYER_STATUS_DESCONECTADO) {
		scores_insert_player(state, pJ);
		player_delete_discon(state, pJ);
	}

	// We get called if a player's last country is conquered. If the player
	// surrendered before that happened we must NOT decrement the player count again
	if(player_is_playing(pJ)) {
		state.game.playing--;
	}

	pJ->estado = PLAYER_STATUS_GAMEOVER;
}


void player_fillname(ServerState& state, PSPLAYER pJ, char *name)
{
	PSPLAYER pJ_new;
	char new_name [ max_playername_length ];

	memset(new_name, 0, sizeof(new_name));
	strncpy(new_name, name, sizeof(new_name) -1);

	strip_invalid(new_name);
	if(player_findbyname(state, new_name, &pJ_new) == TEG_STATUS_SUCCESS && pJ_new->estado != PLAYER_STATUS_DESCONECTADO) {
		/* that name is already registered, assign a new name dynamically */
		size_t n = strlen(new_name);
		if(n < (sizeof(pJ->name) - 2)) {
			new_name[n] = '_';
			player_fillname(state, pJ, new_name);
		} else {
			if(new_name[n-1] < '0' || new_name[n-1] > '9') {
				new_name[n-1]='0';
			} else {
				new_name[n-1]++;
			}
			player_fillname(state, pJ, new_name);
		}
	} else {
		strncpy(pJ->name, new_name, sizeof(pJ->name)-1);
		pJ->name[ sizeof(pJ->name) -1 ] = 0;
	}
}

/* return a disconected player with the same name as pJ */
PSPLAYER player_return_disconnected(ServerState& state, PSPLAYER pJ)
{

	PSPLAYER result = nullptr;
	player_map(state, [&state, pJ, &result](PSPLAYER pJ_new) {
		if((pJ_new->estado == PLAYER_STATUS_DESCONECTADO) &&
		        strcmp(pJ->name, pJ_new->name) == 0
		  ) {
			state.game.players++;
			state.game.playing++;
			state.game.connections++;
			result = pJ_new;
		}
	}, PlayerMapPolicy::everyone);
	return result;
}

/* return true if the player is disconnected */
bool player_is_disconnected(ServerState& state, PSPLAYER pJ)
{
	/// \todo replace the parameter by a char pointer

	bool result = false;
	player_map(state, [&result, pJ](PSPLAYER pJ_new) {
		if((pJ_new->estado == PLAYER_STATUS_DESCONECTADO) &&
		        strcmp(pJ->name, pJ_new->name) == 0
		  ) {
			result = true;
		}
	}, PlayerMapPolicy::everyone);
	return result;
}

/* insert all the player but the ones in GAME OVER */
void player_insert_scores(ServerState& state, PSPLAYER pJ)
{
	scores_insert_player(state, pJ);
}

/* kick a robot from the game */
static void player_kick_robot(ServerState& state, PSPLAYER pJ)
{
	if(! pJ->human) {
		player_del_hard(state, pJ);
		con_text_out_wop(state, M_INF, _("Robot %s was kicked from the game\n"), pJ->name);
	}
}

/* kick robots when no human is available */
TEG_STATUS player_kick_unparent_robots(ServerState& state)
{
	if(state.server.kick_unparent_robots) {
		PlayerCount counts = player_count(state);

		if(counts.robots && ! counts.humans) {
			con_text_out_wop(state, M_INF, _("Kicking unwanted robots...\n"));
			player_map_erasable(state, [&state](PlayersList::iterator it) {
				player_kick_robot(state, &*it);
			});
		}
	}
	return TEG_STATUS_SUCCESS;
}

SPLAYER *find_next_player(ServerState& state, SPLAYER* after, std::function<bool(SPLAYER&)> acceptable)
{
	using It = PlayersList::iterator; // just a shortcut for the iterator type

	// find the element after which we search for the next one
	It const playerIt = std::find_if(state.players.begin(), state.players.end(),
	[after](SPLAYER& p) {
		return after == &p;
	});

	if(playerIt == state.players.end()) {
		PDEBUG("Internal error: request for non-existing player (%p).", after);
		return nullptr;
	}

	{
		// Search in the interval after playerIt to the end, if one wanted element is there
		It inEnd = std::find_if(std::next(playerIt), state.players.end(), acceptable);
		if(inEnd != state.players.end()) {
			return &*inEnd;
		}
	}

	{
		// There was nothing in the second part of the list, search the [begin, playerIt) interval
		It inBeginning = std::find_if(state.players.begin(), playerIt, acceptable);
		if(inBeginning != playerIt) {
			return &*inBeginning;
		}
	}

	return nullptr;
}

}
