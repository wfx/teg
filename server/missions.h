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

#include <stdbool.h>

#include "../common/common.h"
#include "player.h"

/// \brief Does player pJ accomplished his secret mission ?
TEG_STATUS mission_chequear(PSPLAYER pJ);

/// \brief Assigns a secret mission to player pJ
TEG_STATUS mission_asignar(PSPLAYER pJ);

/// \brief Initialize the secret missions datastructure
void mission_init();

/// \brief sets the option: play to conquer the world, or with secret missions
TEG_STATUS mission_set(bool a);

/// \brief Enables/Disables playing with common secret mission
TEG_STATUS mission_common_mission(bool a);
