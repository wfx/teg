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

#include <string>

#include "cont.h"

typedef struct _missions {
	/// The name of the mission
	char const *name;

	/// number of countries to conquer on each continent
	int	continents[CONTINENTE_LAST];

	/** number countries additionaly to own in neighboring continents in
	 * addition to the counties in the continentes */
	int	frontiering_countries;

	/// total number of countries to conquer
	int	tot_countries;
} MISSIONS, *PMISSIONS;

enum {
	MISSION_CONQWORLD, ///< "fallback mission": conquer everything
	MISSION_COMMON, ///< optional common mission: conquer 30 countries
};

/* returns the total number of missions that exists */
int missions_cant();

/// \return the definition of the mission with the given number
MISSIONS const& mission_get(int number);

/// \return the name of a mission (translated)
std::string missions_get_name(CommonState const& state, int number);
