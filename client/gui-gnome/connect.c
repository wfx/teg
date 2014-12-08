/*	$Id: connect.c,v 1.7 2007/07/08 13:47:21 nordi Exp $	*/
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
#include <gnome.h>
#include <gconf/gconf-client.h>

#include "gui.h"
#include "client.h"
#include "support.h"
#include "interface.h"
#include "callbacks.h"
#include "connect.h"
#include "priv.h"
#include "colors.h"
#include "ggz_client.h"

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

/* metaserver */
static GtkListStore *metaserver_store;
static void meta_list_callback(GtkTreeSelection *select, GtkTreeModel *model);
static void meta_update_callback(GtkWidget *w, gpointer data);

enum {
	METASERVER_NAME,
	METASERVER_PORT,
	METASERVER_VERSION,
	METASERVER_COMMENT,
};



static TEG_STATUS connect_real()
{
	if( teg_connect() == TEG_STATUS_SUCCESS ) {
		gui_private.tag = gdk_input_add( g_game.fd, GDK_INPUT_READ, (GdkInputFunction) pre_client_recv, (gpointer) NULL );

		if( !g_game.with_ggz ) {
			out_id();
			if( connect_window ) destroy_window( connect_window, &connect_window );
		}
#ifdef WITH_GGZ
		else {
			gui_private.tag_ggz = gdk_input_add( ggz_client_get_fd(), GDK_INPUT_READ, (GdkInputFunction) ggz_client_handle, (gpointer) NULL );
		}
#endif /* WITH_GGZ */
		countries_redraw_all();
		return TEG_STATUS_SUCCESS;
	}
	return TEG_STATUS_ERROR;
}

static gint connect_button_con_cb(GtkWidget *area, GdkEventExpose *event, gpointer user_data)
{
	strncpy(g_game.myname,gtk_entry_get_text(GTK_ENTRY(con_entry_name)),PLAYERNAME_MAX_LEN);
	strncpy(g_game.sername,gtk_entry_get_text(GTK_ENTRY(con_entry_server)),SERVER_NAMELEN);
	g_game.serport = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(con_spinner_port));

	g_game.observer = GTK_TOGGLE_BUTTON(button_observe)->active;

	/* if you checked "start local server" we'll copy "localhost" to g_game.sername */
	if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(button_launch) ) ) {
		launch_server(g_game.serport);
	}

	gconf_client_set_int(g_conf_client, "/apps/teg/port", g_game.serport, NULL);
	gconf_client_set_string(g_conf_client, "/apps/teg/servername", g_game.sername, NULL);
	gconf_client_set_string(g_conf_client, "/apps/teg/playername", g_game.myname, NULL);

	connect_real();

	return FALSE;
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

	/* metaserver */
	GtkWidget *book, *vbox, *list, *scrolled, *update;
	GtkTreeSelection *selection;
	GtkCellRenderer *renderer;

#ifdef WITH_GGZ
	if( g_game.with_ggz ) {
		connect_real();
		return;
	}
#endif /* WITH_GGZ */


	if( connect_window != NULL ) {
		gtk_widget_show_all(connect_window);
		raise_and_focus(connect_window);
		return ;
	}

	connect_window = teg_dialog_new(_("Connect to server"),_("Connect to server")); 
	gnome_dialog_append_buttons(GNOME_DIALOG(connect_window),
		GNOME_STOCK_BUTTON_OK,
		GNOME_STOCK_BUTTON_CANCEL,
		NULL );
	gnome_dialog_set_default(GNOME_DIALOG(connect_window),0);

	gnome_dialog_button_connect (GNOME_DIALOG(connect_window),
		1, GTK_SIGNAL_FUNC(destroy_window),&connect_window);

	gnome_dialog_button_connect (GNOME_DIALOG(connect_window),
		0, GTK_SIGNAL_FUNC(connect_button_con_cb),connect_window);

	gtk_signal_connect( GTK_OBJECT(connect_window),
			"delete_event", GTK_SIGNAL_FUNC(destroy_window),
			&connect_window);

	gtk_signal_connect( GTK_OBJECT(connect_window),
			"destroy", GTK_SIGNAL_FUNC(destroy_window),
			&connect_window);



/* notebook: TEG Server Selection */
	book = gtk_notebook_new();
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(connect_window)->vbox), book, TRUE, TRUE, 0);
	label=gtk_label_new(_("TEG Server Selection"));
	vbox=gtk_vbox_new(FALSE, 0);
	gtk_notebook_append_page(GTK_NOTEBOOK (book), vbox, label);


	/* server options */
	table = gtk_table_new (3, 2, TRUE);
	gtk_container_border_width (GTK_CONTAINER (table), GNOME_PAD_SMALL);
	gtk_table_set_row_spacings (GTK_TABLE (table), GNOME_PAD_SMALL);
	gtk_table_set_col_spacings (GTK_TABLE (table), GNOME_PAD_SMALL);

	frame = gtk_frame_new (_("Server Options"));
	gtk_container_border_width (GTK_CONTAINER (frame), 1);
	gtk_container_add(GTK_CONTAINER(vbox), frame);

	/* server port */
	label = gtk_label_new(_("Server port:"));
	gtk_table_attach_defaults( GTK_TABLE(table), label, 0, 1, 0, 1 );
	adj = (GtkAdjustment *) gtk_adjustment_new( g_game.serport, 1.0, 65536.0, 1.0, 5.0, 1.0 );
	con_spinner_port = gtk_spin_button_new( adj, 0.0, 0);
	gtk_table_attach_defaults( GTK_TABLE(table), con_spinner_port, 1, 2, 0, 1 );

	/* server name */
	label = gtk_label_new(_("Server name:"));
	gtk_table_attach_defaults( GTK_TABLE(table), label, 0, 1, 1, 2 );
	con_entry_server = gtk_entry_new( );
	gtk_entry_set_text( GTK_ENTRY( con_entry_server ), g_game.sername);
	gtk_table_attach_defaults( GTK_TABLE(table), con_entry_server, 1, 2, 1, 2 );
	

	/* player name */
	label = gtk_label_new(_("Name:"));
	gtk_table_attach_defaults( GTK_TABLE(table), label, 0, 1, 2, 3 );
	con_entry_name = gtk_entry_new( );
	gtk_entry_set_text( GTK_ENTRY( con_entry_name ), g_game.myname);
	gtk_table_attach_defaults( GTK_TABLE(table), con_entry_name, 1, 2, 2, 3 );


	gtk_container_add(GTK_CONTAINER( frame), table );

	/* launch localhost server */
	button_launch = gtk_check_button_new_with_label(_("Start server locally"));
	gtk_container_add(GTK_CONTAINER(vbox), button_launch);
	gtk_signal_connect (GTK_OBJECT (button_launch), "toggled",
	    	GTK_SIGNAL_FUNC (prop_changed), NULL);

	/* observer mode */
	button_observe = gtk_check_button_new_with_label(_("Don't play, just observe"));
	gtk_container_add(GTK_CONTAINER(vbox), button_observe);

/* notebook: TEG Server Selection */
	label=gtk_label_new(_("Metaserver"));
	vbox=gtk_vbox_new(FALSE, 0);
	gtk_notebook_append_page(GTK_NOTEBOOK (book), vbox, label);

	metaserver_store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING );

	list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(metaserver_store));
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list));
	g_object_unref(metaserver_store);
	gtk_tree_view_columns_autosize(GTK_TREE_VIEW(list));

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list),
		-1, _("Server Name"), renderer, "text", 0, NULL);
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list),
		-1, _("Port"), renderer, "text", 1, NULL);
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list),
		-1, _("Version"), renderer, "text", 2, NULL);
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list),
		-1, _("Status"), renderer, "text", 3, NULL);

	scrolled=gtk_scrolled_window_new(NULL,NULL);
	gtk_container_add(GTK_CONTAINER(scrolled), list);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);

	update=gtk_button_new_with_label(_("Update"));
	gtk_box_pack_start(GTK_BOX(vbox), update, FALSE, FALSE, 2);

	g_signal_connect(selection, "changed",
			G_CALLBACK(meta_list_callback), NULL);
	g_signal_connect(update, "clicked",
			G_CALLBACK(meta_update_callback), NULL);

/* end*/
	gtk_widget_show_all(connect_window);
	raise_and_focus(connect_window);
}

/*
 * Select a color
 */
static void colortype_ok_cb (GtkWidget *window )
{
	int i;

	for(i=0;i<TEG_MAX_PLAYERS;i++)  {
		if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( boton_color[i])) )
			break;
	}

	out_color( i );

	gtk_widget_destroy(colortype_dialog);
	colortype_dialog = NULL;
}

static GtkWidget *create_color_button( int i )
{
	GtkWidget *vbox;
	GnomeCanvasItem *image;
	GtkWidget	*canvas;

	if( i < 0 || i >= TEG_MAX_PLAYERS )
		return NULL;

	vbox = gtk_vbox_new (FALSE, 0);
	if( ! vbox )
		return NULL;

	canvas = gnome_canvas_new();
	if( ! canvas )
		return NULL;

	gtk_widget_set_usize (canvas, gdk_pixbuf_get_width(g_color_players[i]), gdk_pixbuf_get_height(g_color_players[i]) );
	gnome_canvas_set_scroll_region (GNOME_CANVAS (canvas), 0, 0, 
			gdk_pixbuf_get_width(g_color_players[i]),
			gdk_pixbuf_get_height(g_color_players[i]) );
	image = gnome_canvas_item_new(
		gnome_canvas_root(GNOME_CANVAS(canvas)),
		gnome_canvas_pixbuf_get_type (),
		"pixbuf", g_color_players[i],
		"x", 0.0,
		"y", 0.0,
		"width", (double) gdk_pixbuf_get_width(g_color_players[i]),
		"height", (double) gdk_pixbuf_get_height(g_color_players[i]),
		"anchor", GTK_ANCHOR_NW,
		NULL);

	if( ! image )
		return NULL;

	gtk_box_pack_start_defaults( GTK_BOX(vbox), GTK_WIDGET(canvas));
	gtk_widget_show (canvas);

	if( i==0 ) {
		boton_color[i] = gtk_radio_button_new_with_label (NULL, _(g_colores[i]) );
	} else {
		boton_color[i] = gtk_radio_button_new_with_label_from_widget ( GTK_RADIO_BUTTON(boton_color[0]), _(g_colores[i]));
	}

	gtk_box_pack_start_defaults(GTK_BOX (vbox), boton_color[i]);
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

	if( colortype_dialog != NULL ) {
		gdk_window_show( colortype_dialog->window);
		gdk_window_raise( colortype_dialog->window);
		return ;
	}

	colortype_dialog = teg_dialog_new(_("Select your color"),_("Select your color"));

	gtk_signal_connect (GTK_OBJECT (colortype_dialog), "destroy",
			GTK_SIGNAL_FUNC (gtk_widget_destroyed), &colortype_dialog);

	gnome_dialog_append_button( GNOME_DIALOG(colortype_dialog), GNOME_STOCK_BUTTON_OK);

	gnome_dialog_button_connect(GNOME_DIALOG(colortype_dialog), 0, GTK_SIGNAL_FUNC(colortype_ok_cb), NULL);

	vbox_dia = GNOME_DIALOG(colortype_dialog)->vbox;

	/* desired color */
	frame = gtk_frame_new (_("Select your desired color"));
	gtk_container_border_width (GTK_CONTAINER (frame), 0);

	table = gtk_table_new (2, 3, TRUE);
	gtk_container_border_width (GTK_CONTAINER (table), GNOME_PAD_SMALL);
	gtk_table_set_row_spacings (GTK_TABLE (table), GNOME_PAD_SMALL);
	gtk_table_set_col_spacings (GTK_TABLE (table), GNOME_PAD_SMALL);


	for(i=0;i<TEG_MAX_PLAYERS;i++) {
		GtkWidget *button = create_color_button( i );

		if( i%2 == 0 ) {
			gtk_table_attach_defaults( GTK_TABLE(table), button, i/2, (i/2)+1, 0, 1 );
		} else {
			gtk_table_attach_defaults( GTK_TABLE(table), button, (i-1)/2, ((i-1)/2)+1, 1, 2 );
		}

		/* UGLY: I know that boton_color[i] is used... */
		gtk_widget_set_sensitive( button, !c[i] );
		if( !c[i] & first_active ) {
			first_active = 0;
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (boton_color[i]), TRUE);
		}
	}

	gtk_container_add (GTK_CONTAINER (frame), table);
	gtk_container_add (GTK_CONTAINER( vbox_dia), frame );

	/* show all */
	if (!GTK_WIDGET_VISIBLE (colortype_dialog))
		gtk_widget_show_all (colortype_dialog);
	else
		gtk_widget_destroy (colortype_dialog);
}

/* type of game: with secret missions, or to conquer the world */
static void gametype_ok_cb (GtkWidget *window )
{
	int a,b,c,ar1,ar2;

	a = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(gametype_button_conqworld));
	b = ! gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(gametype_button_viewall));
	c = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(gametype_button_commonmission) );
	ar1 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(gametype_spinner_armies1));
	ar2 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(gametype_spinner_armies2));

	out_set_typeofgame( a, b, c, ar1, ar2 );
	out_start();

	gtk_widget_destroy(gametype_dialog);
	gametype_dialog = NULL;
}

/* Shows the dialog that choose the game type */
void gametype_view( void )
{
	GtkWidget *frame;
	GtkWidget *vbox_dia,*vbox, *hbox, *label;
        GtkAdjustment *adj;

	if( gametype_dialog != NULL ) {
		gdk_window_show( gametype_dialog->window);
		gdk_window_raise( gametype_dialog->window);
		return ;
	}

	gametype_dialog = teg_dialog_new(_("Select type of game"),_("Select type of game"));

	gtk_signal_connect (GTK_OBJECT (gametype_dialog), "destroy",
			GTK_SIGNAL_FUNC (gtk_widget_destroyed), &gametype_dialog);

	gnome_dialog_append_button( GNOME_DIALOG(gametype_dialog), GNOME_STOCK_BUTTON_OK);

	gnome_dialog_button_connect(GNOME_DIALOG(gametype_dialog), 0, GTK_SIGNAL_FUNC(gametype_ok_cb), NULL);

	vbox_dia = GNOME_DIALOG(gametype_dialog)->vbox;

	/* Conquer the world / Secret missions */
	frame = gtk_frame_new (_("Missions"));
	gtk_container_border_width (GTK_CONTAINER (frame), 0);

	vbox = gtk_vbox_new (FALSE, 0);

	gametype_button_conqworld =
		gtk_radio_button_new_with_label (
				NULL, _("Play to conquer the world"));
	gtk_box_pack_start_defaults(GTK_BOX (vbox), gametype_button_conqworld);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gametype_button_conqworld), TRUE);

	gametype_button_secret =
		gtk_radio_button_new_with_label (
			gtk_radio_button_group (GTK_RADIO_BUTTON (gametype_button_conqworld)), _("Play with secret missions"));
	gtk_box_pack_start_defaults(GTK_BOX (vbox), gametype_button_secret);
	gtk_signal_connect (GTK_OBJECT (gametype_button_secret), "toggled",
	    	GTK_SIGNAL_FUNC (button_secret_enabled), NULL);

	hbox = gtk_hbox_new( FALSE, 0 );
	gametype_button_commonmission = gtk_check_button_new_with_label(_("Play with common mission\n(One wins, also, conquering 30 countries)"));
	gtk_widget_set_sensitive( gametype_button_commonmission, 0 );
	gtk_box_pack_start_defaults(GTK_BOX (hbox), gametype_button_commonmission);
	gtk_box_pack_start_defaults(GTK_BOX (vbox), hbox);

	gtk_container_add (GTK_CONTAINER (frame), vbox);

	gtk_container_add (GTK_CONTAINER( vbox_dia), frame );

	/* Fog of War / Normal */
	frame = gtk_frame_new (_("Visibility"));
	gtk_container_border_width (GTK_CONTAINER (frame), 0);

	vbox = gtk_vbox_new (TRUE, 0);

	gametype_button_viewall=
		gtk_radio_button_new_with_label (
				NULL, _("Standard (You see all countries)"));
	gtk_box_pack_start_defaults(GTK_BOX (vbox), gametype_button_viewall);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gametype_button_viewall), TRUE);

	gametype_button_fow=
		gtk_radio_button_new_with_label (
			gtk_radio_button_group (GTK_RADIO_BUTTON (gametype_button_viewall)), _("Fog of War (You see only border countries)"));
	gtk_box_pack_start_defaults(GTK_BOX (vbox), gametype_button_fow);

	gtk_container_add (GTK_CONTAINER (frame), vbox);

	gtk_container_add (GTK_CONTAINER( vbox_dia), frame );

	/* Armies */
	frame = gtk_frame_new (_("Armies"));
	gtk_container_border_width (GTK_CONTAINER (frame), 0);

	vbox = gtk_vbox_new (TRUE, 0);

	/* 1 */
	hbox = gtk_hbox_new( FALSE, 0 );
	label = gtk_label_new(_("Armies to place in the 1st step"));
	gtk_box_pack_start_defaults(GTK_BOX (hbox), label);
	adj = (GtkAdjustment *) gtk_adjustment_new( 5, 1.0, 30.0, 1.0, 5.0, 1.0 );
	gametype_spinner_armies1 = gtk_spin_button_new( adj, 0.0, 0);
	gtk_box_pack_start_defaults(GTK_BOX (hbox), gametype_spinner_armies1);
	gtk_container_add (GTK_CONTAINER (vbox), hbox);

	/* 2 */
	hbox = gtk_hbox_new( FALSE, 0 );
	label = gtk_label_new(_("Armies to place in the 2nd step"));
	gtk_box_pack_start_defaults(GTK_BOX (hbox), label);
	adj = (GtkAdjustment *) gtk_adjustment_new( 3, 1.0, 25.0, 1.0, 5.0, 1.0 );
	gametype_spinner_armies2 = gtk_spin_button_new( adj, 0.0, 0);
	gtk_box_pack_start_defaults(GTK_BOX (hbox), gametype_spinner_armies2);
	gtk_container_add (GTK_CONTAINER (vbox), hbox);


	gtk_container_add (GTK_CONTAINER (frame), vbox);

	gtk_container_add (GTK_CONTAINER( vbox_dia), frame );


	/* show all */
	if (!GTK_WIDGET_VISIBLE (gametype_dialog))
		gtk_widget_show_all (gametype_dialog);
	else
		gtk_widget_destroy (gametype_dialog);
}


/* metaserver helpers */
static void meta_update_callback(GtkWidget *w, gpointer data)
{
	GtkTreeIter iter;
	PMETASERVER pM;
	PLIST_ENTRY l;

	gtk_list_store_clear( metaserver_store );

	if( metaserver_get_servers() != TEG_STATUS_SUCCESS )
		return;

	l = g_list_metaserver.Flink;

	while( !IsListEmpty( &g_list_metaserver ) && (l != &g_list_metaserver) )
	{
		pM = (PMETASERVER) l;

		gtk_list_store_append (metaserver_store, &iter);
		gtk_list_store_set (metaserver_store, &iter,
				METASERVER_NAME, pM->name,
				METASERVER_PORT, pM->port,
				METASERVER_VERSION, pM->version,
				METASERVER_COMMENT, pM->comment,
				-1 );

		l = LIST_NEXT(l);
	}

	return ;
}

static void meta_list_callback(GtkTreeSelection *select, GtkTreeModel *dummy)
{
	GtkTreeIter it;
	char *name;
	int port;

	if (!gtk_tree_selection_get_selected(select, NULL, &it))
		return;

	gtk_tree_model_get(GTK_TREE_MODEL(metaserver_store), &it,
			METASERVER_NAME, &name,
			METASERVER_PORT, &port,
			-1);

	gtk_entry_set_text( GTK_ENTRY( con_entry_server ), name);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(con_spinner_port), port);
}
