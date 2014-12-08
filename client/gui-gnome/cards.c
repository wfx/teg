/*	$Id: cards.c,v 1.38 2002/08/31 17:45:58 riq Exp $	*/
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
 * @file cards.c
 * Crea las tarjetas que fueron obtenidas
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include <assert.h>

#include "gui.h"
#include "client.h"

#include "chatline.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "priv.h"
#include "g_country.h"
#include "cards.h"
#include "themes.h"
#include "fonts.h"
#include "locate_country.h"

static GtkWidget 	*cards_dialog=NULL;

struct _tarjs_sensi {
	GtkWidget	*card;
	GtkWidget	*button_armies;
	GtkWidget	*button_select;
	PCOUNTRY		country;
	BOOLEAN		selected;
} tarjs_sensi[TEG_MAX_TARJETAS];

struct {
	GdkPixbuf	*tar;
	TARJTIPO	tipo;
	char 		*filename;
} tarjs[] = {
	{ NULL,	TARJ_CANION,	"tar_canon.png" },
	{ NULL,	TARJ_GALEON,	"tar_galeon.png" },
	{ NULL,	TARJ_GLOBO,	"tar_globo.png" },
	{ NULL,	TARJ_COMODIN,	"tar_comodin.png" },
};
#define NRTARJS	( sizeof(tarjs) / sizeof(tarjs[0]))

/**
 * @fn TEG_STATUS cards_load()
 * Carga las imagenes de las tarjetas en variables globlales
 */
TEG_STATUS cards_load()
{
	char *filename;
	int i;

	TCards cards;
	/* obtain cards from theme */
	if( theme_giveme_cards(&cards) == TEG_STATUS_SUCCESS ) {
		if(cards.cannon)
			tarjs[0].filename = cards.cannon;
		if(cards.ship)
			tarjs[1].filename = cards.ship;
		if(cards.balloon)
			tarjs[2].filename = cards.balloon;
		if(cards.jocker)
			tarjs[3].filename = cards.jocker;
	}


	for(i=0; i<NRTARJS;i++) {

		if(!tarjs[i].tar) {
			filename = theme_load_file( tarjs[i].filename );
			tarjs[i].tar = gdk_pixbuf_new_from_file(filename, NULL);
			if( tarjs[i].tar == NULL )
				g_warning("Error, couldn't find file:%s", tarjs[i].filename);
		}
	}

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS cards_init()
{
	int i;

	for(i=0;i<TEG_MAX_TARJETAS;i++) {
		memset( &tarjs_sensi[i], 0, sizeof(tarjs_sensi[i]));
	}
	return TEG_STATUS_SUCCESS;
}

/* Callbacks de las funciones de los botones y funciones auxiliares */
static int cuantos_selected( int countries[TEG_MAX_TARJETAS])
{
	int i,j=0;
	int selected=0;

	for(i=0;i<TEG_MAX_TARJETAS;i++) {
		if( tarjs_sensi[i].country != NULL ) {
			if( tarjs_sensi[i].selected ) {
				countries[j++] = tarjs_sensi[i].country->id;
				selected++;
			}
		}
	}
	return selected;
}

static void cards_cb_button_canje (GtkWidget *widget )
{
	int sel;
	int countries[TEG_MAX_TARJETAS];

	assert(widget);

	sel = cuantos_selected( countries );
	if( sel != 3 ) {
		textmsg(M_ERR,"Error, you must select 3 cards and not %d.",sel);
		return;
	}
	canje_out( countries[0], countries[1], countries[2] );
}

static void cards_cb_button_armies (GtkWidget *widget, int index )
{
	if( tarjs_sensi[ index ].country == NULL ) {
		textmsg(M_ERR,"Error in cards_cb_button_armies");
	} else {
		if( ejer2_out( tarjs_sensi[ index ].country->id ) == TEG_STATUS_SUCCESS )
			cards_update();
	}
}

static void cards_cb_button_select (GtkWidget *widget, int index )
{
	if( tarjs_sensi[ index ].country == NULL ) {
		textmsg(M_ERR,"Error in cards_cb_button_selected");
	} else {
		tarjs_sensi[ index ].selected = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(widget) );
	}
}

static void cards_cb_button_locate (GtkWidget *widget, int index )
{
	if( tarjs_sensi[ index ].country == NULL ) {
		textmsg(M_ERR,"Error in cards_cb_button_selected");
	} else {
		locate_country_add_country( tarjs_sensi[ index ].country );
	}
}

/* funciones de creacion */
static GtkWidget *cards_create( PTARJETA pT, int tarjs_index )
{
	GtkWidget	*vbox;
//	GtkWidget	*hbox;
	GtkWidget	*canvas;
	GtkWidget	*button_armies;
	GtkWidget	*button_select;
	GtkWidget	*button_locate;
	PCOUNTRY		pP;
	GnomeCanvasItem *image;
	int i;
	TCards cards;

	if( theme_giveme_cards(&cards) != TEG_STATUS_SUCCESS )
		return NULL;


	for(i=0;i<NRTARJS;i++) {
		if( tarjs[i].tipo == pT->tarjeta )
			break;
	}

	pP = (PCOUNTRY) COUNTRY_FROM_TARJETA( pT );

	vbox = gtk_vbox_new (FALSE, 0);
	if( !vbox ) return NULL;

	/* create picture of card (canvas with labels) */
	canvas = gnome_canvas_new();
	if( !canvas ) {
		textmsg( M_ERR, _("Error creating canvas\n"));
		return NULL;
	}

	gtk_widget_set_usize (canvas, gdk_pixbuf_get_width(tarjs[i].tar), gdk_pixbuf_get_height(tarjs[i].tar) );
	gnome_canvas_set_scroll_region (GNOME_CANVAS (canvas), 0, 0, gdk_pixbuf_get_width(tarjs[i].tar), gdk_pixbuf_get_height(tarjs[i].tar) );
	image = gnome_canvas_item_new(
		gnome_canvas_root(GNOME_CANVAS(canvas)),
		gnome_canvas_pixbuf_get_type (),
		"pixbuf", tarjs[i].tar,
		"x", 0.0,
		"y", 0.0,
		"width", (double) gdk_pixbuf_get_width(tarjs[i].tar),
		"height", (double) gdk_pixbuf_get_height(tarjs[i].tar),
		"anchor", GTK_ANCHOR_NW,
		NULL);

	if( !image ) textmsg( M_ERR, _("Error creating image\n"));

	image = gnome_canvas_item_new(
		gnome_canvas_root(GNOME_CANVAS(canvas)),
		gnome_canvas_text_get_type(),
		"text", countries_get_name(pP->id),
		"x", (double) gdk_pixbuf_get_width(tarjs[i].tar)/2,
		"y", (double) cards.pos_y,
		"x_offset", (double) -1,
		"y_offset", (double) -1,
//		"font", "-adobe-helvetica-medium-r-normal--12-*-72-72-p-*-iso8859-1",
		"font", HELVETICA_12_FONT,
		"fill_color", "brown",
		"anchor",GTK_ANCHOR_NORTH,
		NULL);

	if( !image ) textmsg( M_ERR, _("Error creating image\n"));

	image = gnome_canvas_item_new(
		gnome_canvas_root(GNOME_CANVAS(canvas)),
		gnome_canvas_text_get_type(),
		"text", cont_get_name(pP->continente),
		"x", (double) gdk_pixbuf_get_width(tarjs[i].tar)/2,
		"y", (double) cards.pos_y + 15,
		"x_offset", (double) -1,
		"y_offset", (double) -1,
//		"font", "-adobe-helvetica-medium-r-normal--9-*-72-72-p-*-iso8859-1",
		"font", HELVETICA_8_FONT,
		"fill_color", "brown",
		"anchor",GTK_ANCHOR_NORTH,
		NULL);

	if( !image ) textmsg( M_ERR, _("Error creating image\n"));

	gtk_box_pack_start_defaults( GTK_BOX(vbox), GTK_WIDGET(canvas));
	gtk_widget_show (canvas);

	/* botones */
//	hbox = gtk_hbox_new( FALSE, 0);
	button_armies = gtk_button_new_with_label(_("Put 2 armies"));
	gtk_box_pack_start_defaults( GTK_BOX(vbox), GTK_WIDGET(button_armies));
	gtk_signal_connect (GTK_OBJECT (button_armies), "clicked", GTK_SIGNAL_FUNC
			(cards_cb_button_armies), (void* )tarjs_index);
	gtk_widget_show(button_armies);

	button_select = gtk_check_button_new_with_label(_("Select this card"));
	gtk_box_pack_start_defaults( GTK_BOX(vbox), GTK_WIDGET(button_select));
	gtk_signal_connect (GTK_OBJECT (button_select), "clicked", GTK_SIGNAL_FUNC
			(cards_cb_button_select), (void* )tarjs_index);
	gtk_widget_show(button_select);

	button_locate = gtk_button_new_with_label(_("Locate country"));
	gtk_box_pack_start_defaults( GTK_BOX(vbox), GTK_WIDGET(button_locate));
	gtk_signal_connect (GTK_OBJECT (button_locate), "clicked", GTK_SIGNAL_FUNC
			(cards_cb_button_locate), (void* )tarjs_index);
	gtk_widget_show(button_locate);

//	gtk_box_pack_start_defaults( GTK_BOX(vbox), GTK_WIDGET(hbox));

	tarjs_sensi[tarjs_index].card = vbox;
	tarjs_sensi[tarjs_index].button_armies = button_armies;
	tarjs_sensi[tarjs_index].button_select = button_select;
	tarjs_sensi[tarjs_index].country = pP;
	tarjs_sensi[tarjs_index].selected = FALSE;

	return vbox;
}

/*
 * funciones exportadas
 */
/**
 * @fn void cards_view(int country)
 */
void cards_view(int country)
{
	GtkWidget *vbox;
	GtkWidget *scrolledwindow;
	static GtkTable	*table = NULL;
	GtkWidget	*tarjeta;	


	if( IsListEmpty( &g_game.tarjetas_list ) && country == -1 ) {
		textmsg(M_ERR,_("You dont have any cards yet"));
		return;
	}

	if( cards_dialog == NULL) {

		memset(tarjs_sensi,0,sizeof(tarjs_sensi));	

		cards_init();
		cards_load();

		cards_dialog = gnome_dialog_new(_("Country Cards"), _("Exchange"), GNOME_STOCK_BUTTON_CLOSE, NULL );
		gnome_dialog_set_parent( GNOME_DIALOG(cards_dialog), GTK_WINDOW(main_window));

		gnome_dialog_button_connect ( GNOME_DIALOG(cards_dialog),
				0, GTK_SIGNAL_FUNC(cards_cb_button_canje), cards_dialog);

		gnome_dialog_button_connect (GNOME_DIALOG(cards_dialog),
				1, GTK_SIGNAL_FUNC(dialog_close), cards_dialog);

		gtk_signal_connect( GTK_OBJECT(cards_dialog),
				"destroy", GTK_SIGNAL_FUNC(destroy_window),
				&cards_dialog);

		vbox = GNOME_DIALOG(cards_dialog)->vbox;

		scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(scrolledwindow),
				GTK_POLICY_AUTOMATIC,
				GTK_POLICY_AUTOMATIC);
		gtk_widget_set_usize(scrolledwindow, 300, 160);


		table    = GTK_TABLE( gtk_table_new ( 1, 3, TRUE) );
		gtk_table_set_col_spacings (table, 1);
		gtk_table_set_row_spacings (table, 5);

		{
			int x=0,y=0, index=0;
			PLIST_ENTRY pL = g_game.tarjetas_list.Flink;
			PTARJETA pT;
			while( !IsListEmpty( &g_game.tarjetas_list ) && (pL != &g_game.tarjetas_list )) {
				pT = (PTARJETA) pL;

				if(x >= 3 ) {
					x=0;
					y++;
					gtk_table_resize(table,y+1,3);
				}
				tarjeta = cards_create( pT, index );
				if( tarjeta ) {
					gtk_widget_show (tarjeta );
					gtk_table_attach_defaults ( table, tarjeta, x, x+1, y, y+1);
				}
				x++, index++;

				pL = LIST_NEXT( pL );
			}
		}

		gtk_box_pack_start( GTK_BOX(vbox), GTK_WIDGET(table), TRUE, TRUE, 0);
		gtk_widget_show (GTK_WIDGET(table));
		gtk_widget_show (scrolledwindow);

	} else if(country!=-1) { /* Ya estan las demas, solo hay que crear una sola tarjeta */
		int x=0, y=0,i;
		for(i=0;i<TEG_MAX_TARJETAS;i++) {

			if(x >= 3 ) {
				x=0;
				y++;
				gtk_table_resize(table,y+1,3);
			}

			if( tarjs_sensi[i].country == NULL ) {
				tarjeta = cards_create( &g_countries[country].tarjeta, i );
				if( tarjeta ) {
					gtk_widget_show (tarjeta );
					gtk_table_attach_defaults ( table, tarjeta, x, x+1, y, y+1);
				}
				break;
			}
			x++;
		}
	}

	cards_update();
	cards_update_para_canje();

	gtk_widget_show_all(cards_dialog);
	raise_and_focus(cards_dialog);
}

/**
 * @fn void cards_delete( int p1, int p2, int p3 )
 */
void cards_delete( int p1, int p2, int p3 )
{
	int i;
	if( cards_dialog == NULL) 
		return;

	for(i=0;i<TEG_MAX_TARJETAS;i++) {

		if( tarjs_sensi[i].country != NULL  && 
				( tarjs_sensi[i].country->id == p1 ||
				  tarjs_sensi[i].country->id == p2 ||
				  tarjs_sensi[i].country->id == p3 ) ) {

			if( tarjs_sensi[i].card && cards_dialog )
				gtk_widget_destroy( tarjs_sensi[i].card );
			tarjs_sensi[i].country = NULL;
		}
	}

	cards_update_para_canje();
	gtk_widget_show_all(cards_dialog);
	raise_and_focus(cards_dialog);
}

/**
 * @fn void cards_flush()
 * Elimina todas las tarjetas que tenga
 */
void cards_flush()
{
	int i;

	for(i=0;i<TEG_MAX_TARJETAS;i++) {
		if( tarjs_sensi[i].country  && tarjs_sensi[i].card && cards_dialog) {
			gtk_widget_destroy( tarjs_sensi[i].card );
		}
		tarjs_sensi[i].country = NULL;
	}
}


/**
 * @fn void cards_update( void )
 * Se encarga de hacer el update de las tarjetas, habilitando los botones
 * y esas cosas.
 */
void cards_update( void )
{
	int i;
	if( cards_dialog == NULL )
		return;
	
	for(i=0;i<TEG_MAX_TARJETAS;i++) {
		if( tarjs_sensi[i].country != NULL ) {
			if( !ESTADO_ES(PLAYER_STATUS_TARJETA)
					|| tarjeta_es_usada( &tarjs_sensi[i].country->tarjeta)
					|| tarjs_sensi[i].country->numjug != WHOAMI())
				gtk_widget_set_sensitive (tarjs_sensi[i].button_armies, FALSE);
			else
				gtk_widget_set_sensitive (tarjs_sensi[i].button_armies, TRUE);
		}
	}
}

/**
 * @fn void cards_update_para_canje( void )
 */
void cards_update_para_canje( void )
{
	int i;

	if( cards_dialog == NULL )
		return;

	for(i=0;i<TEG_MAX_TARJETAS;i++) {
		if( tarjs_sensi[i].country != NULL ) {
			if( ESTADO_ES( PLAYER_STATUS_FICHASC ) && canje_puedo(NULL,NULL,NULL)==TEG_STATUS_SUCCESS) {
				gtk_widget_set_sensitive (tarjs_sensi[i].button_select, TRUE);
			} else {
				gtk_widget_set_sensitive (tarjs_sensi[i].button_select, FALSE);
			}
		}
	}

	if( ESTADO_ES( PLAYER_STATUS_FICHASC ) && canje_puedo(NULL,NULL,NULL)==TEG_STATUS_SUCCESS)
		gnome_dialog_set_sensitive( GNOME_DIALOG(cards_dialog), 0, TRUE );
	else
		gnome_dialog_set_sensitive( GNOME_DIALOG(cards_dialog), 0, FALSE );
}

/**
 * @fn TEG_STATUS cards_select(int p1, int p2, int p3 )
 * Selecciona las cartas pedidas
 */
TEG_STATUS cards_select(int p1, int p2, int p3 )
{
	int i;

	cards_view(-1);

	for(i=0;i<TEG_MAX_TARJETAS;i++) {
		if( tarjs_sensi[i].country == NULL ) continue;

		if(( tarjs_sensi[i].country->id == p1 ) ||
				(tarjs_sensi[i].country->id == p2 ) ||
				(tarjs_sensi[i].country->id == p3 ) ) {
			tarjs_sensi[i].selected = TRUE;
			GTK_TOGGLE_BUTTON(tarjs_sensi[i].button_select)->active=1;
		} else {
			tarjs_sensi[i].selected = FALSE;
			GTK_TOGGLE_BUTTON(tarjs_sensi[i].button_select)->active=0;
		}
	}

	return TEG_STATUS_SUCCESS;
}

/**
 * @fn void cards_free()
 * Libera los recursos alocados
 */
void cards_free()
{
	int i;

	for(i=0;i<NRTARJS;i++) {
		if( tarjs[i].tar ) {
			/* gdk_imlib_destroy_image ( tarjs[i].tar ); */
			tarjs[i].tar = NULL;
		}
	}
}
