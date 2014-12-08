/*	$Id: player.h,v 1.1 2002/09/09 14:09:14 riq Exp $	*/
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

#ifndef __TEGC_CPLAYER_H
#define __TEGC_CPLAYER_H

typedef struct _player {
	LIST_ENTRY next;
	char name[PLAYERNAME_MAX_LEN];
	char addr[PLAYERADDR_MAX_LEN];
	int color;
	int score;
	int numjug;
	PLAYER_STATUS estado;
	int tot_countries;
<<<<<<< HEAD
	int tot_countries_won;
	int tot_countries_lost;
	int tot_armies;
	int tot_armies_killed;
	int tot_armies_lost;
=======
	int tot_armies;
>>>>>>> b8e1f4d5000e931e81e397369fa5ec789dffe26c
	int tot_cards;
	int empezo_turno;		/* dice si empezo el turno */
	int human;
} CPLAYER, *PCPLAYER;


/* funciones exportadas */
extern LIST_ENTRY g_list_player;

TEG_STATUS player_whois( int numjug, PCPLAYER *j);
TEG_STATUS player_update( PCPLAYER j );
PCPLAYER player_ins( PCPLAYER j);
TEG_STATUS player_del( PCPLAYER j );
TEG_STATUS player_flush();
TEG_STATUS player_init();

#endif /* __TEGC_CPLAYER_H */
