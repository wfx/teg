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
 * misc functions
 */

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

#include <glib.h>

#include "../common/net.h"
#include "protocol.h"
#include "fcintl.h"
#include "client.h"

CJUEGO g_game;			/**< client game */

static void close_descriptors(void)
{
	int i, open_max;

	/* force manually the close of the socket */
	if(g_game.fd >0) {
		net_close(g_game.fd);
	}

	open_max = sysconf(_SC_OPEN_MAX);
	for(i = 3; i < open_max; i++) {
		fcntl(i, F_SETFD, FD_CLOEXEC);
	}
}

/* re-initializes a game. called every time a new game will start */
TEG_STATUS game_reinit()
{
	int i;

	g_game.playeres = 0;
	g_game.dados_srccountry=-1;
	g_game.dados_dstcountry=-1;
	for(i=0; i<3; i++) {
		g_game.dados_src[i] = 0;
		g_game.dados_dst[i] = 0;
	}

	InitializeListHead(&g_game.tarjetas_list);
	g_game.tarjetas_cant = 0;
	for(i=0; i<COUNTRIES_CANT; i++) {
		tarjeta_inittarj(&g_countries[i].tarjeta);
	}

	g_game.secret_mission = -1;
	g_game.whos_turn = -1;
	g_game.who_started_round = -1;
	g_game.round_number = -1;
	g_game.with_secret_mission = -1;
	g_game.with_common_mission = -1;
	g_game.with_fog_of_war = -1;

	return TEG_STATUS_SUCCESS;
}


/* initializes a game */
void game_init()
{
	static int firsttime=1;
	g_game.fd = -1;
	g_game.numjug = -1;
	g_game.observer = 0;

	/* variables seteadas por command line y que pueden cambiar estos defaults */
	if(firsttime) {
		dirs_create();
		g_game.already_connected = 0;
	}

	ESTADO_SET(PLAYER_STATUS_DESCONECTADO);

	game_reinit();

	/* name, server, serverport, color is assigned in the gui */

	firsttime=0;
}

void game_finalize()
{
	countries_init();
	game_reinit();
}

TEG_STATUS teg_connect()
{
	if(ESTADO_ES(PLAYER_STATUS_DESCONECTADO)) {

		/* standar mode */
		if(! g_game.already_connected) {
			g_game.fd = net_connect_tcp((char*) &g_game.sername, g_game.serport);
			if(g_game.fd < 0) {
				textmsg(M_ERR, _("Error while trying to connect to server '%s' at port %d"), g_game.sername, g_game.serport);
				return TEG_STATUS_ERROR;
			}
		} else if(g_game.already_connected) {
			g_game.fd = 3;
		}

		return TEG_STATUS_SUCCESS;
	} else {
		textmsg(M_ERR, _("Error, you are already connected"));
		return TEG_STATUS_ERROR;
	}
}

void teg_disconnect()
{
	ESTADO_SET(PLAYER_STATUS_DESCONECTADO);

	player_flush();

	if(g_game.fd > 0) {
		net_close(g_game.fd);
		g_game.fd = -1;
	}

	game_finalize();
	game_init();

	gui_disconnect();
}

TEG_STATUS playerid_restore_from_error(void)
{
	textmsg(M_ERR, _("The game has already started. Connect as an observer."));
	teg_disconnect();
	return TEG_STATUS_SUCCESS;
}

/* Launch a server in localost */
TEG_STATUS launch_server(int port)
{
	pid_t pid;
	char *args[6];

	if((pid = fork()) < 0) {
		perror("tegclient:");
		return TEG_STATUS_ERROR;
	} else if(pid == 0) {

		char buffer[100];
		close_descriptors();
		args[0] = "x-terminal-emulator";
		args[1] = "-e";
		args[2] = BINDIR"/tegserver";
		args[3] = "--port";
		snprintf(buffer, sizeof(buffer)-1, "%d", port);
		buffer[ sizeof(buffer)-1 ] = 0;
		args[4] = buffer;
		args[5] = NULL;

		if(execvp(args[0], args) < 0) {
			perror(args[0]);
			/* last chance, launch tegserver without console */
			args[0] = BINDIR"/tegserver";
			args[1] = "--console";
			args[2] = "0";
			args[3] = "--port";
			snprintf(buffer, sizeof(buffer)-1, "%d", port);
			buffer[ sizeof(buffer)-1 ] = 0;
			args[4] = buffer;
			args[5] = NULL;
			if(execv(args[0], args) < 0) {
				fprintf(stderr, "Launching server failed. Does the file '%s' exists ?\n", args[0]);
				perror("exe:");

				/* This saves a crash */
				args[0] = "/bin/true";
				args[1] = NULL;
				execv(args[0], args);
				exit(1);
			}
		}
		return TEG_STATUS_ERROR;
	} else {
		/* wait util server is launched */
		sleep(2);
	}
	return TEG_STATUS_SUCCESS;
}

/* Launch robot in localhost */
TEG_STATUS launch_robot(void)
{
	pid_t pid;
	char *args[7];
	char port[50];

	/* launch robot in the server */
	if(g_game.robot_in_server) {
		return out_robot();
	}

	/* launch robot in the client */
	if((pid = fork()) < 0) {
		perror("tegclient:");
		return TEG_STATUS_ERROR;
	} else if(pid == 0) {

		close_descriptors();

		sprintf(port, "%d", g_game.serport);

		args[0] = BINDIR"/tegrobot";
		args[1] = "--server";
		args[2] = g_game.sername;
		args[3] = "--port";
		args[4] = port;
		args[5] = "--quiet";
		args[6] = NULL;

		if(execv(args[0], args) < 0) {
			fprintf(stderr, "Launching robot failed. Does the file '%s' exists ?\n", args[0]);
			perror("exe:");

			/* This saves a crash */
			args[0] = "/bin/true";
			args[1] = NULL;
			execv(args[0], args);
			exit(1);
		}
		return TEG_STATUS_ERROR;
	} else {
		/* nothing */
	}
	return TEG_STATUS_SUCCESS;
}

/* display a message */
TEG_STATUS textmsg(int level, char *format, ...)
{
	va_list args;
	char buf[PROT_MAX_LEN];

	va_start(args, format);
	vsnprintf(buf, sizeof(buf) -1, format, args);
	va_end(args);

	buf[ sizeof(buf) -1 ] = 0;

	if(g_game.msg_show & level) {
		gui_textmsg(buf);
	}
	return TEG_STATUS_SUCCESS;
}

/* Creates defaults dir for TEG */
TEG_STATUS dirs_create()
{
	DIR *dir;
	char buf[1000];

	memset(buf, 0, sizeof(buf));

	snprintf(buf, sizeof(buf)-1, "%s/%s", g_get_home_dir(), TEG_DIRRC);

	if((dir = opendir(buf)) == NULL) {
		mkdir(buf, 0755);
	} else {
		closedir(dir);
	}

	snprintf(buf, sizeof(buf)-1, "%s/%s/themes", g_get_home_dir(), TEG_DIRRC);
	if((dir = opendir(buf)) == NULL) {
		mkdir(buf, 0755);
	} else {
		closedir(dir);
	}

	return TEG_STATUS_SUCCESS;
}
