/*	$Id: ai.h,v 1.9 2006/03/13 22:55:51 nordi Exp $	*/
/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <riq@core-sdi.com>
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
 * @file ai.h
 * Inteligecia artificial del robot
 */

#ifndef __TEGR_AI_H
#define __TEGR_AI_H

extern int ai_puntaje[COUNTRIES_CANT*10];
extern int ai_sorted[COUNTRIES_CANT*10];

TEG_STATUS ai_fichas( int cant );
TEG_STATUS ai_fichasc( int cant, int conts);
TEG_STATUS ai_init();
TEG_STATUS ai_turno();
TEG_STATUS ai_reagrupe();
TEG_STATUS ai_tropas( int src, int dst, int cant);
TEG_STATUS ai_puedocanje( int *p1, int *p2, int *p3 );
BOOLEAN ai_own_continent( int c );
TEG_STATUS ai_puntaje_sort(int cant);
TEG_STATUS ai_puntaje_clean();

#endif /* __TEGR_AI_H */
