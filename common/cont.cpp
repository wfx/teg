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

#include "cont.h"

#include <stdlib.h>

#include "fcintl.h"
#include "state.h"

Continents make_conts()
{
	return {{
			{ N_("South America"), CONTINENTE_AMERICASUR, CONT_AMERICASUR_CANT, 3 },
			{ N_("North America"), CONTINENTE_AMERICANORTE, CONT_AMERICANORTE_CANT, 5 },
			{ N_("Africa"), CONTINENTE_AFRICA, CONT_AFRICA_CANT, 3},
			{ N_("Oceania"), CONTINENTE_OCEANIA, CONT_OCEANIA_CANT, 2 },
			{ N_("Europe"), CONTINENTE_EUROPA, CONT_EUROPA_CANT, 5 },
			{ N_("Asia"), CONTINENTE_ASIA, CONT_ASIA_CANT, 7 },
		}};
}

int cont_tot(CommonState const& state, unsigned long conts)
{
	int max = 0;

	for(auto const& cont: state.conts) {
		if(conts & 1) {
			max += cont.fichas_x_cont;
		}
		conts >>= 1;
	}

	return max;
}

char const* cont_get_name(CommonState const& state, unsigned i)
{
	if(i < state.conts.size()) {
		return _(state.conts[i].name);
	} else {
		return _("Unknown");
	}
}
