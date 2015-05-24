/*	$Id: main.c,v 1.88 2004/08/04 13:03:07 riq Exp $	*/
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
 * initialization functions
 */

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>

#include "server.h"
#include "scores.h"
#include "xmlscores.h"


#undef DEBUG_MAIN

#ifdef DEBUG_MAIN
# define MAIN_DEBUG(x...) PDEBUG(x)
#else
# define MAIN_DEBUG(x)
#endif

/*
 * private variables
 */
static fd_set all_set;
static int max_fd;

/* 
 * global variables 
 */
SGAME g_game;
SERVER g_server;

/* ends the game */
void game_end( PSPLAYER winner )
{
	char strout[PROT_MAX_LEN + PLAYERNAME_MAX_LEN * TEG_MAX_PLAYERS + 200];
	PLIST_ENTRY l = g_list_player.Flink;
	PSPLAYER pJ;

	/* add points to the winner */
	if( winner && g_game.round_number > 0 ) {
		int points = 350 / g_game.round_number;

		winner->player_stats.score += points;
	}

	/* update scores */
	player_map( player_insert_scores );

	if( aux_token_stasta(strout, sizeof(strout) -1 ) == TEG_STATUS_SUCCESS ) {

		/* send the last status to all the players */
		while( !IsListEmpty( &g_list_player ) && (l != &g_list_player) ) {
			pJ = (PSPLAYER) l;

			if( pJ->is_player && pJ->fd != 1)
				net_printf(pJ->fd,TOKEN_STATUS"=%s\n",strout);

			l = LIST_NEXT(l);
		}
	}

	/* delete disconn players */
	player_map( player_delete_discon );

	/* send who is the winner */
	if( winner )
		netall_printf( TOKEN_WINNER"=%d,%d\n",winner->numjug,winner->mission );

	game_new();
}

/* Creates a new game */
void game_new()
{
	static int first_time = 1;

	countries_init();
	mission_init();
	pactos_flush();

	player_map( player_initplayer );
	player_all_set_status( PLAYER_STATUS_HABILITADO );
	g_game.playing = 0;
	

	g_game.turno = NULL;
	g_game.empieza_turno = NULL;
	g_game.estado = JUEGO_ESTADO_ESPERANDO;
	g_game.round_number = 1;

	if( first_time ) {
		first_time = 0;
		g_game.gamenumber = 0;
		g_game.seed = get_int_from_dev_random();
	} else {
		g_game.gamenumber++;
		g_game.seed = rand();
		srand( g_game.seed );
	}

	player_kick_unparent_robots();
}

BOOLEAN game_is_finished( void )
{
	if( JUEGO_EMPEZADO && g_game.playing < 2 )
		return TRUE;
	return FALSE;
}

/* Initialize the server for the first game */
void game_init()
{
	player_init();
	color_init();
	pactos_init();
	scores_init();
	xmlscores_load();
	metaserver_init();

	g_game.connections = 0;
	g_game.players = 0;
	g_game.playing = 0;

	/* default values */
	g_game.fichas = 5;
	g_game.fichas2 = 3;
	g_game.mission = FALSE;
	g_game.cmission = TRUE;
	g_game.reglas = TEG_RULES_TEG;
	g_game.fog_of_war = FALSE;
	g_game.player_fow = NULL;

	game_new();
}

void server_init( void )
{
	gethostname(g_server.name,SERVER_NAMELEN);
	g_server.port=TEG_DEFAULT_PORT;
	g_server.debug=FALSE;
	g_server.with_console=TRUE;
	g_server.with_ggz=FALSE;
	g_server.kick_unparent_robots=TRUE;
}

void server_exit( int sock )
{
	console_quit();
	close(sock);
	if( g_server.with_console )
		close(CONSOLE_FD);
	player_flush();
	printf(_("Goodbye.\n"));
	exit(1);
}

void server_is_idle()
{
	metaserver_publish();
}

void main_loop( void )
{
	int listenfd,fd, nready;
	struct sockaddr client;
	ssize_t client_len;
	fd_set read_set;
	struct timeval timeout, timeofday_old, timeofday_new;
	struct timezone tz;

	listenfd = net_listen(NULL,g_server.port);
	if( listenfd < 0 )
		return;

	max_fd=listenfd;

	FD_ZERO(&all_set);
	FD_SET(listenfd,&all_set);

	if( g_server.with_console ) {
		FD_SET(CONSOLE_FD, &all_set);
	}

#define TIMEOUT_SEC	(180) 	/* 3 minutes */
	/* 5 minutes */
	timeout.tv_sec = TIMEOUT_SEC;
	timeout.tv_usec = 0;
	gettimeofday( &timeofday_old, &tz );

	memset( &tz, 0, sizeof(tz) );

	while(1) {

		read_set = all_set;
		nready = select( max_fd+1, &read_set, NULL, NULL, &timeout );

		/* recompute the timeout */
		if( gettimeofday( &timeofday_new, &tz ) == 0 ) {

			int s = timeofday_new.tv_sec - timeofday_old.tv_sec;

			if( TIMEOUT_SEC > s )
				timeout.tv_sec = TIMEOUT_SEC - s;
			else
				timeout.tv_sec = TIMEOUT_SEC;

			/* it is aprox 3 minutes */
			if( s == 0 )
				timeout.tv_usec = timeofday_new.tv_usec - timeofday_old.tv_usec;
			else
				timeout.tv_usec = 0;

			/* may occur sometimes */
			if( s > TIMEOUT_SEC ) {
				timeout.tv_sec = TIMEOUT_SEC;
				timeout.tv_usec = 0;
				gettimeofday( &timeofday_old, &tz );
				server_is_idle();
			}
		}

		/* error ?*/
		if( nready == -1 ) {
			if(errno!=EINTR) {
				fprintf(stderr,_("tegserver: Abnormal error in select()\n"));
				perror("tegserver:");
			}
			continue;

		/* timeout ? */
		} else if( nready == 0 ) {
			MAIN_DEBUG("timeout\n");
			timeout.tv_sec = TIMEOUT_SEC;
			timeout.tv_usec = 0;
			gettimeofday( &timeofday_old, &tz );
			server_is_idle();

			continue;
		}

		/* new client */
		if(FD_ISSET( listenfd, &read_set) ) {	
			MAIN_DEBUG("new client\n");
			client_len = sizeof( client );
			fd = accept( listenfd, (struct sockaddr *)&client, &client_len );

			if( fd != -1 )
				fd_add( fd );

			if(--nready <= 0)
				continue;
		}

		/* input from console */
		if( g_server.with_console && FD_ISSET(CONSOLE_FD, &read_set)) {
			TEG_STATUS ts = console_handle(CONSOLE_FD);

			if(ts==TEG_STATUS_GAMEOVER || ts==TEG_STATUS_CONNCLOSED)
				server_exit(listenfd);

			if(--nready <= 0)
				continue;

		}

		/* input from players */
    		for(fd=0;fd<=max_fd;fd++) {
			if( (fd!=listenfd && fd!=CONSOLE_FD) && FD_ISSET(fd,&read_set) ) {
				if(play_teg( fd )==TEG_STATUS_CONNCLOSED) {
					MAIN_DEBUG("closing connection\n");

					/* kick robots if they are alone */
					player_kick_unparent_robots();
				}

				if(--nready <= 0)
					break;
			}
		}
	}
}

/* Clears a fd from the all_set descriptors */
void fd_remove( int fd )
{
	if( g_server.debug )
		con_text_out(M_INF,_("Removing fd %d\n"),fd);

	if( fd > 0 ) {
		net_close(fd);
		FD_CLR(fd,&all_set);
	}
}

/* adds a fd to the all_set descriptors */
void fd_add( int fd )
{
	if( g_server.debug )
		con_text_out(M_INF,_("Accepting fd %d\n"),fd);

	FD_SET(fd, &all_set );
	if( fd > max_fd )
		max_fd = fd;
}

/* parse the arguments */
void argument_init( int argc, char **argv)
{
	int i;
	char *option;

	i = 1;
	while (i < argc) {
		if( is_option("--help", argv[i]) ) {
			fprintf(stderr, _("Usage: %s [option ...]\nValid options are:\n"), argv[0]);
			fprintf(stderr, _("  -h, --help\t\tPrint a summary of the options\n"));
			fprintf(stderr, _("  -p, --port PORT\tBind the server to port PORT (default 2000)\n"));
			fprintf(stderr, _("  -s, --seed SEED\tNew seed for random\n"));
			fprintf(stderr, _("  -v, --version\t\tPrint the version number\n"));
			fprintf(stderr, _("  -c, --console BOOLEAN\tEnable the console or not (default 1)\n"));
#ifdef WITH_GGZ
			fprintf(stderr, _("  -g, --ggz\t\tEnable the GGZ mode (default OFF)\n"));
#endif /* WITH_GGZ */
			fprintf(stderr, _("  -m, --metaserver BOOLEAN\tPublish this server with the metaserver (default 0)\n"));
			fprintf(stderr, _("  -d, --debug\tEnable verbosity in server\n"));
			exit(0);
		} else if (is_option("--version",argv[i])) {
			fprintf(stderr, TEG_NAME" v"VERSION"\n\n");
			exit(0);
		} else if ((option = get_option("--port",argv,&i,argc)) != NULL) {
			g_server.port=atoi(option);
		} else if ((option = get_option("--seed",argv,&i,argc)) != NULL) {
			g_game.seed=atoi(option);
		} else if ((option = get_option("--console",argv,&i,argc)) != NULL) {
			g_server.with_console=atoi(option);
#ifdef WITH_GGZ
		} else if (is_option("--ggz",argv[i])) {
			g_server.with_ggz=1;
			g_server.with_console=FALSE;
#endif /* WITH_GGZ */
		} else if ((option = get_option("--metaserver",argv,&i,argc)) != NULL) {
			g_server.metaserver_on=atoi(option);
		} else if ( is_option("--debug",argv[i])) {
			g_server.debug=1;
		} else {
			fprintf(stderr, _("Unrecognized option: \"%s\"\n"), argv[i]);
			exit(1);
		}
		i++;
	}
}

int main( int argc, char **argv)
{
	init_nls();
	printf( "%s v%s server - by Ricardo Quesada\n\n", TEG_NAME, VERSION);

	dont_run_as_root();

	server_init();		/* default values for server */
	game_init();		/* default values for the game */

	argument_init(argc, argv);	/* parse command line */
	printf( _("Bound to port: %d\n"),g_server.port );
	printf( _("Using seed: %u\n\n"),g_game.seed );

	if( g_server.with_console ) {
		if( ! g_server.metaserver_on )
			printf( _("Type '%s %s' to publish this server in the metaserver\n"),TOKEN_METASERVER, OPTION_META_ON);
		printf( _("Type '%s' for more help\n"),TOKEN_HELP);
		console_init();		/* initialize console */
	} else {
		if( !g_server.with_ggz )
			printf(_("Standalone server.\n"));
		else
			printf(_("GGZ mode activated.\n"));
	}

	srand( g_game.seed );

#ifdef WITH_GGZ
	if( g_server.with_ggz )
		return ggz_server_main_loop();
	else
#endif /* WITH_GGZ */

	metaserver_publish();

	main_loop();

	if( g_server.with_console )
		console_quit();

	return 1;
}
