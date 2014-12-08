/*	$Id: reagrupe.h,v 1.6 2002/08/31 17:45:57 riq Exp $	*/
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
 * @file reagrupe.h
 * Contiene algunas funciones auxiliares para el manejo del brief estado 'ESTADO_REAGRUPE'
 */

#ifndef __TEGC_REAGRUPE_H
#define __TEGC_REAGRUPE_H

void reagrupe_reset( void );
TEG_STATUS reagrupe_init( void );
TEG_STATUS reagrupe_click( PCOUNTRY p );
TEG_STATUS reagrupe_check( void );
TEG_STATUS reagrupe_set_and_save( int src, int dst, int cant );
void reagrupe_bigreset( void );
TEG_STATUS reagrupe_restore_from_error( void );
TEG_STATUS reagrupe_out( int src, int dst, int cant);
TEG_STATUS reagrupe_enter( PCOUNTRY p );
TEG_STATUS reagrupe_leave( PCOUNTRY p );

#endif /* __TEGC_REAGRUPE_H */
