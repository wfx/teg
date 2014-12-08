/*	$Id: ggz_client.c,v 1.3 2004/08/04 13:03:06 riq Exp $	*/
/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2002 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <riq@corest.com>
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
 * ggz client code
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef WITH_GGZ

#include <ggzmod.h>

#include "ggz_client.h"

#define PRINT_DEBUG(a)

static GGZMod *g_ggzmod;
static int g_is_initialized=0;
static int g_fd = -1;

static void handle_ggzmod_server(GGZMod *mod, GGZModEvent e, void *data)
{
	PRINT_DEBUG("handle_ggzmod_server()\n");

	g_fd = *(int*)data;

	ggzmod_set_state(mod, GGZMOD_STATE_PLAYING);
}

void ggz_client_init()
{
	PRINT_DEBUG("ggz_client_init()\n");

	g_ggzmod = ggzmod_new(GGZMOD_GAME);

	ggzmod_set_handler(g_ggzmod, GGZMOD_EVENT_SERVER, &handle_ggzmod_server);

	g_is_initialized = 1;
}

int ggz_client_connect()
{
	int i;
	PRINT_DEBUG("ggz_client_connect()\n");

	if( g_fd == -1 )
		ggz_client_init();

	ggzmod_connect( g_ggzmod );

	i=0;
	while( g_fd == -1 ) {
		ggzmod_dispatch( g_ggzmod );

		if( ++i > 10000000 )
			break;
	}

	return g_fd;
}

int ggz_client_disconnect()
{
	PRINT_DEBUG("ggz_client_disconnect()\n");

	if( ! g_is_initialized )
		return -1;

	return ggzmod_disconnect( g_ggzmod );
}

int ggz_client_get_fd()
{
	PRINT_DEBUG("ggz_client_get_fd()\n");

	if( ! g_is_initialized )
		ggz_client_init();

	return ggzmod_get_fd( g_ggzmod );
}

void ggz_client_handle( void *ignore, void *ignore2)
{
	PRINT_DEBUG("ggz_client_handle()\n");

	ggzmod_dispatch( g_ggzmod );
}

#endif /* WITH_GGZ */
