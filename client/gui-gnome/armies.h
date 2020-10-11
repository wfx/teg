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
#ifndef __GUI_GNOME_ARMIES_H
#define __GUI_GNOME_ARMIES_H

typedef struct _armies_pos {
	int cant;
	int orig_cant;
	GooCanvasItem *armies_text;
} ARMIES_POS, *PARMIES_POS;

TEG_STATUS armies_add(int country);
TEG_STATUS armies_del(int country);
TEG_STATUS armies_view(int cant, int conts);
TEG_STATUS armies_view_more( int cant );
TEG_STATUS armies_unview();
TEG_STATUS armies_init();

#endif /* __GUI_GNOME_ARMIES_H */
