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
#ifndef __GUI_GNOME_CALLBACKS_H
#define __GUI_GNOME_CALLBACKS_H

#include <gtk/gtk.h>
#include "gui.h"
#include "client.h"

void on_connect_activate(GtkMenuItem     *menuitem, gpointer         user_data);

void on_disconnect_activate(GtkMenuItem     *menuitem, gpointer         user_data);

void on_scores_activate(GtkMenuItem *widget, gpointer user_data);

void on_exit_activate(GtkWidget	*widget, gpointer user_data);

void on_enviarfichas_activate (GtkMenuItem *menuitem, gpointer user_data);

void on_reagrupe_activate (GtkMenuItem *menuitem, gpointer user_data);

void on_updatemap_activate (GtkMenuItem *menuitem, gpointer user_data);

void on_enviarattack_activate (GtkMenuItem *menuitem, gpointer user_data);

void on_enviarreattack_activate (GtkMenuItem *menuitem, gpointer user_data);

void on_enviarattackreset_activate (GtkMenuItem *menuitem, gpointer user_data);

void on_pedirtarjeta_activate (GtkMenuItem *menuitem, gpointer user_data);

void on_endturn_activate (GtkMenuItem *menuitem, gpointer user_data);

void on_launchrobot_activate (GtkMenuItem *menuitem, gpointer user_data);

void on_properties1_activate(GtkMenuItem     *menuitem, gpointer         user_data);

void on_preferences1_activate(GtkMenuItem     *menuitem, gpointer         user_data);

void on_surrender_activate(GtkMenuItem *menuitem, gpointer user_data);

void on_about_activate(GtkMenuItem     *menuitem, gpointer         user_data);

void on_status_activate(GtkMenuItem  *menuitem, gpointer         user_data);

void on_viewcards_activate(GtkMenuItem  *menuitem, gpointer         user_data);

void on_viewdices_activate(GtkMenuItem  *menuitem, gpointer         user_data);

void on_viewmission_activate(GtkMenuItem  *menuitem, gpointer         user_data);

void on_start_activate(GtkMenuItem  *menuitem, gpointer         user_data);

void on_help_activate(GtkMenuItem *menuitem, gpointer user_data);

void destroy_window( GtkWidget * widget, GtkWidget **window );

void raise_and_focus (GtkWidget *widget);

gboolean pre_client_recv(GIOChannel *source, GIOCondition cond, gpointer data);

#endif /* __GUI_GNOME_CALLBACKS_H */
