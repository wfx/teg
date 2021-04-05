/*	$Id: callbacks.c,v 1.70 2006/03/12 09:56:04 nordi Exp $	*/
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
 * @file callbacks.c
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>

#include "gui.h"
#include "client.h"

#include "chatline.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "priv.h"
#include "g_country.h"
#include "cards.h"
#include "dices.h"
#include "reagrupe.h"
#include "mission.h"
#include "status.h"
#include "connect.h"
#include "armies.h"
#include "colors.h"
#include "preferences.h"
#include "g_scores.h"


/*
 * funciones auxiliares
 */

/* close a window */
void destroy_window( GtkWidget * widget, GtkWidget **window )
{
	if( *window != NULL) {
		gtk_widget_destroy(*window);
	}

	*window=NULL;
}


/* Brings attention to a window by raising it and giving it focus */
void raise_and_focus (GtkWidget *widget)
{
	g_assert (GTK_WIDGET_REALIZED (widget));
	gdk_window_show (widget->window);
	gtk_widget_grab_focus (widget);
}

/*
 * otras funciones
 */
TEG_STATUS pre_client_recv( gpointer data, int sock, GdkInputCondition GDK_INPUT_READ )
{
	client_recv( sock );
	return TEG_STATUS_SUCCESS;
}

/*
 * funciones callbacks
 */
/* game */
void on_connect_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	connect_view();
}

void on_disconnect_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	teg_disconnect();
}

void on_launchrobot_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	launch_robot();
}

void on_scores_activate(GtkMenuItem *widget, gpointer user_data)
{
	out_scores();
	gui_scores_view();
}

void on_exit_activate (GtkWidget *widget, gpointer user_data)
{
	teg_disconnect();
	cards_free();
	colors_free();
	gtk_main_quit();
}

/* actions */
void on_enviarfichas_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	int i;
	int cant, conts;

	if( fichas_out() != TEG_STATUS_SUCCESS )  {

		fichas_get_wanted( &cant, &conts );
		armies_view( cant, conts );

		for(i=0;i<COUNTRIES_CANT;i++)
			G_country_draw_ejer(i);
	} else {
		armies_unview();
	}
	set_sensitive_tb();
}

void on_updatemap_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	out_countries();
}

void on_enviarreattack_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	attackre_out();
}

void on_enviarattackreset_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	if( attack_pre_reset() == TEG_STATUS_SUCCESS )
		textmsg(M_INF,_("The attack was reset. Please, select the source country to continue your attack"));
}

void on_reagrupe_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	if( reagrupe_init() == TEG_STATUS_SUCCESS ) {
		if (gui_private.dialog_show & (1 <<DIALOG_REGROUP_ARMIES) ) {
			teg_dialog( _("Regrouping armies"),_("Regrouping armies"),
	_("Select two countries:\n1st: click on the source country\n2nd: click on the destination country"));
		}
	}
}

void on_pedirtarjeta_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	out_tarjeta();
	set_sensitive_tb();
}

void on_endturn_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	out_endturn();
	set_sensitive_tb();
}

static void surrender_cb (int button, gpointer data)
{
	if (button == 0)
		out_surrender();
}
void on_surrender_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	gnome_question_dialog_parented (_("Really surrender ?"), surrender_cb, NULL, GTK_WINDOW(main_window));
}


void
on_start_activate(GtkMenuItem  *menuitem, gpointer         user_data)
{
	gametype_view();
}


void
on_status_activate(GtkMenuItem  *menuitem, gpointer         user_data)
{
	status_view();
}

void
on_viewcards_activate(GtkMenuItem  *menuitem, gpointer         user_data)
{
	cards_view(-1);
}

void
on_viewdices_activate(GtkMenuItem  *menuitem, gpointer         user_data)
{
	dices_view();
}

void
on_viewmission_activate(GtkMenuItem  *menuitem, gpointer         user_data)
{
	mission_view();
}


void
on_preferences1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	preferences_activate();
}


void on_about_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	static GtkWidget *about = NULL;

	GtkWidget *href, *hbox;
	GdkPixbuf *pixbuf = NULL;
	const gchar *documenters[] = {
		"Ricardo Quesada, english",
		"Arkadiusz Lipiec, polish",
		NULL
	};	
	const gchar *translator_credits = _("translator_credits");
	const char *authors[] = {
		"Main Authors:",
		"    Ricardo Quesada, main coder",
		"    Wolfgang Morawetz, main artist",
		"",
		"Main Contributors:",
		"    Thomas R. Koll",
		"    Raymond Ostertag",
		"",
		"Detailed info in the PEOPLE file or in the homepage",
		NULL
	};

	{
     		gchar* logo_filename = NULL;
		logo_filename = g_strdup(PIXMAPDIR"/teg_icono.png");

		if (logo_filename != NULL)
		{
			pixbuf = gdk_pixbuf_new_from_file(logo_filename, NULL);
			g_free (logo_filename);
		}
	}

	if (!about) {
		about = gnome_about_new (
			_("Tenes Empanadas Graciela"), VERSION,
			_("Copyright (C) 2000, 2002 Ricardo Quesada"),
			_("A clone of T.E.G. (a Risk clone)."),
			(const char**) authors,
			(const char**) documenters,
			 strcmp (translator_credits, "translator_credits") != 0 ? translator_credits : NULL,
			pixbuf);

		gtk_signal_connect (GTK_OBJECT (about), "destroy",
				    GTK_SIGNAL_FUNC (gtk_widget_destroyed),
				    &about);


		hbox = gtk_hbox_new (TRUE, 0);
		href = gnome_href_new ("https://github.com/wfx/teg", _("TEG Home Page"));
		gtk_box_pack_start (GTK_BOX (hbox), href, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (about)->vbox),
			    hbox, TRUE, FALSE, 0);
		gtk_widget_show_all (hbox);
	}

	gtk_widget_show_now (about);
	raise_and_focus (about);
}
