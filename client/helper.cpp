/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <riq@corest.com>
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
/**
 * @file aux.c
 * Funciones auxiliares del cliente
 */

#include <string.h>

#include "parser.h"
#include "fcintl.h"
#include "client.h"

namespace teg::client
{

/**
 * @fn TEG_STATUS aux_status( PCPLAYER pj, char *str )
 * parsea el status de los playeres
 */
TEG_STATUS aux_status(PCPLAYER pj, char const *str)
{
	PARSER p{str};

	memset(pj, 0, sizeof(*pj));

	if(strlen(str)==0) {
		goto error;
	}

	if(p.parse_fragment()) {
		strncpy(pj->name, p.token, sizeof(pj->name)-1);
	} else {
		goto error;
	}

	if(p.parse_fragment()) {
		pj->color = atoi(p.token);
	} else {
		goto error;
	}

	if(p.parse_fragment()) {
		pj->score = atoi(p.token);
	} else {
		goto error;
	}

	if(p.parse_fragment()) {
		pj->numjug = atoi(p.token);
	} else {
		goto error;
	}

	if(p.parse_fragment()) {
		const int statuscode = atoi(p.token);
		if((statuscode < 0) || (statuscode >= PLAYER_STATUS_LAST)) {
			goto error;
		}
		pj->estado = static_cast<PLAYER_STATUS>(statuscode);
	} else {
		goto error;
	}

	if(p.parse_fragment()) {
		pj->tot_countries = atoi(p.token);
	} else {
		goto error;
	}

	if(p.parse_fragment()) {
		pj->tot_armies = atoi(p.token);
	} else {
		goto error;
	}

	if(p.parse_fragment()) {
		pj->tot_cards = atoi(p.token);
	} else {
		goto error;
	}

	if(p.parse_fragment()) {
		pj->empezo_turno = atoi(p.token);
	} else {
		goto error;
	}

	if(p.parse_fragment()) {
		pj->human = atoi(p.token);
	} else {
		goto error;
	}

	if(p.parse_everything()) {
		strncpy(pj->addr, p.token, sizeof(pj->addr)-1);
	} else {
		goto error;
	}

	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR, "error in aux_status()");
	return TEG_STATUS_ERROR;
}


TEG_STATUS aux_scores(PSCORES pS, char const *str)
{
	PARSER p{str, DELIM{.valid=false}, DELIM{','}};

	memset(pS, 0, sizeof(*pS));

	if(strlen(str)==0) {
		goto error;
	}

	if(p.parse_fragment()) {
		strncpy(pS->name, p.token, sizeof(pS->name)-1);
	} else {
		goto error;
	}

	if(p.parse_fragment()) {
		pS->color= atoi(p.token);
	} else {
		goto error;
	}

	if(p.parse_fragment()) {
		strncpy(pS->date, p.token, sizeof(pS->date)-1);
	} else {
		goto error;
	}

	if(p.parse_fragment()) {
		pS->score = atoi(p.token);
	} else {
		goto error;
	}

	if(p.parse_everything()) {
		pS->human= atoi(p.token);
	} else {
		goto error;
	}

	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR, "error in aux_scores()");
	return TEG_STATUS_ERROR;
}


/**
 * @fn TEG_STATUS aux_countries( int numjug, char *str )
 * parsea los ejercitos en countries de un player
 */
TEG_STATUS aux_countries(int numjug, char const *str)
{
	int i, country, cant;
	PARSER p{str};

	if(strlen(str)==0) {
		return TEG_STATUS_SUCCESS;
	}

	do {
		if((i = p.parse())) {
			country = atoi(p.token);
			cant = atoi(p.value);
			if(g_countries[country].numjug != numjug || g_countries[country].ejercitos != cant) {
				g_countries[country].numjug = numjug;
				g_countries[country].ejercitos = cant;
				callbacks::gui_country(g_countries[country].id);
			}
		}
	} while(i && p.can_continue());

	return TEG_STATUS_SUCCESS;
}

/**
 * @fn void aux_draw_all_countries()
 * Dibuja todos los countries
 */
void aux_draw_all_countries()
{
	int i;
	for(i=0; i<COUNTRIES_CANT; i++) {
		callbacks::gui_country(i);
	}
}

TEG_STATUS aux_player_init(PCPLAYER pJ, int numjug, char *name, int color)
{
	memset(pJ, 0, sizeof(*pJ));

	pJ->numjug = numjug;
	strncpy(pJ->name, name, sizeof(pJ->name)-1);
	pJ->name[sizeof(pJ->name)-1]=0;
	pJ->color = color;
	pJ->estado = PLAYER_STATUS_HABILITADO;
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS aux_start_error()
{
	textmsg(M_ERR, _("Error in start. Are there at least 2 players?"));
	return TEG_STATUS_SUCCESS;
}

}
