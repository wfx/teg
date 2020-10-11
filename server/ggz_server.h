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
/*
 * helpers for the GGZ server side code
 */

#ifndef __TEGS_GGZ_SERVER_H
#define __TEGS_GGZ_SERVER_H

int ggz_server_main_loop();
TEG_STATUS ggz_server_find_ggzname( int fd, char *n, int len );
void ggz_server_gameover( int fd );

#endif /* __TEGS_TEGGGZ_H */
