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

namespace teg::server
{

#define CONSOLE_FD 0

/// \brief Try to read and handle one command from the console
TEG_STATUS console_handle(int fd);

/// \brief Print the console ready prompt
void con_show_prompt();

/** \brief Print a text to the server console and show the ready prompt
 *
 * \note The parameter \p level is ignored
 */
void con_text_out(int level, const char *format, ...);

/** \brief Print a text to the server console without printing a ready prompt
 *
 * \note The parameter \p level is ignored
 */
TEG_STATUS con_text_out_wop(int level, const char *format, ...);

/// \brief Initialize the server console
TEG_STATUS console_init(void);

/// \brief Deactivate the server console
TEG_STATUS console_quit(void);

}
