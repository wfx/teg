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

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Sends the request to exchange cards.
 *
 * This function checks, if it is possible to exchange the cards \p p1, \p p2
 * and \p p3. If this is so, the request is sent to the game controller, and
 * the cards are marked unused locally.
 *
 * \todo change the return type to bool.
 *
 * \return TEG_STATUS_SUCCESS on success, TEG_STATUS_ERROR otherwise.
 */
TEG_STATUS canje_out(int p1, int p2, int p3);

/** \brief Check if the player has cards which can be exchanged.
 *
 * This function tries to find a combination of country cards which can be
 * exchanged into additional armies. On success this combination will be stored
 * into \p p1, \p p2 and \p p3 (if they are not NULL).
 *
 * \todo change the return type to bool.
 *
 * \return TEG_STATUS_SUCCESS on success, TEG_STATUS_ERROR otherwise.
 */
TEG_STATUS canje_puedo(int *p1, int *p2, int *p3);

#ifdef __cplusplus
}
#endif
