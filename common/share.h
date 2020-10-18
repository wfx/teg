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

#ifdef __cplusplus
extern "C" {
#endif

void dont_run_as_root();
void init_nls();

/** \brief searches for a long option parameter.
 *
 * This function starts to look for the option \p option_name at position \p i
 *  within \p argv. If the option is found, i is advanced to the next index in
 * \p argv which comes after the argument element.
 *
 * The option can match either by the full name, or by the first character.
 *
 * The search is limited to the range \p i to \p argc. When the search reaches
 * the end of \p argv, the function advances i to the last element, and returns
 * null.
 *
 * \return The function either returns the parameter string for \p option_name,
 *  or NULL if the option could not be found.
*/
char * get_option(const char *option_name, const char **argv, int *i, int argc);

/** \brief Checks for long/short boolean program options
 *
 * This function returns, if either \p option is equal to the
 * long \p option_name,
 * or if \p option_name is the short (one character) option.
 *
 * \p option_name must be preceeded with two dashes. Short options can't be
 * merged into one option paramter.
 *
 * \return true if the option was found, false otherwise.
 *
 * Examples:
 *   option_name="--foo", option="--foo" -> true      # long option match
 *   option_name="--foo", option="--foobar" -> false  # long option mismatch
 *   option_name="--foo", option="-fx" -> true        # short option match
 *   option_name="--foo", option="-xf" -> false       # combining of short options is not supported
 */
int is_option(const char *option_name, char const *option);

#ifdef __cplusplus
}
#endif
