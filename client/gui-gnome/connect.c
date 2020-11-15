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
/*
 * connection dialogs, color dialogs, and game type
 *
 * some metaserver code and ideas were taken from freeciv-gtk-2.0 client
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <assert.h>
#include <goocanvas.h>
#include <glib/gi18n.h>

#include "gui.h"
#include "client.h"
#include "support.h"
#include "interface.h"
#include "callbacks.h"
#include "connect.h"
#include "priv.h"
#include "colors.h"

#define GNOME_PAD_SMALL 4

static GtkWidget *connect_window = NULL;
static GtkWidget *con_entry_name=NULL;
static GtkWidget *con_spinner_port=NULL;
static GtkWidget *con_entry_server=NULL;

static GtkWidget *button_launch=NULL;

static GtkWidget *button_observe=NULL;

static GtkWidget *colortype_dialog=NULL;

static GtkWidget *gametype_dialog=NULL;
static GtkWidget *gametype_button_conqworld=NULL;
static GtkWidget *gametype_button_secret=NULL;
static GtkWidget *gametype_button_viewall=NULL;
static GtkWidget *gametype_button_fow=NULL;
static GtkWidget *gametype_button_commonmission=NULL;
static GtkWidget *gametype_spinner_armies1=NULL;
static GtkWidget *gametype_spinner_armies2=NULL;


static GtkWidget *boton_color[TEG_MAX_PLAYERS] = { NULL, NULL, NULL, NULL, NULL, NULL };

static GIOChannel *channel = NULL;

void shutdown_channel(void)
{
	if(channel == NULL) {
		return;
	}

	if (gui_private.tag > 0) {
		    g_source_remove(gui_private.tag);
			gui_private.tag = 0;
	}

	g_io_channel_shutdown (channel, FALSE, NULL);
	g_io_channel_unref (channel);
	channel = NULL;
}

static TEG_STATUS connect_real()
{
	if( teg_connect() == TEG_STATUS_SUCCESS ) {
	        if ( !channel )
	                channel = g_io_channel_unix_new ( g_game.fd );

	        gui_private.tag = g_io_add_watch_full( channel,
	                                               G_PRIORITY_DEFAULT,
			                                       G_IO_IN | G_IO_PRI | G_IO_ERR | G_IO_HUP | G_IO_NVAL,
	                                               (GIOFunc) pre_client_recv,
	                                               NULL, NULL );

		out_id();
		for(size_t i=0; i<COUNTRIES_CANT; i++)
			gui_country( i );

		return TEG_STATUS_SUCCESS;
	}
	return TEG_STATUS_ERROR;
}

static void connect_button_con_cb(GtkDialog *dialog, gint id, gpointer user_data)
{
        if ( id != 0 )
                return;

	string_copy(g_game.myname, sizeof(g_game.myname), gtk_entry_get_text(GTK_ENTRY(con_entry_name)));
	string_copy(g_game.sername, sizeof(g_game.sername), gtk_entry_get_text(GTK_ENTRY(con_entry_server)));
	g_game.serport = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(con_spinner_port));

	g_game.observer = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
	                                               (button_observe));

	/* if you checked "start local server" we'll copy "localhost" to g_game.sername */
	if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(button_launch) ) ) {
	        launch_server(g_game.serport);
	}

	g_settings_set_int(settings, "port", g_game.serport);
	g_settings_set_string(settings, "servername", g_game.sername);
	g_settings_set_string(settings, "playername", g_game.myname);

	connect_real();
}

/*
 * Called when something has changed on the properybox
 */
static void
prop_changed (GtkWidget *w, void *n)
{
	if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(button_launch) ) )
		gtk_entry_set_text( GTK_ENTRY( con_entry_server ), "localhost");
}

static void button_secret_enabled( GtkWidget *w, void *n )
{
	if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(gametype_button_secret) ) )
		gtk_widget_set_sensitive( gametype_button_commonmission, 1 );
	else
		gtk_widget_set_sensitive( gametype_button_commonmission, 0 );
}

/* connection window */
void connect_view()
{
	GtkWidget *label;
	GtkWidget *table;
	GtkWidget *frame;
        GtkAdjustment *adj;

	connect_window = teg_dialog_new(_("Connect to server"),_("Connect to server"));
	gtk_dialog_add_buttons(GTK_DIALOG(connect_window),
	                       _("_OK"), 0, _("_Cancel"), 1,
		NULL );
	gtk_dialog_set_default_response(GTK_DIALOG(connect_window),0);

	g_signal_connect (connect_window, "response",
	                  G_CALLBACK (connect_button_con_cb), NULL);

	GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area
	                           (GTK_DIALOG(connect_window))), vbox,
	                   TRUE, TRUE, 0);

	/* server options */
	table = gtk_grid_new ();
	gtk_grid_set_column_homogeneous (GTK_GRID (table), TRUE);
	gtk_container_set_border_width (GTK_CONTAINER (table), GNOME_PAD_SMALL);
	gtk_grid_set_row_spacing (GTK_GRID (table), GNOME_PAD_SMALL);
	gtk_grid_set_column_spacing (GTK_GRID (table), GNOME_PAD_SMALL);

	frame = gtk_frame_new (_("Server Options"));
	gtk_container_set_border_width (GTK_CONTAINER (frame), 1);
	gtk_container_add(GTK_CONTAINER(vbox), frame);

	/* server port */
	label = gtk_label_new(_("Server port:"));
	gtk_grid_attach( GTK_GRID(table), label, 0, 0, 1, 1 );
	adj = (GtkAdjustment *) gtk_adjustment_new( g_game.serport, 1.0, 65536.0, 1.0, 5.0, 1.0 );
	con_spinner_port = gtk_spin_button_new( adj, 0.0, 0);
	gtk_grid_attach (GTK_GRID(table), con_spinner_port, 1, 0, 1, 1);

	/* server name */
	label = gtk_label_new(_("Server name:"));
	gtk_grid_attach( GTK_GRID(table), label, 0, 1, 1, 1);
	con_entry_server = gtk_entry_new( );
	gtk_entry_set_text( GTK_ENTRY( con_entry_server ), g_game.sername);
	gtk_grid_attach (GTK_GRID (table), con_entry_server, 1, 1, 1, 1);
	

	/* player name */
	label = gtk_label_new(_("Name:"));
	gtk_grid_attach( GTK_GRID(table), label, 0, 2, 1, 1);
	con_entry_name = gtk_entry_new( );
	gtk_entry_set_text( GTK_ENTRY( con_entry_name ), g_game.myname);
	gtk_grid_attach( GTK_GRID(table), con_entry_name, 1, 2, 1, 1 );

	gtk_container_add(GTK_CONTAINER( frame), table );

	/* launch localhost server */
	button_launch = gtk_check_button_new_with_label(_("Start server locally"));
	gtk_container_add(GTK_CONTAINER(vbox), button_launch);
	g_signal_connect (G_OBJECT (button_launch), "toggled",
	    	G_CALLBACK (prop_changed), NULL);

	/* observer mode */
	button_observe = gtk_check_button_new_with_label(_("Dont play, just observe"));
	gtk_container_add(GTK_CONTAINER(vbox), button_observe);

/* end*/
	gtk_widget_show_all(connect_window);
	gtk_dialog_run(GTK_DIALOG (connect_window));
	gtk_widget_destroy(connect_window);
}

/*
 * Select a color
 */
static void colortype_ok_cb (GtkDialog *dialog, gint id, gpointer data )
{
	int i;

	for(i=0;i<TEG_MAX_PLAYERS;i++)  {
		if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( boton_color[i])) )
			break;
	}

	out_color( i );
}

static GtkWidget *create_color_button( int i )
{
	GtkWidget *vbox;
	GooCanvasItem *image;
	GtkWidget	*canvas;

	if( i < 0 || i >= TEG_MAX_PLAYERS )
		return NULL;

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	if( ! vbox )
		return NULL;

	canvas = goo_canvas_new();
	if( ! canvas )
		return NULL;

	gtk_widget_set_size_request (canvas,
	                             gdk_pixbuf_get_width(g_color_players[i]),
	                             gdk_pixbuf_get_height(g_color_players[i]));
	goo_canvas_set_bounds (GOO_CANVAS (canvas), 0, 0,
			gdk_pixbuf_get_width(g_color_players[i]),
			gdk_pixbuf_get_height(g_color_players[i]) );
	image = goo_canvas_image_new(
		goo_canvas_get_root_item(GOO_CANVAS(canvas)),
		g_color_players[i],
		0.0,
		0.0,
		"width", (double) gdk_pixbuf_get_width(g_color_players[i]),
		"height", (double) gdk_pixbuf_get_height(g_color_players[i]),
		NULL);

	if( ! image )
		return NULL;

	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(canvas), TRUE, TRUE, 0);
	gtk_widget_show (canvas);

	if( i==0 ) {
		boton_color[i] = gtk_radio_button_new_with_label (NULL, _(g_colores[i]) );
	} else {
		boton_color[i] = gtk_radio_button_new_with_label_from_widget ( GTK_RADIO_BUTTON(boton_color[0]), _(g_colores[i]));
	}

	gtk_box_pack_start(GTK_BOX (vbox), boton_color[i], TRUE, TRUE, 0);
	gtk_widget_show ( boton_color[i] );

	return vbox;
}

/* show dialog of 'choose color' */
void colortype_view( char *c)
{
	GtkWidget *frame;
	GtkWidget *vbox_dia;
	GtkWidget *table;
	int i;
	int first_active =1;

	colortype_dialog = teg_dialog_new(_("Select your color"),_("Select your color"));

	gtk_dialog_add_button( GTK_DIALOG(colortype_dialog), _("_OK"), 0);

	g_signal_connect (colortype_dialog, "response",
	                  G_CALLBACK (colortype_ok_cb), NULL);

	vbox_dia = gtk_dialog_get_content_area (GTK_DIALOG(colortype_dialog));

	/* desired color */
	frame = gtk_frame_new (_("Select your desired color"));
	gtk_container_set_border_width (GTK_CONTAINER (frame), 0);

	table = gtk_grid_new ();
	gtk_container_set_border_width (GTK_CONTAINER (table), GNOME_PAD_SMALL);
	gtk_grid_set_row_spacing (GTK_GRID (table), GNOME_PAD_SMALL);
	gtk_grid_set_column_spacing (GTK_GRID (table), GNOME_PAD_SMALL);
	gtk_grid_set_column_homogeneous (GTK_GRID (table), TRUE);


	for(i=0;i<TEG_MAX_PLAYERS;i++) {
		GtkWidget *button = create_color_button( i );

		if( i%2 == 0 ) {
	                gtk_grid_attach(GTK_GRID(table), button, i/2, 0, 1, 1);
		} else {
	                gtk_grid_attach(GTK_GRID(table), button, (i-1)/2, 1, 1, 1);
		}

		/* UGLY: I know that boton_color[i] is used... */
		gtk_widget_set_sensitive( button, !c[i] );
		if( (!c[i]) && first_active ) {
			first_active = 0;
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (boton_color[i]), TRUE);
		}
	}

	gtk_container_add (GTK_CONTAINER (frame), table);
	gtk_container_add (GTK_CONTAINER( vbox_dia), frame );

	gtk_widget_show_all (colortype_dialog);
	gtk_dialog_run (GTK_DIALOG (colortype_dialog));
	gtk_widget_destroy (colortype_dialog);
}

/* type of game: with secret missions, or to conquer the world */
static void gametype_ok_cb (GtkDialog *dialog, gint id, gpointer data)
{
	int a,b,c,ar1,ar2;

	if ( id != 0 )
	        return;

	a = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(gametype_button_conqworld));
	b = ! gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(gametype_button_viewall));
	c = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(gametype_button_commonmission) );
	ar1 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(gametype_spinner_armies1));
	ar2 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(gametype_spinner_armies2));

	out_set_typeofgame( a, b, c, ar1, ar2 );
	out_start();
}

/* Shows the dialog that choose the game type */
void gametype_view( void )
{
	GtkWidget *frame;
	GtkWidget *vbox_dia,*vbox, *hbox, *label;
        GtkAdjustment *adj;

	gametype_dialog = teg_dialog_new(_("Select type of game"),_("Select type of game"));

	gtk_dialog_add_button( GTK_DIALOG(gametype_dialog), _("_OK"), 0);

	g_signal_connect (gametype_dialog, "response",
	                  G_CALLBACK (gametype_ok_cb), NULL);

	vbox_dia = gtk_dialog_get_content_area (GTK_DIALOG(gametype_dialog));

	/* Conquer the world / Secret missions */
	frame = gtk_frame_new (_("Missions"));
	gtk_container_set_border_width (GTK_CONTAINER (frame), 0);

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

	gametype_button_conqworld =
		gtk_radio_button_new_with_label (
				NULL, _("Play to conquer the world"));
	gtk_box_pack_start(GTK_BOX (vbox), gametype_button_conqworld,
	                  TRUE, TRUE, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gametype_button_conqworld), TRUE);

	gametype_button_secret =
		gtk_radio_button_new_with_label (
	                gtk_radio_button_get_group(GTK_RADIO_BUTTON
	                                           (gametype_button_conqworld)),
	                _("Play with secret missions"));
	gtk_box_pack_start(GTK_BOX (vbox), gametype_button_secret,
	                   TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (gametype_button_secret), "toggled",
	                  G_CALLBACK (button_secret_enabled), NULL);

	hbox = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 0 );
	gametype_button_commonmission = gtk_check_button_new_with_label(_("Play with common mission\n(One wins, also, conquering 30 countries)"));
	gtk_widget_set_sensitive( gametype_button_commonmission, 0 );
	gtk_box_pack_start(GTK_BOX (hbox), gametype_button_commonmission,
	                   TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX (vbox), hbox, TRUE, TRUE, 0);

	gtk_container_add (GTK_CONTAINER (frame), vbox);

	gtk_container_add (GTK_CONTAINER( vbox_dia), frame );

	/* Fog of War / Normal */
	frame = gtk_frame_new (_("Visibility"));
	gtk_container_set_border_width (GTK_CONTAINER (frame), 0);

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

	gametype_button_viewall=
		gtk_radio_button_new_with_label (
				NULL, _("Standard (You see all countries)"));
	gtk_box_pack_start(GTK_BOX (vbox), gametype_button_viewall,
	                   TRUE, TRUE, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gametype_button_viewall), TRUE);

	gametype_button_fow=
		gtk_radio_button_new_with_label (
			gtk_radio_button_get_group (GTK_RADIO_BUTTON
	                                            (gametype_button_viewall)),
	                _("Fog of War (You see only border countries)"));
	gtk_box_pack_start(GTK_BOX (vbox), gametype_button_fow, TRUE, TRUE, 0);

	gtk_container_add (GTK_CONTAINER (frame), vbox);

	gtk_container_add (GTK_CONTAINER( vbox_dia), frame );

	/* Armies */
	frame = gtk_frame_new (_("Armies"));
	gtk_container_set_border_width (GTK_CONTAINER (frame), 0);

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

	/* 1 */
	hbox = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 0 );
	label = gtk_label_new(_("Armies to place in the 1st step"));
	gtk_box_pack_start(GTK_BOX (hbox), label, TRUE, TRUE, 0);
	adj = (GtkAdjustment *) gtk_adjustment_new( 5, 1.0, 30.0, 1.0, 5.0, 1.0 );
	gametype_spinner_armies1 = gtk_spin_button_new( adj, 0.0, 0);
	gtk_box_pack_start(GTK_BOX (hbox), gametype_spinner_armies1,
	                   TRUE, TRUE, 0);
	gtk_container_add (GTK_CONTAINER (vbox), hbox);

	/* 2 */
	hbox = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 0 );
	label = gtk_label_new(_("Armies to place in the 2nd step"));
	gtk_box_pack_start(GTK_BOX (hbox), label, TRUE, TRUE, 0);
	adj = (GtkAdjustment *) gtk_adjustment_new( 3, 1.0, 25.0, 1.0, 5.0, 1.0 );
	gametype_spinner_armies2 = gtk_spin_button_new( adj, 0.0, 0);
	gtk_box_pack_start (GTK_BOX (hbox), gametype_spinner_armies2,
	                    TRUE, TRUE, 0);
	gtk_container_add (GTK_CONTAINER (vbox), hbox);


	gtk_container_add (GTK_CONTAINER (frame), vbox);

	gtk_container_add (GTK_CONTAINER( vbox_dia), frame );

	gtk_widget_show_all (gametype_dialog);
	gtk_dialog_run (GTK_DIALOG (gametype_dialog));
	gtk_widget_destroy (gametype_dialog);
}
