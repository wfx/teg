/*	$Id: cards.h,v 1.8 2002/08/31 17:45:58 riq Exp $	*/
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
#ifndef __TEG_GUI_GNOME_CARDS_H
#define __TEG_GUI_GNOME_CARDS_H

void cards_view( int country);
void cards_update( void );
void cards_update_para_canje( void );
void cards_delete( int p1, int p2, int p3 );
void cards_flush();
void cards_free();
TEG_STATUS cards_select(int p1, int p2, int p3 );

#endif  /* __TEG_GUI_GNOME_CARDS_H */
