/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <rquesada@core-sdi.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; only version 2 of the License.
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

#pragma once

#include <sys/types.h>

int net_connect_unix(char *path );
#ifdef __cplusplus
extern "C" {
#endif

ssize_t net_readline(int sock, void *gs,size_t maxlen );
int net_printf(int sock, char *format, ...);
int net_print(int sock, char *msg);
int net_listen(const char *host, int port );
int net_connect_tcp( const char *host, int port );
int net_close( int s );
ssize_t net_read( int s, void *buf, size_t maxlen );

#ifdef __cplusplus
}
#endif
