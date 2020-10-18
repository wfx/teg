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
 * @file aux.h
 * funciones auxiliares del server
 */

#ifndef __TEGS_AUX_H
#define __TEGS_AUX_H

typedef int (*get_random_func)(int i);

TEG_STATUS aux_token_fichas( int fd, char *str, int maximo, unsigned long conts );
TEG_STATUS aux_token_stasta(char *strout, size_t buflen );
TEG_STATUS aux_token_attack( int src, int dst, int *src_lost, int *dst_lost, char *datos_src, char *dados_dst );
void aux_token_fichasc( PSPLAYER pJ );
TEG_STATUS aux_token_countries( PSPLAYER pJ, char *buf, int buflen );

PCOUNTRY get_random_country( get_random_func func);

void color_init();
BOOLEAN color_libre( int *color );
void color_del( int i );
TEG_STATUS colores_libres( char *c );

TEG_STATUS fichas_next( void );
TEG_STATUS fichas2_next( void );
TEG_STATUS fichasc_next( void );

TEG_STATUS aux_find_inaddr( PSPLAYER pJ );

TEG_STATUS launch_robot( int *robot_socket, char *mode );

#endif /* __TEGS_AUX_H */
