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

#define _XOPEN_SOURCE 500

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <assert.h>
#include <math.h>
#include <goocanvas.h>
#include <glib/gi18n.h>

#include "client.h"
#include "gui_state.h"
#include "support.h"
#include "interface.h"
#include "callbacks.h"
#include "status.h"
#include "colors.h"
#include "priv.h"
#include "fonts.h"

namespace teg::client::callbacks
{

/* some defines for the position */
#define RECT_SIZE (8)
#define PLAYERS_COLORS_OFFSET (246)
#define ROUND_OFFSET (410)
#define OFFSET_LEFT (6)
#define OFFSET_RIGHT (-8)


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
	gtk_list_store_clear(store);

	players_map(client_state(), [store](Player &player) {
		gchar *const name = translate_to_utf8(player.name);

		GtkTreeIter iter;
		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter,
		                   STATUS_COLUMN_COLOR, _(color_name(player.color)),
		                   STATUS_COLUMN_NUMBER, player.numjug,
		                   STATUS_COLUMN_NAME, name,
		                   STATUS_COLUMN_SCORE, player.score,
		                   STATUS_COLUMN_ADDR, player.addr,
		                   STATUS_COLUMN_HUMAN, player.human,
		                   STATUS_COLUMN_COUNTRIES, player.tot_countries,
		                   STATUS_COLUMN_ARMIES, player.tot_armies,
		                   STATUS_COLUMN_CARDS, player.tot_cards,
		                   STATUS_COLUMN_STATUS, _(player_status_name(player.estado)),
		                   STATUS_COLUMN_WHO, player.empezo_turno,
		                   -1);
		free(name);
	});
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS status_update_visibility_of_columns(void)
{
	GtkTreeViewColumn *column;
	int i;

	if(!GTK_IS_WIDGET(gui_state().status.treeview)) {
		return TEG_STATUS_ERROR;
	}

	for(i=0; i < STATUS_COLUMN_LAST; i++) {
		column = gtk_tree_view_get_column(GTK_TREE_VIEW(gui_state().status.treeview), i);
		gtk_tree_view_column_set_visible(column, (gui_state().priv.status_show & (1<<i)) != 0);
	}

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS status_update_dialog()
{

	if(gui_state().status.dialog == NULL) {
		return TEG_STATUS_ERROR;
	}

	/* create tree model */
	gui_state().status.model = status_create_model();

	/* create tree view */
	gui_state().status.treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(gui_state().status.model));
	gtk_tree_view_set_search_column(GTK_TREE_VIEW(gui_state().status.treeview),
	                                STATUS_COLUMN_SCORE);

	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area
	                           (GTK_DIALOG(gui_state().status.dialog))),
	                   GTK_WIDGET(gui_state().status.treeview), TRUE, TRUE, 0);

	/* add columns to the tree view */
	status_add_columns(GTK_TREE_VIEW(gui_state().status.treeview));

	status_update_model(gui_state().status.model);

	return TEG_STATUS_SUCCESS;
}

/* view the status of players */
void status_view()
{
	gui_state().status.dialog = teg_dialog_new(_("Status of Players"),
	                            _("Status of Players"));

	gtk_window_set_transient_for(GTK_WINDOW(gui_state().status.dialog),
	                             GTK_WINDOW(gui_state().ui.main_window));

	gtk_dialog_add_buttons(GTK_DIALOG(gui_state().status.dialog), _("_Refresh"), 0,
	                       _("_Close"), 1, NULL);

	gtk_dialog_set_default_response(GTK_DIALOG(gui_state().status.dialog), 1);

	out_status(client_state());
	status_update_dialog();

	gtk_widget_show_all(gui_state().status.dialog);

	while(gtk_dialog_run(GTK_DIALOG(gui_state().status.dialog)) == 0) {
		out_status(client_state());
		status_update_dialog();
		gtk_widget_queue_draw(gui_state().status.dialog);
	}

	if(GTK_IS_WIDGET(gui_state().status.dialog)) {
		gtk_widget_destroy(gui_state().status.dialog);
		gui_state().status.dialog = NULL;
	}
}


/*
 *
 *  mini status
 *
 */
TEG_STATUS ministatus_update()
{
	if(gui_state().status.ministatus == NULL) {
		return TEG_STATUS_ERROR;
	}

	gtk_widget_queue_draw(gui_state().status.ministatus);

	return TEG_STATUS_SUCCESS;
}

static gboolean ministatus_expose_cb(GtkWidget *area, cairo_t *cr, gpointer user_data)
{
	gint width, height;
	int i=0;

	if(client_status(client_state()) == PLAYER_STATUS_DESCONECTADO || client_state().game.observer) {
		i = -1;
	} else {
		i = client_state().game.numjug;
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
	if(gui_state().status.ministatus == NULL) {
		gui_state().status.ministatus = gtk_drawing_area_new();

		g_signal_connect(G_OBJECT(gui_state().status.ministatus), "draw",
		                 G_CALLBACK(ministatus_expose_cb), NULL);
	}
	gtk_widget_set_size_request(gui_state().status.ministatus, 15, -1);
	gtk_widget_show(gui_state().status.ministatus);

	return gui_state().status.ministatus;
}


/*
 * Main Status
 */

#define MAINSTATUS_X (641)
#define MAINSTATUS_Y (48)
TEG_STATUS mainstatus_create(GtkWidget **window)
{
	int failed=1;

	if(gui_state().status.mainstatus_canvas) {
		goto error;
	}

	gui_state().status.mainstatus_canvas = goo_canvas_new();
	if(! gui_state().status.mainstatus_canvas) {
		goto error;
	}

	/* load colors for started_item, and player_colors */
	colors_load_images();

	gtk_widget_set_size_request(gui_state().status.mainstatus_canvas,
	                            MAINSTATUS_X, MAINSTATUS_Y + 1);
	goo_canvas_set_bounds(GOO_CANVAS(gui_state().status.mainstatus_canvas), 0, 0,
	                      MAINSTATUS_X, MAINSTATUS_Y);
	g_object_set(gui_state().status.mainstatus_canvas, "anchor",
	             GOO_CANVAS_ANCHOR_CENTER, NULL);

	/* background */
	if(gui_state().theme.toolbar_custom && gui_state().theme.toolbar_name) {
		GdkPixbuf *im;

		const auto fpath = theme_load_file(client_state(), gui_state().theme.toolbar_name);
		char const*const filename = fpath ? fpath->c_str() : nullptr;
		im = gdk_pixbuf_new_from_file(filename, NULL);

		if(im) {
			goo_canvas_image_new(
			    goo_canvas_get_root_item(GOO_CANVAS(gui_state().status.mainstatus_canvas)),
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
		    goo_canvas_get_root_item(GOO_CANVAS(gui_state().status.mainstatus_canvas)),
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
	    goo_canvas_get_root_item(GOO_CANVAS(gui_state().status.mainstatus_canvas)),
	    _("Round started by:"),
	    (double) ROUND_OFFSET,
	    (double) 3,
	    (double) -1,
	    GOO_CANVAS_ANCHOR_NORTH_EAST,
	    "height", (double) -1,
	    "font", HELVETICA_10_FONT,
	    "fill-color", gui_state().theme.toolbar_custom && gui_state().theme.toolbar_text_color ? gui_state().theme.toolbar_text_color : "black",
	    NULL);

	gui_state().status.color_started_item = goo_canvas_image_new(
	        goo_canvas_get_root_item(GOO_CANVAS(gui_state().status.mainstatus_canvas)),
	        gui_state().colors.circle_pixbufs[maximum_player_count],
	        (double) ROUND_OFFSET + 4,
	        (double) 4,
	        "width", (double) gdk_pixbuf_get_width(gui_state().colors.circle_pixbufs[maximum_player_count]),
	        "height", (double) gdk_pixbuf_get_height(gui_state().colors.circle_pixbufs[maximum_player_count]),
	        NULL);
	g_object_set(gui_state().status.color_started_item, "visibility",
	             GOO_CANVAS_ITEM_INVISIBLE, NULL);

	/* round number */
	goo_canvas_text_new(
	    goo_canvas_get_root_item(GOO_CANVAS(gui_state().status.mainstatus_canvas)),
	    _("Round number:"),
	    (double) ROUND_OFFSET,
	    (double) MAINSTATUS_Y/2 + 1,
	    (double) -1,
	    GOO_CANVAS_ANCHOR_NORTH_EAST,
	    "height", (double) -1,
	    "font", HELVETICA_10_FONT,
	    "fill-color", gui_state().theme.toolbar_custom && gui_state().theme.toolbar_text_color ? gui_state().theme.toolbar_text_color : "black",
	    NULL);

	gui_state().status.round_number_item = goo_canvas_text_new(
	        goo_canvas_get_root_item(GOO_CANVAS(gui_state().status.mainstatus_canvas)),
	        _("?"),
	        (double) ROUND_OFFSET + 4,
	        (double) MAINSTATUS_Y/2 + 2,
	        (double) -1,
	        GOO_CANVAS_ANCHOR_NORTH_WEST,
	        "height", (double) -1,
	        "font", HELVETICA_12_FONT,
	        "fill-color", gui_state().theme.toolbar_custom && gui_state().theme.toolbar_text_color ? gui_state().theme.toolbar_text_color : "black",
	        NULL);

	/* players turn */
	goo_canvas_text_new(
	    goo_canvas_get_root_item(GOO_CANVAS(gui_state().status.mainstatus_canvas)),
	    _("Players turn:"),
	    (double) PLAYERS_COLORS_OFFSET - 4,
	    (double) 3,
	    (double) -1,
	    GOO_CANVAS_ANCHOR_NORTH_EAST,
	    "height", (double) -1,
	    "font", HELVETICA_10_FONT,
	    "fill-color", gui_state().theme.toolbar_custom && gui_state().theme.toolbar_text_color ? gui_state().theme.toolbar_text_color : "black",
	    NULL);

	/* create canvas for the circles & and load the circles */
	for(unsigned i=0; i<maximum_player_count; i++) {
		gui_state().status.players_color[i] = goo_canvas_image_new(
		        goo_canvas_get_root_item(GOO_CANVAS(gui_state().status.mainstatus_canvas)),
		        gui_state().colors.circle_pixbufs[maximum_player_count],
		        0.0,
		        0.0,
		        "width", (double) gdk_pixbuf_get_width(gui_state().colors.circle_pixbufs[maximum_player_count]),
		        "height", (double) gdk_pixbuf_get_height(gui_state().colors.circle_pixbufs[maximum_player_count]),
		        NULL);
		g_object_set(gui_state().status.players_color[i], "visibility",
		             GOO_CANVAS_ITEM_INVISIBLE, NULL);
	}

	gui_state().status.players_color_over = goo_canvas_image_new(
	        goo_canvas_get_root_item(GOO_CANVAS(gui_state().status.mainstatus_canvas)),
	        gui_state().colors.circle_over_pixbuf,
	        0.0,
	        0.0,
	        "width", (double) gdk_pixbuf_get_width(gui_state().colors.circle_over_pixbuf),
	        "height", (double) gdk_pixbuf_get_height(gui_state().colors.circle_over_pixbuf),
	        NULL);
	g_object_set(gui_state().status.players_color_over, "visibility",
	             GOO_CANVAS_ITEM_INVISIBLE, NULL);


	mainstatus_update();

	*window = gui_state().status.mainstatus_canvas;
	gtk_widget_show(gui_state().status.mainstatus_canvas);
	return TEG_STATUS_SUCCESS;

error:
	return TEG_STATUS_ERROR;
}

TEG_STATUS mainstatus_update_colors()
{
	if(! gui_state().status.mainstatus_canvas) {
		return TEG_STATUS_ERROR;
	}

	std::size_t i=0;

	g_object_set(gui_state().status.players_color_over, "visibility",
	             GOO_CANVAS_ITEM_INVISIBLE, NULL);
	players_map_int(client_state(), [&i](Player& player) {

		if(player.color >= 0 && player.numjug >= 0) {
			g_object_set(gui_state().status.players_color[i], "visibility",
			             GOO_CANVAS_ITEM_VISIBLE, NULL);

			if(client_state().game.whos_turn == player.numjug) {
				g_object_set(
				    gui_state().status.players_color_over,
				    "pixbuf", gui_state().colors.circle_over_pixbuf,
				    "x", (double) PLAYERS_COLORS_OFFSET - 1 + (i%3) * 14,
				    "y", (double) 2 + 13 * (i<3?0:1),
				    "width", (double) gdk_pixbuf_get_width(gui_state().colors.circle_over_pixbuf) + 4,
				    "height", (double) gdk_pixbuf_get_height(gui_state().colors.circle_over_pixbuf) + 4,
				    "visibility", GOO_CANVAS_ITEM_VISIBLE,
				    NULL);
			}

			g_object_set(
			    gui_state().status.players_color[i],
			    "pixbuf", gui_state().colors.circle_pixbufs[player.color],
			    "x", (double) PLAYERS_COLORS_OFFSET + (i%3) * 14,
			    "y", (double) 4 + 13 * (i<3?0:1),
			    "width", (double) gdk_pixbuf_get_width(gui_state().colors.circle_pixbufs[player.color]),
			    "height", (double) gdk_pixbuf_get_height(gui_state().colors.circle_pixbufs[player.color]),
			    NULL);

			i++;
		}

		return i < maximum_player_count;
	});

	{
		PCPLAYER pJ;
		g_object_set(gui_state().status.color_started_item, "visibility",
		             GOO_CANVAS_ITEM_INVISIBLE, NULL);
		if(client_state().game.who_started_round >= 0 && client_state().game.who_started_round < static_cast<int>(maximum_player_count)) {

			if(player_whois(client_state(), client_state().game.who_started_round, &pJ) == TEG_STATUS_SUCCESS) {
				g_object_set(
				    gui_state().status.color_started_item,
				    "pixbuf", gui_state().colors.circle_pixbufs[pJ->color],
				    "visibility", GOO_CANVAS_ITEM_VISIBLE,
				    NULL);
			}
		}
	}

	for(; i < maximum_player_count ; i++)
		g_object_set(gui_state().status.players_color[i], "visibility",
		             GOO_CANVAS_ITEM_INVISIBLE, NULL);

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS mainstatus_update()
{
	char buffer[256];


	if(! gui_state().status.mainstatus_canvas) {
		return TEG_STATUS_ERROR;
	}

	if(gui_state().status.offset_left == -1 && gui_state().status.offset_right == -1) {
		gui_state().status.offset_left = gui_state().theme.toolbar_offset_left != -1 ? gui_state().theme.toolbar_offset_left : OFFSET_LEFT;
		gui_state().status.offset_right = gui_state().theme.toolbar_offset_right != -1 ? gui_state().theme.toolbar_offset_right : OFFSET_RIGHT;
	}

	/* game status */

	if(gui_state().status.gamestatus_item) {
		goo_canvas_item_remove(gui_state().status.gamestatus_item);
		gui_state().status.gamestatus_item = NULL;
	}

	gui_state().status.gamestatus_item = goo_canvas_text_new(
	        goo_canvas_get_root_item(GOO_CANVAS(gui_state().status.mainstatus_canvas)),
	        _(player_status_name(client_state().game.estado)),
	        (double)(MAINSTATUS_X + gui_state().status.offset_right),
	        (double)(MAINSTATUS_Y/2),
	        (double) -1,
	        GOO_CANVAS_ANCHOR_EAST,
	        "height", (double) -1,
	        "font", HELVETICA_14_FONT,
	        "fill-color", gui_state().theme.toolbar_custom && gui_state().theme.toolbar_text_color ? gui_state().theme.toolbar_text_color : "black",
	        NULL);

	/* country */
	if(gui_state().status.country_item) {
		goo_canvas_item_remove(gui_state().status.country_item);
		gui_state().status.country_item = NULL;
	}

	if(gui_state().priv.country_i_am < 0 || gui_state().priv.country_i_am >= static_cast<int>(COUNTRIES_CANT)) {
		snprintf(buffer, sizeof(buffer)-1, " ");
	} else {
		snprintf(buffer, sizeof(buffer)-1, "%s", countries_get_name(client_state().game.common, gui_state().priv.country_i_am));
	}

	gui_state().status.country_item = goo_canvas_text_new(
	                                      goo_canvas_get_root_item(GOO_CANVAS(gui_state().status.mainstatus_canvas)),
	                                      buffer,
	                                      (double) gui_state().status.offset_left,
	                                      (double)(2),
	                                      (double) -1,
	                                      GOO_CANVAS_ANCHOR_NORTH_WEST,
	                                      "height", (double) -1,
	                                      "font", HELVETICA_12_FONT,
	                                      "fill-color", gui_state().theme.toolbar_custom && gui_state().theme.toolbar_text_color ? gui_state().theme.toolbar_text_color : "black",
	                                      NULL);

	/* continent */
	if(gui_state().status.cont_item) {
		goo_canvas_item_remove(gui_state().status.cont_item);
		gui_state().status.cont_item = NULL;
	}

	if(gui_state().priv.country_i_am < 0 || gui_state().priv.country_i_am >= static_cast<int>(COUNTRIES_CANT)) {
		snprintf(buffer, sizeof(buffer)-1, " ");
	} else {
		snprintf(buffer, sizeof(buffer)-1, "%s", cont_get_name(client_state().game.common, client_state().game.common.countries[gui_state().priv.country_i_am].continente));
	}

	gui_state().status.cont_item = goo_canvas_text_new(
	                                   goo_canvas_get_root_item(GOO_CANVAS(gui_state().status.mainstatus_canvas)),
	                                   buffer,
	                                   (double) gui_state().status.offset_left,
	                                   (double)(16),
	                                   (double) -1,
	                                   GOO_CANVAS_ANCHOR_NORTH_WEST,
	                                   "height", (double) -1,
	                                   "font", HELVETICA_10_FONT,
	                                   "fill-color", gui_state().theme.toolbar_custom && gui_state().theme.toolbar_text_color ? gui_state().theme.toolbar_text_color : "black",
	                                   NULL);

	/* round number */
	{
		char buffer[256];

		g_object_set(gui_state().status.round_number_item, "visibility",
		             GOO_CANVAS_ITEM_INVISIBLE, NULL);
		if(client_state().game.round_number >= 0) {

			sprintf(buffer, "%d", client_state().game.round_number);

			g_object_set(
			    gui_state().status.round_number_item,
			    "text", buffer,
			    "visibility", GOO_CANVAS_ITEM_VISIBLE,
			    NULL);
		}
	}

	mainstatus_update_colors();

	return TEG_STATUS_SUCCESS;
}

}
