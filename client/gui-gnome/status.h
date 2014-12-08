/*	$Id: status.h,v 1.11 2002/09/13 05:38:59 riq Exp $	*/
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
 * @file status.h
 */
#ifndef __GUI_GNOME_STATUS_H
#define __GUI_GNOME_STATUS_H

enum {
	STATUS_COLUMN_COLOR,
	STATUS_COLUMN_NUMBER,
	STATUS_COLUMN_NAME,
	STATUS_COLUMN_SCORE,
	STATUS_COLUMN_ADDR,
	STATUS_COLUMN_HUMAN,
	STATUS_COLUMN_COUNTRIES,
<<<<<<< HEAD
	STATUS_COLUMN_COUNTRIES_WON,
	STATUS_COLUMN_COUNTRIES_LOST,
	STATUS_COLUMN_COUNTRIES_RATIO,
	STATUS_COLUMN_ARMIES,
	STATUS_COLUMN_ARMIES_KILLED,
	STATUS_COLUMN_ARMIES_LOST,
	STATUS_COLUMN_ARMIES_RATIO,
=======
	STATUS_COLUMN_ARMIES,
>>>>>>> b8e1f4d5000e931e81e397369fa5ec789dffe26c
	STATUS_COLUMN_CARDS,
	STATUS_COLUMN_STATUS,
	STATUS_COLUMN_WHO,

	STATUS_COLUMN_LAST,
};

extern GtkWidget *status_dialog;
extern GtkWidget *status_clist;

TEG_STATUS status_fill_clist( GtkWidget *clist );

/**! to view the status */
void status_view();

/**! update the status dialog */
TEG_STATUS status_update_dialog();

/* mini */
GtkWidget *ministatus_build();
TEG_STATUS ministatus_update();

/**! Creates the widget for the Main Status */
TEG_STATUS mainstatus_create( GtkWidget **window );

/**! updated the contents of the mainstatus */
TEG_STATUS mainstatus_update();

/**! update the colors */
TEG_STATUS mainstatus_update_colors();

/**!  update the visibility of the status columns */
TEG_STATUS status_update_visibility_of_columns( void );

#endif /* __GUI_GNOME_STATUS_H */
