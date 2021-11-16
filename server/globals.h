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

#include "../common/common.h"
#include "player.h"

typedef struct _sgame {
	int connections;	/**< quantity of connections */
	int players;		/**< quantity of players */
	int playing;		/**< quantity of players playing */
	BOOLEAN mission;	/**< with secret mission or not */
	BOOLEAN cmission;	/**< with common secret mission or not */
	BOOLEAN fog_of_war;	/**< play with fog of war */
	PSPLAYER player_fow;	/**< player who request a Fog of War command */
	int reglas;		/**< reglas a jugar: TEG o RISK */
	PSPLAYER turno;		/**< de quien es el turno */
	PSPLAYER empieza_turno;	/**< quien empezo el turno 1ro */
	PSPLAYER old_turn;	/**< the player who had the previous turn */
	int estado;		/**< se empezo a jugar o no */
	int fichas;		/**< initial armies (default 5)*/
	int fichas2;		/**< initial armies2 (default 3)*/
	unsigned int seed;	/**< seed for random */
	int gamenumber;		/**< game number */
	int round_number;	/**< round number */
} SGAME, *PSGAME;
extern SGAME g_game;

enum {
	JUEGO_ESTADO_ESPERANDO,	/* no se comenzo y pueder ingresar playeres */
	JUEGO_ESTADO_EMPEZADO,	/* no pueden entrar mas playeres */
	JUEGO_ESTADO_FICHAS,	/* poner las 5 fichas iniciales */
	JUEGO_ESTADO_FICHAS2,	/* poner las 3 fichas iniciales */
	JUEGO_ESTADO_TURNO,	/* esta en un cambio de turno */
	JUEGO_ESTADO_ATAQUE	/* se esta en pleno ataque */
};

#define JUEGO_EMPEZADO		(g_game.estado >= JUEGO_ESTADO_EMPEZADO)
#define JUEGO_ESPERANDO		(g_game.estado == JUEGO_ESTADO_ESPERANDO)

#define JUEGO_EN_TURNO		(g_game.estado = JUEGO_ESTADO_TURNO)
#define JUEGO_EN_EMPEZAR	(g_game.estado = JUEGO_ESTADO_EMPEZADO)
#define JUEGO_EN_FICHAS		(g_game.estado = JUEGO_ESTADO_FICHAS)
#define JUEGO_EN_FICHAS2	(g_game.estado = JUEGO_ESTADO_FICHAS2)

typedef struct _server {
	char name[SERVER_NAMELEN];
	BOOLEAN debug;		/* debug mode */
	BOOLEAN with_console;	/* enable console mode */
	int port;		/* port to listen to (default: 2000) */
	int max_players;	/* max players ( default: 6 ) */
	int max_connections;	/* max connections ( default: 15 ) */

	char metaserver_name[SERVER_NAMELEN];	/* metaserver name */
	int metaserver_port;	/* port of metaserver */
	BOOLEAN metaserver_on;	/* is the metaserver on */
	BOOLEAN kick_unparent_robots;	/* kick robots when there are no humans */
} SERVER, *PSERVER;
extern SERVER g_server;
