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

#include <functional>

#include "tarjeta.h"
#include "cont.h"

constexpr std::size_t COUNTRYNAME_MAX_LEN{50};
constexpr std::size_t COUNTRIES_CANT{50};

enum CountrySelectionStatusDetail: unsigned {
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

using CountrySelectionStatus = std::underlying_type<CountrySelectionStatusDetail>::type;

struct COUNTRY {
	COUNTRY();
	COUNTRY(CountryId id, char const* name, CONTINENTE continente, TARJTIPO kindOfCard);

	CountryId id;					/**< numero de country */
	char const *name;				/**< name del country*/
	int numjug=-1;				/**< Number of owning player */
	int ejercitos=0;				/**< cant de ejercitos */
	int ejer_reagrupe=0;			/**< cant de ejercitos pasados en reagru */
	CONTINENTE continente;			/**< continente al que pertenece */
	TARJETA tarjeta;			/**< tipo de dibujo de tarjeta */
	CountrySelectionStatus selected = COUNTRY_SELECT_NONE;	/**< util para la gui. Dice si esta seleccionado */
};
using PCOUNTRY=COUNTRY*;


/*
 * Funciones, variables exportadas
 */
extern COUNTRY g_countries[];

bool countries_eslimitrofe(int a, int b);
void countries_initcountry(PCOUNTRY p);
void countries_init();
bool country_libre(int i);

/// \brief decides if the country number is valid.
bool countrynumber_is_valid(int country_number);

/**! returns the name of 'country' */
const char *countries_get_name(int country);

using CountryMapFn = std::function<void(COUNTRY&)>;
using InterruptableCountryMapFn = std::function<bool(COUNTRY&)>;

void countries_map(CountryMapFn fn);
void countries_map(int player, CountryMapFn fn);
void countries_map_int(int player, InterruptableCountryMapFn fn);


COUNTRY& COUNTRY_FROM_TARJETA(TARJETA& tarjeta);
