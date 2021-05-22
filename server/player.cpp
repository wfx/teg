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

using PlayersList = std::list<SPLAYER>;

PlayersList players;

typedef struct {
	int humans;
	int robots;
} PlayerCount;

PlayerCount player_count(void)
{
	PlayerCount result = {0, 0};

	player_map([&result](PSPLAYER pJ) {
		if(pJ->human) {
			result.humans++;
		} else {
			result.robots++;
		}
	});
	return result;
}

TEG_STATUS player_whois(int numjug, PSPLAYER *pJ)
{
	TEG_STATUS result=TEG_STATUS_PLAYERNOTFOUND;
	player_map([numjug, &result, pJ](PSPLAYER pJ_new) {
		if(pJ_new->numjug == numjug) {
			*pJ = pJ_new;
			result = TEG_STATUS_SUCCESS;
		}
	});
	return result;
}

TEG_STATUS player_findbyname(char const *name, PSPLAYER *pJ)
{
	TEG_STATUS result = TEG_STATUS_PLAYERNOTFOUND;
	player_map([name, &pJ, &result](PSPLAYER pJ_new) {
		if(!strncmp(pJ_new->name, name, sizeof(pJ_new->name))) {
			*pJ = pJ_new;
			result = TEG_STATUS_SUCCESS;
		}
	});
	return result;
}

void player_erase(PSPLAYER pJ)
{
	auto player_it{std::find_if(players.begin(), players.end(),
	                            [pJ](SPLAYER& p)
	{
		return pJ == &p;
	})};
	if(player_it != players.end()) {
		players.erase(player_it);
	}
}

void player_yield_color(SPLAYER& player)
{
	if(player.color != -1) {
		color_del(player.color);
	}
}

bool player_is_to_be_deleted(SPLAYER const& player)
{
	return (PLAYER_STATUS_DESCONECTADO==player.estado)
	       || (-1 == player.fd);
}

void player_delete_discon(PSPLAYER pJ)
{
	if(player_is_to_be_deleted(*pJ)) {
		player_yield_color(*pJ);
		player_erase(pJ);
	}
}

void player_map_erasable(std::function<void(PlayersList::iterator it)> fn)
{
	for(auto it=players.begin(); it!=players.end(); /*advance in body*/) {
		auto follow=std::next(it); // Make a copy of the following iterator
		fn(it);
		/* it may be now invalid, and so is (it++). This is why we already
		 * stored the following iterator */
		it=follow;
	}
}

void player_delete_all_disconnected()
{
	player_map_erasable([](PlayersList::iterator it) {
		if(player_is_to_be_deleted(*it)) {
			player_yield_color(*it);
			players.erase(it);

		}
	});
}

void player_initplayer(PSPLAYER pJ)
{
	assert(pJ);

	InitializeListHead(&pJ->countries);
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

TEG_STATUS player_numjug_libre(int *libre)
{
	char jugs[maximum_player_count] = {0};
	int i;

	assert(libre);

	player_map([&jugs](PSPLAYER pJ) {
		if(pJ->numjug >= 0 && pJ->numjug < maximum_player_count) {
			jugs[pJ->numjug] = 1;
		}
	});

	for(i=0; i<maximum_player_count; i++) {
		if(jugs[i] == 0) {
			*libre = i;
			return TEG_STATUS_SUCCESS;
		}
	}

	/* server is full */
	return TEG_STATUS_ERROR;
}

/* creates a player and initialize it */
PSPLAYER player_ins(PSPLAYER pJ, bool esplayer)
{
	int numjug;

	assert(pJ);

	if(esplayer && player_numjug_libre(&numjug) != TEG_STATUS_SUCCESS) {
		return NULL;
	}

	pJ->numjug = -1;
	pJ->color = -1;
	players.push_back(*pJ);

	SPLAYER &newJ{players.back()};
	player_initplayer(&newJ);

	newJ.is_player = esplayer;
	newJ.estado = PLAYER_STATUS_CONNECTED;

	if(esplayer) {
		newJ.numjug = numjug;
		g_game.players++;
	}

	g_game.connections++;

	return &newJ;
}

TEG_STATUS player_flush()
{
	for(SPLAYER &player: players) {
		if(player.fd > 0) {
			fd_remove(player.fd);
			player.fd = -1;
		}
		con_text_out(M_INF, ("Deleting %s\n"), player.name);
	}

	g_game.connections = 0;
	g_game.players = 0;
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
TEG_STATUS player_give_turn_away(PSPLAYER pJ)
{

	PLAYER_STATUS status = pJ->estado;

	/* needed to prevent loops in xxx_next or xxx_prev algorithms */
	pJ->estado = PLAYER_STATUS_GAMEOVER;

	/* si el player tenia el turno, lo tiene que pasar al sig*/
	if(g_game.turno &&  g_game.turno->numjug == pJ->numjug) {
		if(status <= PLAYER_STATUS_POSTFICHAS) {
			fichas_next();
		} else if(status <= PLAYER_STATUS_POSTFICHAS2) {
			fichas2_next();
		} else if(status <= PLAYER_STATUS_POSTFICHASC) {
			fichasc_next();
		} else { /* tenia el turno */
			turno_next();
		}
	}

	pJ->estado = status;

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS player_del_soft(PSPLAYER pJ)
{
	assert(pJ);

	if(! player_is_playing(pJ)) {
		return TEG_STATUS_ERROR;
	}

	g_game.playing--;

	/* game without players */
	if(JUEGO_EMPEZADO && g_game.playing == 0) {

		con_text_out(M_INF, _("Game without players. Initializing another game.\n"));
		game_end(NULL);

		/* game with just one player... the winner */
	} else if(g_game.playing == 1 && JUEGO_EMPEZADO) {

		PSPLAYER winner=NULL;
		player_map([pJ, &winner](PSPLAYER pJ2) {
			if(pJ2->numjug!=pJ->numjug && player_is_playing(pJ2)) {
				con_text_out(M_INF, _("Game with one player. Player %s(%d) is the winner\n"), pJ2->name, pJ2->numjug);
				pJ2->estado = PLAYER_STATUS_GAMEOVER;
				winner = pJ2;
			}
		});

		if(winner != nullptr) {
			game_end(winner);
		}

		/* game may continue normally */
	} else {
		player_give_turn_away(pJ);
		pJ->estado = PLAYER_STATUS_GAMEOVER;
	}

	return TEG_STATUS_SUCCESS;
}

void player_del_hard(PSPLAYER pJ)
{
	assert(pJ);

	g_game.connections--;

	/* close the connection */
	fd_remove(pJ->fd);
	pJ->fd = -1;

	if(pJ->is_player) {

		con_text_out(M_INF, _("Player %s(%d) quit the game\n"), pJ->name, pJ->numjug);
		netall_printf(TOKEN_EXIT"=%d\n", pJ->numjug);

		if(player_is_playing(pJ)) {


			player_del_soft(pJ);

			g_game.players--;
			pJ->status_before_discon = PLAYER_STATUS_IDLE;
			pJ->estado = PLAYER_STATUS_DESCONECTADO;
			return;

		} else if(pJ->estado == PLAYER_STATUS_GAMEOVER) {

			g_game.players--;
			pJ->status_before_discon = PLAYER_STATUS_GAMEOVER;
			pJ->estado = PLAYER_STATUS_DESCONECTADO;
			return;
		}

		color_del(pJ->color);
		g_game.players--;

	} else {
		con_text_out(M_INF, _("Observer %s(%d) quit the game\n"), pJ->name, pJ->numjug);
	}

	player_erase(pJ);

	return;
}

TEG_STATUS player_from_indice(int j, int *real_j)
{
	TEG_STATUS result = TEG_STATUS_PLAYERNOTFOUND;
	int i=0;

	player_map([&i, j, &real_j, &result](PSPLAYER pJ) {
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

TEG_STATUS player_asignarcountry(int numjug, PCOUNTRY p)
{
	PSPLAYER pJ;

	if(player_whois(numjug, &pJ) != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_PLAYERNOTFOUND;
	}

	InsertTailList(&pJ->countries, (PLIST_ENTRY) p);
	p->numjug = numjug;
	pJ->tot_countries++;
	pJ->tot_armies++;		/* cada country viene con un ejercito */
	return TEG_STATUS_SUCCESS;
}

/* given a fd, return the player who owns it */
TEG_STATUS player_whoisfd(int fd, PSPLAYER *j)
{
	TEG_STATUS result = TEG_STATUS_PLAYERNOTFOUND;

	player_map([fd, &j, &result](PSPLAYER i) {
		if(i->fd == fd) {
			*j = i;
			result = TEG_STATUS_SUCCESS;
		}
	}, PlayerMapPolicy::everyone);
	return result;
}

bool player_esta_xxx(int fd, PLAYER_STATUS state, bool exact)
{
	PSPLAYER pJ;
	return player_esta_xxx_plus(fd, state, exact, &pJ);
}

bool player_esta_xxx_plus(int fd, PLAYER_STATUS state, bool strict, PSPLAYER *j)
{
	if(player_whoisfd(fd, j) == TEG_STATUS_SUCCESS) {
		if(strict) {
			return ((*j)->estado == state);
		} else {
			return ((*j)->estado >= state);
		}
	} else {
		return false;
	}
}

void player_listar_countries(PSPLAYER pJ, int *countries)
{
	PLIST_ENTRY list;
	PCOUNTRY pP;

	assert(pJ);
	assert(countries);


	list = pJ->countries.Flink;

	while(!IsListEmpty(&pJ->countries) && (list != &pJ->countries)) {
		pP = (PCOUNTRY) list;

		countries[ pP->continente ]++;

		list = LIST_NEXT(list);
	}
}

void player_listar_conts(PSPLAYER pJ, unsigned long *ret)
{
	int countries[CONT_CANT];
	int i;

	assert(pJ);
	assert(ret);

	memset(countries, 0, sizeof(countries));

	player_listar_countries(pJ, countries);

	*ret = 0;

	for(i=0; i< CONT_CANT; i++) {
		if(countries[i] == g_conts[i].cant_countries) {
			*ret |= 1 << i;
		}
	}
}

void player_clear_turn(PSPLAYER pJ)
{
	int i;

	assert(pJ);

	pJ->turno_conq = 0;
	pJ->country_src = -1;
	pJ->country_dst = -1;
	pJ->estado =  PLAYER_STATUS_IDLE;

	/* clean all the regroups the player could have done */
	for(i=0; i<COUNTRIES_CANT; i++) {
		if(g_countries[i].numjug == pJ->numjug) {
			g_countries[i].ejer_reagrupe = 0;
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

void player_all_set_status(PLAYER_STATUS estado)
{
	player_map([estado](PSPLAYER pJ) {
		if(pJ->estado >= PLAYER_STATUS_GAMEOVER) {
			pJ->estado = estado;
		}
	});
}

void player_map(jug_map_func func,
                PlayerMapPolicy policy)
{
	assert(func);

	std::for_each(players.begin(), players.end(),
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

void player_poner_perdio(PSPLAYER pJ)
{
	assert(pJ);

	if(pJ->estado == PLAYER_STATUS_DESCONECTADO) {
		scores_insert_player(pJ);
		player_delete_discon(pJ);
	}

	// We get called if a player's last country is conquered. If the player
	// surrendered before that happened we must NOT decrement the player count again
	if(player_is_playing(pJ)) {
		g_game.playing--;
	}

	pJ->estado = PLAYER_STATUS_GAMEOVER;
}


void player_fillname(PSPLAYER pJ, char *name)
{
	PSPLAYER pJ_new;
	char new_name [ max_playername_length ];

	memset(new_name, 0, sizeof(new_name));
	strncpy(new_name, name, sizeof(new_name) -1);

	strip_invalid(new_name);
	if(player_findbyname(new_name, &pJ_new) == TEG_STATUS_SUCCESS && pJ_new->estado != PLAYER_STATUS_DESCONECTADO) {
		/* that name is already registered, assign a new name dynamically */
		size_t n = strlen(new_name);
		if(n < (sizeof(pJ->name) - 2)) {
			new_name[n] = '_';
			player_fillname(pJ, new_name);
		} else {
			if(new_name[n-1] < '0' || new_name[n-1] > '9') {
				new_name[n-1]='0';
			} else {
				new_name[n-1]++;
			}
			player_fillname(pJ, new_name);
		}
	} else {
		strncpy(pJ->name, new_name, sizeof(pJ->name)-1);
		pJ->name[ sizeof(pJ->name) -1 ] = 0;
	}
}

/* return a disconected player with the same name as pJ */
PSPLAYER player_return_disconnected(PSPLAYER pJ)
{

	PSPLAYER result = nullptr;
	player_map([pJ, &result](PSPLAYER pJ_new) {
		if((pJ_new->estado == PLAYER_STATUS_DESCONECTADO) &&
		        strcmp(pJ->name, pJ_new->name) == 0
		  ) {
			g_game.players++;
			g_game.playing++;
			g_game.connections++;
			result = pJ_new;
		}
	}, PlayerMapPolicy::everyone);
	return result;
}

/* return true if the player is disconnected */
bool player_is_disconnected(PSPLAYER pJ)
{
	/// \todo replace the parameter by a char pointer

	bool result = false;
	player_map([&result, pJ](PSPLAYER pJ_new) {
		if((pJ_new->estado == PLAYER_STATUS_DESCONECTADO) &&
		        strcmp(pJ->name, pJ_new->name) == 0
		  ) {
			result = true;
		}
	}, PlayerMapPolicy::everyone);
	return result;
}

/* insert all the player but the ones in GAME OVER */
void player_insert_scores(PSPLAYER pJ)
{
	scores_insert_player(pJ);
}

/* kick a robot from the game */
static void player_kick_robot(PSPLAYER pJ)
{
	if(! pJ->human) {
		player_del_hard(pJ);
		con_text_out_wop(M_INF, _("Robot %s was kicked from the game\n"), pJ->name);
	}
}

/* kick robots when no human is available */
TEG_STATUS player_kick_unparent_robots(void)
{
	if(g_server.kick_unparent_robots) {
		PlayerCount counts = player_count();

		if(counts.robots && ! counts.humans) {
			con_text_out_wop(M_INF, _("Kicking unwanted robots...\n"));
			player_map_erasable([](PlayersList::iterator it) {
				player_kick_robot(&*it);
			});
		}
	}
	return TEG_STATUS_SUCCESS;
}

SPLAYER *find_next_player(SPLAYER* after, std::function<bool(SPLAYER&)> acceptable)
{
	using It = PlayersList::iterator; // just a shortcut for the iterator type

	// find the element after which we search for the next one
	It const playerIt = std::find_if(players.begin(), players.end(),
	[after](SPLAYER& p) {
		return after == &p;
	});

	if(playerIt == players.end()) {
		PDEBUG("Internal error: request for non-existing player (%p).", after);
		return nullptr;
	}

	{
		// Search in the interval after playerIt to the end, if one wanted element is there
		It inEnd = std::find_if(std::next(playerIt), players.end(), acceptable);
		if(inEnd != players.end()) {
			return &*inEnd;
		}
	}

	{
		// There was nothing in the second part of the list, search the [begin, playerIt) interval
		It inBeginning = std::find_if(players.begin(), playerIt, acceptable);
		if(inBeginning != playerIt) {
			return &*inBeginning;
		}
	}

	return nullptr;
}

}
