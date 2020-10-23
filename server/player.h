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
 * @file player.h
 */
#ifndef __TEGS_SPLAYER_H
#define __TEGS_SPLAYER_H

#include "server.h"
#include "stats.h"
#include "country.h"

/* server player */
typedef struct _player{
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
	int tot_exchanges;			/**< number of exchanges */
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

typedef void (*jug_map_func)( PSPLAYER pJ);

/*
 * funciones y variables exportadas
 */
extern LIST_ENTRY g_list_player;

void player_initplayer( PSPLAYER j );
void player_init( void );
PSPLAYER player_ins( PSPLAYER j, BOOLEAN esplayer );
#define player_ins_player(a) player_ins(a,TRUE)
#define player_ins_ro(a) player_ins(a,FALSE)
void player_del_hard(PSPLAYER j);
TEG_STATUS player_del_soft( PSPLAYER pJ );
TEG_STATUS player_flush();
TEG_STATUS player_asignarcountry( int numjug, PCOUNTRY p);
TEG_STATUS player_whois( int numjug, PSPLAYER *j);
TEG_STATUS player_whoisfd( int fd, PSPLAYER *j);
BOOLEAN player_esta_xxx( int fd, int condicion, int strict );
BOOLEAN player_esta_xxx_plus( int fd, int condicion, int strict, PSPLAYER *j );
void player_listar_countries( PSPLAYER pJ, int *countries );
void player_listar_conts( PSPLAYER pJ, unsigned long *ret );
TEG_STATUS player_clear_turn( PSPLAYER j );
int player_fichasc_cant( PSPLAYER pJ );
TEG_STATUS player_all_set_status( PLAYER_STATUS );
BOOLEAN player_is_lost( PSPLAYER pJ );
TEG_STATUS player_poner_perdio( PSPLAYER pJ );
TEG_STATUS player_numjug_libre( int *libre);
TEG_STATUS player_from_indice( int j, int *real_j );
TEG_STATUS player_map( jug_map_func func );
TEG_STATUS player_findbyname( char *name, PSPLAYER *pJ);
TEG_STATUS player_fillname( PSPLAYER pJ, char *name);
BOOLEAN player_is_playing( PSPLAYER pJ );
/*! return the PSPLAYER that is disconnected */
PSPLAYER player_return_disconnected( PSPLAYER pJ );
/*! return TRUE if pJ is a disconnected player */
BOOLEAN player_is_disconnected( PSPLAYER pJ );
/*! deletes the player if it disconnected */
void player_delete_discon( PSPLAYER pJ );
/*! insert all players in scores but the ones in GAMEOVER */
void player_insert_scores( PSPLAYER pJ );
/*! kick robots when there are no humans */
TEG_STATUS player_kick_unparent_robots( void );

#define SPLAYER_CONNECTED(a) player_esta_xxx(a,PLAYER_STATUS_CONNECTED,0)
#define SPLAYER_HABILITADO(a) player_esta_xxx(a,PLAYER_STATUS_HABILITADO,0)
#define SPLAYER_HABILITADO_P(a,j) player_esta_xxx_plus(a,PLAYER_STATUS_HABILITADO,0,j)
#define SPLAYER_FICHAS(a) player_esta_xxx(a,PLAYER_STATUS_FICHAS,1)
#define SPLAYER_FICHAS2(a) player_esta_xxx(a,PLAYER_STATUS_FICHAS2,1)
#define SPLAYER_ATAQUE(a) player_esta_xxx(a,PLAYER_STATUS_ATAQUE,1)
#define SPLAYER_ATAQUE_P(a,j) player_esta_xxx_plus(a,PLAYER_STATUS_ATAQUE,1,j)
#define SPLAYER_TROPAS(a) player_esta_xxx(a,PLAYER_STATUS_TROPAS,1)
#define SPLAYER_TROPAS_P(a,j) player_esta_xxx_plus(a,PLAYER_STATUS_TROPAS,1,j)

#endif /* __TEGS_SPLAYER_H */
