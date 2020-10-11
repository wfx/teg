/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2002 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <riq@corest.com>
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
 * ggz client code
 */

#ifndef __TEGC_GGZ_CLIENT_H
#define __TEGC_GGZ_CLIENT_H

/* only compiled if WITH_GGZ is defined */
void ggz_client_init();
int ggz_client_connect();
int ggz_client_disconnect();
int ggz_client_get_fd();
void ggz_client_handle( void *, void *);

#endif /* __TEGC_GGZ_CLIENT_H */
