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

DELIM delim_null={ '\0', '\0', '\0' };

/* Que tipo de char es? */
static PARSER_VALUE que_es( char a, PDELIM igualador, PDELIM separador )
{
	if( a==0 || a=='\n' || a=='\r' )
		return PARSER_FIN;
	if( a==igualador->a || a==igualador->b || a==igualador->c )
		return PARSER_IGUAL;
	if( a==separador->a || a==separador->b || a==separador->c )
		return PARSER_SEPARADOR;

	return PARSER_DATA;
}


static PARSER_VALUE 
analiza( int *pos,		/* En que pos corto la cadena */
	char *in,		/* Cadena de entrada */ 
	char *out,		/* Cadena de salida */
	PDELIM igualador,	/* Igualadores */
	PDELIM separador,	/* Separadores */
	int maxlen
	)
{
	PARSER_VALUE pval=PARSER_DATA;
	int i,j;
	int quote=0;
	
	out[0]=0;

	/* copia data */
	for(i=0,j=0;i<maxlen;i++) {
		if( in[i] == '"' ) {
			quote = ! quote;
			continue;
		}

		if( ! quote ) {
			if( (pval=que_es(in[i],igualador,separador)) != PARSER_DATA )
				break;
		}

		out[j++]=in[i];
		out[j]=0;
	}

	/* se termino de copiar en la pos i */
	if(i==PARSER_TOKEN_MAX) {
		return PARSER_ERROR;
	}
	*pos=i;
	return pval;
}

/* Unica funcion exportable */
bool parser_call( PPARSER p_in )
{
	PARSER_VALUE pval;
	int i;

	if( (pval=analiza( &i, p_in->data, p_in->token, p_in->igualador, p_in->separador,PARSER_TOKEN_MAX )) == PARSER_ERROR )
		return false;
	
	p_in->value[0]=0;

	switch(pval) {
	case PARSER_FIN:
		p_in->data=NULL;
		p_in->hay_otro = false;
	    return true;

	case PARSER_SEPARADOR:
		p_in->data=&p_in->data[i+1];
		p_in->hay_otro = true;
	    return true;

	case PARSER_IGUAL:
	{
		int j;
		pval = analiza( &j, &p_in->data[i+1], p_in->value, &delim_null, p_in->separador, PARSER_VALUE_MAX );

		if( pval==PARSER_IGUAL || pval==PARSER_ERROR )
			return false;

		if( pval==PARSER_SEPARADOR ) {
			p_in->data = &p_in->data[j+1 + i+1];
			p_in->hay_otro = true;
		} else { /* PARSER_FIN */
			p_in->data = NULL;
			p_in->hay_otro = false;
		}
		return true;
	}
	default:
	    return false;
	}	
}
