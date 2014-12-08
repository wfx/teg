/*	$Id: locate_country.h,v 1.3 2002/08/31 17:45:59 riq Exp $	*/
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
 * @file cards.h
 */
#ifndef __TEG_GUI_GNOME_LOCATE_COUNTRY_H
#define __TEG_GUI_GNOME_LOCATE_COUNTRY_H

/* constructor */
TEG_STATUS locate_country_init();

/* destructor */
TEG_STATUS locate_country_exit();

/* adds a country to view */
TEG_STATUS locate_country_add_country( PCOUNTRY p );

/* adds the country's army to view */
TEG_STATUS locate_country_add_army( PCOUNTRY p );

#endif /* __TEG_GUI_GNOME_LOCATE_COUNTRY_H */
