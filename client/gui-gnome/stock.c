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
#include <gnome.h>
#include "stock.h"


#include "stock/stock-getcard.xpm"
#include "stock/stock-sendarmies.xpm"
#include "stock/stock-endturn.xpm"
#include "stock/stock-viewplayers.xpm"


/* from libgnomeui2/gnome-stock-icons.c */
static void add_sized (GtkIconFactory *factory,
           const char   **xpm_data,
           GtkIconSize     size,
           const gchar    *stock_id)
{
	GtkIconSet *set;
	GtkIconSource *source;
	GdkPixbuf *pixbuf;

	pixbuf = gdk_pixbuf_new_from_xpm_data(xpm_data);

	source = gtk_icon_source_new ();
	gtk_icon_source_set_pixbuf (source, pixbuf);
	gtk_icon_source_set_size (source, size);

	set = gtk_icon_set_new ();                
	gtk_icon_set_add_source (set, source);

	gtk_icon_factory_add (factory, stock_id, set);

	g_object_unref (G_OBJECT (pixbuf));
	gtk_icon_source_free (source);
	gtk_icon_set_unref (set);
}

void stock_init (void)
{
	static gboolean initialized = FALSE;
	GtkIconFactory *factory;

	static GtkStockItem entries[] ={
		{ STOCK_GETCARD, N_("Get Card"), 0, 0, GETTEXT_PACKAGE },
		{ STOCK_SENDARMIES, N_("Send Armies"), 0, 0, GETTEXT_PACKAGE },
		{ STOCK_ENDTURN, N_("End Turn"), 0, 0, GETTEXT_PACKAGE },
		{ STOCK_VIEWPLAYERS, N_("View Players"), 0, 0, GETTEXT_PACKAGE }
	};

	if (initialized)
		return;
	else
		initialized = TRUE;

    	factory = gtk_icon_factory_new ();
	add_sized (factory, stock_getcard_xpm, GTK_ICON_SIZE_BUTTON, STOCK_GETCARD);
	add_sized (factory, stock_sendarmies_xpm, GTK_ICON_SIZE_BUTTON, STOCK_SENDARMIES);
	add_sized (factory, stock_endturn_xpm, GTK_ICON_SIZE_BUTTON, STOCK_ENDTURN);
	add_sized (factory, stock_viewplayers_xpm, GTK_ICON_SIZE_BUTTON, STOCK_VIEWPLAYERS);

	gtk_icon_factory_add_default (factory);
	gtk_stock_add_static( entries, G_N_ELEMENTS(entries) );
}
