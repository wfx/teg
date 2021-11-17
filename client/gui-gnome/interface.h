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

#pragma once

#include <gtk/gtk.h>

#include "../../common/common.h"

namespace teg::client::callbacks
{

extern GtkTextView *main_message_area;
extern GtkWidget *text_scrollbar;
extern GtkWidget *main_window;
extern GtkWidget *toolbar_main;
extern GtkWidget *statusbar;
extern GtkWidget *canvas_map;

GtkWidget* create_mainwin(void);
int create_canvas_items(GtkWidget *canvas);
TEG_STATUS set_sensitive_tb(void);
void tropas_window(int src, int dst, int cant);
void reagrupe_window(int src, int dst, int cant);

}
