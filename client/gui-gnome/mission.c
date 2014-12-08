/*	$Id: mission.c,v 1.2 2002/08/31 17:29:31 riq Exp $	*/
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
 * @file mission.c
 * Crea las tarjetas que fueron obtenidas
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>

#include "gui.h"
#include "client.h"

#include "chatline.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "mission.h"
#include "fonts.h"

static GdkPixbuf *tar_mission = NULL;
static int mission_number = -1;

static TEG_STATUS mission_load()
{
	char *filename=NULL;

	if(!tar_mission) {
		filename = load_pixmap_file ("objetivo.png");
		tar_mission = gdk_pixbuf_new_from_file(filename, NULL);
		if(filename) g_free( filename );
		if( tar_mission == NULL ) {
			g_warning(_("Error, couldn't find file: objetivo.png"));
			return TEG_STATUS_ERROR;
		}
	}

	return TEG_STATUS_SUCCESS;
}

static TEG_STATUS mission_view_number( GtkWidget *dialog )
{
	static GtkWidget	*canvas=NULL;
	static GnomeCanvasItem *imagebg=NULL;
	static GnomeCanvasItem *imagetxt=NULL;
	static GnomeCanvasItem *imagetxt2=NULL;

	if( mission_number == -1 )
		return TEG_STATUS_ERROR;

	if( canvas == NULL ) {
		canvas = gnome_canvas_new();
		gtk_widget_set_usize (canvas, gdk_pixbuf_get_width(tar_mission), gdk_pixbuf_get_height(tar_mission) );
		gnome_canvas_set_scroll_region (GNOME_CANVAS (canvas), 0, 0, gdk_pixbuf_get_width(tar_mission), gdk_pixbuf_get_height(tar_mission) );
		imagebg = gnome_canvas_item_new(
			gnome_canvas_root(GNOME_CANVAS(canvas)),
			gnome_canvas_pixbuf_get_type (),
			"pixbuf", tar_mission,
			"x", 0.0,
			"y", 0.0,
			"width", (double) gdk_pixbuf_get_width(tar_mission),
			"height", (double) gdk_pixbuf_get_height(tar_mission),
			"anchor", GTK_ANCHOR_NW,
			NULL);


		gdk_pixbuf_unref(tar_mission);

		gtk_box_pack_start_defaults( GTK_BOX(GNOME_DIALOG(dialog)->vbox), GTK_WIDGET(canvas));
	}

	if( imagetxt )
		gtk_object_destroy( GTK_OBJECT(imagetxt) );

	imagetxt = gnome_canvas_item_new(
		gnome_canvas_root(GNOME_CANVAS(canvas)),
		gnome_canvas_text_get_type(),
		"text", missions_get_name( mission_number ),
		"x", (double) 4,
		"y", (double) 10,
		"x_offset", (double) -1,
		"y_offset", (double) -1,
		"font", HELVETICA_12_FONT,
		"fill_color", "brown",
		"anchor",GTK_ANCHOR_NW,
		NULL);

	if( imagetxt2 ) {
		gtk_object_destroy( GTK_OBJECT(imagetxt2) );
		imagetxt2 = NULL;
	}

	if( mission_number == g_game.secret_mission || mission_number == MISSION_COMMON) {
		char *text;

		if( mission_number == g_game.secret_mission )
			text = _("[This is your secret mission.]");
		else 
			text = _("[This is the common mission.]");

		imagetxt2 = gnome_canvas_item_new(
			gnome_canvas_root(GNOME_CANVAS(canvas)),
			gnome_canvas_text_get_type(),
			"text",text,
			"x", (double) 4,
			"y", (double) 124,
			"x_offset", (double) -1,
			"y_offset", (double) -1,
			"font", HELVETICA_10_FONT,
			"fill_color", "brown",
			"anchor",GTK_ANCHOR_NW,
			NULL);
	}

	gtk_widget_show (canvas);

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS mission_view_fake_number( GtkWidget *frame, int mission )
{
	GtkWidget	*canvas;
	GnomeCanvasItem	*imagebg;
	GnomeCanvasItem	*imagetxt;
	GnomeCanvasItem	*imagetxt2;
	char *text;

	if( mission_load() != TEG_STATUS_SUCCESS )
		return TEG_STATUS_ERROR;

	canvas = gnome_canvas_new();
	gtk_widget_set_usize (canvas, gdk_pixbuf_get_width(tar_mission), gdk_pixbuf_get_height(tar_mission) );
	gnome_canvas_set_scroll_region (GNOME_CANVAS (canvas), 0, 0, gdk_pixbuf_get_width(tar_mission), gdk_pixbuf_get_height(tar_mission) );
	imagebg = gnome_canvas_item_new(
		gnome_canvas_root(GNOME_CANVAS(canvas)),
		gnome_canvas_pixbuf_get_type (),
		"pixbuf", tar_mission,
		"x", 0.0,
		"y", 0.0,
		"width", (double) gdk_pixbuf_get_width(tar_mission),
		"height", (double) gdk_pixbuf_get_height(tar_mission),
		"anchor", GTK_ANCHOR_NW,
		NULL);

#if 0
	gtk_signal_connect (GTK_OBJECT (imagebg), "destroy",
		(GtkSignalFunc) free_imlib_image,
		tar_mission);
#endif

	imagetxt = gnome_canvas_item_new(
		gnome_canvas_root(GNOME_CANVAS(canvas)),
		gnome_canvas_text_get_type(),
		"text", missions_get_name(mission),
		"x", (double) 4,
		"y", (double) 10,
		"x_offset", (double) -1,
		"y_offset", (double) -1,
		"font", HELVETICA_12_FONT,
		"fill_color", "brown",
		"anchor",GTK_ANCHOR_NW,
		NULL);

	text = _("[Accomplished mission]");

	imagetxt2 = gnome_canvas_item_new(
		gnome_canvas_root(GNOME_CANVAS(canvas)),
		gnome_canvas_text_get_type(),
		"text",text,
		"x", (double) 4,
		"y", (double) 124,
		"x_offset", (double) -1,
		"y_offset", (double) -1,
		"font", HELVETICA_10_FONT,
		"fill_color", "brown",
		"anchor",GTK_ANCHOR_NW,
		NULL);

	gtk_container_add (GTK_CONTAINER (frame),GTK_WIDGET(canvas));

	return TEG_STATUS_SUCCESS;
}

void mission_view_prev( GtkWidget *button, gpointer data )
{
	GtkWidget *dialog = (GtkWidget*) data;
	int last = missions_cant();
	int first_mission;

	if( mission_number == -1 )
		return;

	first_mission = g_game.with_common_mission ? MISSION_COMMON : MISSION_COMMON + 1;

	mission_number--;
	if( mission_number < first_mission )
		mission_number = last -1;

	mission_view_number( dialog );
	return;
}

void mission_view_next( GtkWidget *button, gpointer data )
{
	GtkWidget *dialog = (GtkWidget*) data;
	int last = missions_cant();
	int first_mission;

	if( mission_number == -1 )
		return;

	first_mission = g_game.with_common_mission ? MISSION_COMMON : MISSION_COMMON + 1;

	mission_number++;
	if( mission_number >= last-1 )
		mission_number = first_mission;

	mission_view_number( dialog );
	return;
}

/* funciones de creacion */
void mission_view()
{
	static GtkWidget *dialog=NULL;


	if( g_game.secret_mission < 0 ) {
		textmsg(M_INF,_("Requesting a mission..."));
		out_missions();
		return;
	}

	mission_number = g_game.secret_mission;

	if( tar_mission == NULL ) {
		if( mission_load() != TEG_STATUS_SUCCESS ) {
			textmsg(M_INF,_("Your mission is: %s"), missions_get_name( mission_number ));
			return;
		}
	}
	

	if( dialog == NULL ) {

		dialog = teg_dialog_new(_("Secret mission"),_("Your mission")); 
		gnome_dialog_append_buttons(GNOME_DIALOG(dialog),
				GNOME_STOCK_BUTTON_PREV,
				GNOME_STOCK_BUTTON_NEXT,
				GNOME_STOCK_BUTTON_CLOSE,
				NULL );
		gnome_dialog_close_hides( GNOME_DIALOG(dialog), TRUE );
		gnome_dialog_set_default(GNOME_DIALOG(dialog),2);

		/* signals de los botones */
		gnome_dialog_button_connect (GNOME_DIALOG(dialog),
						0, GTK_SIGNAL_FUNC(mission_view_prev),dialog);
		gnome_dialog_button_connect (GNOME_DIALOG(dialog),
						1, GTK_SIGNAL_FUNC(mission_view_next),dialog);
		gnome_dialog_button_connect (GNOME_DIALOG(dialog),
						2, GTK_SIGNAL_FUNC(dialog_close), dialog );

		/* create picture of card (canvas with labels) */

	}

	if( g_game.secret_mission == MISSION_CONQWORLD ) {
		gnome_dialog_set_sensitive( GNOME_DIALOG(dialog),0,FALSE);
		gnome_dialog_set_sensitive( GNOME_DIALOG(dialog),1,FALSE);
	} else {
		gnome_dialog_set_sensitive( GNOME_DIALOG(dialog),0,TRUE);
		gnome_dialog_set_sensitive( GNOME_DIALOG(dialog),1,TRUE);
	}
	gnome_dialog_set_default( GNOME_DIALOG(dialog),2);

	mission_view_number( dialog );

	gtk_widget_show_all(dialog);
	raise_and_focus(dialog);
}
