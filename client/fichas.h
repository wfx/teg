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
/**
 * @file fichas.h
 *
 * Everything related to the placement of armies.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "country.h"

/**
 * \brief places one army of the player into country \p p.
 * \return TEG_STATUS_SUCCESS if the army got placed, TEG_STATUS_ERROR if there
 *         are no more armies, or TEG_STATUS_UNEXPECTED if the current player
 *         does not match the current connection.
 */
TEG_STATUS fichas_add(PCOUNTRY p);

/**
 * \brief removes one army of the player from country \p p.
 * \return * TEG_STATUS_SUCCESS if (at least) one army was placed before in the
 *           country and got removed
 *         * TEG_STATUS_ERROR if there are no more armies in the country
 *         * TEG_STATUS_UNEXPECTED if the current active player does not match
 *           the current connection.
 */
TEG_STATUS fichas_sub(PCOUNTRY p);

/**
 * \brief Prepares the array of changes in armies per country.
 * \param[out] ptr the pointer to the changed armies. Do not free this buffer.
 * \return * TEG_STATUS_SUCCESS if all armies are placed according to the rules
 *         * TEG_STATUS_UNEXPECTED if the current game state does not permit to
 *           place armies
 *         * TEG_STATUS_ERROR if not all armies are placed, or not all
 *           continental armies are placed into their continents.
 */
TEG_STATUS fichas_finish(int **ptr);

/**
 * \brief Set the armies cache
 * \param cant total number of armies to place
 * \param conts bitfield of the countries with extra armies
 */
void fichas_init(int cant, int conts);

/// This function rolls back any army placements done via fichas_add/fichas_sub.
void fichas_reset();

/// \addtogroup rollback_armies @{
/// Rolls back the first initial placement */
TEG_STATUS fichas_restore_from_error();

/// Rolls back the second initial placement
TEG_STATUS fichas2_restore_from_error();

/// Rolls back a regular round placement
TEG_STATUS fichasc_restore_from_error();
/// @}

/**
 * \brief Send the placed armies to the game controller
 * \return * TEG_STATUS_SUCCESS on success
 *         * TEG_STATUS_ERROR when the game is not in the correct state.
 */
TEG_STATUS fichas_out();

/// Set the ammount of additional armies got by a cards exchange.
void fichas_add_wanted(int i);

/** Set the selected bitfield of the country to represent if there are still
 * armies available to place.
 *
 * This function is called after an army got placed in the country or the mouse
 * pointer hoovers over a country. */
TEG_STATUS fichas_enter(PCOUNTRY p);

/** Set the selected bitfield of the country to represent if there are still
 * armies available to place.
 *
 * This function is called after an army got removed from a country, or the
 * mouse pointer leaves a country. */
TEG_STATUS fichas_leave(PCOUNTRY p);

/** Returns the numbers of armies to place, and the fully conquered bitfield.
 *
 * The army count (without the additional armies per continent) will be put in
 * \p cant, the continent bitfield will be placed in \p conts. */
void fichas_get_wanted(int *cant, int *conts);

#ifdef __cplusplus
}
#endif
