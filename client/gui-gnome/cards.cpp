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

#include <algorithm> // std::for_each
#include <vector>

#include <goocanvas.h>
#include <glib/gi18n.h>
#include <assert.h>

#include "client.h"
#include "gui_state.h"

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

namespace teg::client::callbacks
{

#define NRTARJS	( sizeof(gui_state().cards.kinds) / sizeof(gui_state().cards.kinds[0]))

/**
 * @fn TEG_STATUS cards_load()
 * Carga las imagenes de las tarjetas en variables globlales
 */
TEG_STATUS cards_load()
{
	TCards cards;
	/* obtain cards from theme */
	if(theme_giveme_cards(client_state(), &cards) == TEG_STATUS_SUCCESS) {
		if(cards.cannon) {
			gui_state().cards.kinds[0].filename = cards.cannon;
		}
		if(cards.ship) {
			gui_state().cards.kinds[1].filename = cards.ship;
		}
		if(cards.balloon) {
			gui_state().cards.kinds[2].filename = cards.balloon;
		}
		if(cards.jocker) {
			gui_state().cards.kinds[3].filename = cards.jocker;
		}
	}

	for(unsigned i=0; i<NRTARJS; i++) {

		if(!gui_state().cards.kinds[i].tar) {
			auto const fpath = theme_load_file(client_state(), gui_state().cards.kinds[i].filename);
			char const*const filename = fpath ? fpath->c_str() : nullptr;
			gui_state().cards.kinds[i].tar = gdk_pixbuf_new_from_file(filename, NULL);
			if(gui_state().cards.kinds[i].tar == NULL) {
				g_warning("Error, couldn't find file:%s", gui_state().cards.kinds[i].filename);
			}
		}
	}

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS cards_init()
{
	std::size_t i;

	for(i=0; i<maximum_country_cards; i++) {
		gui_state().cards.sensi[i] = {};
	}
	return TEG_STATUS_SUCCESS;
}

/* Callbacks de las funciones de los botones y funciones auxiliares */
static int cuantos_selected(int countries[maximum_country_cards])
{
	std::size_t i;
	int j=0;
	int selected=0;

	for(i=0; i<maximum_country_cards; i++) {
		if(gui_state().cards.sensi[i].country != NULL) {
			if(gui_state().cards.sensi[i].selected) {
				countries[j++] = gui_state().cards.sensi[i].country->id;
				selected++;
			}
		}
	}
	return selected;
}

static void cards_cb_button_canje(GtkDialog *dialog, gint id, gpointer data)
{
	int sel;
	int countries[maximum_country_cards];

	assert(dialog);

	if(id == 0) {
		sel = cuantos_selected(countries);
		if(sel != 3) {
			textmsg(client_state(), M_ERR, "Error, you must select 3 cards and "
			        "not %d.", sel);
			return;
		}
		canje_out(client_state(), countries[0], countries[1], countries[2]);
	}

	gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void cards_cb_button_armies(GtkWidget *widget, int index)
{
	if(gui_state().cards.sensi[ index ].country == NULL) {
		textmsg(client_state(), M_ERR, "Error in cards_cb_button_armies");
	} else {
		if(ejer2_out(client_state(), gui_state().cards.sensi[ index ].country->id) == TEG_STATUS_SUCCESS) {
			cards_update();
		}
	}
}

static void cards_cb_button_select(GtkWidget *widget, int index)
{
	if(gui_state().cards.sensi[ index ].country == NULL) {
		textmsg(client_state(), M_ERR, "Error in cards_cb_button_selected");
	} else {
		gui_state().cards.sensi[ index ].selected = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	}
}

static void cards_cb_button_locate(GtkWidget *widget, int index)
{
	if(gui_state().cards.sensi[ index ].country == NULL) {
		textmsg(client_state(), M_ERR, "Error in cards_cb_button_selected");
	} else {
		locate_country_add_country(gui_state().cards.sensi[ index ].country);
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
	GooCanvasItem *image;
	TCards cards;

	if(theme_giveme_cards(client_state(), &cards) != TEG_STATUS_SUCCESS) {
		return NULL;
	}

	unsigned i=0;
	for(; i<NRTARJS; i++) {
		if(gui_state().cards.kinds[i].tipo == pT->tarjeta) {
			break;
		}
	}

	COUNTRY *const pP = &COUNTRY_FROM_TARJETA(client_state().game.common, *pT);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	if(!vbox) {
		return NULL;
	}

	/* create picture of card (canvas with labels) */
	canvas = goo_canvas_new();
	if(!canvas) {
		textmsg(client_state(), M_ERR, _("Error creating canvas\n"));
		return NULL;
	}

	gtk_widget_set_size_request(canvas,
	                            gdk_pixbuf_get_width(gui_state().cards.kinds[i].tar),
	                            gdk_pixbuf_get_height(gui_state().cards.kinds[i].tar));
	goo_canvas_set_bounds(GOO_CANVAS(canvas), 0, 0,
	                      gdk_pixbuf_get_width(gui_state().cards.kinds[i].tar),
	                      gdk_pixbuf_get_height(gui_state().cards.kinds[i].tar));
	image = goo_canvas_image_new(
	            goo_canvas_get_root_item(GOO_CANVAS(canvas)),
	            gui_state().cards.kinds[i].tar,
	            0.0,
	            0.0,
	            "width", (double) gdk_pixbuf_get_width(gui_state().cards.kinds[i].tar),
	            "height", (double) gdk_pixbuf_get_height(gui_state().cards.kinds[i].tar),
	            NULL);

	if(!image) {
		textmsg(client_state(), M_ERR, _("Error creating image\n"));
	}

	image = goo_canvas_text_new(
	            goo_canvas_get_root_item(GOO_CANVAS(canvas)),
	            countries_get_name(client_state().game.common, pP->id),
	            (double) gdk_pixbuf_get_width(gui_state().cards.kinds[i].tar)/2,
	            (double) cards.pos_y,
	            (double) -1,
	            GOO_CANVAS_ANCHOR_NORTH,
	            "height", (double) -1,
	            "font", HELVETICA_12_FONT,
	            "fill-color", "brown",
	            NULL);

	if(!image) {
		textmsg(client_state(), M_ERR, _("Error creating image\n"));
	}

	image = goo_canvas_text_new(
	            goo_canvas_get_root_item(GOO_CANVAS(canvas)),
	            cont_get_name(client_state().game.common, pP->continente),
	            (double) gdk_pixbuf_get_width(gui_state().cards.kinds[i].tar)/2,
	            (double) cards.pos_y + 15,
	            (double) -1,
	            GOO_CANVAS_ANCHOR_NORTH,
	            "height", (double) -1,
	            "font", HELVETICA_8_FONT,
	            "fill-color", "brown",
	            NULL);

	if(!image) {
		textmsg(client_state(), M_ERR, _("Error creating image\n"));
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

	gui_state().cards.sensi[tarjs_index].card = vbox;
	gui_state().cards.sensi[tarjs_index].button_armies = button_armies;
	gui_state().cards.sensi[tarjs_index].button_select = button_select;
	gui_state().cards.sensi[tarjs_index].country = pP;
	gui_state().cards.sensi[tarjs_index].selected = FALSE;

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

	std::vector<TARJETA*> cards_i_have;
	/* Assumption: the player can never have more than maximum_country_cards
	 * cards. This is only an optimization, should this condition not be true,
	 * the vector will grow automatically */
	cards_i_have.reserve(maximum_country_cards);
	countries_map(client_state().game.common, [&cards_i_have](COUNTRY& c) {
		if(c.tarjeta.numjug == who_am_i(client_state())) {
			cards_i_have.push_back(&c.tarjeta);
		}
	});

	if(cards_i_have.empty() && country == -1) {
		textmsg(client_state(), M_ERR, _("You dont have any cards yet"));
		return;
	}

	if(gui_state().cards.dialog == NULL) {

		for(auto& sensi: gui_state().cards.sensi) {
			sensi = {};
		}

		cards_init();
		cards_load();

		gui_state().cards.dialog
		    = gtk_dialog_new_with_buttons(_("Country Cards"),
		                                  GTK_WINDOW(gui_state().ui.main_window),
		                                  GTK_DIALOG_DESTROY_WITH_PARENT,
		                                  _("Exchange"), 0,
		                                  _("_Close"), 1,
		                                  NULL);

		g_signal_connect(gui_state().cards.dialog, "response",
		                 G_CALLBACK(cards_cb_button_canje), NULL);

		g_signal_connect(G_OBJECT(gui_state().cards.dialog),
		                 "destroy", G_CALLBACK(destroy_window),
		                 &gui_state().cards.dialog);

		vbox = gtk_dialog_get_content_area(GTK_DIALOG(gui_state().cards.dialog));

		scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow),
		                               GTK_POLICY_AUTOMATIC,
		                               GTK_POLICY_AUTOMATIC);
		gtk_widget_set_size_request(scrolledwindow, 300, 160);


		gui_state().cards.table = GTK_GRID(gtk_grid_new());
		gtk_grid_set_column_spacing(gui_state().cards.table, 1);
		gtk_grid_set_row_spacing(gui_state().cards.table, 5);
		gtk_grid_set_column_homogeneous(gui_state().cards.table, TRUE);
		gtk_grid_set_row_homogeneous(gui_state().cards.table, TRUE);

		{
			int x=0, y=0, index=0;
			std::for_each(cards_i_have.begin(), cards_i_have.end(),
			[&x, &y, &index](TARJETA *pT) {
				if(x >= 3) {
					x=0;
					y++;
				}
				GtkWidget *const tarjeta = cards_create(pT, index);
				if(tarjeta) {
					gtk_widget_show(tarjeta);
					gtk_grid_attach(gui_state().cards.table, tarjeta,
					                x, y, 1, 1);
				}
				x++, index++;
			});
		}

		gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(gui_state().cards.table), TRUE, TRUE, 0);
		gtk_widget_show(GTK_WIDGET(gui_state().cards.table));
		gtk_widget_show(scrolledwindow);

	} else if(country!=-1) { /* Ya estan las demas, solo hay que crear una sola tarjeta */
		int x=0, y=0;
		std::size_t i;
		for(i=0; i<maximum_country_cards; i++) {

			if(x >= 3) {
				x=0;
				y++;
			}

			if(gui_state().cards.sensi[i].country == NULL) {
				GtkWidget *const tarjeta = cards_create(&client_state().game.common.countries[country].tarjeta, i);
				if(tarjeta) {
					gtk_widget_show(tarjeta);
					gtk_grid_attach(gui_state().cards.table, tarjeta,
					                x, y, 1, 1);
				}
				break;
			}
			x++;
		}
	}

	cards_update();
	cards_update_para_canje();

	gtk_widget_show_all(gui_state().cards.dialog);
	raise_and_focus(gui_state().cards.dialog);
}

/**
 * @fn void cards_delete( int p1, int p2, int p3 )
 */
void cards_delete(int p1, int p2, int p3)
{
	std::size_t i;
	if(gui_state().cards.dialog == NULL) {
		return;
	}

	for(i=0; i<maximum_country_cards; i++) {

		if(gui_state().cards.sensi[i].country != NULL  &&
		        (gui_state().cards.sensi[i].country->id == p1 ||
		         gui_state().cards.sensi[i].country->id == p2 ||
		         gui_state().cards.sensi[i].country->id == p3)) {

			if(gui_state().cards.sensi[i].card && gui_state().cards.dialog) {
				gtk_widget_destroy(gui_state().cards.sensi[i].card);
			}
			gui_state().cards.sensi[i].country = NULL;
		}
	}

	cards_update_para_canje();
	gtk_widget_show_all(gui_state().cards.dialog);
	raise_and_focus(gui_state().cards.dialog);
}

/**
 * @fn void cards_flush()
 * Elimina todas las tarjetas que tenga
 */
void cards_flush()
{
	std::size_t i;

	for(i=0; i<maximum_country_cards; i++) {
		if(gui_state().cards.sensi[i].country  && gui_state().cards.sensi[i].card && gui_state().cards.dialog) {
			gtk_widget_destroy(gui_state().cards.sensi[i].card);
		}
		gui_state().cards.sensi[i].country = NULL;
	}
}


/**
 * @fn void cards_update( void )
 * Se encarga de hacer el update de las tarjetas, habilitando los botones
 * y esas cosas.
 */
void cards_update(void)
{
	std::size_t i;
	if(gui_state().cards.dialog == NULL) {
		return;
	}

	for(i=0; i<maximum_country_cards; i++) {
		if(gui_state().cards.sensi[i].country != NULL) {
			if(!is_status(client_state(), PLAYER_STATUS_TARJETA)
			        || tarjeta_es_usada(&gui_state().cards.sensi[i].country->tarjeta)
			        || gui_state().cards.sensi[i].country->numjug != who_am_i(client_state())) {
				gtk_widget_set_sensitive(gui_state().cards.sensi[i].button_armies, FALSE);
			} else {
				gtk_widget_set_sensitive(gui_state().cards.sensi[i].button_armies, TRUE);
			}
		}
	}
}

/**
 * @fn void cards_update_para_canje( void )
 */
void cards_update_para_canje(void)
{
	std::size_t i;

	if(gui_state().cards.dialog == NULL) {
		return;
	}

	for(i=0; i<maximum_country_cards; i++) {
		if(gui_state().cards.sensi[i].country != NULL) {
			if(is_status(client_state(), PLAYER_STATUS_FICHASC) && canje_puedo(client_state(), NULL, NULL, NULL)==TEG_STATUS_SUCCESS) {
				gtk_widget_set_sensitive(gui_state().cards.sensi[i].button_select, TRUE);
			} else {
				gtk_widget_set_sensitive(gui_state().cards.sensi[i].button_select, FALSE);
			}
		}
	}

	if(is_status(client_state(), PLAYER_STATUS_FICHASC) && canje_puedo(client_state(), NULL, NULL, NULL)==TEG_STATUS_SUCCESS)
		gtk_dialog_set_response_sensitive(GTK_DIALOG(gui_state().cards.dialog),
		                                  0, TRUE);
	else
		gtk_dialog_set_response_sensitive(GTK_DIALOG(gui_state().cards.dialog),
		                                  0, FALSE);
}

/**
 * @fn TEG_STATUS cards_select(int p1, int p2, int p3 )
 * Selecciona las cartas pedidas
 */
TEG_STATUS cards_select(int p1, int p2, int p3)
{
	std::size_t i;

	cards_view(-1);

	for(i=0; i<maximum_country_cards; i++) {
		if(gui_state().cards.sensi[i].country == NULL) {
			continue;
		}

		if((gui_state().cards.sensi[i].country->id == p1) ||
		        (gui_state().cards.sensi[i].country->id == p2) ||
		        (gui_state().cards.sensi[i].country->id == p3)) {
			gui_state().cards.sensi[i].selected = TRUE;
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_state().cards.sensi[i].button_select), TRUE);
		} else {
			gui_state().cards.sensi[i].selected = FALSE;
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_state().cards.sensi[i].button_select), FALSE);
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
		if(gui_state().cards.kinds[i].tar) {
			g_clear_object(&gui_state().cards.kinds[i].tar);
		}
	}
}

}
