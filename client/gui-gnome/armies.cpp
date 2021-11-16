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
#include "interface.h"
#include "support.h"
#include "themes.h"
#include "fonts.h"

extern TTheme	gui_theme;

static ARMIES_POS armies_prop[CONT_CANT+1] = {
	{0, 0, NULL},
	{0, 0, NULL},
	{0, 0, NULL},
	{0, 0, NULL},
	{0, 0, NULL},
	{0, 0, NULL},
	{0, 0, NULL},
};

static int initialized=0;

GooCanvasItem *armies_group;

static double x, y;
static double new_x, new_y;
static double item_x, item_y;
static gboolean dragging;

static gboolean button_press_cb(GooCanvasItem *item, GooCanvasItem *target,
                                GdkEventButton *event, GooCanvas *canvas)
{
	if(!dragging && event->button == 1) {
		item_x = event->x;
		item_y = event->y;
		dragging = TRUE;
		g_object_get(item, "x", &x, "y", &y, NULL);
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
	if(dragging && (event->state & GDK_BUTTON1_MASK)) {
		new_x = event->x - x;
		new_y = event->y - y;
		g_object_set(item, "x", item_x + new_x,
		             "y", item_y + new_y, NULL);
	}

	return FALSE;
}

static gboolean button_release_cb(GooCanvasItem *item, GooCanvasItem *target,
                                  GdkEventButton *event, GooCanvas *canvas)
{
	if(dragging && item != NULL) {
		goo_canvas_pointer_ungrab(canvas, item, event->time);
		dragging = FALSE;
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

	if(armies_prop[i].armies_text) {
		goo_canvas_item_remove(armies_prop[i].armies_text);
		armies_prop[i].armies_text = NULL;
	}

	if(armies_prop[i].cant < 0) {
		snprintf(buffer, sizeof(buffer)-1, "%d", 0);
	} else {
		snprintf(buffer, sizeof(buffer)-1, "%d", armies_prop[i].cant);
	}

	armies_prop[i].armies_text = goo_canvas_text_new(
	                                 armies_group,
	                                 buffer,
	                                 (double) 100,
	                                 (double) i*12+15,
	                                 (double) -1,
	                                 GOO_CANVAS_ANCHOR_NORTH_EAST,
	                                 "height", (double) -1,
	                                 "font", HELVETICA_12_FONT,
	                                 "fill-color", (armies_prop[i].cant > 0) ? "black" : "grey",
	                                 NULL);

	return TEG_STATUS_SUCCESS;
}

static void armies_view_cant()
{
	int i;
	char buffer[200];

	memset(buffer, 0, sizeof(buffer));


	goo_canvas_item_raise(GOO_CANVAS_ITEM(armies_group), NULL);

	for(i=0; i<CONT_CANT; i++) {
		armies_view_cant_x_cont(i);
	}

	if(armies_prop[CONT_CANT].armies_text) {
		goo_canvas_item_remove(armies_prop[CONT_CANT].armies_text);
		armies_prop[CONT_CANT].armies_text = NULL;
	}

	snprintf(buffer, sizeof(buffer)-1, "%d", armies_prop[CONT_CANT].cant);
	armies_prop[CONT_CANT].armies_text = goo_canvas_text_new(
	        armies_group,
	        buffer,
	        (double) 100,
	        (double) 1,
	        (double) -1,
	        GOO_CANVAS_ANCHOR_NORTH_EAST,
	        "height", (double) -1,
	        "font", HELVETICA_12_FONT,
	        "fill-color", (armies_prop[CONT_CANT].cant > 0) ? "black" : "grey",
	        NULL);
}

TEG_STATUS armies_add(int country)
{
	int cont;
	if(!initialized) {
		return TEG_STATUS_ERROR;
	}

	armies_prop[CONT_CANT].cant--;

	cont = g_countries[country].continente;
	if(armies_prop[cont].orig_cant > 0) {
		armies_prop[cont].cant--;
	}

	armies_view_cant();
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS armies_del(int country)
{
	int cont;
	if(!initialized) {
		return TEG_STATUS_ERROR;
	}

	armies_prop[CONT_CANT].cant++;

	cont = g_countries[country].continente;
	if(armies_prop[cont].orig_cant > 0 &&
	        armies_prop[cont].cant < armies_prop[cont].orig_cant) {
		armies_prop[cont].cant++;
	}

	armies_view_cant();
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS armies_view_more(int cant)
{
	if(!initialized) {
		return TEG_STATUS_ERROR;
	}

	armies_prop[CONT_CANT].cant += cant;

	g_object_set(armies_group, "visibility",
	             GOO_CANVAS_ITEM_VISIBLE, NULL);

	armies_view_cant();

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS armies_view(int cant, int conts)
{
	int i;

	if(!initialized) {
		return TEG_STATUS_ERROR;
	}
	if(!armies_group) {
		return TEG_STATUS_ERROR;
	}

	armies_prop[CONT_CANT].cant = cant + cont_tot(conts);

	for(i=0; i<CONT_CANT; i++) {
		if(conts & 1) {
			armies_prop[i].orig_cant = armies_prop[i].cant = g_conts[i].fichas_x_cont;
		} else {
			armies_prop[i].orig_cant = armies_prop[i].cant = 0;
		}

		conts >>= 1;
	}

	g_object_set(armies_group, "visibility",
	             GOO_CANVAS_ITEM_VISIBLE, NULL);

	armies_view_cant();

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS armies_unview()
{
	if(!initialized) {
		return TEG_STATUS_ERROR;
	}

	g_object_set(armies_group, "visibility",
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

	if(initialized) {
		return TEG_STATUS_ERROR;
	}

	armies_group =
	    goo_canvas_group_new(
	        group,
	        "x", (float) gui_theme.armies_x,
	        "y", (float) gui_theme.armies_y,
	        NULL);


	if(armies_group) {
		item = goo_canvas_rect_new(
		           armies_group,
		           0.0,
		           0.0,
		           (double) 100,
		           (double) 90,
		           NULL);

		if(gui_theme.armies_background)
			g_object_set(
			    item,
			    "fill-color", "light yellow",
			    "stroke-color", "black",
			    NULL);


		item = goo_canvas_text_new(
		           armies_group,
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
			           armies_group,
			           cont_get_name(i),
			           (double) 1,
			           (double) i*12+15,
			           (double) -1,
			           GOO_CANVAS_ANCHOR_NORTH_WEST,
			           "height", (double) -1,
			           "font", HELVETICA_10_FONT,
			           "fill-color", "black",
			           NULL);

		};
		g_object_set(armies_group, "visibility",
		             GOO_CANVAS_ITEM_INVISIBLE, NULL);

		if(gui_theme.armies_dragable) {
			setup_dnd_handlers(armies_group);
		}
	}

	initialized=1;
	return TEG_STATUS_SUCCESS;
}
