/*	$Id: colors.c,v 1.17 2007/09/15 15:29:30 nordi Exp $	*/
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
#include <gnome.h>

#include "gui.h"
#include "client.h"

#include "colors.h"
#include "support.h"
#include "interface.h"
#include "fonts.h"
#include "themes.h"


extern TTheme gui_theme;

GdkGC *g_colors_gc = NULL;
GdkFont* g_pixmap_font10 = NULL;

static int allocated=0;

struct _G_colores G_colores[] = {
	{"red", "white"},
	{"yellow", "black"},
	{"blue", "white"},
	{"grey27", "white"},
	{"PaleVioletRed1", "black"},
	{"green", "black"},
	{"grey", "black"},		/* Color no usado por playeres */
};
#define NR_COLORS (sizeof(G_colores)/sizeof(G_colores[0]))
GdkColor colors_players[NR_COLORS];

char *G_colores_common[] = {
	"white", "black"
};
#define NR_COLORS_COMMON (2)
GdkColor colors_common[NR_COLORS_COMMON];

int colors_foreground[] = { 0,1,0,0,1,1,1 };

GdkPixbuf *g_color_players[TEG_MAX_PLAYERS];
GdkPixbuf *g_color_circles[TEG_MAX_PLAYERS + 1];
GdkPixbuf *g_color_circle_over;


TEG_STATUS colors_load_images( void )
{
	static int images_loaded = 0;
	char name[512];
	char *filename=NULL;

	int i;


	if( images_loaded )
		return TEG_STATUS_SUCCESS;

	for(i=0; i < TEG_MAX_PLAYERS ; i++ ) {

		memset(name, 0, sizeof(name) );

		/* load player colors */
		/* IMPORTANT:  Dont translate g_colores! */
		if( ! gui_theme.choose_colors_custom ) {
			snprintf( name, sizeof(name) -1, "color_player_%s.png", g_colores[i] );
			filename = load_pixmap_file (name);
			g_color_players[i] = gdk_pixbuf_new_from_file(filename, NULL);
			if(filename)
				g_free( filename );
		} else {
			snprintf( name, sizeof(name) -1, "%s_%s.png", gui_theme.choose_colors_prefix, g_colores[i] );
			filename = theme_load_file( name );
			g_color_players[i] = gdk_pixbuf_new_from_file(filename, NULL);
		}

		if( g_color_players[i] == NULL ) {
			g_warning(_("Error, couldn't find file: %s"),name);
			return TEG_STATUS_ERROR;
		}

		/* load color circles */
		/* IMPORTANT:  Dont translate g_colores! */
		snprintf( name, sizeof(name) -1, "disc_%s.png", g_colores[i] );
		filename = load_pixmap_file (name);
		g_color_circles[i] = gdk_pixbuf_new_from_file(filename, NULL);
		if(filename)
			g_free( filename );

		if( g_color_circles[i] == NULL ) {
			g_warning(_("Error, couldn't find file: %s"),name);
			return TEG_STATUS_ERROR;
		}
	}

	/* disc for the 'no color' */
	filename = load_pixmap_file ("disc_grey.png");
	g_color_circles[TEG_MAX_PLAYERS] = gdk_pixbuf_new_from_file( filename, NULL );
	if(filename)
		g_free( filename );

	if( g_color_circles[TEG_MAX_PLAYERS] == NULL ) {
		g_warning(_("Error, couldn't find file: %s"),"disc_grey.png");
		return TEG_STATUS_ERROR;
	}

	/* disc over */
	filename = load_pixmap_file("disc_over.png");
	g_color_circle_over = gdk_pixbuf_new_from_file( filename, NULL );
	if(filename)
		g_free( filename );

	if( g_color_circle_over == NULL ) {
		g_warning(_("Error, couldn't find file: %s"),"disc_over.png");
		return TEG_STATUS_ERROR;
	}

	/* success */
	images_loaded = 1;

	return TEG_STATUS_SUCCESS;
}

/* allocate colors */
TEG_STATUS colors_allocate( void )
{
	GdkColormap *colormap;
	int i;

	if(allocated)
		return TEG_STATUS_ERROR;

    	colormap = gtk_widget_get_colormap(main_window);

	for(i=0;i<NR_COLORS;i++) {
		gdk_color_parse( G_colores[i].ellip_color, &colors_players[i]);
		gdk_colormap_alloc_color( colormap, &colors_players[i], FALSE, TRUE);
	}

	for(i=0;i<NR_COLORS_COMMON;i++) {
		gdk_color_parse( G_colores_common[i], &colors_common[i]);
		gdk_colormap_alloc_color( colormap, &colors_common[i], FALSE, TRUE);
	}

	g_colors_gc = gdk_gc_new(main_window->window);
	g_pixmap_font10 = gdk_font_load (HELVETICA_10_FONT_OLD);

	colors_load_images();

	allocated=1;

	return TEG_STATUS_SUCCESS;
}

/* free color's resources */
TEG_STATUS colors_free()
{
	GdkColormap *colormap;
	int i;

	if(allocated)
		return TEG_STATUS_ERROR;

    	colormap = gtk_widget_get_colormap(main_window);

	gdk_colormap_free_colors(colormap,(GdkColor*)&colors_players,NR_COLORS);
	gdk_colormap_free_colors(colormap,(GdkColor*)&colors_common,COLORS_LAST);

	for(i=0; i < G_N_ELEMENTS(g_color_circles); i++ ) {
		if( g_color_players[i] ) {
			gdk_pixbuf_unref( g_color_players[i] );
			g_color_players[i] = NULL;
		}

		if( g_color_circles[i] ) {
			gdk_pixbuf_unref( g_color_circles[i] );
			g_color_circles[i] = NULL;
		}
	}

	if( g_color_circle_over ) {
		gdk_pixbuf_unref( g_color_circle_over );
		g_color_circle_over = NULL;
	}


	if( g_colors_gc ) {
		gdk_gc_unref( g_colors_gc );
		g_colors_gc = NULL;
	}

	if( g_pixmap_font10 ) {
		gdk_font_unref( g_pixmap_font10 );
		g_pixmap_font10 = NULL;
	}

	allocated=0;

	return TEG_STATUS_SUCCESS;
}

GdkColor* colors_get_player( int n )
{
	PCPLAYER pJ;

	if(player_whois(n,&pJ) != TEG_STATUS_SUCCESS)
		return &colors_players[NR_COLORS-1];

	if(pJ->color<0 || pJ->color>=NR_COLORS)
		return &colors_players[NR_COLORS-1];
	return &colors_players[pJ->color];
}

GdkColor* colors_get_player_from_color( int color )
{
	if(color<0 || color>=NR_COLORS)
		return &colors_players[NR_COLORS-1];
	return &colors_players[color];
}

GdkColor* colors_get_player_ink(int n )
{
	PCPLAYER pJ;

	if(player_whois(n,&pJ) != TEG_STATUS_SUCCESS)
		return &colors_common[COLORS_BLACK];

	if(pJ->color<0 || pJ->color>=NR_COLORS)
		return &colors_common[COLORS_BLACK];
	return &colors_common[colors_foreground[pJ->color]];
}

GdkColor* colors_get_player_ink_from_color(int color )
{
	if(color<0 || color>=NR_COLORS)
		return &colors_common[COLORS_BLACK];
	return &colors_common[colors_foreground[color]];
}

GdkColor* colors_get_player_virtual( int n )
{
	if( n == g_game.numjug )
		return &colors_players[COLORS_P_BLUE];
	return &colors_players[COLORS_P_RED];
}

GdkColor* colors_get_common( int n )
{
	if(n<0 || n>=NR_COLORS_COMMON)
		return &colors_players[NR_COLORS_COMMON-1];
	return &colors_common[n];
}

/* a non-reentrant funcion */
char * get_tag_for_color( int color )
{
	static char buffer[256];
	PCPLAYER pJ;

	strcpy((char*)buffer,"default_tag");

	if(player_whois(color,&pJ) != TEG_STATUS_SUCCESS) {
		return (char*)buffer;
	}

	if(pJ->color<0 || pJ->color>=NR_COLORS)
		return (char*)buffer;

	snprintf(buffer,sizeof(buffer)-1,"%s_tag",g_colores[pJ->color]);
	return (char*)buffer;
}


char * get_foreground_for_color( int c )
{
	if(c<0 || c>=NR_COLORS)
		return G_colores[NR_COLORS-1].text_color;
	return G_colores[c].text_color;
}

char * get_background_for_color( int c )
{
	if(c<0 || c>=NR_COLORS)
		return G_colores[NR_COLORS-1].ellip_color;
	return G_colores[c].ellip_color;
}
