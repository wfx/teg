/*	$Id: priv.c,v 1.8 2002/10/14 03:26:36 riq Exp $	*/
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gtk/gtk.h>

#include "gui.h"
#include "client.h"

#include "interface.h"
#include "priv.h"

struct _gui_private gui_private;

/* Prefs */
GSettings *settings = NULL;

TEG_STATUS priv_init( void )
{
	gui_private.tag = -1;
	gui_private.tag_ggz = -1;
	gui_private.msg_show_colors
	  = g_settings_get_boolean( settings, "msgshow-with-color" );
	gui_private.country_i_am = -1;

	return TEG_STATUS_SUCCESS;
}
