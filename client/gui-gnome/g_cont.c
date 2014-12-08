/*	$Id: g_cont.c,v 1.9 2002/08/31 17:45:58 riq Exp $	*/
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gnome.h>

#include "gui.h"
#include "client.h"
#include "interface.h"
#include "g_country.h"
#include "g_cont.h"
#include "themes.h"


static int initialized = 0;

struct _G_conts G_conts[CONT_CANT];

void G_cont_init()
{
	int i;
	TContinent cont;

	if( initialized )
		return;

	for( i=0; i < CONT_CANT ; i++ ) {
		if( theme_giveme_continent(&cont,i) == TEG_STATUS_SUCCESS ) {
			G_conts[i].x = cont.pos_x;
			G_conts[i].y = cont.pos_y;
		}
	}

	initialized = 1;
}

void G_cont_create( int cont, GnomeCanvasGroup *root )
{
	if(!initialized)
		G_cont_init();

	G_conts[cont].cont_group = GNOME_CANVAS_GROUP(
			gnome_canvas_item_new (
				root,
				gnome_canvas_group_get_type (),
				"x", (double) G_conts[cont].x,
				"y", (double) G_conts[cont].y,
				NULL));
}
