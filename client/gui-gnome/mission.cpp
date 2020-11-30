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

#include <goocanvas.h>
#include <glib/gi18n.h>

#include "client.h"

#include "../common/missions.h"
#include "chatline.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "mission.h"
#include "fonts.h"

namespace teg::client::callbacks
{

static GdkPixbuf *tar_mission = NULL;
static int mission_number = -1;

static TEG_STATUS mission_load()
{
	char *filename=NULL;

	if(!tar_mission) {
		filename = load_pixmap_file("objetivo.png");
		tar_mission = gdk_pixbuf_new_from_file(filename, NULL);
		if(filename) {
			g_free(filename);
		}
		if(tar_mission == NULL) {
			g_warning(_("Error, couldn't find file: objetivo.png"));
			return TEG_STATUS_ERROR;
		}
	}

	return TEG_STATUS_SUCCESS;
}

static GtkWidget     *canvas=NULL;
static GooCanvasItem *imagetxt=NULL;
static GooCanvasItem *imagetxt2=NULL;

static void nullify_canvas(void)
{
	canvas = NULL;
	imagetxt = NULL;
	imagetxt2 = NULL;
}

static TEG_STATUS mission_view_number(GtkWidget *dialog)
{
	if(mission_number == -1) {
		return TEG_STATUS_ERROR;
	}

	if(canvas == NULL) {
		canvas = goo_canvas_new();
		/* Destroying the dialog unrefs the canvas and all of
		 * its children, however sometimes some of them become
		 * dangling pointers while the logic relies them to be
		 * NULL.  Macros like GOO_IS_CANVAS_ITEM cannot be
		 * used instead because these are invalid objects so a
		 * crash is inevitable.  */
		g_signal_connect_swapped(dialog, "destroy",
		                         nullify_canvas, NULL);
		gtk_widget_set_halign(canvas, GTK_ALIGN_CENTER);
		gtk_widget_set_size_request(canvas,
		                            gdk_pixbuf_get_width(tar_mission),
		                            gdk_pixbuf_get_height(tar_mission));
		goo_canvas_set_bounds(GOO_CANVAS(canvas), 0, 0,
		                      gdk_pixbuf_get_width(tar_mission),
		                      gdk_pixbuf_get_height(tar_mission));
		goo_canvas_image_new(
		    goo_canvas_get_root_item(GOO_CANVAS(canvas)),
		    tar_mission,
		    0.0,
		    0.0,
		    "width", (double) gdk_pixbuf_get_width(tar_mission),
		    "height", (double) gdk_pixbuf_get_height(tar_mission),
		    NULL);


		g_object_unref(tar_mission);
		tar_mission = NULL;

		gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area
		                           (GTK_DIALOG(dialog))),
		                   GTK_WIDGET(canvas), TRUE, TRUE, 0);
	}


	if(imagetxt) {
		goo_canvas_item_remove(imagetxt);
		imagetxt = NULL;
	}

	imagetxt = goo_canvas_text_new(
	               goo_canvas_get_root_item(GOO_CANVAS(canvas)),
	               missions_get_name(mission_number).c_str(),
	               (double) 4,
	               (double) 10,
	               (double) -1,
	               GOO_CANVAS_ANCHOR_NORTH_WEST,
	               "height", (double) -1,
	               "font", HELVETICA_12_FONT,
	               "fill-color", "brown",
	               NULL);

	if(imagetxt2) {
		goo_canvas_item_remove(imagetxt2);
		imagetxt2 = NULL;
	}

	if(mission_number == g_game.secret_mission || mission_number == MISSION_COMMON) {
		char *text;

		if(mission_number == g_game.secret_mission) {
			text = _("[This is your secret mission.]");
		} else {
			text = _("[This is the common mission.]");
		}

		imagetxt2 = goo_canvas_text_new(
		                goo_canvas_get_root_item(GOO_CANVAS(canvas)),
		                text,
		                (double) 4,
		                (double) 124,
		                (double) -1,
		                GOO_CANVAS_ANCHOR_NORTH_WEST,
		                "height", (double) -1,
		                "font", HELVETICA_10_FONT,
		                "fill-color", "brown",
		                NULL);
	}

	gtk_widget_show(canvas);

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS mission_view_fake_number(GtkWidget *frame, int mission)
{
	GtkWidget	*fcanvas;
	char *text;

	if(mission_load() != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_ERROR;
	}

	fcanvas = goo_canvas_new();
	gtk_widget_set_size_request(fcanvas,
	                            gdk_pixbuf_get_width(tar_mission),
	                            gdk_pixbuf_get_height(tar_mission));
	goo_canvas_set_bounds(GOO_CANVAS(fcanvas), 0, 0,
	                      gdk_pixbuf_get_width(tar_mission),
	                      gdk_pixbuf_get_height(tar_mission));
	goo_canvas_image_new(
	    goo_canvas_get_root_item(GOO_CANVAS(fcanvas)),
	    tar_mission,
	    0.0,
	    0.0,
	    "width", (double) gdk_pixbuf_get_width(tar_mission),
	    "height", (double) gdk_pixbuf_get_height(tar_mission),
	    NULL);

	g_object_unref(tar_mission);
	tar_mission = NULL;
#if 0
	gtk_signal_connect(GTK_OBJECT(imagebg), "destroy",
	                   (GtkSignalFunc) free_imlib_image,
	                   tar_mission);
#endif

	goo_canvas_text_new(
	    goo_canvas_get_root_item(GOO_CANVAS(fcanvas)),
	    missions_get_name(mission).c_str(),
	    (double) 4,
	    (double) 10,
	    (double) -1,
	    GOO_CANVAS_ANCHOR_NORTH_WEST,
	    "height", (double) -1,
	    "font", HELVETICA_12_FONT,
	    "fill-color", "brown",
	    NULL);

	text = _("[Accomplished mission]");

	goo_canvas_text_new(
	    goo_canvas_get_root_item(GOO_CANVAS(fcanvas)),
	    text,
	    (double) 4,
	    (double) 124,
	    (double) -1,
	    GOO_CANVAS_ANCHOR_NORTH_WEST,
	    "height", (double) -1,
	    "font", HELVETICA_10_FONT,
	    "fill-color", "brown",
	    NULL);

	gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(fcanvas));

	return TEG_STATUS_SUCCESS;
}

void mission_view_cb(GtkDialog *dialog, gint id, gpointer data)
{
	int last = missions_cant();
	int first_mission;

	if(id < 1) {
		gtk_widget_destroy(GTK_WIDGET(dialog));
		return;
	}

	if(mission_number == -1) {
		return;
	}

	first_mission = g_game.with_common_mission ? MISSION_COMMON : MISSION_COMMON + 1;

	switch(id) {
	case 1:
		mission_number--;
		if(mission_number < first_mission) {
			mission_number = last -1;
		}
		break;
	case 2:
		mission_number++;
		if(mission_number >  last-1) {
			mission_number = first_mission;
		}
	default:
		break;
	}

	mission_view_number(GTK_WIDGET(dialog));
}

/* funciones de creacion */
void mission_view()
{
	GtkWidget *dialog=NULL;


	if(g_game.secret_mission < 0) {
		textmsg(M_INF, _("Requesting a mission..."));
		out_missions();
		return;
	}

	mission_number = g_game.secret_mission;

	if(tar_mission == NULL) {
		if(mission_load() != TEG_STATUS_SUCCESS) {
			textmsg(M_INF, _("Your mission is: %s"), missions_get_name(mission_number));
			return;
		}
	}


	dialog = teg_dialog_new(_("Secret mission"), _("Your mission"));
	gtk_dialog_add_buttons(GTK_DIALOG(dialog), _("_Previous"), 1,
	                       _("_Next"), 2, _("_Close"), 0, NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), 2);

	/* signals de los botones */
	g_signal_connect(dialog, "response",
	                 G_CALLBACK(mission_view_cb), NULL);
	/* create picture of card (canvas with labels) */

	if(g_game.secret_mission == MISSION_CONQWORLD) {
		gtk_dialog_set_response_sensitive(GTK_DIALOG(dialog), 1, FALSE);
		gtk_dialog_set_response_sensitive(GTK_DIALOG(dialog), 2, FALSE);
	} else {
		gtk_dialog_set_response_sensitive(GTK_DIALOG(dialog), 1, TRUE);
		gtk_dialog_set_response_sensitive(GTK_DIALOG(dialog), 2, TRUE);
	}
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), 0);

	mission_view_number(dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));
}

}
