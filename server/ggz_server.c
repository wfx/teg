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
/*
 * supports for GGZ code
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>

#include "server.h"
#include "player.h"

#ifdef WITH_GGZ

#include <ggzdmod.h>
#include <ggzdmod-ggz.h>

static GGZdMod *g_ggzdmod = NULL;

static void _handle_ggz_state(GGZdMod *ggz, GGZdModEvent event, const void *data);
static void _handle_ggz_error(GGZdMod *ggz, GGZdModEvent event, const void *data);

static void _handle_ggz_join(GGZdMod *ggz, GGZdModEvent event, const void *data);
static void _handle_ggz_leave(GGZdMod *ggz, GGZdModEvent event, const void *data);
static void _handle_ggz_player(GGZdMod *ggz, GGZdModEvent event, const void *data);

static void _handle_ggz_spectator_join(GGZdMod *ggz, GGZdModEvent event, const void *data);
static void _handle_ggz_spectator_leave(GGZdMod *ggz, GGZdModEvent event, const void *data);
static void _handle_ggz_spectator(GGZdMod *ggz, GGZdModEvent event, const void *data);


/* Setup game state and board */
static int teg_ggz_init()
{
	g_ggzdmod = ggzdmod_new(GGZDMOD_GAME);
	if( ! g_ggzdmod )
		return -1;

	/* Setup GGZ game module */
	ggzdmod_set_handler(g_ggzdmod, GGZDMOD_EVENT_STATE, &_handle_ggz_state);
	ggzdmod_set_handler(g_ggzdmod, GGZDMOD_EVENT_ERROR, &_handle_ggz_error);

	ggzdmod_set_handler(g_ggzdmod, GGZDMOD_EVENT_JOIN, &_handle_ggz_join);
	ggzdmod_set_handler(g_ggzdmod, GGZDMOD_EVENT_LEAVE, &_handle_ggz_leave);
	ggzdmod_set_handler(g_ggzdmod, GGZDMOD_EVENT_PLAYER_DATA, &_handle_ggz_player);

	ggzdmod_set_handler(g_ggzdmod, GGZDMOD_EVENT_SPECTATOR_JOIN, &_handle_ggz_spectator_join);
	ggzdmod_set_handler(g_ggzdmod, GGZDMOD_EVENT_SPECTATOR_LEAVE, &_handle_ggz_spectator_leave);
	ggzdmod_set_handler(g_ggzdmod, GGZDMOD_EVENT_SPECTATOR_DATA, &_handle_ggz_spectator);

	return 0;
}
/* Callback for GGZDMOD_EVENT_STATE */
static void _handle_ggz_state(GGZdMod *ggz, GGZdModEvent event, const void *data)
{
	switch(ggzdmod_get_state(ggz)) {
	case GGZDMOD_STATE_PLAYING:
		break;
	case GGZDMOD_STATE_WAITING:
	{
		GGZSeat seat;
		int i;
		int robot_socket;
		int max = ggzdmod_get_num_seats( g_ggzdmod );

		// launch robots if necesary
		for (i = 0; i < max; i++) {
			seat = ggzdmod_get_seat( g_ggzdmod, i );
			if( seat.type == GGZ_SEAT_BOT ) {
				if (launch_robot( &robot_socket, "--ggz" ) == TEG_STATUS_SUCCESS )
				{
					seat.fd = robot_socket;

					ggzdmod_set_seat( g_ggzdmod, &seat );

					net_printf(seat.fd,"%s=1\n",TOKEN_GGZ);
				} else
					ggzdmod_log(g_ggzdmod, "\tegserver: Failed launching bot :-(!");
			}
		}
		break;
	}
	case GGZDMOD_STATE_DONE:
		break;
	default:
		break;
	}
}

static int seats_is_full(void)
{
	return ggzdmod_count_seats(g_ggzdmod, GGZ_SEAT_OPEN)
		+ ggzdmod_count_seats(g_ggzdmod, GGZ_SEAT_RESERVED) == 0;
}

/* Callback for GGZDMOD_EVENT_ERROR */
static void _handle_ggz_error(GGZdMod *ggz, GGZdModEvent event, const void *data)
{
}

/* Callback for GGZDMOD_EVENT_JOIN */
static void _handle_ggz_join(GGZdMod *ggz, GGZdModEvent event, const void *data)
{
	GGZSeat *old_seat = (GGZSeat*)data;
	GGZSeat new_seat = ggzdmod_get_seat(ggz, old_seat->num);

	net_printf(new_seat.fd,"%s=1\n", TOKEN_GGZ);
	
	/* XXX: We start playing only when there are no open seats. */
	if (seats_is_full())
		ggzdmod_set_state( g_ggzdmod, GGZDMOD_STATE_PLAYING);
}

/* Callback for GGZDMOD_EVENT_LEAVE */
static void _handle_ggz_leave(GGZdMod *ggz, GGZdModEvent event, const void *data)
{
	GGZSeat *s = (GGZSeat*) data;
	token_exit( s->fd );

	if( ggzdmod_count_seats(ggz, GGZ_SEAT_PLAYER) == 0 )
		// the game will exit when all human players are gone
		ggzdmod_set_state(ggz, GGZDMOD_STATE_DONE);
}

/* Handle message from player */
static void _handle_ggz_player(GGZdMod *ggz, GGZdModEvent event, const void *data)
{
	int num = *(int*)data;
	int fd;
	GGZSeat seat;

	seat = ggzdmod_get_seat(ggz, num);
	fd = seat.fd;
	if(play_teg( fd )== TEG_STATUS_CONNCLOSED) {
		seat.type = GGZ_SEAT_OPEN;
		ggzdmod_set_seat( g_ggzdmod, &seat );
	}
}

/* Callback for GGZDMOD_EVENT_SPECTATOR_JOIN */
static void _handle_ggz_spectator_join(GGZdMod *ggz, GGZdModEvent event, const void *data)
{
	GGZSpectator s;
	s = ggzdmod_get_spectator( ggz, *(int*) data );

/*	fprintf( stderr, "new spectator in fd: %d-%s (%d)\n",s.fd,s.name, *(int*) data ); */

	net_printf(s.fd,"%s=0\n", TOKEN_GGZ);
}

/* Callback for GGZDMOD_EVENT_SPECTATOR_LEAVE */
static void _handle_ggz_spectator_leave(GGZdMod *ggz, GGZdModEvent event, const void *data)
{
	GGZSeat *s = (GGZSeat*) data;
	
	token_exit( s->fd );
}

/* Handle message from observer*/
static void _handle_ggz_spectator(GGZdMod *ggz, GGZdModEvent event, const void *data)
{
	GGZSpectator s;
	s = ggzdmod_get_spectator( ggz, *(int*) data );
	
/*	fprintf( stderr, "data from spectator in fd: %d-%s (%d)\n",s.fd,s.name, *(int*) data ); */

	if(play_teg( s.fd )== TEG_STATUS_CONNCLOSED) {
		/* XXX: What do I have to do when an spectator leaves ? */
	}
}

TEG_STATUS ggz_server_find_ggzname( int fd, char *n, int len )
{
	int max, i;
	GGZSeat seat;
	GGZSpectator spectator;

	if(!n)
		return TEG_STATUS_ERROR;

	max = ggzdmod_get_num_seats( g_ggzdmod );
	for(i=0;i<max;i++) {
		seat = ggzdmod_get_seat( g_ggzdmod, i );
		if( seat.fd == fd ) {
			/* dont update robot's names */
			if( seat.type == GGZ_SEAT_BOT )
				return TEG_STATUS_ERROR;

			if( seat.name ) {
				strncpy(n,seat.name,len);
				n[len]=0;
				return TEG_STATUS_SUCCESS;
			} else
				return TEG_STATUS_ERROR;
		}
	}

	/* Ok, I cound't find the name in seats, find it in spectators */
	max = ggzdmod_get_max_num_spectators( g_ggzdmod );
	for(i=0;i<max;i++) {
		spectator = ggzdmod_get_spectator( g_ggzdmod, i );
		if( spectator.fd == fd ) {
			if( spectator.name ) {
				strncpy(n,spectator.name,len);
				n[len]=0;
				return TEG_STATUS_SUCCESS;
			}
		}
	}

	return TEG_STATUS_ERROR;
}

void ggz_server_gameover( int fd )
{
	int max, i;
	GGZSeat seat;
	GGZGameResult results[TEG_MAX_PLAYERS];
	int scores[TEG_MAX_PLAYERS];
	PLIST_ENTRY pL;
	PSPLAYER pJ;

	max = ggzdmod_get_num_seats( g_ggzdmod );

	for(i=0;i<max;i++) {
		seat = ggzdmod_get_seat( g_ggzdmod, i );
		if( seat.fd == fd ) {
			results[i] = GGZ_GAME_WIN;
		} else {
			results[i] = GGZ_GAME_LOSS;
		}

		scores[i] = 0;
		pL = g_list_player.Flink;
		while( !IsListEmpty( &g_list_player ) && (pL != &g_list_player) ) {
			pJ = (PSPLAYER) pL;
			if( pJ->fd == seat.fd ){
				scores[i] = pJ->player_stats.score;
			}
			pL = LIST_NEXT(pL);
		}
	}

	ggzdmod_report_game( g_ggzdmod, NULL, results, scores );
}

int ggz_server_main_loop()
{
	if( teg_ggz_init() == -1 )
		return -1;

	/* Connect to GGZ server; main loop */
	if (ggzdmod_connect( g_ggzdmod) < 0)
		return -1;

	(void) ggzdmod_loop( g_ggzdmod );
	(void) ggzdmod_disconnect( g_ggzdmod );

	ggzdmod_free( g_ggzdmod );
	
	return 0;
}


#endif /* WITH_GGZ */
