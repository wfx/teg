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
 * helpers functions to manage the colors in the GUI
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include "client.h"
#include "gui_state.h"

#include "colors.h"
#include "support.h"
#include "interface.h"
#include "fonts.h"
#include "themes.h"

namespace teg::client::callbacks
{

#define NR_COLORS (sizeof(gui_state().colors.table)/sizeof(gui_state().colors.table[0]))
#define NR_COLORS_COMMON (2)


TEG_STATUS colors_load_images(void)
{
	/** \todo transform those »load_from_file(filename)+g_free(filename)« into
	 * a own function */

	char name[512];

	if(gui_state().colors.images_loaded) {
		return TEG_STATUS_SUCCESS;
	}

	for(unsigned i=0; i < maximum_player_count ; i++) {

		memset(name, 0, sizeof(name));

		/* load player colors */
		/* IMPORTANT:  Dont translate color_name()! */
		if(! gui_state().theme.choose_colors_custom) {
			snprintf(name, sizeof(name) -1, "color_player_%s.png", color_name(i));
			gui_state().colors.player_pixbufs[i] = gdk_pixbuf_new_from_file(
			        load_pixmap_file(name).c_str(), NULL);
		} else {
			snprintf(name, sizeof(name) -1, "%s_%s.png", gui_state().theme.choose_colors_prefix, color_name(i));
			auto const fpath = theme_load_file(client_state(), name);
			char const*const filename = fpath ? fpath->c_str() : nullptr;
			gui_state().colors.player_pixbufs[i] = gdk_pixbuf_new_from_file(filename, NULL);
		}

		if(gui_state().colors.player_pixbufs[i] == NULL) {
			g_warning(_("Error, couldn't find file: %s"), name);
			return TEG_STATUS_ERROR;
		}

		/* load color circles */
		/* IMPORTANT:  Dont translate color_name()! */
		snprintf(name, sizeof(name) -1, "disc_%s.png", color_name(i));
		gui_state().colors.circle_pixbufs[i] = gdk_pixbuf_new_from_file(
		        load_pixmap_file(name).c_str(), NULL);

		if(gui_state().colors.circle_pixbufs[i] == NULL) {
			g_warning(_("Error, couldn't find file: %s"), name);
			return TEG_STATUS_ERROR;
		}
	}

	{
		/* disc for the 'no color' */
		gui_state().colors.circle_pixbufs[maximum_player_count]
		    = gdk_pixbuf_new_from_file(load_pixmap_file("disc_grey.png").c_str(), NULL);

		if(gui_state().colors.circle_pixbufs[maximum_player_count] == NULL) {
			g_warning(_("Error, couldn't find file: %s"), "disc_grey.png");
			return TEG_STATUS_ERROR;
		}
	}

	{
		/* disc over */
		gui_state().colors.circle_over_pixbuf
		    = gdk_pixbuf_new_from_file(load_pixmap_file("disc_over.png").c_str(), NULL);

		if(gui_state().colors.circle_over_pixbuf == NULL) {
			g_warning(_("Error, couldn't find file: %s"), "disc_over.png");
			return TEG_STATUS_ERROR;
		}
	}

	/* success */
	gui_state().colors.images_loaded = 1;

	return TEG_STATUS_SUCCESS;
}

/* allocate colors */
TEG_STATUS colors_allocate(void)
{
	if(gui_state().colors.allocated) {
		return TEG_STATUS_ERROR;
	}

	for(unsigned i=0; i<NR_COLORS; i++) {
		gdk_rgba_parse(&gui_state().colors.players_rgba[i], gui_state().colors.table[i].ellip_color);
	}

	for(unsigned i=0; i<NR_COLORS_COMMON; i++) {
		gdk_rgba_parse(&gui_state().colors.common_rgba[i], gui_state().colors.common_names[i]);
	}

	colors_load_images();

	gui_state().colors.allocated=1;

	return TEG_STATUS_SUCCESS;
}

bool color_is_valid(int color)
{
	return (color >= 0)
	       && (((unsigned)color) < NR_COLORS);
}

GdkRGBA* colors_get_player(int n)
{
	PCPLAYER pJ;

	if(player_whois(client_state(), n, &pJ) != TEG_STATUS_SUCCESS) {
		return &gui_state().colors.players_rgba[NR_COLORS-1];
	}

	if(!color_is_valid(pJ->color)) {
		return &gui_state().colors.players_rgba[NR_COLORS-1];
	}
	return &gui_state().colors.players_rgba[pJ->color];
}

GdkRGBA* colors_get_player_from_color(int color)
{
	if(!color_is_valid(color)) {
		return &gui_state().colors.players_rgba[NR_COLORS-1];
	}
	return &gui_state().colors.players_rgba[color];
}

GdkRGBA* colors_get_player_ink(int n)
{
	PCPLAYER pJ;

	if(player_whois(client_state(), n, &pJ) != TEG_STATUS_SUCCESS) {
		return &gui_state().colors.common_rgba[COLORS_BLACK];
	}

	if(!color_is_valid(pJ->color)) {
		return &gui_state().colors.common_rgba[COLORS_BLACK];
	}
	return &gui_state().colors.common_rgba[gui_state().colors.foreground[pJ->color]];
}

GdkRGBA* colors_get_player_ink_from_color(int color)
{
	if(!color_is_valid(color)) {
		return &gui_state().colors.common_rgba[COLORS_BLACK];
	}
	return &gui_state().colors.common_rgba[gui_state().colors.foreground[color]];
}

GdkRGBA* colors_get_player_virtual(int n)
{
	if(n == client_state().game.numjug) {
		return &gui_state().colors.players_rgba[COLORS_P_BLUE];
	}
	return &gui_state().colors.players_rgba[COLORS_P_RED];
}

GdkRGBA* colors_get_common(int n)
{
	if(n<0 || n>=NR_COLORS_COMMON) {
		return &gui_state().colors.players_rgba[NR_COLORS_COMMON-1];
	}
	return &gui_state().colors.common_rgba[n];
}

/* a non-reentrant funcion */
char * get_tag_for_color(int color)
{
	PCPLAYER pJ;

	strcpy((char*)gui_state().colors.tag_buffer, "default_tag");

	if(player_whois(client_state(), color, &pJ) != TEG_STATUS_SUCCESS) {
		return (char*)gui_state().colors.tag_buffer;
	}

	if(!color_is_valid(pJ->color)) {
		return (char*)gui_state().colors.tag_buffer;
	}

	snprintf(gui_state().colors.tag_buffer, sizeof(gui_state().colors.tag_buffer)-1, "%s_tag", color_name(pJ->color));
	return (char*)gui_state().colors.tag_buffer;
}


char const* get_foreground_for_color(int c)
{
	if(!color_is_valid(c)) {
		return gui_state().colors.table[NR_COLORS-1].text_color;
	}
	return gui_state().colors.table[c].text_color;
}

char const* get_background_for_color(int c)
{
	if(!color_is_valid(c)) {
		return gui_state().colors.table[NR_COLORS-1].ellip_color;
	}
	return gui_state().colors.table[c].ellip_color;
}

}
