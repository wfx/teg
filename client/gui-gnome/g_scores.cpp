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
 * @file gui_scores.c
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <assert.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include "client.h"
#include "support.h"
#include "interface.h"
#include "callbacks.h"
#include "g_scores.h"
#include "colors.h"
#include "fonts.h"
#include "support.h"


static GtkWidget *gui_scores_dialog=NULL;
static GtkListStore *store=NULL;

enum {
	COLUMN_POSNUMBER,
	COLUMN_SCORE,
	COLUMN_NAME,
	COLUMN_DATE,
	COLUMN_COLOR,
	COLUMN_HUMAN,

	COLUMN_LAST
};

#define S_CLIST_MAX 100


static GtkListStore *scores_create_model(void)
{
	/* create list store */
	store = gtk_list_store_new(
	            COLUMN_LAST,
	            G_TYPE_UINT,
	            G_TYPE_UINT,
	            G_TYPE_STRING,
	            G_TYPE_STRING,
	            G_TYPE_STRING,
	            G_TYPE_BOOLEAN);

	return store;
}

static void scores_add_columns(GtkTreeView *treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	/* column for pos number */
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Number"),
	         renderer,
	         "text", COLUMN_POSNUMBER,
	         NULL);
	gtk_tree_view_column_set_sort_column_id(column, COLUMN_POSNUMBER);
	gtk_tree_view_append_column(treeview, column);

	/* column for scores */
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Score"),
	         renderer,
	         "text", COLUMN_SCORE,
	         NULL);
	gtk_tree_view_column_set_sort_column_id(column, COLUMN_SCORE);
	gtk_tree_view_append_column(treeview, column);

	/* column for name */
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Name"),
	         renderer,
	         "text", COLUMN_NAME,
	         NULL);
	gtk_tree_view_column_set_sort_column_id(column, COLUMN_NAME);
	gtk_tree_view_append_column(treeview, column);

	/* column for date */
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Date"),
	         renderer,
	         "text", COLUMN_DATE,
	         NULL);
	gtk_tree_view_column_set_sort_column_id(column, COLUMN_DATE);
	gtk_tree_view_append_column(treeview, column);

	/* column for color */
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Color"),
	         renderer,
	         "text", COLUMN_COLOR,
	         NULL);
	gtk_tree_view_column_set_sort_column_id(column, COLUMN_COLOR);
	gtk_tree_view_append_column(treeview, column);

	/* column for is human?*/
	renderer = gtk_cell_renderer_toggle_new();
	column = gtk_tree_view_column_new_with_attributes(_("Human?"),
	         renderer,
	         "active", COLUMN_HUMAN,
	         NULL);
	gtk_tree_view_append_column(treeview, column);
}

/*
 * helper funcion used by mini_scores & big score
 */
static TEG_STATUS paint_color(GtkWidget *dialog, int color, GdkPixbuf **pixmap)
{
	GdkWindow *window;
	PangoLayout *layout;
	PangoAttrList *list;
	PangoAttribute *attr;
	cairo_surface_t *surface;
	cairo_t *cr;
	int i, width;

	assert(pixmap);

	i = (color<0 || color>=TEG_MAX_PLAYERS) ? TEG_MAX_PLAYERS : color;

	window = gtk_widget_get_window(dialog);
	surface = gdk_window_create_similar_image_surface(window,
	          CAIRO_FORMAT_ARGB32,
	          48, 16, 0);

	cr = cairo_create(surface);
	gdk_cairo_set_source_rgba(cr, colors_get_player_from_color(color));
	cairo_rectangle(cr, 0, 0, 47, 15);
	cairo_fill(cr);
	cairo_paint(cr);

	gdk_cairo_set_source_rgba(cr, colors_get_common(COLORS_BLACK));
	cairo_rectangle(cr, 0, 0, 47, 15);
	cairo_stroke(cr);

	gdk_cairo_set_source_rgba(cr,
	                          colors_get_player_ink_from_color(color));

	layout = pango_cairo_create_layout(cr);
	list = pango_attr_list_new();
	attr = pango_attr_size_new_absolute(10 * PANGO_SCALE);
	pango_attr_list_insert(list, attr);
	pango_layout_set_attributes(layout, list);
	pango_layout_set_text(layout, _(g_colores[i]), -1);
	pango_layout_get_size(layout, &width, NULL);
	cairo_move_to(cr, (47 - width / PANGO_SCALE) / 2, 0);
	pango_cairo_show_layout(cr, layout);

	*pixmap = gdk_pixbuf_get_from_surface(surface, 0, 0, 47, 15);

	pango_attr_list_unref(list);
	g_object_unref(layout);
	cairo_destroy(cr);
	cairo_surface_destroy(surface);

	return TEG_STATUS_SUCCESS;
}

struct AddScoreState {
	GtkListStore *store;
	int row;
};

static void append_player_score(PSCORES pS, void* user)
{
	struct AddScoreState *state = (struct AddScoreState *) user;

	gchar *name = translate_to_utf8(pS->name);

	GtkTreeIter iter;
	gtk_list_store_append(state->store, &iter);
	gtk_list_store_set(store, &iter,
	                   COLUMN_POSNUMBER, ++state->row,
	                   COLUMN_NAME, name,
	                   COLUMN_SCORE, pS->score,
	                   COLUMN_DATE, pS->date,
	                   COLUMN_COLOR, g_colores[pS->color],
	                   COLUMN_HUMAN, pS->human,
	                   -1);
	free(name);
}

static TEG_STATUS scores_update_model(GtkListStore *store)
{
	gtk_list_store_clear(store);

	struct AddScoreState ass = {.store = store, .row=0};
	scores_map(append_player_score, &ass);

	return TEG_STATUS_SUCCESS;
}

static TEG_STATUS scores_update_dialog()
{
	static GtkWidget *scores_treeview=NULL;

	if(gui_scores_dialog == NULL) {
		return TEG_STATUS_ERROR;
	}


	if(!GTK_IS_WIDGET(scores_treeview)) {

		/* create tree model */
		if(!store) {
			store = scores_create_model();
		}

		/* create tree view */
		scores_treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL
		                  (store));
		gtk_tree_view_set_search_column(GTK_TREE_VIEW(scores_treeview),
		                                COLUMN_SCORE);

		gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area
		                           (GTK_DIALOG(gui_scores_dialog))),
		                   GTK_WIDGET(scores_treeview), TRUE, TRUE, 0);

		/* add columns to the tree view */
		scores_add_columns(GTK_TREE_VIEW(scores_treeview));

	}

	scores_update_model(store);

	gtk_widget_show_all(scores_treeview);
	return TEG_STATUS_SUCCESS;
}

void gui_scores_view()
{
	if(gui_scores_dialog == NULL) {

		gui_scores_dialog = teg_dialog_new(_("High Scores"), _("High Scores"));
		gtk_dialog_add_buttons(GTK_DIALOG(gui_scores_dialog),
		                       _("_Refresh"), 0,
		                       _("_Close"), 1,
		                       NULL);

		gtk_dialog_set_default_response(GTK_DIALOG(gui_scores_dialog), 1);

	}

	scores_update_dialog();

	if(gtk_dialog_run(GTK_DIALOG(gui_scores_dialog)) == 0) {
		out_scores();
	}

	if(GTK_IS_WIDGET(gui_scores_dialog)) {
		gtk_widget_destroy(gui_scores_dialog);
		gui_scores_dialog = NULL;
	}
}


/*******************************
 *														 *
 *			 embeded scores				 *
 *														 *
 *******************************/

enum {
	MINISCORE_CLIST_NAME,
	MINISCORE_CLIST_SCORE,
	MINISCORE_CLIST_COLOR,

	MINISCORE_CLIST_LAST
};
static TEG_STATUS update_mini_clist(GtkWidget *dialog, GtkListStore *store,
                                    GtkWidget *clist)
{
	PLIST_ENTRY list = &g_list_player;
	PLIST_ENTRY l = list->Flink;
	PCPLAYER pJ;
	int row;
	GdkPixbuf *pixmap;
	GtkTreeIter iter;

	row = 0;
	while(!IsListEmpty(list)&& (l != list)) {
		gchar *name, *score;

		pJ = (PCPLAYER) l;

		name = translate_to_utf8(pJ->name);
		score = g_strdup_printf("%d", pJ->score);

		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter,
		                   MINISCORE_CLIST_NAME, name,
		                   MINISCORE_CLIST_SCORE, score, -1);
		free(name);
		g_free(score);

		paint_color(dialog, pJ->color, &pixmap);

		gtk_list_store_set(store, &iter, MINISCORE_CLIST_COLOR,
		                   pixmap, -1);
		g_object_unref(pixmap);

		l = LIST_NEXT(l);
		row ++;
	}

	return TEG_STATUS_SUCCESS;
}

void gui_scores_embed(GtkWidget *frame)
{
	static GtkWidget *mini_scores_clist=NULL;
	GtkListStore *store = NULL;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *name, *score, *color;

	store = gtk_list_store_new(MINISCORE_CLIST_LAST, G_TYPE_STRING,
	                           G_TYPE_STRING, G_TYPE_OBJECT);
	mini_scores_clist = gtk_tree_view_new_with_model(GTK_TREE_MODEL
	                    (store));

	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(mini_scores_clist),
	                                  TRUE);

	renderer = gtk_cell_renderer_text_new();
	g_object_set(renderer, "xalign", 0.5, NULL);
	name = gtk_tree_view_column_new_with_attributes(_("Name"), renderer,
	        "text",
	        MINISCORE_CLIST_NAME,
	        NULL);
	gtk_tree_view_column_set_alignment(name, 0.5);
	gtk_tree_view_append_column(GTK_TREE_VIEW(mini_scores_clist), name);

	score = gtk_tree_view_column_new_with_attributes(_("Score"), renderer,
	        "text",
	        MINISCORE_CLIST_SCORE,
	        NULL);
	gtk_tree_view_column_set_alignment(score, 0.5);
	gtk_tree_view_append_column(GTK_TREE_VIEW(mini_scores_clist), score);

	renderer = gtk_cell_renderer_pixbuf_new();
	color = gtk_tree_view_column_new_with_attributes(_("Color"), renderer,
	        "pixbuf",
	        MINISCORE_CLIST_COLOR,
	        NULL);
	gtk_tree_view_column_set_alignment(color, 0.5);
	gtk_tree_view_append_column(GTK_TREE_VIEW(mini_scores_clist), color);

	update_mini_clist(frame, store, mini_scores_clist);
	gtk_widget_show(mini_scores_clist);
	gtk_tree_view_columns_autosize(GTK_TREE_VIEW(mini_scores_clist));

	gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(mini_scores_clist));
}
