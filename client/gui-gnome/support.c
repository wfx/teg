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
 * @file support.c
 * Funciones de support
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <iconv.h>

#include <goocanvas.h>
#include <glib/gi18n.h>

#include "gui.h"
#include "client.h"
#include "support.h"
#include "callbacks.h"
#include "interface.h"
#include "mission.h"
#include "g_scores.h"
#include "fonts.h"

/* FONTS */
#define TEG_DIALOG_X	450
#define TEG_DIALOG_Y	200
#define TEG_DIALOG_Y_NEW 45

void generic_window_set_parent (GtkWidget * dialog, GtkWindow   * parent)
{
	g_return_if_fail(dialog != NULL);
	g_return_if_fail(parent != NULL);
	g_return_if_fail(GTK_IS_WINDOW(parent));

	gtk_window_set_transient_for (GTK_WINDOW(dialog), parent);



	if ( ! gtk_widget_get_visible(GTK_WIDGET(parent))) return; /* Can't get its
						  size/pos */

	gtk_window_set_position(GTK_WINDOW(dialog),
	                        GTK_WIN_POS_CENTER_ON_PARENT);
}

/**
 * @fn char * load_pixmap_file( char *name )
 * Carga un pixmap. Busca en $prefix, localdir & $datadir
 * @param name Pixmap a buscar
 * @return NULL si no encontro o path del pixmap. Hay que g_free ese string
 */
char * load_pixmap_file( char *name )
{
	char *filename = NULL;
	char *f = NULL;

	f = g_strconcat( "teg_pix/", name, NULL );
	if( f == NULL )
		return NULL;

	filename = g_strconcat( PIXMAPDIR,f,NULL );
	g_free( f );
	return filename;
}

/**
 * @fn void teg_dialog( char* title, char* bigtitle, char* data )
 * Crea un dialogo un poco mas vistoso, onda gnome_about
 * @param title Titulo de la ventana
 * @param bittitle Titulo del dialogo
 * @param data Informacion del dialogo
 */
void teg_dialog( char* title, char* bigtitle, char* data )
{
	GtkWidget* dialog;
	GtkWidget* canvas;

	dialog = gtk_dialog_new_with_buttons(title,
	                                     GTK_WINDOW(main_window),
	                                     GTK_DIALOG_DESTROY_WITH_PARENT,
	                                     _("_OK"), GTK_RESPONSE_OK,
	                                     NULL );

	canvas = goo_canvas_new();
	gtk_widget_set_size_request ( canvas, TEG_DIALOG_X, TEG_DIALOG_Y );
	goo_canvas_set_bounds (GOO_CANVAS (canvas), 0, 0, TEG_DIALOG_X,
	                       TEG_DIALOG_Y);

	goo_canvas_rect_new(
		goo_canvas_get_root_item(GOO_CANVAS(canvas)),
		0.0,
		0.0,
		(double) TEG_DIALOG_X,
		45.0,
		"fill-color","black",
		"stroke-color","black",
		NULL);

	goo_canvas_rect_new(
		goo_canvas_get_root_item(GOO_CANVAS(canvas)),
		0.0,
		45.0,
		(double) TEG_DIALOG_X,
		(double) TEG_DIALOG_Y,
		"fill-color","light green",
		"stroke-color","light green",
		NULL);

	goo_canvas_text_new(
		goo_canvas_get_root_item(GOO_CANVAS(canvas)),
		bigtitle,
		(double) (TEG_DIALOG_X/2),
		(double) 10,
		-1,
		GOO_CANVAS_ANCHOR_NORTH,
		"font", HELVETICA_20_BFONT,
		"fill-color", "white",
		NULL);

	goo_canvas_text_new(
		goo_canvas_get_root_item(GOO_CANVAS(canvas)),
		data,
		(double) 4,
		(double) 60,
		-1,
		GOO_CANVAS_ANCHOR_NORTH_WEST,
		"font", HELVETICA_12_FONT,
		"fill-color", "black",
		NULL);

	gtk_box_pack_start( GTK_BOX(gtk_dialog_get_content_area
	                            (GTK_DIALOG(dialog))), GTK_WIDGET(canvas),
	                    TRUE, TRUE, 0);

	gtk_widget_show (canvas);
	gtk_widget_show_all(dialog);
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);

	return;
}

/**
 * @fn GtkWidget* teg_dialog_new( char* title, char* bigtitle )
 * Crea un gnome_dialog_new un poco diferente
 */
GtkWidget* teg_dialog_new( char* title, char* bigtitle )
{
	GtkWidget* dialog;
	GtkWidget* canvas;

	dialog = gtk_dialog_new ();

	gtk_window_set_title (GTK_WINDOW (dialog), title);
	gtk_window_set_transient_for ( GTK_WINDOW(dialog),
	                               GTK_WINDOW(main_window) );

	canvas = goo_canvas_new();
	gtk_widget_set_size_request (canvas, TEG_DIALOG_X, TEG_DIALOG_Y_NEW);
	gtk_widget_set_halign (canvas, GTK_ALIGN_CENTER);
	goo_canvas_set_bounds (GOO_CANVAS (canvas), 0, 0, TEG_DIALOG_X,
	                       TEG_DIALOG_Y_NEW);

	goo_canvas_rect_new(
		goo_canvas_get_root_item(GOO_CANVAS(canvas)),
		0.0,
		0.0,
		(double) TEG_DIALOG_X,
		45.0,
		"fill-color","black",
		"stroke-color","black",
		NULL);

	goo_canvas_text_new(
		goo_canvas_get_root_item(GOO_CANVAS(canvas)),
		bigtitle,
		(double) (TEG_DIALOG_X/2),
		(double) 10,
		-1,
		GOO_CANVAS_ANCHOR_NORTH,
		"font", HELVETICA_20_BFONT,
		"fill-color", "white",
		NULL);


	gtk_box_pack_start( GTK_BOX(gtk_dialog_get_content_area
	                            (GTK_DIALOG(dialog))), GTK_WIDGET(canvas),
	                    TRUE, TRUE, 0 );

	gtk_widget_show (canvas);

	return dialog;
}

/**
 * @fn void dialog_close( GtkWidget *button, gpointer data )
 * Closes a generic dialog
 */
void dialog_close( GtkWidget *button, gpointer data )
{
	GtkWidget *dialog = (GtkWidget*) data;

	gtk_widget_destroy( dialog );
	return;
}


void teg_dialog_gameover( int numjug, int mission )
{
	GtkWidget *dialog;
	PCPLAYER pJ;
	GtkWidget *hbox;
	GtkWidget *frame;
	char buf[512];

	if( player_whois( numjug, &pJ) != TEG_STATUS_SUCCESS) {
		dialog = teg_dialog_new(_("Game Over"),_("A player won the game"));

	} else  if( numjug == WHOAMI() ) {
		dialog = teg_dialog_new(_("Game Over"),_("You are the winner"));

	} else {
		snprintf(buf,sizeof(buf)-1,_("%s is the winner"),pJ->name );
		buf[ sizeof(buf) -1 ] = 0;
		dialog = teg_dialog_new(_("Game Over"),buf);
	}

	hbox = gtk_box_new ( GTK_ORIENTATION_HORIZONTAL, 3 );

	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area
	                           (GTK_DIALOG(dialog))), hbox, TRUE, TRUE, 0);


	/* show mission */
	frame = gtk_frame_new( _("Accomplished mission") );
	gtk_container_set_border_width( GTK_CONTAINER(frame), 3 );
	gtk_container_add( GTK_CONTAINER(hbox), frame );

	mission_view_fake_number( frame, mission );


	/* show scores */
	frame = gtk_frame_new( _("Players' Scores") );
	gtk_container_set_border_width( GTK_CONTAINER(frame), 3 );
	gtk_container_add( GTK_CONTAINER(hbox), frame );

	gui_scores_embed( frame );
	
	gtk_dialog_add_button(GTK_DIALOG(dialog), _("_OK"), GTK_RESPONSE_OK);

	gtk_widget_show_all (dialog);
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

gchar *translate_to_utf8(const gchar *string )
{
	iconv_t	iconv_base;
	size_t	input_length, output_length;
	gchar	*input_string, *input_string_pointer;
	gchar	*output_string, *output_string_pointer;
	
	if(!string)
		return NULL;

	if( g_utf8_validate( string, -1, NULL ) )
		return g_strdup( string );
	
	iconv_base = iconv_open("UTF-8", "iso-8859-1");

	if(iconv_base==(iconv_t) -1)
		return g_strdup(string);

	input_string = input_string_pointer = g_strdup(string);
	input_length = (strlen(input_string) + 1);
	
	output_length = (input_length << 1);
	output_string = output_string_pointer = g_malloc(output_length);

	iconv(iconv_base, &input_string, &input_length, &output_string, &output_length);

	free( input_string_pointer );
	
	iconv_close(iconv_base);

	return output_string_pointer;
}
