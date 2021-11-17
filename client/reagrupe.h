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

namespace teg::client
{

/// \brief Cancel the current army movement, and call the GUI to reflect this
void reagrupe_reset(void);

/// \brief Reset the army movement state machine and prepare a new movement
TEG_STATUS reagrupe_init(void);

/**
 * \brief Handle the click into a country
 *
 * According to the internal state machine the country \p p will be selected as
 * source or destinaton country.
 */
TEG_STATUS reagrupe_click(PCOUNTRY p);

/// Checks if the current game state allows to move armies
TEG_STATUS reagrupe_check(void);

/**
 * Saves the movement cache in order to be able to restore the previous state
 * in the case the server does not accept the movement. */
void reagrupe_set_and_save(int src, int dst, int cant);

/**
 * Restore the last movement state (\sa reagrupe_set_and_save) since the server
 * did not accept the movement.
 */
TEG_STATUS reagrupe_restore_from_error(void);

/// Reset the movement state machine
void reagrupe_bigreset(void);

/// Save the current state and send the movement details to the server.
TEG_STATUS reagrupe_out(int src, int dst, int cant);

/**
 * Prepare the countries selected-bitfield so that the GUI can display the
 * correct state
 */
TEG_STATUS reagrupe_enter(PCOUNTRY p);

/**
 * Reset the movement-parts of the selected-bitfield
 */
TEG_STATUS reagrupe_leave(PCOUNTRY p);

}
