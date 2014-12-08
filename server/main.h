/*	$Id: main.h,v 1.8 2002/04/28 16:59:41 riq Exp $	*/
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
 * @file main.h
 */

#ifndef __TEGS_MAIN_H
#define __TEGS_MAIN_H

/**! initialize the game */
void game_init();

/**! initialize a new game */
void game_new();

/**! called when a game in finish */
void game_end();

/**! when the game is over */
BOOLEAN game_is_finished( void );

void server_init();

/**! removes and closes an fd from the list of fd */
void fd_remove( int fd );

/**! adds a new fd to the list of fd */
void fd_add( int fd );

#endif /* __TEGS_MAIN_H */
