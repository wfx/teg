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
 *	IN:	PPARSER  (->data tiene la cadena a parsear )
 *	OUT:	PPARSER->data = Puntero a la data
 *		PPARSER->token = 1er token
 *		PPARSER->valor = su valor
 *		PPARSER->hay_otro = si hay mas datos para leer
 *
 *	Separadores:  son pasados por PDELIM igualador
 *	Igualadores:  son pasados por PDELIM separador
 *	Fin: asciiz o \n o \r
 * Ejemplos validos:
 *	"Quiero un nuevo mundo=TRUE;Me gusta Linux=Si"
 *	"Hola;Como;Te;Va"
 * Ejemplos no validos:
 *	"Hola=343=534"
*/

#include "parser.h"
#include "parser_private.h"

#include <ctype.h>
#include <stdio.h>

bool parser_belongs_to_class(char ch, DELIM const& which)
{
	if(which.valid) {
		for(char const c: which.accept) {
			if(c == ch) {
				return true;
			}
		}
	}

	return false;
}

DELIM const delim_fin = {0, '\n', '\r'};
enum CharClass parser_character_class(char a,
                                      DELIM const& igualador,
                                      DELIM const& separador)
{
	if(parser_belongs_to_class(a, delim_fin)) {
		return ccEnd;
	}
	if(parser_belongs_to_class(a, igualador)) {
		return ccEquals;
	}
	if(parser_belongs_to_class(a, separador)) {
		return ccSeparators;
	}

	return ccData;
}

PARSER_VALUE parser_analyze_token(int *pos, char const *in, char *out,
                                  DELIM const& equals, DELIM const& separators, int maxlen)
{
	PARSER_VALUE pval=PARSER_DATA;
	int i, j;
	bool in_escape = false;

	// Analyzing the input string, copy data characters obeying escaped values.
	for(i=0, j=0; i<maxlen; i++) {
		if(in[i] == '"') {
			in_escape = !in_escape;
			continue;
		}

		if(!in_escape) {
			pval=(PARSER_VALUE)parser_character_class(in[i], equals, separators);
			if(pval != PARSER_DATA) {
				break;
			}
		}

		out[j++]=in[i];
	}
	out[j]=0;

	// Check if the input end was reached without a terminating symbol
	if(i==maxlen) {
		return PARSER_ERROR;
	}

	*pos=i; // signal the new input position to the caller
	return pval;
}

/* Unica funcion exportable */
bool PARSER::parse()
{
	PARSER *const p_in{this};
	PARSER_VALUE pval;
	int i;

	if((pval=parser_analyze_token(&i, p_in->data, p_in->token,
	                              p_in->equals, p_in->separators,
	                              PARSER_TOKEN_MAX)) == PARSER_ERROR) {
		return false;
	}

	p_in->value[0]=0;

	switch(pval) {
	case PARSER_FIN:
		p_in->data=NULL;
		p_in->can_continue = false;
		return true;

	case PARSER_SEPARADOR:
		p_in->data=&p_in->data[i+1];
		p_in->can_continue = true;
		return true;

	case PARSER_IGUAL: {
		int j;
		pval = parser_analyze_token(&j, &p_in->data[i+1], p_in->value,
		                            DELIM{.valid=false}, p_in->separators,
		                            PARSER_VALUE_MAX);

		if(pval==PARSER_IGUAL || pval==PARSER_ERROR) {
			return false;
		}

		if(pval==PARSER_SEPARADOR) {
			p_in->data = &p_in->data[j+1 + i+1];
			p_in->can_continue = true;
		} else { /* PARSER_FIN */
			p_in->data = NULL;
			p_in->can_continue = false;
		}
		return true;
	}
	default:
		return false;
	}
}
