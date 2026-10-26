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

#pragma once

#include "../common/common.h"

namespace teg::robot
{

/// \brief All mutable state of one robot instance.
struct RobotState {
	/// \brief Score per country used by the attack and army placement heuristics
	int ai_puntaje[COUNTRIES_CANT * 10] {};

	/// \brief Order of the countries sorted by their score
	int ai_sorted[COUNTRIES_CANT * 10] {};

	/// \brief Country to attack from a given country
	int attack_dst[COUNTRIES_CANT] {};

	int seed{0};		///< seed used to initialize the robot's RNG
	int timeout{1};		///< seconds the robot waits before acting again
	bool hubo_loque{false};	///< did we already ask the server what to do?
};

/// \brief The state of the running robot
RobotState& robot_state();

}
