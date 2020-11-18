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

#include "tarjeta.h"
#include "cont.h"

#define COUNTRYNAME_MAX_LEN	50
#define COUNTRIES_CANT		50

typedef struct _country {
	LIST_ENTRY next;
	int id;					/**< numero de country */
	char const *name;				/**< name del country*/
	int numjug;				/**< numjug */
	int ejercitos;				/**< cant de ejercitos */
	int ejer_reagrupe;			/**< cant de ejercitos pasados en reagru */
	CONTINENTE continente;			/**< continente al que pertenece */
	TARJETA tarjeta;			/**< tipo de dibujo de tarjeta */
	int selected;				/**< util para la gui. Dice si esta seleccionado */
} COUNTRY, *PCOUNTRY;

enum {
	COUNTRY_SELECT_NONE = 0,
	COUNTRY_SELECT_FICHAS_IN = 1 << 0,
	COUNTRY_SELECT_FICHAS_OUT = 1 << 1,
	COUNTRY_SELECT_ATTACK_ENTER = 1 << 2,
	COUNTRY_SELECT_ATTACK = 1 << 3,
	COUNTRY_SELECT_REGROUP_ENTER = 1 << 4,
	COUNTRY_SELECT_REGROUP = 1 << 5,
	COUNTRY_SELECT_ATTACK_SRC = 1 << 6,
	COUNTRY_SELECT_ATTACK_DST = 1 << 7,
};

/*
 * Funciones, variables exportadas
 */
extern COUNTRY g_countries[];

BOOLEAN countries_eslimitrofe(int a, int b);
void countries_initcountry(PCOUNTRY p);
void countries_init();
BOOLEAN country_libre(int i);

/// \brief decides if the country number is valid.
BOOLEAN countrynumber_is_valid(int country_number);

/**! returns the name of 'country' */
const char *countries_get_name(int country);
