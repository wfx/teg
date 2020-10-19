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

typedef struct _DELIM {
	char a;
	char b;
	char c;
} DELIM, *PDELIM;

#define DELIM_EQ1 {'=','=','='}
#define DELIM_FI1 {';',';',';'}

#define DELIM_EQ2 {':',':',':'}
#define DELIM_FI2 {',',',',','}

#define DELIM_EQ3 {'|','|','|'}
#define DELIM_FI3 {'/','/','/'}

#define DELIM_NULL {'\0','\0','\0'}

typedef struct {
	char *data; 
	int hay_otro;
	char token[PARSER_TOKEN_MAX];
	char value[PARSER_VALUE_MAX];
	PDELIM igualador;
	PDELIM separador;
} PARSER, *PPARSER;

/* Unica funcion publica del parser */
bool parser_call( PPARSER );
