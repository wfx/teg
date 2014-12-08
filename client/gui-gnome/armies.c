/*	$Id: armies.c,v 1.14 2007/07/11 22:29:22 nordi Exp $	*/
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

#include <gnome.h>

#include "gui.h"
#include "client.h"
#include "interface.h"
#include "support.h"
#include "armies.h"
#include "themes.h"
#include "fonts.h"

extern TTheme	gui_theme;

static ARMIES_POS armies_prop[CONT_CANT+1] = {
	{0,0,NULL},
	{0,0,NULL},
	{0,0,NULL},
	{0,0,NULL},
	{0,0,NULL},
	{0,0,NULL},
	{0,0,NULL},
	};

static int initialized=0;

GnomeCanvasGroup *armies_group;

static gint
armies_event (GnomeCanvasItem *item, GdkEvent *event, gpointer data)
{
	static double x, y;
	double new_x, new_y;
	double item_x, item_y;
	static int dragging;

	item_x = event->button.x;
	item_y = event->button.y;
	gnome_canvas_item_w2i (item->parent, &item_x, &item_y);

	switch (event->type) {

	case GDK_BUTTON_PRESS:
		x = item_x;
		y = item_y;
		dragging = TRUE;
		break;

	case GDK_MOTION_NOTIFY:
		if (dragging && (event->motion.state & GDK_BUTTON1_MASK)) {
			new_x = item_x;
			new_y = item_y;
			gnome_canvas_item_move (item, new_x - x, new_y - y);
			x = new_x;
			y = new_y;
		}
		break;

	case GDK_BUTTON_RELEASE:
		gnome_canvas_item_ungrab (item, event->button.time);
		dragging = FALSE;
		break;

	default:
		break;
	}

	return FALSE;
}


static TEG_STATUS armies_view_cant_x_cont( int i )
{
	char buffer[200];

	memset(buffer,0,sizeof(buffer));

	if( i<0 || i > CONT_CANT )
		return TEG_STATUS_ERROR;

	if( armies_prop[i].armies_text ) {
		gtk_object_destroy(GTK_OBJECT ( armies_prop[i].armies_text) );
		armies_prop[i].armies_text = NULL;
	}

	if(armies_prop[i].cant < 0 )
		snprintf(buffer,sizeof(buffer)-1,"%d",0);
	else
		snprintf(buffer,sizeof(buffer)-1,"%d",armies_prop[i].cant);

	armies_prop[i].armies_text = gnome_canvas_item_new(
		armies_group,
		gnome_canvas_text_get_type(),
		"text", buffer,
		"x", (double) 100,
		"y", (double) i*12+15,
		"x_offset", (double) -1,
		"y_offset", (double) -1,
		"font", HELVETICA_10_FONT,
		"fill_color", ( armies_prop[i].cant > 0 ) ? "black" : "grey",
		"anchor",GTK_ANCHOR_NE,
		NULL);

	return TEG_STATUS_SUCCESS;
}

static void armies_view_cant()
{
	int i;
	char buffer[200];

	memset(buffer,0,sizeof(buffer));


	gnome_canvas_item_raise_to_top ( GNOME_CANVAS_ITEM(armies_group));

	for(i=0;i<CONT_CANT;i++)
		armies_view_cant_x_cont( i );

	if( armies_prop[CONT_CANT].armies_text ) {
		gtk_object_destroy(GTK_OBJECT ( armies_prop[CONT_CANT].armies_text) );
		armies_prop[CONT_CANT].armies_text = NULL;
	}

	snprintf(buffer,sizeof(buffer)-1,"%d",armies_prop[CONT_CANT].cant);
	armies_prop[CONT_CANT].armies_text = gnome_canvas_item_new(
		armies_group,
		gnome_canvas_text_get_type(),
		"text", buffer,
		"x", (double) 100,
		"y", (double) 1,
		"x_offset", (double) -1,
		"y_offset", (double) -1,
		"font", HELVETICA_10_FONT,
		"fill_color", ( armies_prop[CONT_CANT].cant > 0 ) ? "black" : "grey",
		"anchor",GTK_ANCHOR_NE,
		NULL);
}

TEG_STATUS armies_add(int country)
{
	int cont;
	if(!initialized) return TEG_STATUS_ERROR;

	armies_prop[CONT_CANT].cant--;

	cont = g_countries[country].continente;
	if( armies_prop[cont].orig_cant > 0 )
		armies_prop[cont].cant--;

	armies_view_cant();
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS armies_del(int country)
{
	int cont;
	if(!initialized) return TEG_STATUS_ERROR;

	armies_prop[CONT_CANT].cant++;

	cont = g_countries[country].continente;
	if( armies_prop[cont].orig_cant > 0 &&
			armies_prop[cont].cant < armies_prop[cont].orig_cant)
		armies_prop[cont].cant++;

	armies_view_cant();
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS armies_view_more( int cant )
{
	if(!initialized) return TEG_STATUS_ERROR;

	armies_prop[CONT_CANT].cant += cant;

	gnome_canvas_item_show( GNOME_CANVAS_ITEM(armies_group) );

	armies_view_cant();

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS armies_view(int cant, int conts)
{
	int i;

	if(!initialized) return TEG_STATUS_ERROR;
	if( !armies_group ) return TEG_STATUS_ERROR;

	armies_prop[CONT_CANT].cant = cant + cont_tot( conts );

	for(i=0;i<CONT_CANT;i++) {
		if( conts & 1 )
			armies_prop[i].orig_cant = armies_prop[i].cant = g_conts[i].fichas_x_cont;
		else
			armies_prop[i].orig_cant = armies_prop[i].cant = 0;

		conts >>= 1;
	}

	gnome_canvas_item_show( GNOME_CANVAS_ITEM(armies_group) );

	armies_view_cant();

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS armies_unview()
{
	if(!initialized) return TEG_STATUS_ERROR;

	gnome_canvas_item_hide( GNOME_CANVAS_ITEM(armies_group) );
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS armies_init( GnomeCanvasGroup *group )
{
	int i;
	GnomeCanvasItem *item;

	if(!group)
		return TEG_STATUS_ERROR;

	if(initialized)
		return TEG_STATUS_ERROR;

	armies_group = GNOME_CANVAS_GROUP(
			gnome_canvas_item_new (
				group,
				gnome_canvas_group_get_type (),
				"x", (float) gui_theme.armies_x,
				"y", (float) gui_theme.armies_y,
				NULL));


	if( armies_group ) {
		item = gnome_canvas_item_new(
			armies_group,
			gnome_canvas_rect_get_type (),
			"x1", 0.0,
			"y1", 0.0,
			"x2", (double) 100,
			"y2", (double) 90,
			NULL);

		if( gui_theme.armies_background )
			gnome_canvas_item_set(
				item,
				"fill_color","light yellow",
				"outline_color","black",
				NULL );


		item = gnome_canvas_item_new(
			armies_group,
			gnome_canvas_text_get_type(),
			"text",_("Total"),
			"x", (double) 1,
			"y", (double) 1,
			"x_offset", (double) -1,
			"y_offset", (double) -1,
			"font", HELVETICA_10_FONT,
			"fill_color", "black",
			"anchor",GTK_ANCHOR_NW,
			NULL);


		for(i=0;i<CONT_CANT;i++) {

			item = gnome_canvas_item_new(
				armies_group,
				gnome_canvas_text_get_type(),
				"text", cont_get_name(i),
				"x", (double) 1,
				"y", (double) i*12+15,
				"x_offset", (double) -1,
				"y_offset", (double) -1,
				"font", HELVETICA_8_FONT,
				"fill_color", "black",
				"anchor",GTK_ANCHOR_NW,
				NULL);

		};
		gnome_canvas_item_hide( GNOME_CANVAS_ITEM(armies_group) );

		if( gui_theme.armies_dragable )
			gtk_signal_connect (GTK_OBJECT (armies_group), "event",
					    (GtkSignalFunc) armies_event,
					    &armies_group);

	}

	initialized=1;
	return TEG_STATUS_SUCCESS;
}
