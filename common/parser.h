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
/*
 * Estructura del parser
 */

#pragma once

#include <stdbool.h>


#define PARSER_TOKEN_MAX 1024
#define PARSER_VALUE_MAX 1024

struct DELIM {
	char accept[3];
	bool valid=true;

	DELIM& operator = (DELIM const* other)
	{
		if(other) {
			*this = *other;
		} else {
			valid = false;
		}
		return *this;
	}
};

struct PARSER {
	char const *data;
	bool can_continue;
	char token[PARSER_TOKEN_MAX];
	char value[PARSER_VALUE_MAX];
	DELIM equals;
	DELIM separators;

	/// Try to parse the input text, and return if it was successful
	bool parse();

	/// Parse with the expectation that there will something remain
	bool parse_fragment()
	{
		return parse() && can_continue;
	}

	/// parse with the expectation that everything is captured
	bool parse_everything()
	{
		return parse() && !can_continue;
	}
};
