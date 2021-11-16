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

#include <stdbool.h>

#include "../common/stats.h"
#include "../common/country.h"

/// server player data structure
typedef struct _player {
	LIST_ENTRY next;
	int numjug;				/**< player number */
	char name[PLAYERNAME_MAX_LEN];		/**< name */
	char addr[PLAYERADDR_MAX_LEN];		/**< internet address */
	int color;				/**< color */
	LIST_ENTRY countries;			/**< countries that he owns */
	LIST_ENTRY deals;			/**< FIXME: Not implemented yet */
	int mission;				/**< mission that the player has */
	BOOLEAN hizo_canje;			/**< exchange done ? */
	BOOLEAN is_player;			/**< player or observer */
	int turno_conq;				/**< Countries conquered in the turn.
						  A player must conquer 1 countrie to ask for cards.
						  And 2 countries, after the 3rd exchange */
	int tot_countries;			/**< number of countries (optimization) */
	int tot_armies;				/**< number of armies (") */
	int tot_cards;				/**< number of cards (") */
	unsigned tot_exchanges;			/**< number of exchanges */
	int fd;					/**< file descriptor */
	PLAYER_STATUS estado;			/**< status of the player */
	PLAYER_STATUS status_before_discon;	/**< status before disconn */
	int country_src;				/**< country source (TOKEN_TROPAS) */
	int country_dst;				/**< country dst (TOKEN_TROPAS) */
	BOOLEAN human;				/**< Is this player controlled by a human */

	int fichasc_armies;			/**< Number of armies to place */
	unsigned int fichasc_conts;		/**< Number of conquered continents */

	PLAYER_STATS player_stats;		/**< player statistics */

} SPLAYER, *PSPLAYER;

/// \brief Mapping function type to traverse all players
typedef void (*jug_map_func)(PSPLAYER pJ);

/// \brief List of all connected players
extern LIST_ENTRY g_list_player;

/** \brief Initialize a single player datastructure for a new game
 *
 * Only the fields needed to be reset for a new game are touched, all other
 * fields are kept unchanged
 */
void player_initplayer(PSPLAYER j);

/// \brief Initialize the entire player list
void player_init(void);

/// \brief Create a new player datastructure and populate it with \p j.
PSPLAYER player_ins(PSPLAYER j, BOOLEAN esplayer);

/// \brief Let a player join
#define player_ins_player(a) player_ins(a,TRUE)

/// \brief Let an observer join
#define player_ins_ro(a) player_ins(a,FALSE)

/// \brief Deletes a player
void player_del_hard(PSPLAYER j);

/// \brief Puts a player in a GAME OVER state
TEG_STATUS player_del_soft(PSPLAYER pJ);

/// \brief delete all players
TEG_STATUS player_flush();

/// \brief Assigns a country \p p to a player number \p numjug
TEG_STATUS player_asignarcountry(int numjug, PCOUNTRY p);

/// \brief given a players' number it returns a pointer the player
TEG_STATUS player_whois(int numjug, PSPLAYER *j);

/// \brief given a players' number it returns a pointer the player
TEG_STATUS player_whoisfd(int fd, PSPLAYER *j);

/**
 * \brief tells if a player is in a given state
 *
 * This function checks, if the player behind \p fd is
 *   * (\p exact == true ) in \p state
 *   * (\p exact == false) at least in \p state
 */
bool player_esta_xxx(int fd, PLAYER_STATUS state, bool exact);

/**
 * \brief Search a player and tells if this player is in a given state
 *
 * This function checks, if the player behind \p fd is
 *   * (\p exact == true ) in \p state
 *   * (\p exact == false) at least in \p state
 */
bool player_esta_xxx_plus(int fd, PLAYER_STATUS state, bool exact, PSPLAYER *j);

/// \brief quantity of countries per contient that a player has
void player_listar_countries(PSPLAYER pJ, int *countries);

/// \brief Calculate the bitfield where the player has at least one country
void player_listar_conts(PSPLAYER pJ, unsigned long *ret);

/// \brief Initialize the start turn variables
void player_clear_turn(PSPLAYER j);

/** Calculate the number of armies the player can place in the next turn,
 *  without additional armies from card exchanges or complete conquered
 *  continents.
 */
int player_fichasc_cant(PSPLAYER pJ);

/// \brief put all the players in a given state
void player_all_set_status(PLAYER_STATUS);

/// \brief Tell if the player lost the game
bool player_is_lost(PSPLAYER pJ);

/// \brief Put the player in GAMEOVER state
void player_poner_perdio(PSPLAYER pJ);

/// \brief returns a free number for the player
TEG_STATUS player_numjug_libre(int *libre);

/// \brief  given an index of player [0..MAX_PLAYERS] return the numjug of it
TEG_STATUS player_from_indice(int j, int *real_j);

/// \brief perform \p func on every player
void player_map(jug_map_func func);

/// \brief finds a player given its name
TEG_STATUS player_findbyname(char *name, PSPLAYER *pJ);

/// \brief Assigns a name to the player that does not conflict with another names
void player_fillname(PSPLAYER pJ, char *name);

/// \brief Tells if a player is playing
bool player_is_playing(PSPLAYER pJ);

/*! return the PSPLAYER that is disconnected */
PSPLAYER player_return_disconnected(PSPLAYER pJ);

/*! return TRUE if pJ is a disconnected player */
BOOLEAN player_is_disconnected(PSPLAYER pJ);

/*! deletes the player if it disconnected */
void player_delete_discon(PSPLAYER pJ);

/*! insert all players in scores but the ones in GAMEOVER */
void player_insert_scores(PSPLAYER pJ);

/*! kick robots when there are no humans */
TEG_STATUS player_kick_unparent_robots(void);

#define SPLAYER_CONNECTED(a) player_esta_xxx(a,PLAYER_STATUS_CONNECTED,0)
#define SPLAYER_HABILITADO(a) player_esta_xxx(a,PLAYER_STATUS_HABILITADO,0)
#define SPLAYER_HABILITADO_P(a,j) player_esta_xxx_plus(a,PLAYER_STATUS_HABILITADO,0,j)
#define SPLAYER_FICHAS(a) player_esta_xxx(a,PLAYER_STATUS_FICHAS,1)
#define SPLAYER_FICHAS2(a) player_esta_xxx(a,PLAYER_STATUS_FICHAS2,1)
#define SPLAYER_ATAQUE(a) player_esta_xxx(a,PLAYER_STATUS_ATAQUE,1)
#define SPLAYER_ATAQUE_P(a,j) player_esta_xxx_plus(a,PLAYER_STATUS_ATAQUE,1,j)
#define SPLAYER_TROPAS(a) player_esta_xxx(a,PLAYER_STATUS_TROPAS,1)
#define SPLAYER_TROPAS_P(a,j) player_esta_xxx_plus(a,PLAYER_STATUS_TROPAS,1,j)
