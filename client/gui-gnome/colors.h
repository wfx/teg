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
 * @file colors.h
 */

#ifndef __TEG_GUI_GNOME_COLORS_H
#define __TEG_GUI_GNOME_COLORS_H

enum {
	COLORS_WHITE,
	COLORS_BLACK,

	COLORS_LAST
};

enum {
	COLORS_P_RED,
	COLORS_P_YELLOW,
	COLORS_P_BLUE,
	COLORS_P_BLACK,
	COLORS_P_PINK,
	COLORS_P_GREEN,

	COLORS_P_LAST
};

struct _G_colores {
	char *ellip_color;
	char *text_color;
};
extern struct _G_colores G_colores[];
extern GdkPixbuf	*g_color_players[];
extern GdkPixbuf	*g_color_circles[];
extern GdkPixbuf	*g_color_circle_over;

TEG_STATUS colors_allocate(void);
TEG_STATUS colors_load_images(void);
GdkRGBA* colors_get_player(int n);
GdkRGBA* colors_get_player_from_color(int color);
GdkRGBA* colors_get_player_ink(int n);
GdkRGBA* colors_get_player_ink_from_color(int color);
GdkRGBA* colors_get_player_virtual(int n);
GdkRGBA* colors_get_common(int n);
char * get_tag_for_color(int color);
char * get_background_for_color(int c);
char * get_foreground_for_color(int c);

#endif /* __TEG_GUI_GNOME_COLORS_H */
