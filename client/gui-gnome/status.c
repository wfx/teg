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
#include <math.h>
#include <goocanvas.h>
#include <glib/gi18n.h>

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

static GooCanvasItem *players_color[TEG_MAX_PLAYERS];
static GooCanvasItem *color_started_item;
static GooCanvasItem *round_number_item;
static GooCanvasItem *players_color_over;

static GtkListStore *status_create_model(void)
{
	GtkListStore *store;

	/* create list store */
	store = gtk_list_store_new(
	            STATUS_COLUMN_LAST,
	            G_TYPE_STRING,	/* color */
	            G_TYPE_UINT,	/* number */
	            G_TYPE_STRING,	/* name */
	            G_TYPE_UINT,	/* score */
	            G_TYPE_STRING,	/* address */
	            G_TYPE_BOOLEAN,	/* human? */
	            G_TYPE_INT,	/* countries */
	            G_TYPE_INT,	/* armies */
	            G_TYPE_UINT,	/* cards */
	            G_TYPE_STRING,	/* status */
	            G_TYPE_BOOLEAN	/* started the turn */
	        );

	return store;
}

static void status_add_columns(GtkTreeView *treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	/* column for pos color */
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Color"),
	         renderer,
	         "text", STATUS_COLUMN_COLOR,
	         NULL);
	gtk_tree_view_append_column(treeview, column);

	/* column for number */
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Number"),
	         renderer,
	         "text", STATUS_COLUMN_NUMBER,
	         NULL);
	gtk_tree_view_column_set_sort_column_id(column, STATUS_COLUMN_NUMBER);
	gtk_tree_view_append_column(treeview, column);

	/* column for name */
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Name"),
	         renderer,
	         "text", STATUS_COLUMN_NAME,
	         NULL);
	gtk_tree_view_append_column(treeview, column);

	/* column for score*/
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Score"),
	         renderer,
	         "text", STATUS_COLUMN_SCORE,
	         NULL);
	gtk_tree_view_column_set_sort_column_id(column, STATUS_COLUMN_SCORE);
	gtk_tree_view_append_column(treeview, column);

	/* column for color */
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Address"),
	         renderer,
	         "text", STATUS_COLUMN_ADDR,
	         NULL);
	gtk_tree_view_append_column(treeview, column);

	/* column for is human?*/
	renderer = gtk_cell_renderer_toggle_new();
	column = gtk_tree_view_column_new_with_attributes(_("Human?"),
	         renderer,
	         "active", STATUS_COLUMN_HUMAN,
	         NULL);
	gtk_tree_view_append_column(treeview, column);

	/* column for countries */
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Countries"),
	         renderer,
	         "text", STATUS_COLUMN_COUNTRIES,
	         NULL);
	gtk_tree_view_column_set_sort_column_id(column, STATUS_COLUMN_COUNTRIES);
	gtk_tree_view_append_column(treeview, column);

	/* column for armies */
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Armies"),
	         renderer,
	         "text", STATUS_COLUMN_ARMIES,
	         NULL);
	gtk_tree_view_column_set_sort_column_id(column, STATUS_COLUMN_ARMIES);
	gtk_tree_view_append_column(treeview, column);

	/* column for cards */
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Cards"),
	         renderer,
	         "text", STATUS_COLUMN_CARDS,
	         NULL);
	gtk_tree_view_column_set_sort_column_id(column, STATUS_COLUMN_CARDS);
	gtk_tree_view_append_column(treeview, column);

	/* column for status */
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Status"),
	         renderer,
	         "text", STATUS_COLUMN_STATUS,
	         NULL);
	gtk_tree_view_append_column(treeview, column);

	/* column for is who started*/
	renderer = gtk_cell_renderer_toggle_new();
	column = gtk_tree_view_column_new_with_attributes(_("Started Turn?"),
	         renderer,
	         "active", STATUS_COLUMN_WHO,
	         NULL);
	gtk_tree_view_append_column(treeview, column);

	status_update_visibility_of_columns();
}

static TEG_STATUS status_update_model(GtkListStore *store)
{
	GtkTreeIter iter;
	PCPLAYER pJ;
	PLIST_ENTRY l = g_list_player.Flink;


	gtk_list_store_clear(store);

	while(!IsListEmpty(&g_list_player) && (l != &g_list_player)) {
		gchar *name;
		pJ = (PCPLAYER) l;

		name = translate_to_utf8(pJ->name);

		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter,
		                   STATUS_COLUMN_COLOR, _(g_colores[pJ->color]),
		                   STATUS_COLUMN_NUMBER, pJ->numjug,
		                   STATUS_COLUMN_NAME, name,
		                   STATUS_COLUMN_SCORE, pJ->score,
		                   STATUS_COLUMN_ADDR, pJ->addr,
		                   STATUS_COLUMN_HUMAN, pJ->human,
		                   STATUS_COLUMN_COUNTRIES, pJ->tot_countries,
		                   STATUS_COLUMN_ARMIES, pJ->tot_armies,
		                   STATUS_COLUMN_CARDS, pJ->tot_cards,
		                   STATUS_COLUMN_STATUS, _(g_estados[pJ->estado]),
		                   STATUS_COLUMN_WHO, pJ->empezo_turno,
		                   -1);
		free(name);

		l = LIST_NEXT(l);
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS status_update_visibility_of_columns(void)
{
	GtkTreeViewColumn *column;
	int i;

	if(!GTK_IS_WIDGET(status_treeview)) {
		return TEG_STATUS_ERROR;
	}

	for(i=0; i < STATUS_COLUMN_LAST; i++) {
		column = gtk_tree_view_get_column(GTK_TREE_VIEW(status_treeview), i);
		gtk_tree_view_column_set_visible(column, (gui_private.status_show & (1<<i)) != 0);
	}

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS status_update_dialog()
{
	static GtkListStore *model = NULL;

	if(status_dialog == NULL) {
		return TEG_STATUS_ERROR;
	}

	/* create tree model */
	model = status_create_model();

	/* create tree view */
	status_treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(model));
	gtk_tree_view_set_search_column(GTK_TREE_VIEW(status_treeview),
	                                STATUS_COLUMN_SCORE);

	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area
	                           (GTK_DIALOG(status_dialog))),
	                   GTK_WIDGET(status_treeview), TRUE, TRUE, 0);

	/* add columns to the tree view */
	status_add_columns(GTK_TREE_VIEW(status_treeview));

	status_update_model(model);

	return TEG_STATUS_SUCCESS;
}

/* view the status of players */
void status_view()
{
	status_dialog = teg_dialog_new(_("Status of Players"),
	                               _("Status of Players"));

	gtk_window_set_transient_for(GTK_WINDOW(status_dialog),
	                             GTK_WINDOW(main_window));

	gtk_dialog_add_buttons(GTK_DIALOG(status_dialog), _("_Refresh"), 0,
	                       _("_Close"), 1, NULL);

	gtk_dialog_set_default_response(GTK_DIALOG(status_dialog), 1);

	out_status();
	status_update_dialog();

	gtk_widget_show_all(status_dialog);

	while(gtk_dialog_run(GTK_DIALOG(status_dialog)) == 0) {
		out_status();
		status_update_dialog();
		gtk_widget_queue_draw(status_dialog);
	}

	if(GTK_IS_WIDGET(status_dialog)) {
		gtk_widget_destroy(status_dialog);
		status_dialog = NULL;
	}
}


/*
 *
 *  mini status
 *
 */
TEG_STATUS ministatus_update()
{
	if(ministatus == NULL) {
		return TEG_STATUS_ERROR;
	}

	gtk_widget_queue_draw(ministatus);

	return TEG_STATUS_SUCCESS;
}

static gboolean ministatus_expose_cb(GtkWidget *area, cairo_t *cr, gpointer user_data)
{
	gint width, height;
	int i=0;

	if(ESTADO_GET() == PLAYER_STATUS_DESCONECTADO || g_game.observer) {
		i = -1;
	} else {
		i = g_game.numjug;
	}

	width = gtk_widget_get_allocated_width(area);
	height = gtk_widget_get_allocated_height(area);

	gdk_cairo_set_source_rgba(cr, colors_get_player(i));
	cairo_arc(cr, width / 2, height / 2, 5, 0, 2 * M_PI);
	cairo_fill(cr);

	gdk_cairo_set_source_rgba(cr, colors_get_common(COLORS_BLACK));
	cairo_set_line_width(cr, 1.0);
	cairo_arc(cr, width / 2, height / 2, 5, 0, 2 * M_PI);
	cairo_stroke(cr);

	return FALSE;
}

GtkWidget *ministatus_build()
{
	if(ministatus == NULL) {
		ministatus = gtk_drawing_area_new();

		g_signal_connect(G_OBJECT(ministatus), "draw",
		                 G_CALLBACK(ministatus_expose_cb), NULL);
	}
	gtk_widget_set_size_request(ministatus, 15, -1);
	gtk_widget_show(ministatus);

	return ministatus;
}


/*
 * Main Status
 */

#define MAINSTATUS_X (641)
#define MAINSTATUS_Y (48)
TEG_STATUS mainstatus_create(GtkWidget **window)
{
	int i;
	int failed=1;

	if(mainstatus_canvas) {
		goto error;
	}

	mainstatus_canvas = goo_canvas_new();
	if(! mainstatus_canvas) {
		goto error;
	}

	/* load colors for started_item, and player_colors */
	colors_load_images();

	gtk_widget_set_size_request(mainstatus_canvas,
	                            MAINSTATUS_X, MAINSTATUS_Y + 1);
	goo_canvas_set_bounds(GOO_CANVAS(mainstatus_canvas), 0, 0,
	                      MAINSTATUS_X, MAINSTATUS_Y);
	g_object_set(mainstatus_canvas, "anchor",
	             GOO_CANVAS_ANCHOR_CENTER, NULL);

	/* background */
	if(gui_theme.toolbar_custom && gui_theme.toolbar_name) {
		GdkPixbuf *im;
		char *filename = theme_load_file(gui_theme.toolbar_name);
		im = gdk_pixbuf_new_from_file(filename, NULL);

		if(im) {
			goo_canvas_image_new(
			    goo_canvas_get_root_item(GOO_CANVAS(mainstatus_canvas)),
			    im,
			    0.0,
			    0.0,
			    /* "width", (double) gdk_pixbuf_get_width(im), */
			    /* "height", (double) gdk_pixbuf_get_height(im), */
			    "width", (double) MAINSTATUS_X,
			    "height", (double) MAINSTATUS_Y,
			    "scale-to-fit", TRUE,
			    NULL);

			failed = 0;

			g_object_unref(im);
		}
	}

	/* load default background */
	if(failed) {
		goo_canvas_rect_new(
		    goo_canvas_get_root_item(GOO_CANVAS(mainstatus_canvas)),
		    0.0,
		    0.0,
		    (double) MAINSTATUS_X,
		    (double) MAINSTATUS_Y,
		    "fill-color", "light green",
		    "stroke-color", "black",
		    NULL);
	}

	/* round started by */
	goo_canvas_text_new(
	    goo_canvas_get_root_item(GOO_CANVAS(mainstatus_canvas)),
	    _("Round started by:"),
	    (double) ROUND_OFFSET,
	    (double) 3,
	    (double) -1,
	    GOO_CANVAS_ANCHOR_NORTH_EAST,
	    "height", (double) -1,
	    "font", HELVETICA_10_FONT,
	    "fill-color", gui_theme.toolbar_custom && gui_theme.toolbar_text_color ? gui_theme.toolbar_text_color : "black",
	    NULL);

	color_started_item = goo_canvas_image_new(
	                         goo_canvas_get_root_item(GOO_CANVAS(mainstatus_canvas)),
	                         g_color_circles[TEG_MAX_PLAYERS],
	                         (double) ROUND_OFFSET + 4,
	                         (double) 4,
	                         "width", (double) gdk_pixbuf_get_width(g_color_circles[TEG_MAX_PLAYERS]),
	                         "height", (double) gdk_pixbuf_get_height(g_color_circles[TEG_MAX_PLAYERS]),
	                         NULL);
	g_object_set(color_started_item, "visibility",
	             GOO_CANVAS_ITEM_INVISIBLE, NULL);

	/* round number */
	goo_canvas_text_new(
	    goo_canvas_get_root_item(GOO_CANVAS(mainstatus_canvas)),
	    _("Round number:"),
	    (double) ROUND_OFFSET,
	    (double) MAINSTATUS_Y/2 + 1,
	    (double) -1,
	    GOO_CANVAS_ANCHOR_NORTH_EAST,
	    "height", (double) -1,
	    "font", HELVETICA_10_FONT,
	    "fill-color", gui_theme.toolbar_custom && gui_theme.toolbar_text_color ? gui_theme.toolbar_text_color : "black",
	    NULL);

	round_number_item = goo_canvas_text_new(
	                        goo_canvas_get_root_item(GOO_CANVAS(mainstatus_canvas)),
	                        _("?"),
	                        (double) ROUND_OFFSET + 4,
	                        (double) MAINSTATUS_Y/2 + 2,
	                        (double) -1,
	                        GOO_CANVAS_ANCHOR_NORTH_WEST,
	                        "height", (double) -1,
	                        "font", HELVETICA_12_FONT,
	                        "fill-color", gui_theme.toolbar_custom && gui_theme.toolbar_text_color ? gui_theme.toolbar_text_color : "black",
	                        NULL);

	/* players turn */
	goo_canvas_text_new(
	    goo_canvas_get_root_item(GOO_CANVAS(mainstatus_canvas)),
	    _("Players turn:"),
	    (double) PLAYERS_COLORS_OFFSET - 4,
	    (double) 3,
	    (double) -1,
	    GOO_CANVAS_ANCHOR_NORTH_EAST,
	    "height", (double) -1,
	    "font", HELVETICA_10_FONT,
	    "fill-color", gui_theme.toolbar_custom && gui_theme.toolbar_text_color ? gui_theme.toolbar_text_color : "black",
	    NULL);

	/* create canvas for the circles & and load the circles */
	for(i=0; i<TEG_MAX_PLAYERS; i++) {
		players_color[i] = goo_canvas_image_new(
		                       goo_canvas_get_root_item(GOO_CANVAS(mainstatus_canvas)),
		                       g_color_circles[TEG_MAX_PLAYERS],
		                       0.0,
		                       0.0,
		                       "width", (double) gdk_pixbuf_get_width(g_color_circles[TEG_MAX_PLAYERS]),
		                       "height", (double) gdk_pixbuf_get_height(g_color_circles[TEG_MAX_PLAYERS]),
		                       NULL);
		g_object_set(players_color[i], "visibility",
		             GOO_CANVAS_ITEM_INVISIBLE, NULL);
	}

	players_color_over = goo_canvas_image_new(
	                         goo_canvas_get_root_item(GOO_CANVAS(mainstatus_canvas)),
	                         g_color_circle_over,
	                         0.0,
	                         0.0,
	                         "width", (double) gdk_pixbuf_get_width(g_color_circle_over),
	                         "height", (double) gdk_pixbuf_get_height(g_color_circle_over),
	                         NULL);
	g_object_set(players_color_over, "visibility",
	             GOO_CANVAS_ITEM_INVISIBLE, NULL);


	mainstatus_update();

	*window = mainstatus_canvas;
	gtk_widget_show(mainstatus_canvas);
	return TEG_STATUS_SUCCESS;

error:
	return TEG_STATUS_ERROR;
}

TEG_STATUS mainstatus_update_colors()
{
	int i;
	PLIST_ENTRY l;
	PCPLAYER pJ;

	if(! mainstatus_canvas) {
		return TEG_STATUS_ERROR;
	}

	l = g_list_player.Flink;

	i=0;

	g_object_set(players_color_over, "visibility",
	             GOO_CANVAS_ITEM_INVISIBLE, NULL);
	while(!IsListEmpty(&g_list_player) && (l != &g_list_player)) {
		pJ = (PCPLAYER) l;

		if(pJ->color >= 0 && pJ->numjug >= 0) {
			g_object_set(players_color[i], "visibility",
			             GOO_CANVAS_ITEM_VISIBLE, NULL);

			if(g_game.whos_turn == pJ->numjug) {
				g_object_set(
				    players_color_over,
				    "pixbuf", g_color_circle_over,
				    "x", (double) PLAYERS_COLORS_OFFSET - 1 + (i%3) * 14,
				    "y", (double) 2 + 13 * (i<3?0:1),
				    "width", (double) gdk_pixbuf_get_width(g_color_circle_over) + 4,
				    "height", (double) gdk_pixbuf_get_height(g_color_circle_over) + 4,
				    "visibility", GOO_CANVAS_ITEM_VISIBLE,
				    NULL);
			}

			g_object_set(
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

		if(i >= TEG_MAX_PLAYERS) {
			break;
		}
	}

	{
		PCPLAYER pJ;
		g_object_set(color_started_item, "visibility",
		             GOO_CANVAS_ITEM_INVISIBLE, NULL);
		if(g_game.who_started_round >= 0 && g_game.who_started_round < TEG_MAX_PLAYERS) {

			if(player_whois(g_game.who_started_round, &pJ) == TEG_STATUS_SUCCESS) {
				g_object_set(
				    color_started_item,
				    "pixbuf", g_color_circles[pJ->color],
				    "visibility", GOO_CANVAS_ITEM_VISIBLE,
				    NULL);
			}
		}
	}

	for(; i < TEG_MAX_PLAYERS ; i++)
		g_object_set(players_color[i], "visibility",
		             GOO_CANVAS_ITEM_INVISIBLE, NULL);

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS mainstatus_update()
{
	char buffer[256];
	static int offset_right=-1, offset_left=-1;
	static GooCanvasItem *gamestatus_item = NULL;
	static GooCanvasItem *country_item = NULL;
	static GooCanvasItem *cont_item = NULL;


	if(! mainstatus_canvas) {
		return TEG_STATUS_ERROR;
	}

	if(offset_left == -1 && offset_right == -1) {
		offset_left = gui_theme.toolbar_offset_left != -1 ? gui_theme.toolbar_offset_left : OFFSET_LEFT;
		offset_right = gui_theme.toolbar_offset_right != -1 ? gui_theme.toolbar_offset_right : OFFSET_RIGHT;
	}

	/* game status */

	if(gamestatus_item) {
		goo_canvas_item_remove(gamestatus_item);
		gamestatus_item = NULL;
	}

	gamestatus_item = goo_canvas_text_new(
	                      goo_canvas_get_root_item(GOO_CANVAS(mainstatus_canvas)),
	                      _(g_estados[g_game.estado]),
	                      (double)(MAINSTATUS_X + offset_right),
	                      (double)(MAINSTATUS_Y/2),
	                      (double) -1,
	                      GOO_CANVAS_ANCHOR_EAST,
	                      "height", (double) -1,
	                      "font", HELVETICA_14_FONT,
	                      "fill-color", gui_theme.toolbar_custom && gui_theme.toolbar_text_color ? gui_theme.toolbar_text_color : "black",
	                      NULL);

	/* country */
	if(country_item) {
		goo_canvas_item_remove(country_item);
		country_item = NULL;
	}

	if(gui_private.country_i_am < 0 || gui_private.country_i_am >= COUNTRIES_CANT) {
		snprintf(buffer, sizeof(buffer)-1, " ");
	} else {
		snprintf(buffer, sizeof(buffer)-1, "%s", countries_get_name(gui_private.country_i_am));
	}

	country_item = goo_canvas_text_new(
	                   goo_canvas_get_root_item(GOO_CANVAS(mainstatus_canvas)),
	                   buffer,
	                   (double) offset_left,
	                   (double)(2),
	                   (double) -1,
	                   GOO_CANVAS_ANCHOR_NORTH_WEST,
	                   "height", (double) -1,
	                   "font", HELVETICA_12_FONT,
	                   "fill-color", gui_theme.toolbar_custom && gui_theme.toolbar_text_color ? gui_theme.toolbar_text_color : "black",
	                   NULL);

	/* continent */
	if(cont_item) {
		goo_canvas_item_remove(cont_item);
		cont_item = NULL;
	}

	if(gui_private.country_i_am < 0 || gui_private.country_i_am >= COUNTRIES_CANT) {
		snprintf(buffer, sizeof(buffer)-1, " ");
	} else {
		snprintf(buffer, sizeof(buffer)-1, "%s", cont_get_name(g_countries[gui_private.country_i_am].continente));
	}

	cont_item = goo_canvas_text_new(
	                goo_canvas_get_root_item(GOO_CANVAS(mainstatus_canvas)),
	                buffer,
	                (double) offset_left,
	                (double)(16),
	                (double) -1,
	                GOO_CANVAS_ANCHOR_NORTH_WEST,
	                "height", (double) -1,
	                "font", HELVETICA_10_FONT,
	                "fill-color", gui_theme.toolbar_custom && gui_theme.toolbar_text_color ? gui_theme.toolbar_text_color : "black",
	                NULL);

	/* round number */
	{
		char buffer[256];

		g_object_set(round_number_item, "visibility",
		             GOO_CANVAS_ITEM_INVISIBLE, NULL);
		if(g_game.round_number >= 0) {

			sprintf(buffer, "%d", g_game.round_number);

			g_object_set(
			    round_number_item,
			    "text", buffer,
			    "visibility", GOO_CANVAS_ITEM_VISIBLE,
			    NULL);
		}
	}

	mainstatus_update_colors();

	return TEG_STATUS_SUCCESS;
}
