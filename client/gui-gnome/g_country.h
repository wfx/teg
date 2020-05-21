/*	$Id: g_country.h,v 1.2 2002/09/09 03:52:06 riq Exp $	*/
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
 * \file g_country.h
 */
#ifndef __TEG_GUI_GNOME_GCOUNTRY_H
#define __TEG_GUI_GNOME_GCOUNTRY_H

#include <goocanvas.h>

struct _G_countries {
	char *gfx_name;			/**< name del archivo grafico del country */
	gint x;				/**< x relativo al continente */
	gint y;				/**< idem anterior */
	gint x_len;			/**< valores updateados en real time. Dont fill	*/
	gint y_len;			/**<		"" 				*/
	gint x_center;			/**< si las fichas no estan centradas usar esto	*/
	gint y_center;			/**<              ""				*/
	GooCanvasItem *country_group;	/**< contenedor del country */
	GooCanvasItem *country_item;	/**< the figure of the map (png) */
	GooCanvasItem *ellip_item;	/**< circle of armies of the country */
	GooCanvasItem *text_item;	/**< the number of armies of the country */
}; 

extern struct _G_countries G_countries[];

void G_country_create( int country );
void G_country_draw( int country );
void G_country_draw_ejer(int country) ;
int G_country_tot();

/* gui */
TEG_STATUS gui_country_select(int country);

#endif /* __TEG_GUI_GNOME_GCOUNTRY_H */
