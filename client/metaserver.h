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
 * metaserver client functions
 */

#ifndef __TEGC_METASERVER_H
#define __TEGC_METASERVER_H

#define META_MAX_NAME	(100)
#define META_MAX_COMMENT	(200)
#define META_VERSION_MAJOR	(1)
#define META_VERSION_MINOR	(0)

extern LIST_ENTRY g_list_metaserver;

typedef struct _metaserver_entry {
	LIST_ENTRY next;
	char name[META_MAX_NAME+1];
	int port;
	char version[META_MAX_NAME+1];
	char comment[META_MAX_COMMENT+1];
} METASERVER, *PMETASERVER;

TEG_STATUS metaserver_init();

TEG_STATUS metaserver_get_servers( void );


#endif /* __TEGC_METASERVER_H */

