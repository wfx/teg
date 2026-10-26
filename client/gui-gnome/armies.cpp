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
 * @file armies.c
 * Muestra las fichas que hay que poner
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "armies.h"

#include <glib/gi18n.h>

#include "client.h"
#include "gui_state.h"
#include "interface.h"
#include "support.h"
#include "themes.h"
#include "fonts.h"

namespace teg::client::callbacks
{

static gboolean button_press_cb(GooCanvasItem *item, GooCanvasItem *target,
                                GdkEventButton *event, GooCanvas *canvas)
{
	if(!gui_state().armies.drag.dragging && event->button == 1) {
		gui_state().armies.drag.item_x = event->x;
		gui_state().armies.drag.item_y = event->y;
		gui_state().armies.drag.dragging = TRUE;
		g_object_get(item, "x", &gui_state().armies.drag.x, "y", &gui_state().armies.drag.y, NULL);
		goo_canvas_pointer_grab(canvas, item,
		                        static_cast<GdkEventMask>(
		                            GDK_POINTER_MOTION_MASK |
		                            GDK_BUTTON_RELEASE_MASK),
		                        NULL, event->time);
	}

	return FALSE;
}

static gboolean motion_notify_cb(GooCanvasItem *item, GooCanvasItem *target,
                                 GdkEventMotion *event, gpointer data)
{
	if(gui_state().armies.drag.dragging && (event->state & GDK_BUTTON1_MASK)) {
		gui_state().armies.drag.new_x = event->x - gui_state().armies.drag.x;
		gui_state().armies.drag.new_y = event->y - gui_state().armies.drag.y;
		g_object_set(item, "x", gui_state().armies.drag.item_x + gui_state().armies.drag.new_x,
		             "y", gui_state().armies.drag.item_y + gui_state().armies.drag.new_y, NULL);
	}

	return FALSE;
}

static gboolean button_release_cb(GooCanvasItem *item, GooCanvasItem *target,
                                  GdkEventButton *event, GooCanvas *canvas)
{
	if(gui_state().armies.drag.dragging && item != NULL) {
		goo_canvas_pointer_ungrab(canvas, item, event->time);
		gui_state().armies.drag.dragging = FALSE;
	}

	return FALSE;
}

static TEG_STATUS armies_view_cant_x_cont(int i)
{
	char buffer[200];

	memset(buffer, 0, sizeof(buffer));

	if(i<0 || i > CONT_CANT) {
		return TEG_STATUS_ERROR;
	}

	if(gui_state().armies.prop[i].armies_text) {
		goo_canvas_item_remove(gui_state().armies.prop[i].armies_text);
		gui_state().armies.prop[i].armies_text = NULL;
	}

	if(gui_state().armies.prop[i].cant < 0) {
		snprintf(buffer, sizeof(buffer)-1, "%d", 0);
	} else {
		snprintf(buffer, sizeof(buffer)-1, "%d", gui_state().armies.prop[i].cant);
	}

	gui_state().armies.prop[i].armies_text = goo_canvas_text_new(
	            gui_state().armies.group,
	            buffer,
	            (double) 120,
	            (double) i*12+16,
	            (double) -1,
	            GOO_CANVAS_ANCHOR_NORTH_EAST,
	            "height", (double) -1,
	            "font", HELVETICA_10_FONT,
	            "fill-color", (gui_state().armies.prop[i].cant > 0) ? "black" : "grey",
	            NULL);

	return TEG_STATUS_SUCCESS;
}

static void armies_view_cant()
{
	int i;
	char buffer[200];

	memset(buffer, 0, sizeof(buffer));


	goo_canvas_item_raise(GOO_CANVAS_ITEM(gui_state().armies.group), NULL);

	for(i=0; i<CONT_CANT; i++) {
		armies_view_cant_x_cont(i);
	}

	if(gui_state().armies.prop[CONT_CANT].armies_text) {
		goo_canvas_item_remove(gui_state().armies.prop[CONT_CANT].armies_text);
		gui_state().armies.prop[CONT_CANT].armies_text = NULL;
	}

	snprintf(buffer, sizeof(buffer)-1, "%d", gui_state().armies.prop[CONT_CANT].cant);
	gui_state().armies.prop[CONT_CANT].armies_text = goo_canvas_text_new(
	            gui_state().armies.group,
	            buffer,
	            (double) 120,
	            (double) 1,
	            (double) -1,
	            GOO_CANVAS_ANCHOR_NORTH_EAST,
	            "height", (double) -1,
	            "font", HELVETICA_12_FONT,
	            "fill-color", (gui_state().armies.prop[CONT_CANT].cant > 0) ? "black" : "grey",
	            NULL);
}

TEG_STATUS armies_add(int country)
{
	int cont;
	if(!gui_state().armies.initialized) {
		return TEG_STATUS_ERROR;
	}

	gui_state().armies.prop[CONT_CANT].cant--;

	cont = client_state().game.common.countries[country].continente;
	if(gui_state().armies.prop[cont].orig_cant > 0) {
		gui_state().armies.prop[cont].cant--;
	}

	armies_view_cant();
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS armies_del(int country)
{
	int cont;
	if(!gui_state().armies.initialized) {
		return TEG_STATUS_ERROR;
	}

	gui_state().armies.prop[CONT_CANT].cant++;

	cont = client_state().game.common.countries[country].continente;
	if(gui_state().armies.prop[cont].orig_cant > 0 &&
	        gui_state().armies.prop[cont].cant < gui_state().armies.prop[cont].orig_cant) {
		gui_state().armies.prop[cont].cant++;
	}

	armies_view_cant();
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS armies_view_more(int cant)
{
	if(!gui_state().armies.initialized) {
		return TEG_STATUS_ERROR;
	}

	gui_state().armies.prop[CONT_CANT].cant += cant;

	g_object_set(gui_state().armies.group, "visibility",
	             GOO_CANVAS_ITEM_VISIBLE, NULL);

	armies_view_cant();

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS armies_view(int cant, int conts)
{
	int i;

	if(!gui_state().armies.initialized) {
		return TEG_STATUS_ERROR;
	}
	if(!gui_state().armies.group) {
		return TEG_STATUS_ERROR;
	}

	gui_state().armies.prop[CONT_CANT].cant = cant + cont_tot(client_state().game.common, conts);

	for(i=0; i<CONT_CANT; i++) {
		if(conts & 1) {
			gui_state().armies.prop[i].orig_cant = gui_state().armies.prop[i].cant = client_state().game.common.conts[i].fichas_x_cont;
		} else {
			gui_state().armies.prop[i].orig_cant = gui_state().armies.prop[i].cant = 0;
		}

		conts >>= 1;
	}

	g_object_set(gui_state().armies.group, "visibility",
	             GOO_CANVAS_ITEM_VISIBLE, NULL);

	armies_view_cant();

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS armies_unview()
{
	if(!gui_state().armies.initialized) {
		return TEG_STATUS_ERROR;
	}

	g_object_set(gui_state().armies.group, "visibility",
	             GOO_CANVAS_ITEM_INVISIBLE, NULL);
	return TEG_STATUS_SUCCESS;
}

static void setup_dnd_handlers(GooCanvasItem *item)
{
	GooCanvas *canvas = goo_canvas_item_get_canvas(item);

	g_signal_connect(item, "button-press-event",
	                 G_CALLBACK(button_press_cb), canvas);
	g_signal_connect(item, "motion-notify-event",
	                 G_CALLBACK(motion_notify_cb), NULL);
	g_signal_connect(item, "button-release-event",
	                 G_CALLBACK(button_release_cb), canvas);
}

TEG_STATUS armies_init(GooCanvasItem *group)
{
	int i;
	GooCanvasItem *item;

	if(!group) {
		return TEG_STATUS_ERROR;
	}

	if(gui_state().armies.initialized) {
		return TEG_STATUS_ERROR;
	}

	gui_state().armies.group =
	    goo_canvas_group_new(
	        group,
	        "x", (float) gui_state().theme.armies_x,
	        "y", (float) gui_state().theme.armies_y,
	        NULL);


	if(gui_state().armies.group) {
		item = goo_canvas_rect_new(
		           gui_state().armies.group,
		           0.0,
		           0.0,
		           (double) 100,
		           (double) 90,
		           "stroke-color", "none",
		           "line-width", 0.0,
		           NULL);

		if(gui_state().theme.armies_background)
			g_object_set(
			    item,
			    "fill-color", "light yellow",
			    "stroke-color", "black",
			    NULL);


		item = goo_canvas_text_new(
		           gui_state().armies.group,
		           _("Total"),
		           (double) 1,
		           (double) 1,
		           (double) -1,
		           GOO_CANVAS_ANCHOR_NORTH_WEST,
		           "height", (double) -1,
		           "font", HELVETICA_12_FONT,
		           "fill-color", "black",
		           NULL);


		for(i=0; i<CONT_CANT; i++) {

			item = goo_canvas_text_new(
			           gui_state().armies.group,
			           cont_get_name(client_state().game.common, i),
			           (double) 1,
			           (double) i*12+16,
			           (double) -1,
			           GOO_CANVAS_ANCHOR_NORTH_WEST,
			           "height", (double) -1,
			           "font", HELVETICA_10_FONT,
			           "fill-color", "black",
			           NULL);

		};
		g_object_set(gui_state().armies.group, "visibility",
		             GOO_CANVAS_ITEM_INVISIBLE, NULL);

		if(gui_state().theme.armies_dragable) {
			setup_dnd_handlers(gui_state().armies.group);
		}
	}

	gui_state().armies.initialized=1;
	return TEG_STATUS_SUCCESS;
}

}
