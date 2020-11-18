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

#ifdef __cplusplus
extern "C" {
#endif


/** \brief Try to get two armies for one country.
 *
 * This function tries to request two armies for the \p country. This works when
 * the country belongs to the player, and nobody before got a card for this
 * country.
 *
 * \todo Change the return type to bool.
 *
 * \return TEG_STATUS_SUCCESS if the function sent the request to the
 *         game controller, TEG_STATUS_ERROR otherwise.
 */
TEG_STATUS ejer2_out(int country);

/** \brief Undo a previous army exchange.
 *
 * This is called when the game controller decided that the exchange of armies
 * is not possible. It rools back the previous ejer2_out call.
 *
 * \todo Change the return type to bool.
 *
 * \return TEG_STATUS_SUCCESS if there was an active ejer2_out call,
 *         TEG_STATUS_ERROR otherwise.
 */
TEG_STATUS ejer2_restore_from_error();

#ifdef __cplusplus
}
#endif
