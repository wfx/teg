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
 * @file console.h
 * Prototipos de las funciones de la consola
 */
#ifndef __TEGS_CONSOLE_H
#define __TEGS_CONSOLE_H

#define CONSOLE_FD 0

TEG_STATUS console_handle( int fd );
void con_show_prompt();
TEG_STATUS con_text_out( int level, char *format, ...);
TEG_STATUS con_text_out_wop( int level, char *format, ...);
TEG_STATUS console_init( void );
TEG_STATUS console_quit( void );


#endif /* __TEGS_CONSOLE_H */
