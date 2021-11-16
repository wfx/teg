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

#include "country.h"

#include <stdexcept>

#include "fcintl.h"

COUNTRY g_countries[] = {
	/* AMERICA DEL SUR */
	COUNTRY{0, N_("Argentina"), CONTINENTE_AMERICASUR, TARJETA_NULL_COMODIN },
	COUNTRY{1, N_("Brazil"), CONTINENTE_AMERICASUR, TARJETA_NULL_GALEON },
	COUNTRY{2, N_("Chile"), CONTINENTE_AMERICASUR, TARJETA_NULL_GLOBO },
	COUNTRY{3, N_("Colombia"), CONTINENTE_AMERICASUR, TARJETA_NULL_GLOBO },
	COUNTRY{4, N_("Peru"), CONTINENTE_AMERICASUR, TARJETA_NULL_GALEON },
	COUNTRY{5, N_("Uruguay"), CONTINENTE_AMERICASUR, TARJETA_NULL_GLOBO },

	/* AMERICA DEL NORTE */
	COUNTRY{ 6, N_("Mexico"), CONTINENTE_AMERICANORTE, TARJETA_NULL_CANION },
	COUNTRY{ 7, N_("California"), CONTINENTE_AMERICANORTE, TARJETA_NULL_CANION },
	COUNTRY{ 8, N_("Oregon"), CONTINENTE_AMERICANORTE, TARJETA_NULL_CANION },
	COUNTRY{ 9, N_("New York"), CONTINENTE_AMERICANORTE, TARJETA_NULL_GALEON },
	COUNTRY{10, N_("Alaska"), CONTINENTE_AMERICANORTE, TARJETA_NULL_GALEON },
	COUNTRY{11, N_("Yukon"), CONTINENTE_AMERICANORTE, TARJETA_NULL_GLOBO },
	COUNTRY{12, N_("Canada"), CONTINENTE_AMERICANORTE, TARJETA_NULL_CANION },
	COUNTRY{13, N_("Terranova"), CONTINENTE_AMERICANORTE, TARJETA_NULL_CANION },
	COUNTRY{14, N_("Labrador"), CONTINENTE_AMERICANORTE, TARJETA_NULL_CANION },
	COUNTRY{15, N_("Greenland"), CONTINENTE_AMERICANORTE, TARJETA_NULL_GALEON },

	/* AFRICA */
	COUNTRY{16, N_("Sahara"), CONTINENTE_AFRICA, TARJETA_NULL_CANION },
	COUNTRY{17, N_("Zaire"), CONTINENTE_AFRICA, TARJETA_NULL_GALEON },
	COUNTRY{18, N_("Etiopia"), CONTINENTE_AFRICA, TARJETA_NULL_GLOBO },
	COUNTRY{19, N_("Egypt"), CONTINENTE_AFRICA, TARJETA_NULL_GLOBO },
	COUNTRY{20, N_("Madagascar"), CONTINENTE_AFRICA, TARJETA_NULL_GALEON },
	COUNTRY{21, N_("South Africa"), CONTINENTE_AFRICA, TARJETA_NULL_CANION },

	/* OCEANIA */
	COUNTRY{22, N_("Australia"), CONTINENTE_OCEANIA, TARJETA_NULL_CANION },
	COUNTRY{23, N_("Borneo"), CONTINENTE_OCEANIA, TARJETA_NULL_GALEON },
	COUNTRY{24, N_("Java"), CONTINENTE_OCEANIA, TARJETA_NULL_CANION },
	COUNTRY{25, N_("Sumatra"), CONTINENTE_OCEANIA, TARJETA_NULL_GLOBO },

	/* EUROPA */
	COUNTRY{26, N_("Spain"), CONTINENTE_EUROPA, TARJETA_NULL_GLOBO },
	COUNTRY{27, N_("France"), CONTINENTE_EUROPA, TARJETA_NULL_GLOBO },
	COUNTRY{28, N_("Germany"), CONTINENTE_EUROPA, TARJETA_NULL_GALEON },
	COUNTRY{29, N_("Italy"), CONTINENTE_EUROPA, TARJETA_NULL_GLOBO },
	COUNTRY{30, N_("Poland"), CONTINENTE_EUROPA, TARJETA_NULL_CANION },
	COUNTRY{31, N_("Russia"), CONTINENTE_EUROPA, TARJETA_NULL_GLOBO },
	COUNTRY{32, N_("Sweden"), CONTINENTE_EUROPA, TARJETA_NULL_GALEON },
	COUNTRY{33, N_("Great Britain"), CONTINENTE_EUROPA, TARJETA_NULL_GALEON },
	COUNTRY{34, N_("Iceland"), CONTINENTE_EUROPA, TARJETA_NULL_GALEON },

	/* ASIA */
	COUNTRY{35, N_("Arabia"), CONTINENTE_ASIA, TARJETA_NULL_CANION },
	COUNTRY{36, N_("Israel"), CONTINENTE_ASIA, TARJETA_NULL_GALEON },
	COUNTRY{37, N_("Turkey"), CONTINENTE_ASIA, TARJETA_NULL_GALEON },
	COUNTRY{38, N_("India"), CONTINENTE_ASIA, TARJETA_NULL_GLOBO },
	COUNTRY{39, N_("Malaysia"), CONTINENTE_ASIA, TARJETA_NULL_CANION },
	COUNTRY{40, N_("Iran"), CONTINENTE_ASIA, TARJETA_NULL_GLOBO },
	COUNTRY{41, N_("Gobi"), CONTINENTE_ASIA, TARJETA_NULL_GLOBO },
	COUNTRY{42, N_("China"), CONTINENTE_ASIA, TARJETA_NULL_GALEON },
	COUNTRY{43, N_("Mongolia"), CONTINENTE_ASIA, TARJETA_NULL_GALEON },
	COUNTRY{44, N_("Siberia"), CONTINENTE_ASIA, TARJETA_NULL_GALEON },
	COUNTRY{45, N_("Aral"), CONTINENTE_ASIA, TARJETA_NULL_CANION },
	COUNTRY{46, N_("Tartary"), CONTINENTE_ASIA, TARJETA_NULL_CANION },
	COUNTRY{47, N_("Taymir"), CONTINENTE_ASIA, TARJETA_NULL_COMODIN },
	COUNTRY{48, N_("Katchatka"), CONTINENTE_ASIA, TARJETA_NULL_GLOBO },
	COUNTRY{49, N_("Japan"), CONTINENTE_ASIA, TARJETA_NULL_CANION }
};
constexpr std::size_t COUNTRIES_TOT = std::extent<decltype(g_countries)>::value;

COUNTRY::COUNTRY()
	: COUNTRY(-1, "INVALID", CONTINENTE_LAST, TARJTIPO(0))
{
}

COUNTRY::COUNTRY(CountryId id, char const* name, CONTINENTE continente, TARJTIPO kindOfCard)
	: id{id}
	, name{name}
	, continente{continente}
	, tarjeta{kindOfCard, -1, false, id}
{
}

#include "limitrof.h"

/* returns true if country a and b are border */
bool countries_eslimitrofe(int a, int b)
{
	if(a > b) {
		return(mat_ady[b][a]==1);
	} else {
		return(mat_ady[a][b]==1);
	}
}

/* Initializes country p */
void countries_initcountry(PCOUNTRY p)
{
	p->numjug=-1;
	p->ejercitos=0;
	tarjeta_inittarj(&p->tarjeta);
	p->selected = COUNTRY_SELECT_NONE;
}

/* initializes all the countries */
void countries_init()
{
	for(size_t i=0; i<COUNTRIES_CANT; i++) {
		countries_initcountry(&g_countries[i]);
	}
}

/* says if a country is free or not */
bool country_libre(int i)
{
	return(g_countries[i].numjug == -1);
}

/* returns the name of a country */
char const * countries_get_name(int i)
{
	if(i >= 0 && i < COUNTRIES_CANT) {
		return _(g_countries[i].name);
	} else {
		return _("Unknown");
	}
}

bool countrynumber_is_valid(int country_number)
{
	return (country_number >= 0) && (country_number < COUNTRIES_CANT);
}

void countries_map(CountryMapFn fn)
{
	std::for_each(std::begin(g_countries), std::end(g_countries), fn);
}

void countries_map(int player, CountryMapFn fn)
{
	std::for_each(std::begin(g_countries), std::end(g_countries),
	[player, fn](COUNTRY & c) {
		if(player == c.numjug) {
			fn(c);
		}
	});
}

void countries_map_int(int player, InterruptableCountryMapFn fn)
{
	for(COUNTRY& country: g_countries) {
		if(player == country.numjug) {
			if(!fn(country)) {
				break;
			}
		}
	}
}

COUNTRY& COUNTRY_FROM_TARJETA(TARJETA& tarjeta)
{
	if(!countrynumber_is_valid(tarjeta.country)) {
		throw std::runtime_error("Can't find the country for this card, because the country number is invalid." + std::to_string(tarjeta.country));
	}

	return g_countries[tarjeta.country];
}
