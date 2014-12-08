/*	$Id: g_scores.c,v 1.14 2007/07/13 20:57:07 nordi Exp $ */
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
#include <gnome.h>

#include "gui.h"
#include "client.h"
#include "support.h"
#include "interface.h"
#include "callbacks.h"
#include "g_scores.h"
#include "colors.h"
#include "fonts.h"
#include "support.h"


static GtkWidget *gui_scores_dialog=NULL;

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

static gint boton_clicked_refresh(GtkWidget *area, GdkEventExpose *event, gpointer user_data)
{
	out_scores();
	return FALSE;
}


static GtkTreeModel *
scores_create_model (void)
{
	GtkListStore *store;

	/* create list store */
	store = gtk_list_store_new (
			COLUMN_LAST,
			G_TYPE_UINT,
			G_TYPE_UINT,
			G_TYPE_STRING,
			G_TYPE_STRING,
			G_TYPE_STRING,
			G_TYPE_BOOLEAN);

	return GTK_TREE_MODEL (store);
}

static void scores_add_columns (GtkTreeView *treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	/* column for pos number */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Number"),
			renderer,
			"text", COLUMN_POSNUMBER,
			NULL);
	gtk_tree_view_column_set_sort_column_id (column, COLUMN_POSNUMBER);
	gtk_tree_view_append_column (treeview, column);

	/* column for scores */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Score"),
			renderer,
			"text", COLUMN_SCORE,
			NULL);
	gtk_tree_view_column_set_sort_column_id (column, COLUMN_SCORE);
	gtk_tree_view_append_column (treeview, column);

	/* column for name */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Name"),
			renderer,
			"text", COLUMN_NAME,
			NULL);
	gtk_tree_view_column_set_sort_column_id (column, COLUMN_NAME);
	gtk_tree_view_append_column (treeview, column);

	/* column for date */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Date"),
			renderer,
			"text", COLUMN_DATE,
			NULL);
	gtk_tree_view_column_set_sort_column_id (column, COLUMN_DATE);
	gtk_tree_view_append_column (treeview, column);

	/* column for color */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Color"),
			renderer,
			"text", COLUMN_COLOR,
			NULL);
	gtk_tree_view_column_set_sort_column_id (column, COLUMN_COLOR);
	gtk_tree_view_append_column (treeview, column);

	/* column for is human?*/
	renderer = gtk_cell_renderer_toggle_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Human?"),
			renderer,
			"active", COLUMN_HUMAN,
			NULL);
	gtk_tree_view_append_column (treeview, column);
}

/*
 * helper function used by mini_scores & big score
 */
static TEG_STATUS paint_color( GtkWidget *dialog, int color, GdkPixmap **pixmap )
{
	int i, h, w;

	assert( pixmap );

	i = (color<0 || color>=TEG_MAX_PLAYERS) ? TEG_MAX_PLAYERS : color;

	*pixmap = gdk_pixmap_new( dialog->window,
		48, 16, gtk_widget_get_visual( dialog)->depth);

	if( *pixmap == NULL )
		return TEG_STATUS_ERROR;

	gdk_gc_set_foreground(g_colors_gc, colors_get_player_from_color(color));
	gdk_draw_rectangle( *pixmap, g_colors_gc, TRUE, 0, 0, 47, 15);

	gdk_gc_set_foreground(g_colors_gc, colors_get_common(COLORS_BLACK));
	gdk_draw_rectangle( *pixmap, g_colors_gc, FALSE, 0, 0, 47, 15);

	gdk_gc_set_foreground(g_colors_gc, colors_get_player_ink_from_color(color));


	h = gdk_string_height (g_pixmap_font10, _(g_colores[i]) );
	w = gdk_string_width	(g_pixmap_font10, _(g_colores[i]) );

	gdk_draw_string( *pixmap, g_pixmap_font10, g_colors_gc, 
			((48 - w )/2),
			((16 - h)/2) + h, _(g_colores[i]));
	return TEG_STATUS_SUCCESS;
}


static TEG_STATUS scores_update_model( GtkTreeModel *model)
{
	PSCORES pS;
	int row;
	GtkListStore *store;
	GtkTreeIter iter;
	PLIST_ENTRY list = scores_get_list();
	PLIST_ENTRY l = list->Flink;


	store = GTK_LIST_STORE( model );

	gtk_list_store_clear( store );

	row = 0;

	while( !IsListEmpty( list )&& (l != list ) )
	{
		gchar *name;
		pS = (PSCORES) l;

		name = translate_to_utf8( pS->name );

		/* sprintf(clist_texts[0],"%s",g_colores[j->color]); */

		gtk_list_store_append (store, &iter);
		pS->color = (pS->color<0 || pS->color>=TEG_MAX_PLAYERS) ? TEG_MAX_PLAYERS : pS->color;

		gtk_list_store_set (store, &iter,
					COLUMN_POSNUMBER, ++row,
					COLUMN_NAME, name,
					COLUMN_SCORE, pS->stats.score,
					COLUMN_DATE, pS->date,
					COLUMN_COLOR, g_colores[pS->color],
					COLUMN_HUMAN, pS->human,
					-1);

		free( name );
		l = LIST_NEXT(l);
	}

	return TEG_STATUS_SUCCESS;
}

static TEG_STATUS scores_update_dialog()
{
	static GtkWidget *scores_treeview=NULL;
	static GtkTreeModel *model = NULL;

	if( gui_scores_dialog == NULL )
		return TEG_STATUS_ERROR;


	if( scores_treeview == NULL ) {

		/* create tree model */
		model = scores_create_model ();

		/* create tree view */
		scores_treeview = gtk_tree_view_new_with_model (model);
		gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (scores_treeview), TRUE);
		gtk_tree_view_set_search_column (GTK_TREE_VIEW (scores_treeview),
				COLUMN_SCORE);

		g_object_unref (G_OBJECT (model)); 
		gtk_box_pack_start_defaults( GTK_BOX(GNOME_DIALOG(gui_scores_dialog)->vbox), GTK_WIDGET(scores_treeview));

		/* add columns to the tree view */
		scores_add_columns (GTK_TREE_VIEW (scores_treeview));

	}

	scores_update_model( model );

	gtk_widget_show_all( scores_treeview );
	return TEG_STATUS_SUCCESS;
}

void gui_scores_view()
{
	if( gui_scores_dialog == NULL) {

		gui_scores_dialog = teg_dialog_new(_("High Scores"),_("High Scores")); 
		gnome_dialog_append_buttons(GNOME_DIALOG(gui_scores_dialog),
				GNOME_STOCK_PIXMAP_REFRESH,
				GNOME_STOCK_BUTTON_CLOSE,
				NULL );
		gnome_dialog_close_hides( GNOME_DIALOG(gui_scores_dialog), TRUE );
		gnome_dialog_set_default(GNOME_DIALOG(gui_scores_dialog),1);

		/* signals de los botones */
		gnome_dialog_button_connect (GNOME_DIALOG(gui_scores_dialog),
						0, GTK_SIGNAL_FUNC(boton_clicked_refresh),gui_scores_dialog);
		gnome_dialog_button_connect (GNOME_DIALOG(gui_scores_dialog),
						1, GTK_SIGNAL_FUNC(dialog_close), gui_scores_dialog );

		gnome_dialog_set_default( GNOME_DIALOG(gui_scores_dialog),1);
	}

	scores_update_dialog();

	gtk_widget_show_all( gui_scores_dialog);
	raise_and_focus( gui_scores_dialog);
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
static TEG_STATUS update_mini_clist( GtkWidget *dialog, GtkWidget *clist )
{
	PLIST_ENTRY list = &g_list_player;
	PLIST_ENTRY l = list->Flink;
	PCPLAYER pJ;
	char clist_text[MINISCORE_CLIST_LAST][S_CLIST_MAX];
	char *clist_texts[MINISCORE_CLIST_LAST];
	int i, row;
	GdkPixmap *pixmap;

	gtk_clist_freeze(GTK_CLIST(clist));

	for(i=0;i<MINISCORE_CLIST_LAST;i++) {
		clist_texts[i] = clist_text[i];
		memset( clist_text[i],0,S_CLIST_MAX);
	}

	row = 0;
	while( !IsListEmpty( list )&& (l != list ) )
	{
		gchar *name;

		pJ = (PCPLAYER) l;

		name = translate_to_utf8( pJ->name );

		snprintf(clist_texts[MINISCORE_CLIST_NAME],S_CLIST_MAX-1,"%s",name);
		snprintf(clist_texts[MINISCORE_CLIST_SCORE],S_CLIST_MAX-1,"%d",pJ->score);

		free( name );

		gtk_clist_append (GTK_CLIST (clist), clist_texts);

		if( paint_color( dialog, pJ->color, &pixmap ) == TEG_STATUS_SUCCESS )
			gtk_clist_set_pixmap(GTK_CLIST(clist), row, MINISCORE_CLIST_COLOR, pixmap, NULL);

		l = LIST_NEXT(l);
		row ++;
	}
	gtk_clist_thaw(GTK_CLIST(clist));

	return TEG_STATUS_SUCCESS;
}

void gui_scores_embed( GtkWidget *frame )
{
	int i;
	static GtkWidget *mini_scores_clist=NULL;
	char *titles[MINISCORE_CLIST_LAST] = {
		_("Name"), _("Score"), _("Color"),
	};

	mini_scores_clist = gtk_clist_new_with_titles (MINISCORE_CLIST_LAST, titles);

	gtk_clist_set_selection_mode (GTK_CLIST (mini_scores_clist), GTK_SELECTION_EXTENDED);
	for (i = 0; i < MINISCORE_CLIST_LAST; i++) {
		gtk_clist_set_column_justification (GTK_CLIST (mini_scores_clist), i, GTK_JUSTIFY_CENTER);
		gtk_clist_set_column_auto_resize (GTK_CLIST (mini_scores_clist), i, TRUE);
	}

	update_mini_clist( frame, mini_scores_clist );

	gtk_container_add (GTK_CONTAINER (frame),GTK_WIDGET(mini_scores_clist));
}
