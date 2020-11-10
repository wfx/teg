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

#ifdef __cplusplus
extern "C" {
#endif

#define PARSER_TOKEN_MAX 1024
#define PARSER_VALUE_MAX 1024

typedef struct _DELIM {
	char a;
	char b;
	char c;
} DELIM, *PDELIM;

typedef struct {
	char const *data;
	int can_continue;
	char token[PARSER_TOKEN_MAX];
	char value[PARSER_VALUE_MAX];
	DELIM const *equals;
	DELIM const *separators;
} PARSER, *PPARSER;

/**
 * @brief Parse an input string into the command token and optional value token.
 * @return true if the parse succeded, false otherwise.
 */
bool parser_parse(PPARSER);

#ifdef __cplusplus
}
#endif
