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
/* checks that the secret missions are accomplished */

#include <assert.h>
#include <string.h>

#include "server.h"
#include "missions.h"

namespace teg::server
{

TEG_STATUS mission_chequear(PSPLAYER pJ)
{
	int i;
	int countries[CONT_CANT];

	assert(pJ);

	if(pJ->mission == -1) {
		if(mission_asignar(pJ) != TEG_STATUS_SUCCESS) {
			return TEG_STATUS_ERROR;
		}
	}


	memset(countries, 0, sizeof(countries));

	player_listar_countries(pJ, countries);

	/* 1st part. Check that the player has the correct number of countries */
	if(g_game.cmission && pJ->mission != MISSION_CONQWORLD) {
		if(pJ->tot_countries >= mission_get(MISSION_COMMON).tot_countries) {
			pJ->mission = MISSION_COMMON;
			goto gano;
		}
	};

	/* 2da parte. Chequear countries por contienente */
	for(i=0; i<CONT_CANT; i++) {
		if(countries[i] < mission_get(pJ->mission).continents[i]) {
			goto no_gano;
		}
	}

	/* TODO: 3ra parte. Chequear si vencio a los otros playeres */

	/* TODO: 4ta parte. Chequear si tiene los countries limitrofes que se piden */
	if((i=mission_get(pJ->mission).frontiering_countries)) {
		int j, k, i_tmp;
		int salir=0;
		for(j=0; j<static_cast<int>(COUNTRIES_CANT) && salir==0; j++) {
			if(g_countries[j].numjug != pJ->numjug) {
				continue;
			}
			i_tmp=0;

			for(k=0; k<static_cast<int>(COUNTRIES_CANT); k++) {
				if(g_countries[k].numjug != pJ->numjug) {
					continue;
				}
				if(countries_eslimitrofe(j, k)) {
					if(++i_tmp >= i) {
						/* mission cumplido, salir */
						salir=1;
						break;
					}
				}

			}
		}
		if(j>=static_cast<int>(COUNTRIES_CANT)) {
			goto no_gano;
		}
	}

	/* tiene todo, entonces gano! */
gano:
	return TEG_STATUS_GAMEOVER;

no_gano:
	return TEG_STATUS_ERROR;
}

TEG_STATUS mission_asignar(PSPLAYER pJ)
{
	int i, obj;

	assert(pJ);

	/* tiene ya un mission asignado ? */
	if(pJ->mission != -1) {
		return TEG_STATUS_SUCCESS;
	}

	if(g_game.mission_players.size() != static_cast<std::size_t>(missions_cant())) {
		mission_init();
	}

	/* Si se juega sin missions, es a conquistar el mundo */
	if(!g_game.mission) {
		pJ->mission = MISSION_CONQWORLD;	/* mission de conquistar al mundo */
		return TEG_STATUS_SUCCESS;
	}

	obj = random_between(0, missions_cant()-1);

	i = obj;

	for(; i < missions_cant() ; i++) {
		if(g_game.mission_players[i] == -1) {
			g_game.mission_players[i] = pJ->numjug;
			pJ->mission = i;
			return TEG_STATUS_SUCCESS;
		}
	}
	for(i=0; i < obj ; i++) {
		if(g_game.mission_players[i] == -1) {
			g_game.mission_players[i] = pJ->numjug;
			pJ->mission = i;
			return TEG_STATUS_SUCCESS;
		}
	}

	return TEG_STATUS_ERROR;
}

void mission_init()
{
	g_game.mission_players.assign(missions_cant(), -1);
	g_game.mission_players[MISSION_CONQWORLD] = 0;
	g_game.mission_players[MISSION_COMMON] = 0;
}

TEG_STATUS mission_set(bool a)
{
	if(JUEGO_EMPEZADO) {
		return TEG_STATUS_ERROR;
	}

	if(a!=0) {
		a=1;
	}
	g_game.mission = a;
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS mission_common_mission(bool a)
{
	if(JUEGO_EMPEZADO) {
		return TEG_STATUS_ERROR;
	}

	if(a!=0) {
		a=1;
	}
	g_game.cmission= a;
	return TEG_STATUS_SUCCESS;
}

}
