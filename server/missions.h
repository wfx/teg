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
 * @file missions.h
 */

#ifndef __TEGS_MISSIONS_H
#define __TEGS_MISSIONS_H

TEG_STATUS mission_chequear( PSPLAYER pJ );
TEG_STATUS mission_asignar( PSPLAYER pJ );
TEG_STATUS mission_init();
TEG_STATUS mission_set( int a );
TEG_STATUS mission_common_mission( int a );

#endif /* __TEGS_MISSIONS_H */
