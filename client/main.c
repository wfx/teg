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
 * archivo principal del cliente
 */

#include <stdio.h>

#include "client.h"

TEG_STATUS main_init()
{
	game_init();
	g_game.human = 1;
	player_init();
	scores_init();
	metaserver_init();
	return TEG_STATUS_SUCCESS;
}

int main( int argc, char **argv)
{
	int i;

	dont_run_as_root();

	i = 1;

	main_init();

	if( gui_init(argc, argv) != TEG_STATUS_SUCCESS ) {
		printf("Aborting...\n");
		exit(-1);
	}

	gui_main();

	return 1;
}
