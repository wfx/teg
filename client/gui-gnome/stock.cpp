/* Eye of Gnome image viewer - stock icons
 *
 * Copyright (C) 1999 The Free Software Foundation
 *
 * Author: Federico Mena-Quintero <federico@gimp.org>
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

#include <config.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "stock.h"

extern "C" {
#include "stock_gresources.h"
}

namespace teg::client::callbacks
{

void stock_init(void)
{
	GtkIconTheme *theme;

	stock_register_resource();
	theme = gtk_icon_theme_get_default();
	gtk_icon_theme_add_resource_path(theme, "/net/sf/teg/icons");
}

}
