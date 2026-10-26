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

#pragma once

#include <functional>
#include <memory>

#include "../common/common.h"
#include "../common/stats.h"
#include "../common/country.h"
#include "../common/transport.h"

namespace teg::server
{

struct ServerState;

/// server player data structure
struct SPLAYER {
	int numjug;				/**< player number */
	char name[max_playername_length];		/**< name */
	char addr[inet_addr_len];		/**< internet address */
	int color;				/**< color */
	int mission;				/**< mission that the player has */
	bool hizo_canje;			/**< exchange done ? */
	bool is_player;			/**< player or observer */
	int turno_conq;				/**< Countries conquered in the turn.
						  A player must conquer 1 countrie to ask for cards.
						  And 2 countries, after the 3rd exchange */
	int tot_countries;			/**< number of countries (optimization) */
	int tot_armies;				/**< number of armies (") */
	int tot_cards;				/**< number of cards (") */
	unsigned tot_exchanges;			/**< number of exchanges */
	std::shared_ptr<Transport> connection;	/**< connection to the client */
	PLAYER_STATUS estado;			/**< status of the player */
	PLAYER_STATUS status_before_discon;	/**< status before disconn */
	int country_src;				/**< country source (TOKEN_TROPAS) */
	int country_dst;				/**< country dst (TOKEN_TROPAS) */
	bool human;				/**< Is this player controlled by a human */

	int fichasc_armies;			/**< Number of armies to place */
	unsigned int fichasc_conts;		/**< Number of conquered continents */

	PLAYER_STATS player_stats;		/**< player statistics */

};
using PSPLAYER = SPLAYER*;

/// \brief The file descriptor of the player connection, or -1
inline int player_fd(SPLAYER const& player)
{
	return player.connection ? player.connection->fd() : -1;
}

/// \brief Sipmle mapping function type to traverse all players
using jug_map_func=std::function<void(PSPLAYER pJ)>;

/** \brief Initialize a single player datastructure for a new game
 *
 * Only the fields needed to be reset for a new game are touched, all other
 * fields are kept unchanged
 */
void player_initplayer(PSPLAYER j);

/// \brief Initialize the entire player list
void player_init(void);

/// \brief Create a new player datastructure and populate it with \p j.
PSPLAYER player_ins(ServerState& state, PSPLAYER j, bool esplayer);

/// \brief Let a player join
#define player_ins_player(state, a) player_ins(state, a,true)

/// \brief Let an observer join
#define player_ins_ro(state, a) player_ins(state, a,false)

/// \brief Deletes a player
void player_del_hard(ServerState& state, PSPLAYER j);

/// \brief Puts a player in a GAME OVER state
TEG_STATUS player_del_soft(ServerState& state, PSPLAYER pJ);

/// \brief delete all players
TEG_STATUS player_flush(ServerState& state);

/// \brief Assigns a country \p p to a player number \p numjug
TEG_STATUS player_asignarcountry(ServerState& state, int numjug, PCOUNTRY p);

/// \brief given a players' number it returns a pointer the player
TEG_STATUS player_whois(ServerState& state, int numjug, PSPLAYER *j);

/// \brief given a connection it returns a pointer the player
TEG_STATUS player_whose(ServerState& state, std::shared_ptr<Transport> const& connection, PSPLAYER *j);

/**
 * \brief tells if a player is in a given state
 *
 * This function checks, if the player behind \p connection is
 *   * (\p exact == true ) in \p wanted
 *   * (\p exact == false) at least in \p wanted
 */
bool player_esta_xxx(ServerState& state, std::shared_ptr<Transport> const& connection, PLAYER_STATUS wanted, bool exact);

/**
 * \brief Search a player and tells if this player is in a given state
 *
 * This function checks, if the player behind \p connection is
 *   * (\p exact == true ) in \p wanted
 *   * (\p exact == false) at least in \p wanted
 */
bool player_esta_xxx_plus(ServerState& state, std::shared_ptr<Transport> const& connection, PLAYER_STATUS wanted, bool exact, PSPLAYER *j);

/// \brief quantity of countries per contient that a player has
void player_listar_countries(ServerState& state, PSPLAYER pJ, int *countries);

/// \brief Calculate the bitfield where the player has at least one country
void player_listar_conts(ServerState& state, PSPLAYER pJ, unsigned long *ret);

/// \brief Initialize the start turn variables
void player_clear_turn(ServerState& state, PSPLAYER j);

/** Calculate the number of armies the player can place in the next turn,
 *  without additional armies from card exchanges or complete conquered
 *  continents.
 */
int player_fichasc_cant(PSPLAYER pJ);

/// \brief put all the players in a given state
void player_all_set_status(ServerState& state, PLAYER_STATUS);

/// \brief Tell if the player lost the game
bool player_is_lost(PSPLAYER pJ);

/// \brief Put the player in GAMEOVER state
///
/// \warning This function must NEVER be called from within player_map, since it
/// can remove players from the list.
void player_poner_perdio(ServerState& state, PSPLAYER pJ);

/// \brief returns a free number for the player
TEG_STATUS player_numjug_libre(ServerState& state, int *libre);

/// \brief  given an index of player [0..MAX_PLAYERS] return the numjug of it
TEG_STATUS player_from_indice(ServerState& state, int j, int *real_j);

enum class PlayerMapPolicy {
	players_only,
	everyone
};

/// \brief perform \p func on every player
void player_map(ServerState& state, jug_map_func func,
                PlayerMapPolicy policy=PlayerMapPolicy::players_only);

/// \brief finds a player given its name
TEG_STATUS player_findbyname(ServerState& state, const char *name, PSPLAYER *pJ);

/// \brief Assigns a name to the player that does not conflict with another names
void player_fillname(ServerState& state, PSPLAYER pJ, char *name);

/// \brief Tells if a player is playing
bool player_is_playing(PSPLAYER pJ);

/*! return the PSPLAYER that is disconnected */
PSPLAYER player_return_disconnected(ServerState& state, PSPLAYER pJ);

/*! return TRUE if pJ is a disconnected player */
bool player_is_disconnected(ServerState& state, PSPLAYER pJ);

/// Removes all disconnected players from the players list
void player_delete_all_disconnected(ServerState& state);

/*! insert all players in scores but the ones in GAMEOVER */
void player_insert_scores(ServerState& state, PSPLAYER pJ);

/*! kick robots when there are no humans */
TEG_STATUS player_kick_unparent_robots(ServerState& state);

/** Find the entry after \p after in the player list which fullfills predicate
 * \p acceptable. This function will roll over after the end of the list and
 * search for other players at the beginning.
 *
 * \return The following player if there is one fullfilling the predicate,
 *         nullptr otherwise. */
PSPLAYER find_next_player(ServerState& state, SPLAYER* after, std::function<bool(SPLAYER&)> acceptable);

#define SPLAYER_CONNECTED(state, a) player_esta_xxx(state, a,PLAYER_STATUS_CONNECTED,0)
#define SPLAYER_HABILITADO(state, a) player_esta_xxx(state, a,PLAYER_STATUS_HABILITADO,0)
#define SPLAYER_HABILITADO_P(state, a,j) player_esta_xxx_plus(state, a,PLAYER_STATUS_HABILITADO,0,j)
#define SPLAYER_FICHAS(state, a) player_esta_xxx(state, a,PLAYER_STATUS_FICHAS,1)
#define SPLAYER_FICHAS2(state, a) player_esta_xxx(state, a,PLAYER_STATUS_FICHAS2,1)
#define SPLAYER_ATAQUE(state, a) player_esta_xxx(state, a,PLAYER_STATUS_ATAQUE,1)
#define SPLAYER_ATAQUE_P(state, a,j) player_esta_xxx_plus(state, a,PLAYER_STATUS_ATAQUE,1,j)
#define SPLAYER_TROPAS(state, a) player_esta_xxx(state, a,PLAYER_STATUS_TROPAS,1)
#define SPLAYER_TROPAS_P(state, a,j) player_esta_xxx_plus(state, a,PLAYER_STATUS_TROPAS,1,j)

}
