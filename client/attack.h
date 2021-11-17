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
#include "../common/country.h"

namespace teg::client
{

/**
 * \brief Checks if attacks are possible, and reset the current user attack
 * if it is so.
 */
TEG_STATUS attack_init();

/**
 * @brief GUI callback to start attacks.
 *
 * This function is called each time when the user clicks on a country. It
 * - selects the country as the source country if no country is selected
 * - unselect the source country if the country is already selected
 * - selects the destination country if the source country is set
 *   - sends out the attack to the game controller.
 *
 * \todo This code should be moved to the GUI.
 * \param p the country which got clicked on.
 */
TEG_STATUS attack_click(PCOUNTRY p);

/**
 * \brief Prepare the attack country numbers.
 *
 * This function checks if the attack can be performed, and will place the
 * attacking country into \p ori, and the attacked country into \p dst.
 *
 * \return TEG_STATUS_SUCCESS when the attack is possible.
 */
TEG_STATUS attack_finish(int *ori, int *dst);

/// \brief restores the last backed up attack state. Used for attackre_out
void attack_restore();

/// \brief Stores the current user generated attack for reuse
void attack_backup();

/**
 * \brief Resets the internal state of any ongoing attack, and update the
 * GUI accordingly.
 *
 * This works on the user-controllable source-dest pair, instead of the
 * game controller source-dest-pair which attack_unshow works on.
 */
void attack_reset();

/// \brief GUI-Callback: cancel attack
/// \todo: The menu item behind this does not seem to be very usefull, so it
///        seems better to remove this functionality.
TEG_STATUS attack_pre_reset();

/// \brief send out the currently prepared attack to the game controller
TEG_STATUS attack_out();

/// \brief Perform the last attack again.
TEG_STATUS attackre_out();

/**
 * \brief Helper function to query the attack state of a county.
 *
 * This function is used for the GUI to query if the country under the mouse
 * is a suitable source or destination for an attack. At the moment it is used
 * to draw the corresponding circles around the army number in the country.
 * This is done by setting or releasing the COUNTRY_SELECT_ATTACK_ENTER flag for
 * the countries `selected`-flag.
 *
 * \todo Refactor this so that the attack code does not call GUI code.
 */
TEG_STATUS attack_enter(PCOUNTRY p);

/**
 * \brief Accompanying function for attack_enter.
 *
 * This function is called by the GUI when the mouse pointer leaves a country.
 * The reason for this is that so the `selected`-flag can be reset if it was
 * set by attack_enter before.
 *
 * \todo: make this function disappear
 */
TEG_STATUS attack_leave(PCOUNTRY p);

/**
 * \brief Callback from the game controller when an attack is performed.
 *
 * Whenever a player attacks a country, this function is called with the
 * attacking country number in \p src, and the attacked country number in
 * \p dst.
 */
void attack_show(int src, int dst);

/**
 * \brief Callback from the game controller when all possible attacks are to
 * be reset.
 *
 * This function is called to reset any attack marks in the GUI.
 */
void attack_unshow();

}
