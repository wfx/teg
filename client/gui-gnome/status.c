/*	$Id: status.c,v 1.51 2007/07/07 19:10:20 nordi Exp $	*/
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
 * functions that manages the status dialog
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <assert.h>
#include <gnome.h>

#include "gui.h"
#include "client.h"
#include "support.h"
#include "interface.h"
#include "callbacks.h"
#include "status.h"
#include "colors.h"
#include "priv.h"
#include "fonts.h"

extern TTheme gui_theme;



/* some defines for the position */
#define RECT_SIZE (8)
#define PLAYERS_COLORS_OFFSET (246)
#define ROUND_OFFSET (410)
#define OFFSET_LEFT (6)
#define OFFSET_RIGHT (-8)

GtkWidget *status_dialog=NULL;
static GtkWidget *status_treeview=NULL;

static GtkWidget *ministatus = NULL;

static GtkWidget* mainstatus_canvas = NULL;

static GnomeCanvasItem *players_color[TEG_MAX_PLAYERS];
static GnomeCanvasItem *color_started_item;
static GnomeCanvasItem *round_number_item;
static GnomeCanvasItem *players_color_over;


static gint status_boton_clicked_cb(GtkWidget *area, GdkEventExpose *event, gpointer user_data)
{
	out_status();
	return FALSE;
}

/*
static TEG_STATUS status_paint_color( int color, GdkPixmap **pixmap )
{
	int i, h, w;

	assert( pixmap );

	i = (color<0 || color>=TEG_MAX_PLAYERS) ? TEG_MAX_PLAYERS : color;

	*pixmap = gdk_pixmap_new(status_dialog->window,
		48, 16, gtk_widget_get_visual(status_dialog)->depth);

	if( *pixmap == NULL )
		return TEG_STATUS_ERROR;


	gdk_gc_set_foreground(g_colors_gc, colors_get_player_from_color(color));
	gdk_draw_rectangle( *pixmap, g_colors_gc, TRUE, 0, 0, 47, 15);

	gdk_gc_set_foreground(g_colors_gc, colors_get_common(COLORS_BLACK));
	gdk_draw_rectangle( *pixmap, g_colors_gc, FALSE, 0, 0, 47, 15);

	gdk_gc_set_foreground(g_colors_gc, colors_get_player_ink_from_color(color));



	h = gdk_string_height (g_pixmap_font10, _(g_colores[i]) );
	w = gdk_string_width  (g_pixmap_font10, _(g_colores[i]) );

	gdk_draw_string( *pixmap, g_pixmap_font10, g_colors_gc, 
			((48 - w )/2),
			((16 - h)/2) + h, _(g_colores[i]));
	return TEG_STATUS_SUCCESS;
}
*/

static GtkTreeModel *
status_create_model (void)
{
	GtkListStore *store;

	/* create list store */
	store = gtk_list_store_new (
			STATUS_COLUMN_LAST,
			G_TYPE_STRING,	/* color */
			G_TYPE_UINT,	/* number */
			G_TYPE_STRING,	/* name */
			G_TYPE_UINT,	/* score */
			G_TYPE_STRING,	/* address */
			G_TYPE_BOOLEAN,	/* human? */
			G_TYPE_INT,	/* countries */
<<<<<<< HEAD
			G_TYPE_INT,	/* countries won */
			G_TYPE_INT,	/* countries lost */
			G_TYPE_FLOAT,	/* countries % */
			G_TYPE_INT,	/* armies */
			G_TYPE_INT,	/* armies killed */
			G_TYPE_INT,	/* armies lost */
			G_TYPE_FLOAT,	/* armies % */
=======
			G_TYPE_INT,	/* armies */
>>>>>>> b8e1f4d5000e931e81e397369fa5ec789dffe26c
			G_TYPE_UINT,	/* cards */
			G_TYPE_STRING,	/* status */
			G_TYPE_BOOLEAN	/* started the turn */
			);

	return GTK_TREE_MODEL (store);
}

static void status_add_columns (GtkTreeView *treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	/* column for pos color */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Color"),
			renderer,
			"text", STATUS_COLUMN_COLOR,
			NULL);
	gtk_tree_view_append_column (treeview, column);

	/* column for number */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Number"),
			renderer,
			"text", STATUS_COLUMN_NUMBER,
			NULL);
	gtk_tree_view_column_set_sort_column_id (column, STATUS_COLUMN_NUMBER);
	gtk_tree_view_append_column (treeview, column);

	/* column for name */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Name"),
			renderer,
			"text", STATUS_COLUMN_NAME,
			NULL);
	gtk_tree_view_append_column (treeview, column);

	/* column for score*/
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Score"),
			renderer,
			"text", STATUS_COLUMN_SCORE,
			NULL);
	gtk_tree_view_column_set_sort_column_id (column, STATUS_COLUMN_SCORE);
	gtk_tree_view_append_column (treeview, column);

	/* column for color */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Address"),
			renderer,
			"text", STATUS_COLUMN_ADDR,
			NULL);
	gtk_tree_view_append_column (treeview, column);

	/* column for is human?*/
	renderer = gtk_cell_renderer_toggle_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Human?"),
			renderer,
			"active", STATUS_COLUMN_HUMAN,
			NULL);
	gtk_tree_view_append_column (treeview, column);

	/* column for countries */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Countries"),
			renderer,
			"text", STATUS_COLUMN_COUNTRIES,
			NULL);
	gtk_tree_view_column_set_sort_column_id (column, STATUS_COLUMN_COUNTRIES);
	gtk_tree_view_append_column (treeview, column);

<<<<<<< HEAD
	/* column for countries won */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Countries won"),
			renderer,
			"text", STATUS_COLUMN_COUNTRIES_WON,
			NULL);
	gtk_tree_view_column_set_sort_column_id (column, STATUS_COLUMN_COUNTRIES_WON);
	gtk_tree_view_append_column (treeview, column);

	/* column for countries lost */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Countries lost"),
			renderer,
			"text", STATUS_COLUMN_COUNTRIES_LOST,
			NULL);
	gtk_tree_view_column_set_sort_column_id (column, STATUS_COLUMN_COUNTRIES_LOST);
	gtk_tree_view_append_column (treeview, column);

	/* column for countries % */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Countries %"),
			renderer,
			"text", STATUS_COLUMN_COUNTRIES_RATIO,
			NULL);
	gtk_tree_view_column_set_sort_column_id (column, STATUS_COLUMN_COUNTRIES_RATIO);
	gtk_tree_view_append_column (treeview, column);

=======
>>>>>>> b8e1f4d5000e931e81e397369fa5ec789dffe26c
	/* column for armies */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Armies"),
			renderer,
			"text", STATUS_COLUMN_ARMIES,
			NULL);
	gtk_tree_view_column_set_sort_column_id (column, STATUS_COLUMN_ARMIES);
	gtk_tree_view_append_column (treeview, column);

<<<<<<< HEAD
	/* column for armies killed */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Armies killed"),
			renderer,
			"text", STATUS_COLUMN_ARMIES_KILLED,
			NULL);
	gtk_tree_view_column_set_sort_column_id (column, STATUS_COLUMN_ARMIES_KILLED);
	gtk_tree_view_append_column (treeview, column);

	/* column for armies lost */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Armies lost"),
			renderer,
			"text", STATUS_COLUMN_ARMIES_LOST,
			NULL);
	gtk_tree_view_column_set_sort_column_id (column, STATUS_COLUMN_ARMIES_LOST);
	gtk_tree_view_append_column (treeview, column);

	/* column for armies % */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Armies %"),
			renderer,
			"text", STATUS_COLUMN_ARMIES_RATIO,
			NULL);
	gtk_tree_view_column_set_sort_column_id (column, STATUS_COLUMN_ARMIES_RATIO);
	gtk_tree_view_append_column (treeview, column);

=======
>>>>>>> b8e1f4d5000e931e81e397369fa5ec789dffe26c
	/* column for cards */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Cards"),
			renderer,
			"text", STATUS_COLUMN_CARDS,
			NULL);
	gtk_tree_view_column_set_sort_column_id (column, STATUS_COLUMN_CARDS);
	gtk_tree_view_append_column (treeview, column);

	/* column for status */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Status"),
			renderer,
			"text", STATUS_COLUMN_STATUS,
			NULL);
	gtk_tree_view_append_column (treeview, column);

	/* column for is who started*/
	renderer = gtk_cell_renderer_toggle_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Started Turn?"),
			renderer,
			"active", STATUS_COLUMN_WHO,
			NULL);
	gtk_tree_view_append_column (treeview, column);

	status_update_visibility_of_columns();
}


TEG_STATUS status_turn_color(PCPLAYER pJ, GdkPixmap **pixmap)
{
	int i;

	assert( pixmap );
	assert( pJ );


	if( pJ->empezo_turno )
		i = pJ->numjug;
	else
		i = -1;

	*pixmap = gdk_pixmap_new(status_dialog->window,
		16, 16, gtk_widget_get_visual(status_dialog)->depth);

	if( *pixmap == NULL )
		return TEG_STATUS_ERROR;

	gdk_gc_set_foreground(g_colors_gc, colors_get_player(i));
	gdk_draw_rectangle( *pixmap, g_colors_gc, TRUE, 0, 0, 15, 15);

	gdk_gc_set_foreground(g_colors_gc, colors_get_common(COLORS_BLACK));
	gdk_draw_rectangle( *pixmap, g_colors_gc, FALSE, 0, 0, 15, 15);

	return TEG_STATUS_SUCCESS;
}


static TEG_STATUS status_update_model( GtkTreeModel *model)
{
	GtkListStore *store;
	GtkTreeIter iter;
	PCPLAYER pJ;
	PLIST_ENTRY l = g_list_player.Flink;


	store = GTK_LIST_STORE( model );

	gtk_list_store_clear( store );

	while( !IsListEmpty( &g_list_player ) && (l != &g_list_player) )
	{
		gchar *name;
		pJ = (PCPLAYER) l;
		
<<<<<<< HEAD
		float cRatio = pJ->tot_countries_won;
		if ( pJ->tot_countries_lost == 0 ) {
			cRatio = pJ->tot_countries_won == 0 ? 1 : 9000 + pJ->tot_countries_won;
		}
		else {
			cRatio /= pJ->tot_countries_lost;
		}

		float aRatio = pJ->tot_armies_killed;
		if ( pJ->tot_armies_lost == 0 ) {
			aRatio = pJ->tot_armies_killed == 0 ? 1 : 9000 + pJ->tot_armies_killed;
		}
		else {
			aRatio /= pJ->tot_armies_lost;
		}

=======
>>>>>>> b8e1f4d5000e931e81e397369fa5ec789dffe26c
		name = translate_to_utf8( pJ->name );

		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter,
				STATUS_COLUMN_COLOR, _(g_colores[pJ->color]),
				STATUS_COLUMN_NUMBER, pJ->numjug,
				STATUS_COLUMN_NAME, name,
				STATUS_COLUMN_SCORE, pJ->score,
				STATUS_COLUMN_ADDR, pJ->addr,
				STATUS_COLUMN_HUMAN, pJ->human,
				STATUS_COLUMN_COUNTRIES, pJ->tot_countries,
<<<<<<< HEAD
				STATUS_COLUMN_COUNTRIES_WON, pJ->tot_countries_won,
				STATUS_COLUMN_COUNTRIES_LOST, pJ->tot_countries_lost,
				STATUS_COLUMN_COUNTRIES_RATIO, cRatio,
				STATUS_COLUMN_ARMIES, pJ->tot_armies,
				STATUS_COLUMN_ARMIES_KILLED, pJ->tot_armies_killed,
				STATUS_COLUMN_ARMIES_LOST, pJ->tot_armies_lost,
				STATUS_COLUMN_ARMIES_RATIO, aRatio,
=======
				STATUS_COLUMN_ARMIES, pJ->tot_armies,
>>>>>>> b8e1f4d5000e931e81e397369fa5ec789dffe26c
				STATUS_COLUMN_CARDS, pJ->tot_cards,
				STATUS_COLUMN_STATUS, _(g_estados[pJ->estado]),
				STATUS_COLUMN_WHO, pJ->empezo_turno,
				-1);
		free( name );

		l = LIST_NEXT(l);
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS status_update_visibility_of_columns( void )
{
	GtkTreeViewColumn *column;
	int i;

	if( status_treeview == NULL )
		return TEG_STATUS_ERROR;

	for( i=0; i < STATUS_COLUMN_LAST; i++ ) {
		column = gtk_tree_view_get_column( GTK_TREE_VIEW(status_treeview), i );
		gtk_tree_view_column_set_visible( column, (gui_private.status_show & (1<<i)) != 0 );
	}

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS status_update_dialog()
{
	static GtkTreeModel *model = NULL;

	if( status_dialog == NULL )
		return TEG_STATUS_ERROR;

	if( status_treeview == NULL ) {

		/* create tree model */
		model = status_create_model ();

		/* create tree view */
		status_treeview = gtk_tree_view_new_with_model (model);
		gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (status_treeview), TRUE);
		gtk_tree_view_set_search_column (GTK_TREE_VIEW (status_treeview),
				STATUS_COLUMN_SCORE);

		g_object_unref (G_OBJECT (model)); 
		gtk_box_pack_start_defaults( GTK_BOX(GNOME_DIALOG(status_dialog)->vbox), GTK_WIDGET(status_treeview));

		/* add columns to the tree view */
		status_add_columns (GTK_TREE_VIEW (status_treeview));

	}

	status_update_model( model );

	gtk_widget_show_all( status_treeview );
	return TEG_STATUS_SUCCESS;
}

/* view the status of players */
void status_view()
{
	if( status_dialog == NULL) {

		status_dialog = teg_dialog_new(_("Status of Players"),_("Status of Players")); 

		gtk_window_set_transient_for (GTK_WINDOW(status_dialog), NULL);

		gnome_dialog_append_buttons(GNOME_DIALOG(status_dialog),
				GNOME_STOCK_PIXMAP_REFRESH,
				GNOME_STOCK_BUTTON_CLOSE,
				NULL );

		gnome_dialog_close_hides( GNOME_DIALOG(status_dialog), TRUE );
		gnome_dialog_set_default(GNOME_DIALOG(status_dialog),1);

		/* signals de los botones */
		gnome_dialog_button_connect (GNOME_DIALOG(status_dialog),
						0, GTK_SIGNAL_FUNC(status_boton_clicked_cb),status_dialog);
		gnome_dialog_button_connect (GNOME_DIALOG(status_dialog),
						1, GTK_SIGNAL_FUNC(dialog_close),status_dialog);
		gnome_dialog_set_default( GNOME_DIALOG(status_dialog),1);

	}

	status_update_dialog();

	gtk_widget_show_all(status_dialog);
	raise_and_focus(status_dialog);

	out_status();
}


/*
 *
 *  mini status
 *
 */
TEG_STATUS ministatus_update()
{
	if( ministatus == NULL )
		return TEG_STATUS_ERROR;

	gtk_widget_draw( ministatus, NULL);

	return TEG_STATUS_SUCCESS;
}

static gint ministatus_expose_cb(GtkWidget *area, GdkEventExpose *event, gpointer user_data)
{
	static GdkGC *ms_gc = NULL;
	int i=0;

	if( area == NULL )
		return FALSE;

	if( area->window == NULL )
		return FALSE;;

	if( ms_gc == NULL )
		ms_gc = gdk_gc_new(area->window);

	if( ESTADO_GET() == PLAYER_STATUS_DESCONECTADO || g_game.observer )
		i = -1;
	else
		i = g_game.numjug;

	gdk_gc_set_foreground(ms_gc, colors_get_player(i));
	gdk_draw_arc( area->window, ms_gc, TRUE, 0, 3, 10, 10, 0, 360 * 64);

	gdk_gc_set_foreground(ms_gc, colors_get_common(COLORS_BLACK));
	gdk_draw_arc( area->window, ms_gc, FALSE, 0, 3, 10, 10, 0, 360* 64);

	return FALSE;
}

GtkWidget *ministatus_build()
{
	if( ministatus == NULL ) {
		ministatus = gtk_drawing_area_new();

		gtk_signal_connect(GTK_OBJECT(ministatus), "expose_event",
			   GTK_SIGNAL_FUNC(ministatus_expose_cb), NULL);
	}
	gtk_widget_set_usize(ministatus, 15, -1);
	gtk_widget_show( ministatus );

	return ministatus;
}


/*
 * Main Status
 */

#define MAINSTATUS_X (642)
#define MAINSTATUS_Y (30)
TEG_STATUS mainstatus_create( GtkWidget **window )
{
	int i;
	int failed=1;

	if( mainstatus_canvas )
		goto error;

	mainstatus_canvas = gnome_canvas_new();
	if( ! mainstatus_canvas )
		goto error;

	/* load colors for started_item, and player_colors */
	colors_load_images();

	gtk_widget_set_usize ( mainstatus_canvas, MAINSTATUS_X, MAINSTATUS_Y + 1 );
	gnome_canvas_set_scroll_region (GNOME_CANVAS (mainstatus_canvas), 0, 0, MAINSTATUS_X, MAINSTATUS_Y);

	/* background */
	if( gui_theme.toolbar_custom && gui_theme.toolbar_name )
	{
		GdkPixbuf *im;
		char *filename = theme_load_file( gui_theme.toolbar_name );
		im = gdk_pixbuf_new_from_file(filename, NULL);

		if( im ) {
			gnome_canvas_item_new(
				gnome_canvas_root(GNOME_CANVAS(mainstatus_canvas)),
				gnome_canvas_pixbuf_get_type (),
				"pixbuf", im,
				"x", 0.0,
				"y", 0.0,
				/* "width", (double) gdk_pixbuf_get_width(im), */
				/* "height", (double) gdk_pixbuf_get_height(im), */
				"width", (double) MAINSTATUS_X,
				"height", (double) MAINSTATUS_Y,
				"anchor", GTK_ANCHOR_NW,
				NULL);

			failed = 0;

			gdk_pixbuf_unref( im );
		}
	}

	/* load default background */
	if( failed ) {
		gnome_canvas_item_new(
			gnome_canvas_root(GNOME_CANVAS(mainstatus_canvas)),
			gnome_canvas_rect_get_type (),
			"x1", 0.0,
			"y1", 0.0,
			"x2", (double) MAINSTATUS_X,
			"y2", (double) MAINSTATUS_Y,
			"fill_color","light green",
			"outline_color","black",
			NULL);
	}

	/* round started by */
	gnome_canvas_item_new(
		gnome_canvas_root(GNOME_CANVAS(mainstatus_canvas)),
		gnome_canvas_text_get_type(),
		"text",_("Round started by:"),
		"x", (double) ROUND_OFFSET,
		"y", (double) 3,
		"x_offset", (double) -1,
		"y_offset", (double) -1,
		"font", HELVETICA_10_FONT,
		"fill_color", gui_theme.toolbar_custom && gui_theme.toolbar_text_color ? gui_theme.toolbar_text_color : "black",
		"anchor",GTK_ANCHOR_NE,
		NULL);

	color_started_item = gnome_canvas_item_new(
		gnome_canvas_root(GNOME_CANVAS(mainstatus_canvas)),
		gnome_canvas_pixbuf_get_type (),
		"pixbuf", g_color_circles[TEG_MAX_PLAYERS],
		"x", (double) ROUND_OFFSET + 4,
		"y", (double) 4,
		"width", (double) gdk_pixbuf_get_width(g_color_circles[TEG_MAX_PLAYERS]),
		"height", (double) gdk_pixbuf_get_height(g_color_circles[TEG_MAX_PLAYERS]),
		"anchor",GTK_ANCHOR_NW,
		NULL);
	gnome_canvas_item_hide( color_started_item );

	/* round number */
	gnome_canvas_item_new(
		gnome_canvas_root(GNOME_CANVAS(mainstatus_canvas)),
		gnome_canvas_text_get_type(),
		"text",_("Round number:"),
		"x", (double) ROUND_OFFSET,
		"y", (double) MAINSTATUS_Y/2 + 1,
		"x_offset", (double) -1,
		"y_offset", (double) -1,
		"font", HELVETICA_10_FONT,
		"fill_color", gui_theme.toolbar_custom && gui_theme.toolbar_text_color ? gui_theme.toolbar_text_color : "black",
		"anchor",GTK_ANCHOR_NE,
		NULL);

	round_number_item = gnome_canvas_item_new(
		gnome_canvas_root(GNOME_CANVAS(mainstatus_canvas)),
		gnome_canvas_text_get_type(),
		"text",_("?"),
		"x", (double) ROUND_OFFSET + 4,
		"y", (double) MAINSTATUS_Y/2 + 2,
		"x_offset", (double) -1,
		"y_offset", (double) -1,
		"font", HELVETICA_12_FONT,
		"fill_color", gui_theme.toolbar_custom && gui_theme.toolbar_text_color ? gui_theme.toolbar_text_color : "black",
		"anchor",GTK_ANCHOR_NW,
		NULL);

	/* players turn */
	gnome_canvas_item_new(
		gnome_canvas_root(GNOME_CANVAS(mainstatus_canvas)),
		gnome_canvas_text_get_type(),
		"text",_("Players turn:"),
		"x", (double) PLAYERS_COLORS_OFFSET - 4,
		"y", (double) 3,
		"x_offset", (double) -1,
		"y_offset", (double) -1,
		"font", HELVETICA_10_FONT,
		"fill_color", gui_theme.toolbar_custom && gui_theme.toolbar_text_color ? gui_theme.toolbar_text_color : "black",
		"anchor",GTK_ANCHOR_NE,
		NULL);

	/* create canvas for the circles & and load the circles */
	for(i=0;i<TEG_MAX_PLAYERS;i++)
	{
		players_color[i] = gnome_canvas_item_new(
			gnome_canvas_root(GNOME_CANVAS(mainstatus_canvas)),
			gnome_canvas_pixbuf_get_type (),
			"pixbuf", g_color_circles[TEG_MAX_PLAYERS],
			"x", 0.0,
			"y", 0.0,
			"width", (double) gdk_pixbuf_get_width(g_color_circles[TEG_MAX_PLAYERS]),
			"height", (double) gdk_pixbuf_get_height(g_color_circles[TEG_MAX_PLAYERS]),
			"anchor", GTK_ANCHOR_NW,
			NULL);
		gnome_canvas_item_hide( players_color[i] );
	}

	players_color_over = gnome_canvas_item_new(
		gnome_canvas_root(GNOME_CANVAS(mainstatus_canvas)),
		gnome_canvas_pixbuf_get_type (),
		"pixbuf", g_color_circle_over,
		"x", 0.0,
		"y", 0.0,
		"width", (double) gdk_pixbuf_get_width(g_color_circle_over),
		"height", (double) gdk_pixbuf_get_height(g_color_circle_over),
		"anchor", GTK_ANCHOR_NW,
		NULL);
	gnome_canvas_item_hide( players_color_over );


	mainstatus_update();

	*window = mainstatus_canvas;
	gtk_widget_show( mainstatus_canvas );
	return TEG_STATUS_SUCCESS;

error:
	return TEG_STATUS_ERROR;
}

TEG_STATUS mainstatus_update_colors()
{
	int i;
	PLIST_ENTRY l;
	PCPLAYER pJ;

	if( ! mainstatus_canvas )
		return TEG_STATUS_ERROR;

	l = g_list_player.Flink;

	i=0;

	gnome_canvas_item_hide( players_color_over );
	while( !IsListEmpty( &g_list_player ) && (l != &g_list_player) )
	{
		pJ = (PCPLAYER) l;

		if( pJ->color >= 0 && pJ->numjug >= 0 ) {
			gnome_canvas_item_show( players_color[i] );

			if( g_game.whos_turn == pJ->numjug )
			{
				gnome_canvas_item_set(
					players_color_over,
					"pixbuf", g_color_circle_over,
					"x", (double) PLAYERS_COLORS_OFFSET - 1 + (i%3) * 14,
					"y", (double) 2 + 13 * (i<3?0:1),
					"width", (double) gdk_pixbuf_get_width(g_color_circle_over) + 4,
					"height", (double) gdk_pixbuf_get_height(g_color_circle_over) + 4,
					NULL);

				gnome_canvas_item_show( players_color_over );
			}

			gnome_canvas_item_set(
				players_color[i],
				"pixbuf", g_color_circles[pJ->color],
				"x", (double) PLAYERS_COLORS_OFFSET + (i%3) * 14,
				"y", (double) 4 + 13 * (i<3?0:1),
				"width", (double) gdk_pixbuf_get_width(g_color_circles[pJ->color]),
				"height", (double) gdk_pixbuf_get_height(g_color_circles[pJ->color]),
				NULL);

			i++;
		}

		l = LIST_NEXT(l);

		if( i >= TEG_MAX_PLAYERS )
			break;
	}

	{
		PCPLAYER pJ;
		gnome_canvas_item_hide( color_started_item );
		if( g_game.who_started_round >= 0 && g_game.who_started_round < TEG_MAX_PLAYERS ) {

			if( player_whois( g_game.who_started_round, &pJ ) == TEG_STATUS_SUCCESS )
			{
				gnome_canvas_item_set(
					color_started_item,
					"pixbuf", g_color_circles[pJ->color],
					NULL);
				gnome_canvas_item_show( color_started_item );
			}
		}
	}

	for( ; i < TEG_MAX_PLAYERS ; i++ )
			gnome_canvas_item_hide( players_color[i] );

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS mainstatus_update()
{
	char buffer[256];
	static int offset_right=-1, offset_left=-1;
	static GnomeCanvasItem *gamestatus_item = NULL;
	static GnomeCanvasItem *country_item = NULL;
	static GnomeCanvasItem *cont_item = NULL;


	if( ! mainstatus_canvas )
		return TEG_STATUS_ERROR;

	if( offset_left == -1 && offset_right == -1)
	{
		offset_left = gui_theme.toolbar_offset_left != -1 ? gui_theme.toolbar_offset_left : OFFSET_LEFT;
		offset_right = gui_theme.toolbar_offset_right != -1 ? gui_theme.toolbar_offset_right : OFFSET_RIGHT;
	}

	/* game status */
	if( gamestatus_item  )
		gtk_object_destroy( GTK_OBJECT(gamestatus_item) );

	gamestatus_item = gnome_canvas_item_new(
		gnome_canvas_root(GNOME_CANVAS(mainstatus_canvas)),
		gnome_canvas_text_get_type(),
		"text",_(g_estados[g_game.estado]),
		"x", (double) (MAINSTATUS_X + offset_right),
		"y", (double) (MAINSTATUS_Y/2),
		"x_offset", (double) -1,
		"y_offset", (double) -1,
		"font", HELVETICA_14_FONT,
		"fill_color", gui_theme.toolbar_custom && gui_theme.toolbar_text_color ? gui_theme.toolbar_text_color : "black",
		"anchor",GTK_ANCHOR_EAST,
		NULL);

	/* country */
	if( country_item  )
		gtk_object_destroy( GTK_OBJECT(country_item) );

	if( gui_private.country_i_am < 0 || gui_private.country_i_am >= COUNTRIES_CANT )
		snprintf(buffer,sizeof(buffer)-1," ");
	else
		snprintf(buffer,sizeof(buffer)-1,"%s", countries_get_name(gui_private.country_i_am) );
		
	country_item = gnome_canvas_item_new(
		gnome_canvas_root(GNOME_CANVAS(mainstatus_canvas)),
		gnome_canvas_text_get_type(),
		"text",buffer,
		"x", (double) offset_left,
		"y", (double) (2),
		"x_offset", (double) -1,
		"y_offset", (double) -1,
		"font", HELVETICA_12_FONT,
		"fill_color", gui_theme.toolbar_custom && gui_theme.toolbar_text_color ? gui_theme.toolbar_text_color : "black",
		"anchor",GTK_ANCHOR_NW,
		NULL);

	/* continent */
	if( cont_item  )
		gtk_object_destroy( GTK_OBJECT(cont_item) );

	if( gui_private.country_i_am < 0 || gui_private.country_i_am >= COUNTRIES_CANT )
		snprintf(buffer,sizeof(buffer)-1," ");
	else
		snprintf(buffer,sizeof(buffer)-1,"%s", cont_get_name( g_countries[gui_private.country_i_am].continente )  );
		
	cont_item = gnome_canvas_item_new(
		gnome_canvas_root(GNOME_CANVAS(mainstatus_canvas)),
		gnome_canvas_text_get_type(),
		"text",buffer,
		"x", (double) offset_left,
		"y", (double) (16),
		"x_offset", (double) -1,
		"y_offset", (double) -1,
		"font", HELVETICA_10_FONT,
		"fill_color", gui_theme.toolbar_custom && gui_theme.toolbar_text_color ? gui_theme.toolbar_text_color : "black",
		"anchor",GTK_ANCHOR_NW,
		NULL);

	/* round number */
	{
		char buffer[256];

		gnome_canvas_item_hide( round_number_item );
		if( g_game.round_number >= 0 ) {

			sprintf(buffer,"%d",g_game.round_number );

			gnome_canvas_item_set(
				round_number_item,
				"text",buffer,
				NULL);
			gnome_canvas_item_show( round_number_item );
		}
	}

	mainstatus_update_colors();

	return TEG_STATUS_SUCCESS;
}
