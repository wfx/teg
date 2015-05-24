/*	$Id: g_country.c,v 1.3 2003/08/30 04:34:39 riq Exp $	*/
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
 * @file g_country.c
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gnome.h>

#include "gui.h"
#include "client.h"
#include "interface.h"
#include "g_country.h"
#include "g_cont.h"
#include "support.h"
#include "armies.h"
#include "colors.h"
#include "themes.h"
#include "fonts.h"
#include "priv.h"
#include "status.h"

#undef GENERATE_LIMITROFE
#undef DRAG_DROP_COUNTRIES

struct _G_countries G_countries[COUNTRIES_CANT];

static int initialized = 0;

static gint
item_event (GnomeCanvasItem *item, GdkEvent *event, gpointer data)
{
	static double x, y;
	double new_x, new_y;
#ifdef DRAG_DROP_COUNTRIES
	GdkCursor *fleur;
#endif /* DRAG_DROP_COUNTRIES */
	static int dragging;
	double item_x, item_y;
	PCOUNTRY country = (PCOUNTRY) data;

#ifdef GENERATE_LIMITROFE
	static int borrame=0;
	static int buf[50];
	static FILE *fp=NULL;
#endif /* GENERATE_LIMITROFE */

	/* set item_[xy] to the event x,y position in the parent's item-relative coordinates */
	item_x = event->button.x;
	item_y = event->button.y;
	gnome_canvas_item_w2i (item->parent, &item_x, &item_y);

	switch (event->type) {
	case GDK_ENTER_NOTIFY:
		{
			char buff[250];
			if (country->ejercitos > 0)
				snprintf(buff,sizeof(buff)-1,_("%s armies: %d"), countries_get_name(country->id),country->ejercitos);
			else
				snprintf(buff,sizeof(buff)-1, "%s", countries_get_name(country->id));

			gnome_appbar_set_status( GNOME_APPBAR(statusbar),  buff);

			gui_private.country_i_am = country->id;
			mainstatus_update();

			switch( ESTADO_GET() ) {
			case PLAYER_STATUS_FICHAS:
			case PLAYER_STATUS_FICHAS2:
			case PLAYER_STATUS_FICHASC:
				fichas_enter( country );
				break;
			case PLAYER_STATUS_ATAQUE:
			case PLAYER_STATUS_TROPAS:
				attack_enter( country );
				break;
			case PLAYER_STATUS_REAGRUPE:
				reagrupe_enter( country );
				break;
			default:
				break;
			}
		}
		break;
	case GDK_LEAVE_NOTIFY:
		gnome_appbar_set_status( GNOME_APPBAR(statusbar),"" );

		gui_private.country_i_am = -1;
		mainstatus_update();

		switch( ESTADO_GET() ) {
		case PLAYER_STATUS_FICHAS:
		case PLAYER_STATUS_FICHAS2:
		case PLAYER_STATUS_FICHASC:
			fichas_leave( country );
			break;
		case PLAYER_STATUS_ATAQUE:
		case PLAYER_STATUS_TROPAS:
			attack_leave( country );
			break;
		case PLAYER_STATUS_REAGRUPE:
			reagrupe_leave( country );
			break;
		default:
			break;
		}
		break;
	case GDK_BUTTON_PRESS:
		switch (event->button.button) {
		case 1:
			switch( ESTADO_GET() ) {
#ifdef GENERATE_LIMITROFE
			case PLAYER_STATUS_DESCONECTADO:
				if( country->id < borrame ) {
					textmsg(M_DBG,"not added :%s", countries_get_name(country->id) );
					break;
				}
				buf[ country->id ] = 1;
				textmsg(M_DBG,"added :%s",countries_get_name(country->id) );
				break;
#endif /* GENERATE_LIMITROFE */
			case PLAYER_STATUS_FICHAS:
			case PLAYER_STATUS_FICHAS2:
			case PLAYER_STATUS_FICHASC:
				if( fichas_add( country ) == TEG_STATUS_SUCCESS ) {
					G_country_draw_ejer(country->id);
					armies_add( country->id );
				}; 
				break;
			case PLAYER_STATUS_ATAQUE:
			case PLAYER_STATUS_TROPAS:
				attack_click( country );
				break;
			case PLAYER_STATUS_REAGRUPE:
				reagrupe_click( country );
				break;
			default:
				break;
			}

			/* XXX: Remove this block when the game is finished */
#ifdef DRAG_DROP_COUNTRIES
			{
				if (event->button.state & GDK_SHIFT_MASK)
					gtk_object_destroy (GTK_OBJECT (item));
				else {
					x = item_x;
					y = item_y;

					fleur = gdk_cursor_new (GDK_FLEUR);
					gnome_canvas_item_grab (item,
								GDK_POINTER_MOTION_MASK | GDK_BUTTON_RELEASE_MASK,
								fleur,
								event->button.time);
					gdk_cursor_destroy (fleur);
					dragging = TRUE;
				}
			}
#endif	/* DRAG_DROP_COUNTRIES */
			break;

		case 2:
			
#ifdef GENERATE_LIMITROFE
			{
				int i;
				for(i=0;i<50;i++)
					buf[i]=0;
				borrame=0;

				if( fp ) fclose( fp );
				fp = fopen( "test.h", "w+" );
				textmsg(M_DBG,"Processing %s", countries_get_name(borrame) );
				break;
			}
#endif /* GENERATE_LIMITROFE */
#ifdef DRAG_DROP_COUNTRIES 
			{
			int xx,yy;
			int cont = country->continente;
			xx= G_conts[cont].x ;
			yy= G_conts[cont].y ;
			
			printf("%s :%f %f\n",countries_get_name(country->id),item->x1-xx,item->y1-yy);

			if (event->button.state & GDK_SHIFT_MASK)
				gnome_canvas_item_lower_to_bottom (item);
			else
				gnome_canvas_item_lower (item, 1);
			break;
			}
#endif /* DRAG_DROP_COUNTRIES */
			break;
		case 3:
			switch( ESTADO_GET() ) {
#ifdef GENERATE_LIMITROFE
			case PLAYER_STATUS_DESCONECTADO:
				{
					int i;

					if( !fp ) break;
					buf[borrame] = 2;

					fprintf(fp,"{");
					for(i=0;i<50;i++) {
						fprintf( fp,"%d,",buf[i] );
					}
					fprintf(fp,"} /* %s (%d) */\n", countries_get_name(borrame),g_countries[borrame].id);

					borrame++;

					for(i=0;i<50;i++)
						buf[i]=0;

					if( borrame == 50 )
						fclose( fp );
					textmsg(M_DBG,"Processing %s", countries_get_name(borrame) );
					break;
				}
#endif /* GENERATE_LIMITROFE */
			case PLAYER_STATUS_FICHAS:
			case PLAYER_STATUS_FICHAS2:
			case PLAYER_STATUS_FICHASC:
				if( fichas_sub( country ) == TEG_STATUS_SUCCESS ) {
					G_country_draw_ejer(country->id);
					armies_del( country->id );
				}
				break;
			default:
				break;
			}

#ifdef DRAG_DROP_COUNTRIES
			if (event->button.state & GDK_SHIFT_MASK)
				gnome_canvas_item_raise_to_top (item);
			else
				gnome_canvas_item_raise (item, 1);
			break;
#endif /* DRAG_DROP_COUNTRIES */

		default:
			break;
		}

		break;

	case GDK_MOTION_NOTIFY:
		if (dragging && (event->motion.state & GDK_BUTTON1_MASK)) {
			new_x = item_x;
			new_y = item_y;

			gnome_canvas_item_move (item, new_x - x, new_y - y);
			x = new_x;
			y = new_y;
		}
		break;

	case GDK_BUTTON_RELEASE:
		gnome_canvas_item_ungrab (item, event->button.time);
		dragging = FALSE;
		break;

	default:
		break;
	}

	return FALSE;
}

/**
 * @fn void G_country_init()
 * Initialize the countrys from the theme
 */
void G_country_init()
{
	int i;
	TCountry country;

	if( initialized )
		return;

	for( i=0; i < COUNTRIES_CANT ; i++ ) {

		/* find continent, find country number */
		int c = g_countries[i].continente;
		int sum,j,p;

		sum=0;
		for(j=0;j<c;j++)
			sum += g_conts[j].cant_countries;

		p = i - sum;

		if( theme_giveme_country(&country,c,p) == TEG_STATUS_SUCCESS ) {
			G_countries[i].x = country.pos_x;
			G_countries[i].y = country.pos_y;
			G_countries[i].x_center = country.army_x;
			G_countries[i].y_center = country.army_y;
			G_countries[i].gfx_name = country.file;
		}
	}

	initialized = 1;
}


/**
 * @fn void G_country_create( int country )
 * Crea el country country
 * @param country numero de country a crear
 */
void G_country_create( int country )
{
	if (!initialized)
		G_country_init();

	G_countries[country].country_group = GNOME_CANVAS_GROUP(
			gnome_canvas_item_new (
				G_conts[g_countries[country].continente].cont_group,
				gnome_canvas_group_get_type (),
				"x", (float) G_countries[country].x,
				"y", (float) G_countries[country].y,
//				"anchor", GTK_ANCHOR_NW,
				NULL));
	if( G_countries[country].country_group != NULL )
		gtk_signal_connect (GTK_OBJECT (G_countries[country].country_group), "event",
				    (GtkSignalFunc) item_event,
				    &g_countries[country]);
}

/* Draw the country 'country' */
void G_country_draw( int country )
{
	GdkPixbuf *im;
	char name[200];
	char *filename=NULL;

	snprintf( name,sizeof(name)-1,"%s",G_countries[country].gfx_name);
	name[sizeof(name)-1]=0;

	filename = theme_load_file( name );

	im = gdk_pixbuf_new_from_file(filename, NULL);

	if( im == NULL ) {

		textmsg(M_ERR,_("Error, couldn't find file: %s"),G_countries[country].gfx_name);
		g_warning ("Could not find country %s file",G_countries[country].gfx_name);

	} else {

		G_countries[country].x_len = gdk_pixbuf_get_width( im );
		G_countries[country].y_len = gdk_pixbuf_get_height( im );

		G_countries[country].country_item = gnome_canvas_item_new(
				G_countries[country].country_group,
				gnome_canvas_pixbuf_get_type (),
				"pixbuf", im,
				"x", 0.0,
				"y", 0.0,
				"width", (double) gdk_pixbuf_get_width( im ),
				"height", (double) gdk_pixbuf_get_height( im ),
				"anchor", GTK_ANCHOR_NW,
				NULL);
				
		gdk_pixbuf_unref(im);
	};
}

/* draw the armies of a given country */
void G_country_draw_ejer(int country) 
{
	PCPLAYER j;
	double size, size_tmp;
	int color;
	GnomeCanvasItem *e;
	int x,y;
	char size_string[10];

	color = 6;
	if( player_whois( g_countries[country].numjug, &j) == TEG_STATUS_SUCCESS) {
		if( j->color >=0 && j->color < TEG_MAX_PLAYERS )
			color = j->color;
	} else
		color = TEG_MAX_PLAYERS;

	switch( g_countries[country].ejercitos) {
		case 0: size = 4; size_tmp = 0;break;
		case 1: size = 6; size_tmp = 0;break;
		case -1:
		case 2: size = 6; size_tmp = 1;break;
		case 3: size = 7; size_tmp = 0;break;
		case 4: size = 7; size_tmp = 1;break;
		case 5: size = 8; size_tmp = 0;break;
		case 6: size = 8; size_tmp = 1;break;
		case 7: size = 9; size_tmp = 0;break;
		case 8: size = 9; size_tmp = 1;break;
		case 9: size = 10; size_tmp = 0;break;
		case 10: size = 10; size_tmp = 1;break;
		default: size = 11; size_tmp = 0;break;
	}


	if( G_countries[country].ellip_item != NULL)
		gtk_object_destroy(GTK_OBJECT ( G_countries[country].ellip_item ) );

	if( G_countries[country].text_item != NULL)
		gtk_object_destroy(GTK_OBJECT ( G_countries[country].text_item ) );

	if( G_countries[country].country_group == NULL)
		g_warning("G_country_draw_ejer()");

	/* crear elipse */
	x = G_countries[country].x_len + G_countries[country].x_center;
	y = G_countries[country].y_len + G_countries[country].y_center;

	e = gnome_canvas_item_new(
		G_countries[country].country_group,
		gnome_canvas_ellipse_get_type(),
		"x1", (double) x/2-size-size_tmp,
		"y1", (double) y/2-size,
		"x2", (double) x/2+size+size_tmp,
		"y2", (double) y/2+size,
		"fill_color", G_colores[color].ellip_color,
		"outline_color", "black",
		"width_units", (double) 1,
		NULL);

	/* 'e' puede ser NULL, pero si es NULL me interesa que ellip lo sea tambien */
	G_countries[country].ellip_item = e;

	gui_country_select( g_countries[country].id );

	/* crear numero */
	if( g_countries[country].ejercitos ) {
		if( g_countries[country].ejercitos == -1 )
			sprintf(size_string,"%s", "?");
		else
			sprintf(size_string,"%d", g_countries[country].ejercitos);
		e = gnome_canvas_item_new(
			G_countries[country].country_group,
			gnome_canvas_text_get_type(),
			"text", size_string,
			"x", (double) x/2,
			"y", (double) y/2,
			"x_offset", (double) -1,
			"y_offset", (double) -1,
			"font", HELVETICA_10_FONT,
			"fill_color", G_colores[color].text_color,
			"anchor",GTK_ANCHOR_CENTER,
			NULL);

	} else {
		e  = NULL;
	}
	G_countries[country].text_item = e;
}

/*
 * Funciones GUI
 */
/**
 * @fn TEG_STATUS gui_country_select(int country) 
 * selecciona un country
 * @param country country a seleccionar
 */
TEG_STATUS gui_country_select( int country ) 
{
	if( country < 0 || country >= COUNTRIES_CANT )
		return TEG_STATUS_ERROR;

	if( g_countries[country].selected == COUNTRY_SELECT_NONE ) {
		gnome_canvas_item_set(
			G_countries[country].ellip_item, 
			"outline_color", "black",
			"width_units", (double) 1,
			NULL);
	}
	if( g_countries[country].selected & COUNTRY_SELECT_FICHAS_IN ) {
		gnome_canvas_item_set(
			G_countries[country].ellip_item, 
			"outline_color", "white",
			"width_units", (double) 2,
			NULL);
	}
	if( g_countries[country].selected & COUNTRY_SELECT_FICHAS_OUT ) {
		gnome_canvas_item_set(
			G_countries[country].ellip_item, 
			"outline_color", "DarkOrange1",
			"width_units", (double) 2,
			NULL);
	}
	if( g_countries[country].selected & COUNTRY_SELECT_ATTACK_ENTER ) {
		gnome_canvas_item_set(
			G_countries[country].ellip_item, 
			"outline_color", "white",
			"width_units", (double) 2,
			NULL);
	}
#if 0
	if( g_countries[country].selected & COUNTRY_SELECT_ATTACK ) {
		gnome_canvas_item_set(
			G_countries[country].ellip_item, 
			"outline_color", "red",
			"width_units", (double) 3,
			NULL);
	}
#endif
	if( g_countries[country].selected & COUNTRY_SELECT_REGROUP_ENTER ) {
		gnome_canvas_item_set(
			G_countries[country].ellip_item, 
			"outline_color", "white",
			"width_units", (double) 2,
			NULL);
	}
#if 0
	if( g_countries[country].selected & COUNTRY_SELECT_REGROUP ) {
		gnome_canvas_item_set(
			G_countries[country].ellip_item, 
			"outline_color", "blue",
			"width_units", (double) 2,
			NULL);
	}
#endif

	if( g_countries[country].selected & COUNTRY_SELECT_ATTACK_SRC ) {
		gnome_canvas_item_set(
			G_countries[country].ellip_item, 
			"outline_color", "white",
			"width_units", (double) 2,
			NULL);
	}

	if( g_countries[country].selected & COUNTRY_SELECT_ATTACK_DST ) {
		gnome_canvas_item_set(
			G_countries[country].ellip_item, 
			"outline_color", "white",
			"width_units", (double) 2,
			NULL);
	}


	if (GNOME_CANVAS(canvas_map)->idle_id == 0)
		gnome_canvas_update_now( GNOME_CANVAS(canvas_map) );

	return TEG_STATUS_SUCCESS;
}

/**
 * @fn TEG_STATUS g_country_init()
 * Initialize the countries
 */
TEG_STATUS g_country_init()
{
	return TEG_STATUS_SUCCESS;
}
