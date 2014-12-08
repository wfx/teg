/*	$Id: misc.h,v 1.18 2002/09/02 03:20:10 riq Exp $	*/
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
 * @file misc.h
 */
#ifndef __TEGC_MISC_H
#define __TEGC_MISC_H

TEG_STATUS game_init();
TEG_STATUS game_finalize();
TEG_STATUS teg_connect();
TEG_STATUS teg_disconnect();
TEG_STATUS playerid_restore_from_error( void );
TEG_STATUS launch_server( int port );
TEG_STATUS launch_robot( void );
TEG_STATUS textmsg( int level, char *format, ...);
TEG_STATUS dirs_create();

#define ESTADO_ES(a) (g_game.estado==(a))
#define ESTADO_MENOR(a) (g_game.estado<(a))
#define ESTADO_MAYOR(a) (g_game.estado>(a))
#define ESTADO_MENOR_IGUAL(a) (g_game.estado<=(a))
#define ESTADO_MAYOR_IGUAL(a) (g_game.estado>=(a))

#define WHOAMI() g_game.numjug
#define ESTADO_GET() g_game.estado
#define ESTADO_SET(a) (g_game.estado=(a))

#endif /* __TEGC_MISC_H */
