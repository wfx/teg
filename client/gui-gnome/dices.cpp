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

#include "client.h"
#include "gui_state.h"

#include "dices.h"

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "priv.h"
#include "dices.h"
#include "themes.h"
#include "fonts.h"

namespace teg::client::callbacks
{

typedef enum dices_type {
	DICES_ATTACKER,
	DICES_DEFENDER
} dices_type_t;


/*
 * helper functions
 */
static TEG_STATUS dices_load_from_file(char *name, int i)
{
	auto const fpath = theme_load_file(client_state(), name);
	char const*const filename = fpath ? fpath->c_str() : nullptr;

	gui_state().dices.pixbufs[i] = gdk_pixbuf_new_from_file(filename, NULL);
	if(gui_state().dices.pixbufs[i] == NULL) {
		g_warning(_("Error, couldn't find file: %s"), name);
		return TEG_STATUS_ERROR;
	}

	return TEG_STATUS_SUCCESS;
}

static TEG_STATUS dices_load()
{
	int i;

	if(gui_state().dices.initialized) {
		return TEG_STATUS_SUCCESS;
	}

	for(i=0; i<static_cast<int>(sides_on_the_dice); i++) {
		char name[40];
		memset(name, 0, sizeof(name));
		snprintf(name, sizeof(name)-1, "dice-%d.png", i+1);
		if(!gui_state().dices.pixbufs[i]) {
			dices_load_from_file(name, i);
		}
	}

	return TEG_STATUS_SUCCESS;
}

static void dices_show_text(int country, dices_type_t type)
{
	if(gui_state().dices.texts[type]) {
		goo_canvas_item_remove(gui_state().dices.texts[type]);
		gui_state().dices.texts[type] = NULL;
	}

	gui_state().dices.texts[type] = goo_canvas_text_new(
	                                    gui_state().dices.group,
	                                    countries_get_name(client_state().game.common, country),
	                                    (double)(type == DICES_ATTACKER) ? gui_state().dices.positions.attacker_text.x :  gui_state().dices.positions.defender_text.x,
	                                    (double)(type == DICES_ATTACKER) ? gui_state().dices.positions.attacker_text.y :  gui_state().dices.positions.defender_text.y,
	                                    (double) -1,
	                                    GOO_CANVAS_ANCHOR_NORTH,
	                                    "height", (double) -1,
	                                    "font", HELVETICA_10_BFONT,
	                                    "fill-color", gui_state().theme.dices_color,
	                                    NULL);
}

static void dices_show_image(int dice_index, dices_type_t type, int pos)
{
	int i;
	double x, y;

	if(type == DICES_ATTACKER) {
		x = gui_state().dices.positions.attacker[pos].x;
		y = gui_state().dices.positions.attacker[pos].y;

		i = pos;
	} else {
		x = gui_state().dices.positions.defender[pos].x;
		y = gui_state().dices.positions.defender[pos].y;

		i = pos + 3;
	}

	if(gui_state().dices.items[i] == NULL) {
		gui_state().dices.items[i] = goo_canvas_image_new(
		                                 gui_state().dices.group,
		                                 gui_state().dices.pixbufs[dice_index],
		                                 x,
		                                 y,
		                                 "width", (double) gdk_pixbuf_get_width(gui_state().dices.pixbufs[dice_index]),
		                                 "height", (double) gdk_pixbuf_get_height(gui_state().dices.pixbufs[dice_index]),
		                                 NULL);

	} else {
		g_object_set(gui_state().dices.items[i],
		             "pixbuf", gui_state().dices.pixbufs[dice_index],
		             "x", x,
		             "y", y,
		             "width", (double) gdk_pixbuf_get_width(gui_state().dices.pixbufs[dice_index]),
		             "height", (double) gdk_pixbuf_get_height(gui_state().dices.pixbufs[dice_index]),
		             "visibility", GOO_CANVAS_ITEM_VISIBLE,
		             NULL);
	}
}

/*
 * exported functions
 */
void dices_init(GooCanvasItem *root)
{
	if(gui_state().dices.initialized) {
		return;
	}

	if(!root) {
		return;
	}

	gui_state().dices.group =
	    goo_canvas_group_new(
	        root,
	        "x", (float) gui_state().theme.dices_x,
	        "y", (float) gui_state().theme.dices_y,
	        NULL
	    );

	if(!gui_state().dices.group) {
		return;
	}

	dices_load();


	/* load dices position */
	{
		int i;
		if(! theme_using_extended_dices(client_state())) {
			/* basic behaviour */
			double x = 0;
			for(i=0; i<3; i++) {
				gui_state().dices.positions.attacker[i].x = x;
				gui_state().dices.positions.attacker[i].y = 4;
				gui_state().dices.positions.defender[i].x = x;
				gui_state().dices.positions.defender[i].y = gdk_pixbuf_get_height(gui_state().dices.pixbufs[0]) + 14;

				x += gdk_pixbuf_get_width(gui_state().dices.pixbufs[0]);
			}
			gui_state().dices.positions.attacker_text.x = (double)(gdk_pixbuf_get_width(gui_state().dices.pixbufs[0])*3)/2;
			gui_state().dices.positions.attacker_text.y = gui_state().dices.positions.attacker[0].y - 10;
			gui_state().dices.positions.defender_text.x = (double)(gdk_pixbuf_get_width(gui_state().dices.pixbufs[0])*3)/2;
			gui_state().dices.positions.defender_text.y = gui_state().dices.positions.defender[0].y - 10;
		} else {
			/* extended features added */
			for(i=0; i<3; i++) {
				gui_state().dices.positions.attacker[i].x = gui_state().theme.dices_ext_x[i];
				gui_state().dices.positions.attacker[i].y = gui_state().theme.dices_ext_y[i];

				gui_state().dices.positions.defender[i].x = gui_state().theme.dices_ext_x[i+3];
				gui_state().dices.positions.defender[i].y = gui_state().theme.dices_ext_y[i+3];
			}

			gui_state().dices.positions.attacker_text.x = gui_state().theme.dices_ext_text_x[0];
			gui_state().dices.positions.attacker_text.y = gui_state().theme.dices_ext_text_y[0];
			gui_state().dices.positions.defender_text.x = gui_state().theme.dices_ext_text_x[1];
			gui_state().dices.positions.defender_text.y = gui_state().theme.dices_ext_text_y[1];
		}
	}

	gui_state().dices.initialized=1;
}

void dices_unview()
{
	int i;

	for(i=0; i<static_cast<int>(sides_on_the_dice); i++) {
		if(gui_state().dices.items[i])
			g_object_set(gui_state().dices.items[i], "visibility",
			             GOO_CANVAS_ITEM_INVISIBLE, NULL);
	}

	for(i=0; i<2; i++) {
		if(gui_state().dices.texts[i]) {
			goo_canvas_item_remove(gui_state().dices.texts[i]);
			gui_state().dices.texts[i] = NULL;
		}
	}
}

void dices_view()
{
	int i;

	if(!gui_state().dices.initialized) {
		return;
	}

	if(client_state().game.dados_srccountry == -1 || client_state().game.dados_dstcountry == -1) {
		return;
	}

	/* hide all dices */
	for(i=0; i<static_cast<int>(sides_on_the_dice); i++) {
		if(gui_state().dices.items[i])
			g_object_set(gui_state().dices.items[i], "visibility",
			             GOO_CANVAS_ITEM_INVISIBLE, NULL);
	}

	for(i=0; i<3; i++) {
		if(client_state().game.dados_src[i] == 0) {
			break;
		}

		dices_show_image(client_state().game.dados_src[i]-1, DICES_ATTACKER, i);
	}
	dices_show_text(client_state().game.dados_srccountry, DICES_ATTACKER);


	for(i=0; i<3; i++) {
		if(client_state().game.dados_dst[i] == 0) {
			break;
		}
		dices_show_image(client_state().game.dados_dst[i]-1, DICES_DEFENDER, i);
	}
	dices_show_text(client_state().game.dados_dstcountry, DICES_DEFENDER);
}

}
