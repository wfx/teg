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

#include "fcintl.h"

COUNTRY g_countries[] = {
	/* AMERICA DEL SUR */
	{ LENTRY_NULL,  0, N_("Argentina"), 0, 0, 0, CONTINENTE_AMERICASUR, TARJETA_NULL_COMODIN, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL,  1, N_("Brazil"), 0, 0, 0, CONTINENTE_AMERICASUR, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL,  2, N_("Chile"), 0, 0, 0, CONTINENTE_AMERICASUR, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL,  3, N_("Colombia"), 0, 0, 0, CONTINENTE_AMERICASUR, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL,  4, N_("Peru"), 0, 0, 0, CONTINENTE_AMERICASUR, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL,  5, N_("Uruguay"), 0, 0, 0, CONTINENTE_AMERICASUR, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE  },

	/* AMERICA DEL NORTE */
	{ LENTRY_NULL,  6, N_("Mexico"), 0, 0, 0, CONTINENTE_AMERICANORTE, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL,  7, N_("California"), 0, 0, 0, CONTINENTE_AMERICANORTE, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL,  8, N_("Oregon"), 0, 0, 0, CONTINENTE_AMERICANORTE, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL,  9, N_("New York"), 0, 0, 0, CONTINENTE_AMERICANORTE, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 10, N_("Alaska"), 0, 0, 0, CONTINENTE_AMERICANORTE, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 11, N_("Yukon"), 0, 0, 0, CONTINENTE_AMERICANORTE, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 12, N_("Canada"), 0, 0, 0, CONTINENTE_AMERICANORTE, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 13, N_("Terranova"), 0, 0, 0, CONTINENTE_AMERICANORTE, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 14, N_("Labrador"), 0, 0, 0, CONTINENTE_AMERICANORTE, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 15, N_("Greenland"), 0, 0, 0, CONTINENTE_AMERICANORTE, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE  },

	/* AFRICA */
	{ LENTRY_NULL, 16, N_("Sahara"), 0, 0, 0, CONTINENTE_AFRICA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 17, N_("Zaire"), 0, 0, 0, CONTINENTE_AFRICA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 18, N_("Etiopia"), 0, 0, 0, CONTINENTE_AFRICA, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 19, N_("Egypt"), 0, 0, 0, CONTINENTE_AFRICA, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 20, N_("Madagascar"), 0, 0, 0, CONTINENTE_AFRICA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 21, N_("South Africa"), 0, 0, 0, CONTINENTE_AFRICA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE  },

	/* OCEANIA */
	{ LENTRY_NULL, 22, N_("Australia"), 0, 0, 0, CONTINENTE_OCEANIA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 23, N_("Borneo"), 0, 0, 0, CONTINENTE_OCEANIA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 24, N_("Java"), 0, 0, 0, CONTINENTE_OCEANIA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 25, N_("Sumatra"), 0, 0, 0, CONTINENTE_OCEANIA, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE  },

	/* EUROPA */
	{ LENTRY_NULL, 26, N_("Spain"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 27, N_("France"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 28, N_("Germany"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 29, N_("Italy"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 30, N_("Poland"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 31, N_("Russia"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 32, N_("Sweden"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 33, N_("Great Britain"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 34, N_("Iceland"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE  },

	/* ASIA */
	{ LENTRY_NULL, 35, N_("Arabia"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 36, N_("Israel"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 37, N_("Turkey"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 38, N_("India"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 39, N_("Malaysia"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 40, N_("Iran"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 41, N_("Gobi"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 42, N_("China"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 43, N_("Mongolia"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 44, N_("Siberia"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 45, N_("Aral"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 46, N_("Tartary"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 47, N_("Taymir"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_COMODIN, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 48, N_("Katchatka"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE  },
	{ LENTRY_NULL, 49, N_("Japan"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE  }
};
#define COUNTRIES_TOT (sizeof(g_countries) /sizeof(g_countries[0]))

#include "limitrof.h"

/* returns true if country a and b are border */
BOOLEAN countries_eslimitrofe(int a, int b)
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
	InitializeListHead(&p->next);
	InitializeListHead(&p->tarjeta.next);
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
BOOLEAN country_libre(int i)
{
	return(g_countries[i].numjug == -1);
}

/* returns the name of a country */
char * countries_get_name(int i)
{
	if(i >= 0 && i < COUNTRIES_CANT) {
		return _(g_countries[i].name);
	} else {
		return _("Unknown");
	}
}

BOOLEAN countrynumber_is_valid(int country_number)
{
	return (country_number >= 0) && (country_number < COUNTRIES_CANT);
}
