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

#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include "../../common/tegdebug.h"

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
void destroy_window(GtkWidget * widget, GtkWidget **window)
{
	if(*window != NULL) {
		gtk_widget_destroy(*window);
	}

	*window=NULL;
}


/* Brings attention to a window by raising it and giving it focus */
void raise_and_focus(GtkWidget *widget)
{
	g_assert(gtk_widget_get_realized(widget));
	gdk_window_show(gtk_widget_get_window(widget));
	gtk_widget_grab_focus(widget);
}

gboolean pre_client_recv(GIOChannel *source, GIOCondition cond, gpointer data)
{
	int const fd=g_io_channel_unix_get_fd(source);
	const bool ok = (cond == (cond & (G_IO_IN | G_IO_PRI)));

	char const*const line = ok ? "++++++++++++++++++++++++++++++++++++++++++++"
	                        : "++##++##++##++##++##++##++##++##++##++##++##";

	PDEBUG("%s\n"
	       "fd=%d, cond=%u (in=%u, out=%u, pri=%u, err=%u, hup=%u, inv=%u)\n",
	       line,
	       fd,
	       (unsigned) cond,
	       (unsigned) G_IO_IN,
	       (unsigned) G_IO_OUT,
	       (unsigned) G_IO_PRI,
	       (unsigned) G_IO_ERR,
	       (unsigned) G_IO_HUP,
	       (unsigned) G_IO_NVAL
	      );

	if(ok) {
		client_recv(fd);
	} else {
		disconnect(DR_IO_ERROR);
	}

	PDEBUG("--------------------------------------------\n"
	       "recv done");

	/* no need to return false here, since the channel got already destroyed by
	 * the call to disconnect() */
	return TRUE;
}

/*
 * funciones callbacks
 */
/* game */
void on_connect_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	connect_view();
}

void on_disconnect_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	disconnect(DR_NORMAL_DISCONNECT);
}

void on_launchrobot_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	launch_robot();
}

void on_scores_activate(GtkMenuItem *widget, gpointer user_data)
{
	out_scores();
	gui_scores_view();
}

void on_exit_activate(GtkWidget *widget, gpointer user_data)
{
	disconnect(DR_NORMAL_DISCONNECT);
	cards_free();
	gtk_main_quit();
}

/* actions */
void on_enviarfichas_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	int i;
	int cant, conts;

	if(fichas_out() != TEG_STATUS_SUCCESS)  {

		fichas_get_wanted(&cant, &conts);
		armies_view(cant, conts);

		for(i=0; i<COUNTRIES_CANT; i++) {
			G_country_draw_ejer(i);
		}
	} else {
		armies_unview();
	}
	set_sensitive_tb();
}

void on_updatemap_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	out_countries();
}

void on_enviarreattack_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	attackre_out();
}

void on_enviarattackreset_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	if(attack_pre_reset() == TEG_STATUS_SUCCESS) {
		textmsg(M_INF, _("The attack was reset. Please, select the source country to continue your attack"));
	}
}

void on_reagrupe_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	if(reagrupe_init() == TEG_STATUS_SUCCESS) {
		if(gui_private.dialog_show & (1 <<DIALOG_REGROUP_ARMIES)) {
			teg_dialog(_("Regrouping armies"), _("Regrouping armies"),
			           _("Select two countries:\n1st: click on the source country\n2nd: click on the destination country"));
		}
	}
}

void on_pedirtarjeta_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	out_tarjeta();
	set_sensitive_tb();
}

void on_endturn_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	out_endturn();
	set_sensitive_tb();
}

void on_surrender_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	GtkWidget *dialog;
	gint response;

	dialog = gtk_message_dialog_new(GTK_WINDOW(main_window),
	                                GTK_DIALOG_DESTROY_WITH_PARENT,
	                                GTK_MESSAGE_INFO,
	                                GTK_BUTTONS_YES_NO,
	                                _("Really surrender ?"));

	response = gtk_dialog_run(GTK_DIALOG(dialog));
	if(response == GTK_RESPONSE_YES) {
		out_surrender();
	}

	gtk_widget_destroy(dialog);
}


void on_start_activate(GtkMenuItem  *menuitem, gpointer         user_data)
{
	gametype_view();
}


void on_status_activate(GtkMenuItem  *menuitem, gpointer         user_data)
{
	status_view();
}

void on_viewcards_activate(GtkMenuItem  *menuitem, gpointer         user_data)
{
	cards_view(-1);
}

void on_viewdices_activate(GtkMenuItem  *menuitem, gpointer         user_data)
{
	dices_view();
}

void on_viewmission_activate(GtkMenuItem  *menuitem, gpointer         user_data)
{
	mission_view();
}


void on_preferences1_activate(GtkMenuItem     *menuitem,
                              gpointer         user_data)
{
	preferences_activate();
}


void on_about_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	GdkPixbuf *pixbuf = NULL;
	const gchar *documenters[] = {
		"Ricardo Quesada, english",
		"Arkadiusz Lipiec, polish",
		NULL
	};
	const gchar *translator_credits = _("translator_credits");
	const char *authors[] = {
		"Ricardo Quesada, main coder",
		"",
		"Main Contributors:",
		"    Thomas R. Koll",
		"    Raymond Ostertag",
		"",
		"Detailed info in the PEOPLE file or in the homepage",
		"",
		NULL
	};
	const char *artists[] = {
		"Wolfgang Morawetz, main artist",
		NULL
	};

	{
		gchar* logo_filename = NULL;
		logo_filename = g_strdup(PIXMAPDIR"/teg_icono.png");

		if(logo_filename != NULL) {
			pixbuf = gdk_pixbuf_new_from_file(logo_filename, NULL);
			g_free(logo_filename);
		}
	}

	gtk_show_about_dialog(GTK_WINDOW(main_window),
	                      "program-name", _("Tenes Empanadas Graciela"),
	                      "version", VERSION,
	                      "copyright",
	                      _("Copyright (C) 2000, 2002 Ricardo Quesada"),
	                      "comments",
	                      _("A clone of T.E.G. (a Risk clone)."),
	                      "authors", authors,
	                      "documenters", documenters,
	                      "artists", artists,
	                      "translator-credits", translator_credits,
	                      "website", "https://github.com/wfx/teg",
	                      "website-label", _("TEG Home Page"),
	                      "logo", pixbuf,
	                      NULL);
}

void on_help_activate(GtkMenuItem *item, gpointer user_data)
{
	gtk_show_uri_on_window(GTK_WINDOW(main_window), "ghelp:teg",
	                       GDK_CURRENT_TIME, NULL);
}
