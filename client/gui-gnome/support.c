/*	$Id: support.c,v 1.36 2006/03/14 12:34:18 nordi Exp $	*/
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

#include <gnome.h>

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
	gint x, y, w, h, dialog_x, dialog_y;

	g_return_if_fail(dialog != NULL);
	g_return_if_fail(parent != NULL);
	g_return_if_fail(GTK_IS_WINDOW(parent));

	gtk_window_set_transient_for (GTK_WINDOW(dialog), parent);



	if ( ! GTK_WIDGET_VISIBLE(parent)) return; /* Can't get its
						  size/pos */

	/* Throw out other positioning */
	gtk_window_set_position(GTK_WINDOW(dialog),GTK_WIN_POS_NONE);

	gdk_window_get_origin (GTK_WIDGET(parent)->window, &x, &y);
	gdk_window_get_size   (GTK_WIDGET(parent)->window, &w, &h);

	/*
	 * The problem here is we don't know how big the dialog is.
	 * So "centered" isn't really true. We'll go with 
	 * "kind of more or less on top"
	 */

	dialog_x = x + w/4;
	dialog_y = y + h/4;

	gtk_widget_set_uposition(GTK_WIDGET(dialog), dialog_x, dialog_y); 
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

	filename = gnome_pixmap_file (f);
	if( filename == NULL ) {
		filename = g_strconcat( PIXMAPDIR,f,NULL );
	}
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
	GnomeCanvasItem *item;

	dialog = gnome_dialog_new(title, 
			GNOME_STOCK_BUTTON_OK,
			NULL );
	gnome_dialog_set_parent( GNOME_DIALOG(dialog), GTK_WINDOW(main_window));
	gnome_dialog_set_close( GNOME_DIALOG(dialog), TRUE );
	

	canvas = gnome_canvas_new();
	gtk_widget_set_usize (canvas, TEG_DIALOG_X, TEG_DIALOG_Y );
	gnome_canvas_set_scroll_region (GNOME_CANVAS (canvas), 0, 0, TEG_DIALOG_X, TEG_DIALOG_Y );

	item = gnome_canvas_item_new(
		gnome_canvas_root(GNOME_CANVAS(canvas)),
		gnome_canvas_rect_get_type (),
		"x1", 0.0,
		"y1", 0.0,
		"x2", (double) TEG_DIALOG_X,
		"y2", 45.0,
		"fill_color","black",
		"outline_color","black",
		NULL);

	item = gnome_canvas_item_new(
		gnome_canvas_root(GNOME_CANVAS(canvas)),
		gnome_canvas_rect_get_type (),
		"x1", 0.0,
		"y1", 45.0,
		"x2", (double) TEG_DIALOG_X,
		"y2", (double) TEG_DIALOG_Y,
		"fill_color","light green",
		"outline_color","light green",
		NULL);

	item = gnome_canvas_item_new(
		gnome_canvas_root(GNOME_CANVAS(canvas)),
		gnome_canvas_text_get_type(),
		"text",bigtitle,
		"x", (double) (TEG_DIALOG_X/2),
		"y", (double) 10,
		"x_offset", (double) -1,
		"y_offset", (double) -1,
		"font", HELVETICA_20_BFONT,
		"fill_color", "white",
		"anchor",GTK_ANCHOR_NORTH,
		NULL);

	item = gnome_canvas_item_new(
		gnome_canvas_root(GNOME_CANVAS(canvas)),
		gnome_canvas_text_get_type(),
		"text",data,
		"x", (double) 4,
		"y", (double) 60,
		"x_offset", (double) -1,
		"y_offset", (double) -1,
		"font", HELVETICA_12_FONT,
		"fill_color", "black",
		"anchor",GTK_ANCHOR_NW,
		NULL);

	gtk_box_pack_start_defaults( GTK_BOX(GNOME_DIALOG(dialog)->vbox), GTK_WIDGET(canvas));

	gtk_widget_show (canvas);
	gtk_widget_show_all(dialog);

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
	GnomeCanvasItem *item;

	dialog = gnome_dialog_new(title, NULL );

	gnome_dialog_set_parent( GNOME_DIALOG(dialog), GTK_WINDOW(main_window));

	canvas = gnome_canvas_new();
	gtk_widget_set_usize (canvas, TEG_DIALOG_X, TEG_DIALOG_Y_NEW );
	gnome_canvas_set_scroll_region (GNOME_CANVAS (canvas), 0, 0, TEG_DIALOG_X, TEG_DIALOG_Y_NEW );

	item = gnome_canvas_item_new(
		gnome_canvas_root(GNOME_CANVAS(canvas)),
		gnome_canvas_rect_get_type (),
		"x1", 0.0,
		"y1", 0.0,
		"x2", (double) TEG_DIALOG_X,
		"y2", 45.0,
		"fill_color","black",
		"outline_color","black",
		NULL);

	item = gnome_canvas_item_new(
		gnome_canvas_root(GNOME_CANVAS(canvas)),
		gnome_canvas_text_get_type(),
		"text",bigtitle,
		"x", (double) (TEG_DIALOG_X/2),
		"y", (double) 10,
		"x_offset", (double) -1,
		"y_offset", (double) -1,
		"font", HELVETICA_20_BFONT,
		"fill_color", "white",
		"anchor",GTK_ANCHOR_NORTH,
		NULL);


	gtk_box_pack_start_defaults( GTK_BOX(GNOME_DIALOG(dialog)->vbox), GTK_WIDGET(canvas));

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

	gnome_dialog_close( GNOME_DIALOG(dialog) );
	return;
}


GtkWidget * teg_dialog_gameover( int numjug, int mission )
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

	hbox = gtk_hbox_new ( FALSE, 3);

	gtk_box_pack_start_defaults( GTK_BOX(GNOME_DIALOG(dialog)->vbox), hbox);


	/* show mission */
	frame = gtk_frame_new( _("Accomplished mission") );
	gtk_container_border_width( GTK_CONTAINER(frame), 3 );
	gtk_container_add( GTK_CONTAINER(hbox), frame );

	mission_view_fake_number( frame, mission );


	/* show scores */
	frame = gtk_frame_new( _("Players' Scores") );
	gtk_container_border_width( GTK_CONTAINER(frame), 3 );
	gtk_container_add( GTK_CONTAINER(hbox), frame );

	gui_scores_embed( frame );
	
	gnome_dialog_append_buttons(GNOME_DIALOG(dialog),
			GNOME_STOCK_BUTTON_OK,
			NULL );
	gnome_dialog_button_connect (GNOME_DIALOG(dialog),
			0, GTK_SIGNAL_FUNC(dialog_close), dialog );

	gtk_widget_show_all(dialog);
	raise_and_focus(dialog);

	return dialog;
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

#if defined __GLIBC__ && __GLIBC_MINOR__ <= 1
	iconv(iconv_base, (const gchar **) &input_string, &input_length, &output_string, &output_length);
#else
	iconv(iconv_base, &input_string, &input_length, &output_string, &output_length);
#endif

	free( input_string_pointer );
	
	iconv_close(iconv_base);

	return output_string_pointer;
}
