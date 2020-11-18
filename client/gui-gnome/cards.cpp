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

#include <goocanvas.h>
#include <glib/gi18n.h>
#include <assert.h>

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
	char const *filename;
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

	TCards cards;
	/* obtain cards from theme */
	if(theme_giveme_cards(&cards) == TEG_STATUS_SUCCESS) {
		if(cards.cannon) {
			tarjs[0].filename = cards.cannon;
		}
		if(cards.ship) {
			tarjs[1].filename = cards.ship;
		}
		if(cards.balloon) {
			tarjs[2].filename = cards.balloon;
		}
		if(cards.jocker) {
			tarjs[3].filename = cards.jocker;
		}
	}

	for(unsigned i=0; i<NRTARJS; i++) {

		if(!tarjs[i].tar) {
			filename = theme_load_file(tarjs[i].filename);
			tarjs[i].tar = gdk_pixbuf_new_from_file(filename, NULL);
			if(tarjs[i].tar == NULL) {
				g_warning("Error, couldn't find file:%s", tarjs[i].filename);
			}
		}
	}

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS cards_init()
{
	int i;

	for(i=0; i<TEG_MAX_TARJETAS; i++) {
		memset(&tarjs_sensi[i], 0, sizeof(tarjs_sensi[i]));
	}
	return TEG_STATUS_SUCCESS;
}

/* Callbacks de las funciones de los botones y funciones auxiliares */
static int cuantos_selected(int countries[TEG_MAX_TARJETAS])
{
	int i, j=0;
	int selected=0;

	for(i=0; i<TEG_MAX_TARJETAS; i++) {
		if(tarjs_sensi[i].country != NULL) {
			if(tarjs_sensi[i].selected) {
				countries[j++] = tarjs_sensi[i].country->id;
				selected++;
			}
		}
	}
	return selected;
}

static void cards_cb_button_canje(GtkDialog *dialog, gint id, gpointer data)
{
	int sel;
	int countries[TEG_MAX_TARJETAS];

	assert(dialog);

	if(id == 0) {
		sel = cuantos_selected(countries);
		if(sel != 3) {
			textmsg(M_ERR, "Error, you must select 3 cards and "
			        "not %d.", sel);
			return;
		}
		canje_out(countries[0], countries[1], countries[2]);
	}

	gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void cards_cb_button_armies(GtkWidget *widget, int index)
{
	if(tarjs_sensi[ index ].country == NULL) {
		textmsg(M_ERR, "Error in cards_cb_button_armies");
	} else {
		if(ejer2_out(tarjs_sensi[ index ].country->id) == TEG_STATUS_SUCCESS) {
			cards_update();
		}
	}
}

static void cards_cb_button_select(GtkWidget *widget, int index)
{
	if(tarjs_sensi[ index ].country == NULL) {
		textmsg(M_ERR, "Error in cards_cb_button_selected");
	} else {
		tarjs_sensi[ index ].selected = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	}
}

static void cards_cb_button_locate(GtkWidget *widget, int index)
{
	if(tarjs_sensi[ index ].country == NULL) {
		textmsg(M_ERR, "Error in cards_cb_button_selected");
	} else {
		locate_country_add_country(tarjs_sensi[ index ].country);
	}
}

/* funciones de creacion */
static GtkWidget *cards_create(PTARJETA pT, int tarjs_index)
{
	GtkWidget	*vbox;
	GtkWidget	*canvas;
	GtkWidget	*button_armies;
	GtkWidget	*button_select;
	GtkWidget	*button_locate;
	PCOUNTRY		pP;
	GooCanvasItem *image;
	TCards cards;

	if(theme_giveme_cards(&cards) != TEG_STATUS_SUCCESS) {
		return NULL;
	}

	unsigned i=0;
	for(; i<NRTARJS; i++) {
		if(tarjs[i].tipo == pT->tarjeta) {
			break;
		}
	}

	pP = (PCOUNTRY) COUNTRY_FROM_TARJETA(pT);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	if(!vbox) {
		return NULL;
	}

	/* create picture of card (canvas with labels) */
	canvas = goo_canvas_new();
	if(!canvas) {
		textmsg(M_ERR, _("Error creating canvas\n"));
		return NULL;
	}

	gtk_widget_set_size_request(canvas,
	                            gdk_pixbuf_get_width(tarjs[i].tar),
	                            gdk_pixbuf_get_height(tarjs[i].tar));
	goo_canvas_set_bounds(GOO_CANVAS(canvas), 0, 0,
	                      gdk_pixbuf_get_width(tarjs[i].tar),
	                      gdk_pixbuf_get_height(tarjs[i].tar));
	image = goo_canvas_image_new(
	            goo_canvas_get_root_item(GOO_CANVAS(canvas)),
	            tarjs[i].tar,
	            0.0,
	            0.0,
	            "width", (double) gdk_pixbuf_get_width(tarjs[i].tar),
	            "height", (double) gdk_pixbuf_get_height(tarjs[i].tar),
	            NULL);

	if(!image) {
		textmsg(M_ERR, _("Error creating image\n"));
	}

	image = goo_canvas_text_new(
	            goo_canvas_get_root_item(GOO_CANVAS(canvas)),
	            countries_get_name(pP->id),
	            (double) gdk_pixbuf_get_width(tarjs[i].tar)/2,
	            (double) cards.pos_y,
	            (double) -1,
	            GOO_CANVAS_ANCHOR_NORTH,
	            "height", (double) -1,
	            "font", HELVETICA_12_FONT,
	            "fill-color", "brown",
	            NULL);

	if(!image) {
		textmsg(M_ERR, _("Error creating image\n"));
	}

	image = goo_canvas_text_new(
	            goo_canvas_get_root_item(GOO_CANVAS(canvas)),
	            cont_get_name(pP->continente),
	            (double) gdk_pixbuf_get_width(tarjs[i].tar)/2,
	            (double) cards.pos_y + 15,
	            (double) -1,
	            GOO_CANVAS_ANCHOR_NORTH,
	            "height", (double) -1,
	            "font", HELVETICA_8_FONT,
	            "fill-color", "brown",
	            NULL);

	if(!image) {
		textmsg(M_ERR, _("Error creating image\n"));
	}

	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(canvas), TRUE, TRUE, 0);
	gtk_widget_show(canvas);

	/* botones */
	button_armies = gtk_button_new_with_label(_("Put 2 armies"));
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(button_armies),
	                   TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(button_armies), "clicked",
	                 G_CALLBACK(cards_cb_button_armies),
	                 GINT_TO_POINTER(tarjs_index));
	gtk_widget_show(button_armies);

	button_select = gtk_check_button_new_with_label(_("Select this card"));
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(button_select), TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(button_select), "clicked", G_CALLBACK
	                 (cards_cb_button_select), GINT_TO_POINTER(tarjs_index));
	gtk_widget_show(button_select);

	button_locate = gtk_button_new_with_label(_("Locate country"));
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(button_locate),
	                   TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(button_locate), "clicked",
	                 G_CALLBACK(cards_cb_button_locate),
	                 GINT_TO_POINTER(tarjs_index));
	gtk_widget_show(button_locate);

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
	static GtkGrid	*table = NULL;
	GtkWidget	*tarjeta;


	if(IsListEmpty(&g_game.tarjetas_list) && country == -1) {
		textmsg(M_ERR, _("You dont have any cards yet"));
		return;
	}

	if(cards_dialog == NULL) {

		memset(tarjs_sensi, 0, sizeof(tarjs_sensi));

		cards_init();
		cards_load();

		cards_dialog
		    = gtk_dialog_new_with_buttons(_("Country Cards"),
		                                  GTK_WINDOW(main_window),
		                                  GTK_DIALOG_DESTROY_WITH_PARENT,
		                                  _("Exchange"), 0,
		                                  _("_Close"), 1,
		                                  NULL);

		g_signal_connect(cards_dialog, "response",
		                 G_CALLBACK(cards_cb_button_canje), NULL);

		g_signal_connect(G_OBJECT(cards_dialog),
		                 "destroy", G_CALLBACK(destroy_window),
		                 &cards_dialog);

		vbox = gtk_dialog_get_content_area(GTK_DIALOG(cards_dialog));

		scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow),
		                               GTK_POLICY_AUTOMATIC,
		                               GTK_POLICY_AUTOMATIC);
		gtk_widget_set_size_request(scrolledwindow, 300, 160);


		table = GTK_GRID(gtk_grid_new());
		gtk_grid_set_column_spacing(table, 1);
		gtk_grid_set_row_spacing(table, 5);
		gtk_grid_set_column_homogeneous(table, TRUE);
		gtk_grid_set_row_homogeneous(table, TRUE);

		{
			int x=0, y=0, index=0;
			PLIST_ENTRY pL = g_game.tarjetas_list.Flink;
			PTARJETA pT;
			while(!IsListEmpty(&g_game.tarjetas_list) && (pL != &g_game.tarjetas_list)) {
				pT = (PTARJETA) pL;

				if(x >= 3) {
					x=0;
					y++;
				}
				tarjeta = cards_create(pT, index);
				if(tarjeta) {
					gtk_widget_show(tarjeta);
					gtk_grid_attach(table, tarjeta,
					                x, y, 1, 1);
				}
				x++, index++;

				pL = LIST_NEXT(pL);
			}
		}

		gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(table), TRUE, TRUE, 0);
		gtk_widget_show(GTK_WIDGET(table));
		gtk_widget_show(scrolledwindow);

	} else if(country!=-1) { /* Ya estan las demas, solo hay que crear una sola tarjeta */
		int x=0, y=0, i;
		for(i=0; i<TEG_MAX_TARJETAS; i++) {

			if(x >= 3) {
				x=0;
				y++;
			}

			if(tarjs_sensi[i].country == NULL) {
				tarjeta = cards_create(&g_countries[country].tarjeta, i);
				if(tarjeta) {
					gtk_widget_show(tarjeta);
					gtk_grid_attach(table, tarjeta,
					                x, y, 1, 1);
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
void cards_delete(int p1, int p2, int p3)
{
	int i;
	if(cards_dialog == NULL) {
		return;
	}

	for(i=0; i<TEG_MAX_TARJETAS; i++) {

		if(tarjs_sensi[i].country != NULL  &&
		        (tarjs_sensi[i].country->id == p1 ||
		         tarjs_sensi[i].country->id == p2 ||
		         tarjs_sensi[i].country->id == p3)) {

			if(tarjs_sensi[i].card && cards_dialog) {
				gtk_widget_destroy(tarjs_sensi[i].card);
			}
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

	for(i=0; i<TEG_MAX_TARJETAS; i++) {
		if(tarjs_sensi[i].country  && tarjs_sensi[i].card && cards_dialog) {
			gtk_widget_destroy(tarjs_sensi[i].card);
		}
		tarjs_sensi[i].country = NULL;
	}
}


/**
 * @fn void cards_update( void )
 * Se encarga de hacer el update de las tarjetas, habilitando los botones
 * y esas cosas.
 */
void cards_update(void)
{
	int i;
	if(cards_dialog == NULL) {
		return;
	}

	for(i=0; i<TEG_MAX_TARJETAS; i++) {
		if(tarjs_sensi[i].country != NULL) {
			if(!ESTADO_ES(PLAYER_STATUS_TARJETA)
			        || tarjeta_es_usada(&tarjs_sensi[i].country->tarjeta)
			        || tarjs_sensi[i].country->numjug != WHOAMI()) {
				gtk_widget_set_sensitive(tarjs_sensi[i].button_armies, FALSE);
			} else {
				gtk_widget_set_sensitive(tarjs_sensi[i].button_armies, TRUE);
			}
		}
	}
}

/**
 * @fn void cards_update_para_canje( void )
 */
void cards_update_para_canje(void)
{
	int i;

	if(cards_dialog == NULL) {
		return;
	}

	for(i=0; i<TEG_MAX_TARJETAS; i++) {
		if(tarjs_sensi[i].country != NULL) {
			if(ESTADO_ES(PLAYER_STATUS_FICHASC) && canje_puedo(NULL, NULL, NULL)==TEG_STATUS_SUCCESS) {
				gtk_widget_set_sensitive(tarjs_sensi[i].button_select, TRUE);
			} else {
				gtk_widget_set_sensitive(tarjs_sensi[i].button_select, FALSE);
			}
		}
	}

	if(ESTADO_ES(PLAYER_STATUS_FICHASC) && canje_puedo(NULL, NULL, NULL)==TEG_STATUS_SUCCESS)
		gtk_dialog_set_response_sensitive(GTK_DIALOG(cards_dialog),
		                                  0, TRUE);
	else
		gtk_dialog_set_response_sensitive(GTK_DIALOG(cards_dialog),
		                                  0, FALSE);
}

/**
 * @fn TEG_STATUS cards_select(int p1, int p2, int p3 )
 * Selecciona las cartas pedidas
 */
TEG_STATUS cards_select(int p1, int p2, int p3)
{
	int i;

	cards_view(-1);

	for(i=0; i<TEG_MAX_TARJETAS; i++) {
		if(tarjs_sensi[i].country == NULL) {
			continue;
		}

		if((tarjs_sensi[i].country->id == p1) ||
		        (tarjs_sensi[i].country->id == p2) ||
		        (tarjs_sensi[i].country->id == p3)) {
			tarjs_sensi[i].selected = TRUE;
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tarjs_sensi[i].button_select), TRUE);
		} else {
			tarjs_sensi[i].selected = FALSE;
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tarjs_sensi[i].button_select), FALSE);
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
	for(unsigned i=0; i<NRTARJS; i++) {
		if(tarjs[i].tar) {
			g_clear_object(&tarjs[i].tar);
		}
	}
}
