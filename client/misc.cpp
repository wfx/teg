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
#include <cstring>

#include <glib.h>

#include "../common/net.h"
#include "../common/execute.hpp"

#include "protocol.h"
#include "fcintl.h"
#include "client.h"

namespace teg::client
{

static void close_descriptors(ClientState& state)
{
	int i, open_max;

	/* force manually the close of the socket */
	if(state.game.fd >0) {
		net_close(state.game.fd);
	}

	open_max = sysconf(_SC_OPEN_MAX);
	for(i = 3; i < open_max; i++) {
		fcntl(i, F_SETFD, FD_CLOEXEC);
	}
}

/* re-initializes a game. called every time a new game will start */
TEG_STATUS game_reinit(ClientState& state)
{
	int i;

	state.game.playeres = 0;
	state.game.dados_srccountry=-1;
	state.game.dados_dstcountry=-1;
	for(i=0; i<3; i++) {
		state.game.dados_src[i] = 0;
		state.game.dados_dst[i] = 0;
	}

	state.game.tarjetas_cant = 0;
	for(i=0; i<static_cast<int>(COUNTRIES_CANT); i++) {
		tarjeta_inittarj(&state.game.common.countries[i].tarjeta);
	}

	state.game.secret_mission = -1;
	state.game.whos_turn = -1;
	state.game.who_started_round = -1;
	state.game.round_number = -1;
	state.game.with_secret_mission = -1;
	state.game.with_common_mission = -1;
	state.game.with_fog_of_war = -1;

	return TEG_STATUS_SUCCESS;
}


/* initializes a game */
void game_init(ClientState& state)
{
	state.game.fd = -1;
	state.game.numjug = -1;
	state.game.observer = 0;

	/* variables seteadas por command line y que pueden cambiar estos defaults */
	if(state.first_time) {
		dirs_create();
		state.game.already_connected = 0;
	}

	set_status(state, PLAYER_STATUS_DESCONECTADO);

	game_reinit(state);

	/* name, server, serverport, color is assigned in the gui */

	state.first_time = false;
}

void game_finalize(ClientState& state)
{
	countries_init(state.game.common);
	game_reinit(state);
}

TEG_STATUS teg_connect(ClientState& state)
{
	if(is_status(state, PLAYER_STATUS_DESCONECTADO)) {

		/* standar mode */
		if(! state.game.already_connected) {
			state.game.fd = net_connect_tcp((char*) &state.game.sername, state.game.serport);
			if(state.game.fd < 0) {
				textmsg(state, M_ERR, _("Error while trying to connect to server '%s' at port %d"), state.game.sername, state.game.serport);
				return TEG_STATUS_ERROR;
			}
		} else if(state.game.already_connected) {
			state.game.fd = 3;
		}

		return TEG_STATUS_SUCCESS;
	} else {
		textmsg(state, M_ERR, _("Error, you are already connected"));
		return TEG_STATUS_ERROR;
	}
}

void teg_disconnect(ClientState& state)
{
	set_status(state, PLAYER_STATUS_DESCONECTADO);

	player_flush(state);

	if(state.game.fd > 0) {
		net_close(state.game.fd);
		state.game.fd = -1;
	}

	game_finalize(state);
	game_init(state);

	callbacks::gui_disconnect(state);
}

TEG_STATUS playerid_restore_from_error(ClientState& state)
{
	textmsg(state, M_ERR, _("The game has already started. Connect as an observer."));
	teg_disconnect(state);
	return TEG_STATUS_SUCCESS;
}

/* Launch a server in localost */
TEG_STATUS launch_server(ClientState& state, int port)
{
	pid_t pid;
	char const *args[6];

	if((pid = fork()) < 0) {
		perror("tegclient:");
		return TEG_STATUS_ERROR;
	} else if(pid == 0) {

		auto const servername = program_directory() / "tegserver";
		char buffer[100];
		close_descriptors(state);
		args[0] = "x-terminal-emulator";
		args[1] = "-e";
		args[2] = servername.c_str();
		args[3] = "--port";
		snprintf(buffer, sizeof(buffer)-1, "%d", port);
		buffer[ sizeof(buffer)-1 ] = 0;
		args[4] = buffer;
		args[5] = NULL;

		if(!execute_program(args)) {
			perror(args[0]);
			/* last chance, launch tegserver without console */
			args[0] = servername.c_str();
			args[1] = "--console";
			args[2] = "0";
			args[3] = "--port";
			snprintf(buffer, sizeof(buffer)-1, "%d", port);
			buffer[ sizeof(buffer)-1 ] = 0;
			args[4] = buffer;
			args[5] = NULL;
			if(!execute_program(args)) {
				fprintf(stderr, "Launching server failed. Does the file '%s' exists ?\n", args[0]);
				perror("exe:");
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
TEG_STATUS launch_robot(ClientState& state)
{
	pid_t pid;
	char const *args[7];
	char port[50];

	/* launch robot in the server */
	if(state.game.robot_in_server) {
		return out_robot(state);
	}

	/* launch robot in the client */
	if((pid = fork()) < 0) {
		perror("tegclient:");
		return TEG_STATUS_ERROR;
	} else if(pid == 0) {

		close_descriptors(state);

		sprintf(port, "%d", state.game.serport);
		auto const robot = program_directory() / "tegrobot";
		args[0] = robot.c_str();
		args[1] = "--server";
		args[2] = state.game.sername;
		args[3] = "--port";
		args[4] = port;
		args[5] = "--quiet";
		args[6] = NULL;

		if(execute_program(args)) {
			fprintf(stderr, "Launching robot failed. Does the file '%s' exists ?\n", args[0]);
			perror("exe:");
			exit(1);
		}
		return TEG_STATUS_ERROR;
	} else {
		/* nothing */
	}
	return TEG_STATUS_SUCCESS;
}

/* display a message */
TEG_STATUS textmsg(ClientState& state, int level, const char *format, ...)
{
	va_list args;
	char buf[PROT_MAX_LEN];

	va_start(args, format);
	vsnprintf(buf, sizeof(buf) -1, format, args);
	va_end(args);

	buf[ sizeof(buf) -1 ] = 0;

	if(state.game.msg_show & level) {
		callbacks::gui_textmsg(state, buf);
	}
	return TEG_STATUS_SUCCESS;
}

/* Creates defaults dir for TEG */
TEG_STATUS dirs_create()
{
	DIR *dir;
	char buf[1000];

	memset(buf, 0, sizeof(buf));

	snprintf(buf, sizeof(buf)-1, "%s/%s", g_get_home_dir(), rc_directory_name);

	if((dir = opendir(buf)) == NULL) {
		mkdir(buf, 0755);
	} else {
		closedir(dir);
	}

	snprintf(buf, sizeof(buf)-1, "%s/%s/themes", g_get_home_dir(), rc_directory_name);
	if((dir = opendir(buf)) == NULL) {
		mkdir(buf, 0755);
	} else {
		closedir(dir);
	}

	return TEG_STATUS_SUCCESS;
}

}
