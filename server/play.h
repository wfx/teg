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

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Start a new game
 *
 * The function checks if the preconditions to (re)start the game are met. If it
 * is so, a new game is start.
 *
 * \return TEG_STATUS_SUCCESS if a new game is started
 *         TEG_STATUS_PARSEERROR if there was any error.
 */
TEG_STATUS token_start(int fd);

/** \brief Shut down the server
 *
 * \return This function always returns TEG_STATUS_CONNCLOSED. It can't be void
 *         since it is used as a callback function.
 */
TEG_STATUS token_exit(int fd);

/// \brief Read a message from a client fd and do the appropriate action
TEG_STATUS play_teg(int fd);

#ifdef __cplusplus
}
#endif
