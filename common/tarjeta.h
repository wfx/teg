/*	$Id: tarjeta.h,v 1.12 2002/08/31 17:45:59 riq Exp $	*/
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
 * @file tarjeta.h
 */

#ifndef __TEG_TARJETA_H
#define __TEG_TARJETA_H 

#include "all.h"

#define TARJETA_NULL { {NULL,NULL}, 0, TRUE, FALSE }
#define TARJETA_NULL_COMODIN { {NULL,NULL}, TARJ_COMODIN, -1, FALSE }
#define TARJETA_NULL_GALEON { {NULL,NULL}, TARJ_GALEON, -1, FALSE }
#define TARJETA_NULL_CANION { {NULL,NULL}, TARJ_CANION, -1, FALSE }
#define TARJETA_NULL_GLOBO { {NULL,NULL}, TARJ_GLOBO, -1, FALSE }

typedef enum {
	TARJ_GALEON = 1,		/**< valor del galeon */
	TARJ_CANION = 4,		/**< valor del canion */
	TARJ_GLOBO = 8,			/**< valor del globo */
	TARJ_COMODIN = 32,		/**< valor del comodin */
} TARJTIPO, *PTARJTIPO;

typedef struct _tarjeta {
	LIST_ENTRY next;		/**< siguiente */
	TARJTIPO tarjeta;		/**< tipo de tarjeta */
	int	numjug;			/**< si la tiene algun jugador y que jugador */
	BOOLEAN usada;			/**< si se pusieron las 2 fichas del country */
} TARJETA, *PTARJETA;

#define COUNTRY_FROM_TARJETA( pT ) ( ((char*)(pT)) - (( ((char*)&((PCOUNTRY)pT)->tarjeta) - ((char*)pT) )))

/* 
 * Funciones y varialbes exportadas
 */
BOOLEAN tarjeta_puedocanje( int numjug, int t1, int t2, int t3 );
void tarjeta_init( void );
void tarjeta_inittarj( PTARJETA t );
void tarjeta_poner( PTARJETA t );
void tarjeta_sacar( PTARJETA t, int numjug );
void tarjeta_usar( PTARJETA pT );
void tarjeta_desusar( PTARJETA pT );
int tarjeta_es_libre( int i );
BOOLEAN tarjeta_es_usada( PTARJETA pT );

#endif /* __TEG_TARJETA_H */
