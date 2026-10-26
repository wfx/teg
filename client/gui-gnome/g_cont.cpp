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
#include <goocanvas.h>

#include "client.h"
#include "gui_state.h"
#include "interface.h"
#include "g_country.h"
#include "g_cont.h"
#include "themes.h"

namespace teg::client::callbacks
{

void G_cont_init()
{
	int i;
	TContinent cont;

	if(gui_state().map.cont_initialized) {
		return;
	}

	for(i=0; i < CONT_CANT ; i++) {
		if(theme_giveme_continent(client_state(), &cont, i) == TEG_STATUS_SUCCESS) {
			gui_state().map.conts[i].x = cont.pos_x;
			gui_state().map.conts[i].y = cont.pos_y;
		}
	}

	gui_state().map.cont_initialized = 1;
}

void G_cont_create(int cont, GooCanvasItem *root)
{
	if(!gui_state().map.cont_initialized) {
		G_cont_init();
	}

	gui_state().map.conts[cont].cont_group =
	    goo_canvas_group_new(
	        root,
	        "x", (double) gui_state().map.conts[cont].x,
	        "y", (double) gui_state().map.conts[cont].y,
	        NULL);
}

}
