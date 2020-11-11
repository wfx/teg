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
 * @file main.c
 * archivo principal del robot
 */

#include <stdio.h>
#include <string.h>

#include "../common/share.h"
#include "client.h"
#include "version.h"
#include "fcintl.h"

TEG_STATUS main_init()
{
	game_init();
	g_game.human = 0;
	player_init();
	return TEG_STATUS_SUCCESS;
}

int main( int argc, char **argv_var)
{
	int i,quiet=0;
	char const *option;
	char const **argv = (char const**) argv_var; // save, since we add more constness here

	init_nls();
	dont_run_as_root();

	g_game.serport = 0;
	g_game.myname[0] = 0;
	g_game.sername[0] = 0;

	main_init();

	g_game.msg_show = M_ERR | M_IMP | M_MSG;

	i = 1;
	while (i < argc) {
		if( is_option("--help", argv[i]) ) {
			fprintf(stderr, _("Usage: %s [option ...]\nValid options are:\n"), argv[0]);
			fprintf(stderr, _("  -h, --help\t\tPrint a summary of the options\n"));
			fprintf(stderr, _("  -n, --name NAME\tUse NAME as name\n"));
			fprintf(stderr, _("  -p, --port PORT\tConnect to server port PORT\n"));
			fprintf(stderr, _("  -s, --server HOST\tConnect to the server at HOST\n"));
			fprintf(stderr, _("  -v, --version\t\tPrint the version number\n"));
			fprintf(stderr, _("  -c, --connected\t\tThe robot is already connected\n"));
			fprintf(stderr, _("  -q, --quiet\t\tdont show messages\n"));
			exit(0);
		} else if (is_option("--version",argv[i])) {
			fprintf(stderr, TEG_NAME" v"VERSION"\n");
			exit(0);
		} else if ((option = get_option("--name",argv,&i,argc)) != NULL) {
			strncpy( g_game.myname, option, PLAYERNAME_MAX_LEN );
		} else if ((option = get_option("--port",argv,&i,argc)) != NULL) {
			g_game.serport=atoi(option);
		} else if ((option = get_option("--server",argv,&i,argc)) != NULL) {
			strncpy( g_game.sername, option, SERVER_NAMELEN );
		} else if ( is_option("--connected",argv[i])) {
			g_game.already_connected = 1;
			quiet = 1;
		} else if ( is_option("--quiet",argv[i])) {
			quiet = 1;
		} else {
			fprintf(stderr, _("Unrecognized option: \"%s\"\n"), argv[i]);
			exit(1);
		}
		i++;
	}

	if(quiet) g_game.msg_show = M_MSG;

	textmsg(M_IMP,_("Tenes Empanadas Graciela - Robot v%s - by Ricardo Quesada\n"),VERSION);
	textmsg(M_IMP,_("Robot intelligence: %d%%"),62);

	gui_init(argc, argv_var);

	gui_main();

	return 1;
}
