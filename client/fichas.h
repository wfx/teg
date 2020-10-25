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
 * @file fichas.h
 *
 * Everything related to the placement of armies.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "country.h"

TEG_STATUS fichas_add( PCOUNTRY p );
TEG_STATUS fichas_sub( PCOUNTRY p );
TEG_STATUS fichas_finish( int **ptr );
TEG_STATUS fichas_init(int cant, int conts);
TEG_STATUS fichas_reset();
TEG_STATUS fichas_restore_from_error();
TEG_STATUS fichas2_restore_from_error();
TEG_STATUS fichasc_restore_from_error();
TEG_STATUS fichas_out();
void fichas_add_wanted( int i );
TEG_STATUS fichas_enter( PCOUNTRY p );
TEG_STATUS fichas_leave( PCOUNTRY p );
TEG_STATUS fichas_get_wanted( int *cant, int *conts );

#ifdef __cplusplus
}
#endif
