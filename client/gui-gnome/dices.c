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
 * @file dices.c
 * Muestra los dados
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <goocanvas.h>
#include <glib/gi18n.h>

#include "gui.h"
#include "client.h"

#include "dices.h"

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "priv.h"
#include "dices.h"
#include "themes.h"
#include "fonts.h"


extern TTheme gui_theme;


static GdkPixbuf *dices[DICES_CANT] = { NULL, NULL, NULL, NULL, NULL, NULL };
static GooCanvasItem *images[DICES_CANT] = { NULL, NULL, NULL, NULL, NULL, NULL };
static GooCanvasItem *text[2] = {NULL, NULL};
static GooCanvasItem* dices_group=NULL;
static int dices_initialized=0;

struct _dices_coord {
	double x;
	double y;
};

struct _dices_pos {
	struct _dices_coord attacker[3];
	struct _dices_coord attacker_text;
	struct _dices_coord defender[3];
	struct _dices_coord defender_text;
} dices_pos;

typedef enum dices_type {
	DICES_ATTACKER,
	DICES_DEFENDER
} dices_type_t;


/*
 * helper functions
 */
static TEG_STATUS dices_load_from_file(char *name, int i)
{
	char *filename = NULL;

	filename = theme_load_file(name);

	dices[i] = gdk_pixbuf_new_from_file(filename, NULL);
	if(dices[i] == NULL) {
		g_warning(_("Error, couldn't find file: %s"), name);
		return TEG_STATUS_ERROR;
	}

	return TEG_STATUS_SUCCESS;
}

static TEG_STATUS dices_load()
{
	int i;

	if(dices_initialized) {
		return TEG_STATUS_SUCCESS;
	}

	for(i=0; i<DICES_CANT; i++) {
		char name[40];
		memset(name, 0, sizeof(name));
		snprintf(name, sizeof(name)-1, "dice-%d.png", i+1);
		if(!dices[i]) {
			dices_load_from_file(name, i);
		}
	}

	return TEG_STATUS_SUCCESS;
}

static void dices_show_text(int country, dices_type_t type)
{
	if(text[type]) {
		goo_canvas_item_remove(text[type]);
		text[type] = NULL;
	}

	text[type] = goo_canvas_text_new(
	                 dices_group,
	                 countries_get_name(country),
	                 (double)(type == DICES_ATTACKER) ? dices_pos.attacker_text.x :  dices_pos.defender_text.x,
	                 (double)(type == DICES_ATTACKER) ? dices_pos.attacker_text.y :  dices_pos.defender_text.y,
	                 (double) -1,
	                 GOO_CANVAS_ANCHOR_NORTH,
	                 "height", (double) -1,
	                 "font", HELVETICA_12_BFONT,
	                 "fill-color", gui_theme.dices_color,
	                 NULL);
}

static void dices_show_image(int dice_index, dices_type_t type, int pos)
{
	int i;
	double x, y;

	if(type == DICES_ATTACKER) {
		x = dices_pos.attacker[pos].x;
		y = dices_pos.attacker[pos].y;

		i = pos;
	} else {
		x = dices_pos.defender[pos].x;
		y = dices_pos.defender[pos].y;

		i = pos + 3;
	}

	if(images[i] == NULL) {
		images[i] = goo_canvas_image_new(
		                dices_group,
		                dices[dice_index],
		                x,
		                y,
		                "width", (double) gdk_pixbuf_get_width(dices[dice_index]),
		                "height", (double) gdk_pixbuf_get_height(dices[dice_index]),
		                NULL);

	} else {
		g_object_set(images[i],
		             "pixbuf", dices[dice_index],
		             "x", x,
		             "y", y,
		             "width", (double) gdk_pixbuf_get_width(dices[dice_index]),
		             "height", (double) gdk_pixbuf_get_height(dices[dice_index]),
		             "visibility", GOO_CANVAS_ITEM_VISIBLE,
		             NULL);
	}
}

/*
 * exported functions
 */
void dices_init(GooCanvasItem *root)
{
	if(dices_initialized) {
		return;
	}

	if(!root) {
		return;
	}

	dices_group =
	    goo_canvas_group_new(
	        root,
	        "x", (float) gui_theme.dices_x,
	        "y", (float) gui_theme.dices_y,
	        NULL
	    );

	if(!dices_group) {
		return;
	}

	dices_load();


	/* load dices position */
	{
		int i;
		if(! theme_using_extended_dices()) {
			/* basic behaviour */
			double x = 0;
			for(i=0; i<3; i++) {
				dices_pos.attacker[i].x = x;
				dices_pos.attacker[i].y = 4;
				dices_pos.defender[i].x = x;
				dices_pos.defender[i].y = gdk_pixbuf_get_height(dices[0]) + 14;

				x += gdk_pixbuf_get_width(dices[0]);
			}
			dices_pos.attacker_text.x = (double)(gdk_pixbuf_get_width(dices[0])*3)/2;
			dices_pos.attacker_text.y = dices_pos.attacker[0].y - 10;
			dices_pos.defender_text.x = (double)(gdk_pixbuf_get_width(dices[0])*3)/2;
			dices_pos.defender_text.y = dices_pos.defender[0].y - 10;
		} else {
			/* extended features added */
			for(i=0; i<3; i++) {
				dices_pos.attacker[i].x = gui_theme.dices_ext_x[i];
				dices_pos.attacker[i].y = gui_theme.dices_ext_y[i];

				dices_pos.defender[i].x = gui_theme.dices_ext_x[i+3];
				dices_pos.defender[i].y = gui_theme.dices_ext_y[i+3];
			}

			dices_pos.attacker_text.x = gui_theme.dices_ext_text_x[0];
			dices_pos.attacker_text.y = gui_theme.dices_ext_text_y[0];
			dices_pos.defender_text.x = gui_theme.dices_ext_text_x[1];
			dices_pos.defender_text.y = gui_theme.dices_ext_text_y[1];
		}
	}

	dices_initialized=1;
}

void dices_unview()
{
	int i;

	for(i=0; i<DICES_CANT; i++) {
		if(images[i])
			g_object_set(images[i], "visibility",
			             GOO_CANVAS_ITEM_INVISIBLE, NULL);
	}

	for(i=0; i<2; i++) {
		if(text[i]) {
			goo_canvas_item_remove(text[i]);
			text[i] = NULL;
		}
	}
}

void dices_view()
{
	int i;

	if(!dices_initialized) {
		return;
	}

	if(g_game.dados_srccountry == -1 || g_game.dados_dstcountry == -1) {
		return;
	}


	/* hide all dices */
	for(i=0; i<DICES_CANT; i++) {
		if(images[i])
			g_object_set(images[i], "visibility",
			             GOO_CANVAS_ITEM_INVISIBLE, NULL);
	}

	for(i=0; i<3; i++) {
		if(g_game.dados_src[i] == 0) {
			break;
		}

		dices_show_image(g_game.dados_src[i]-1, DICES_ATTACKER, i);
	}
	dices_show_text(g_game.dados_srccountry, DICES_ATTACKER);


	for(i=0; i<3; i++) {
		if(g_game.dados_dst[i] == 0) {
			break;
		}
		dices_show_image(g_game.dados_dst[i]-1, DICES_DEFENDER, i);
	}
	dices_show_text(g_game.dados_dstcountry, DICES_DEFENDER);
}
