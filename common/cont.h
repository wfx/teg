/*	$Id: cont.h,v 1.10 2002/09/09 03:52:06 riq Exp $	*/
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
 * @file cont.h
 */

#ifndef __TEG_CONT_H
#define __TEG_CONT_H

typedef enum {
	CONTINENTE_AMERICASUR,
	CONTINENTE_AMERICANORTE,
	CONTINENTE_AFRICA,
	CONTINENTE_OCEANIA,
	CONTINENTE_EUROPA,
	CONTINENTE_ASIA,
	CONTINENTE_LAST,
} CONTINENTE, *PCONTINENTE;

#define		CONT_CANT	CONTINENTE_LAST

#define CONT_AMERICASUR_CANT	6
#define CONT_AMERICANORTE_CANT	10
#define CONT_AFRICA_CANT	6
#define CONT_OCEANIA_CANT	4
#define CONT_EUROPA_CANT	9
#define CONT_ASIA_CANT		15

typedef struct _cont {
	char	*name;		/**< name del continenete */
	CONTINENTE id;			/**< id del continente */
	int	cant_countries;		/**< cantidad de countries que tiene el continenete */
	int	fichas_x_cont;		/**< cantidad de fichas adicionales por tener el cont*/
} CONT, *PCONT;

extern CONT g_conts[];

/* returns the total number of continents */
int cont_tot( unsigned long conts );

/* returns the name of a country */
char * cont_get_name( int cont_number );

#endif /* __TEG_CONT_H */
