/*	$Id: missions.h,v 1.3 2002/09/09 03:52:06 riq Exp $	*/
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
/**
 * @file missions.h
 */

#ifndef __TEG_MISSIONS_H
#define __TEG_MISSIONS_H

typedef struct _missions {
	int	numjug;				/**< a quien se asigno el mission */
	char	*name;			/**< name del mission */
	int	continentes[CONTINENTE_LAST];	/**< continentes a conquistar */
	int	jugadores[TEG_MAX_PLAYERS];	/**< jugadores a eliminar */
	int	limitrofes;			/**< countries limitrofes a tener */
	int	tot_countries;			/**< cantidad de countries a conquistar */
} MISSIONS, *PMISSIONS;

enum {
	MISSION_CONQWORLD,			/**< mission 0. Conquistar el mundo */
	MISSION_COMMON,			/**< mission comun: Conquistar 30 countries */
};

extern MISSIONS g_missions[];

/* returns the total number of missions that exists */
int missions_cant();

/* return the name of a mission (translated) */
char * missions_get_name( int number );

#endif /* __TEG_MISSIONS_H */
