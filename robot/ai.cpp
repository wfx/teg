/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <riq@core-sdi.com>
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
 * @file ai.c
 * Inteligecia artificial del robot
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <unistd.h>

#include "client.h"
#include "ai.h"
#include "state.h"
#include "ai_fichas.h"

namespace teg::robot
{

namespace c = ::teg::client;

/**
 * Risk of buffer overflow: When cards are traded for armies, there is NO upper
 * limit for how many armies you can place! When set to 50 this was a cause of
 * overflows when games lasted longer. But 500 armies should be enough for everyone.
*/
TEG_STATUS ai_puntaje_sort(int cant)
{
	std::size_t i;
	int j;
	int max;

	//check if the buffer is big enough
	if(cant < 0 || static_cast<std::size_t>(cant) > COUNTRIES_CANT * 10) {
		return TEG_STATUS_ERROR;
	}

	for(j=0; j<cant; j++) {
		max = 0;
		for(i=0; i<COUNTRIES_CANT; i++) {
			if(robot_state().ai_puntaje[i] > robot_state().ai_puntaje[max]) {
				max = i;
			}
		}
		robot_state().ai_sorted[j] = max;
		robot_state().ai_puntaje[max] -= 2;
	}
	return TEG_STATUS_SUCCESS;
}

/**
 * @fn TEG_STATUS ai_puntaje_sort_for_attack( int cant)
 * Algoritmo para ordernar como se va a hacer el attack
 */
TEG_STATUS ai_puntaje_sort_for_attack()
{
	std::size_t i;
	int max=0;

	for(i=0; i<COUNTRIES_CANT; i++) {
		if(robot_state().ai_puntaje[i] > robot_state().ai_puntaje[max]) {
			max = i;
		}
	}
	robot_state().ai_sorted[0] = max;
	return TEG_STATUS_SUCCESS;
}

void ai_puntaje_clean()
{
	std::size_t i;
	for(i=0; i<COUNTRIES_CANT; i++) {
		robot_state().ai_puntaje[i] = -10000;
	}
}

/**
 * @fn int ai_many_country_enemigo( int p )
 * Dice la cantidad de countries enemigos que tiene un country
 */
int ai_many_country_enemigo(c::ClientState& state, int p)
{
	std::size_t i;
	int c=0;

	for(i=0; i<COUNTRIES_CANT; i++) {
		if(state.game.common.countries[i].numjug != c::who_am_i(state) && countries_eslimitrofe(p, i)) {
			c++;
		}
	}

	return c;
}

/**
 * @fn BOOLEAN ai_is_country_border( int p )
 * Tells if a country border of its continent
 */
bool ai_is_country_border(c::ClientState& state, int p)
{
	std::size_t i;
	for(i=0; i<COUNTRIES_CANT; i++) {
		if(countries_eslimitrofe(i, p) && state.game.common.countries[i].continente != state.game.common.countries[p].continente) {
			return true;
		}
	}

	return false;
}

bool ai_own_continent(c::ClientState& state, CONTINENTE c)
{
	std::size_t i;
	int t=0;

	for(i=0; i<COUNTRIES_CANT; i++) {
		if(state.game.common.countries[i].numjug == c::who_am_i(state)
		        && state.game.common.countries[i].continente == c) {
			t++;
		}
	}

	return (t == state.game.common.conts[c].cant_countries);

}

TEG_STATUS ai_init(c::ClientState& state)
{
	ai_puntaje_clean();
	return TEG_STATUS_SUCCESS;
}

/**
 * This function decides if it is dangerous to attack country dst by src. It is
 * taken into consideration if the the player of the country to be attacked has
 * already done with the turn.
 */
bool ai_is_country_peligroso(c::ClientState& state, int src, int dst)
{
	/// \todo rename and scope limit variables
	bool aparecio_empezo = false;
	bool dst_jugo = false;
	int tmp;

	c::players_map_int(state,
	[&state, &aparecio_empezo, dst, &dst_jugo](c::Player& player) {

		if(player.empezo_turno) {
			aparecio_empezo = true;
		}

		if(player.numjug == state.game.common.countries[dst].numjug) {
			dst_jugo = aparecio_empezo;
			return false;
		}
		return true;
	});

	if(dst_jugo) {
		return false;
	}

	tmp = (state.game.common.countries[src].ejercitos > 3) ? 3: state.game.common.countries[src].ejercitos-1;
	if(state.game.common.countries[dst].ejercitos > (state.game.common.countries[src].ejercitos-tmp)) {
		return true;
	}
	return false;
}

/**
 * @fn int ai_puntaje_atacar_warning( int src )
 * Resta puntos si existe un country limitrofe a 'src' que puede atacarlo antes
 * de que se acabe el turno
 */
int ai_puntaje_atacar_warning(c::ClientState& state, int src)
{
	int p=0;
	std::size_t i;

	for(i=0; i<COUNTRIES_CANT; i++) {
		if(countries_eslimitrofe(src, i) &&
		        state.game.common.countries[src].numjug != state.game.common.countries[i].numjug &&
		        ai_is_country_peligroso(state, src, i)) {

			p += (state.game.common.countries[i].ejercitos - state.game.common.countries[src].ejercitos);
		}
	}

	return -(5+2*p);
}


/**
 * @fn int ai_puntaje_atacar_country( int src, int dst )
 * Dice los puntos que tiene de atacar src a dst
 */
int ai_puntaje_atacar_country(c::ClientState& state, int src, int dst)
{
	int p=0;

	if(state.game.common.countries[src].ejercitos > state.game.common.countries[dst].ejercitos) {

		if(state.game.common.countries[src].continente == state.game.common.countries[dst].continente) {
			p++;
		}

		p += 10 + state.game.common.countries[src].ejercitos - state.game.common.countries[dst].ejercitos;

	} else {

		int r = random_between(1, 10);

		if((r>7) && (state.game.common.countries[src].ejercitos > 7)) {
			p++;
		}
	}

	return p;
}

/**
 * @fn int ai_puntaje_attack( int p )
 * Evalua los puntos que tiene el country para atacar
 * y dice al country que convendria atacar
 */
TEG_STATUS ai_puntaje_atacar(c::ClientState& state, int country)
{
	std::size_t i;
	int p=0;
	int p_tmp;
	int dst=-1;

	for(i=0; i<COUNTRIES_CANT; i++) {
		if(countries_eslimitrofe(country, i) && state.game.common.countries[i].numjug != c::who_am_i(state)) {
			p_tmp = ai_puntaje_atacar_country(state, country, i);

			/* son negativos estos valores, por eso sumo */
			p_tmp += ai_puntaje_atacar_warning(state, country);

			if(p_tmp > p) {
				p = p_tmp;
				dst = i;
			}
		}
	}

	/* return the best candidate */
	if(dst >=0) {
		robot_state().ai_puntaje[ country ] = p;
		robot_state().attack_dst[ country ] = dst;
	}

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS ai_turno(c::ClientState& state)
{
	std::size_t i;
	int j;

	ai_puntaje_clean();
	for(i=0; i<COUNTRIES_CANT; i++) {
		if(state.game.common.countries[i].numjug == c::who_am_i(state) && state.game.common.countries[i].ejercitos > 1) {
			ai_puntaje_atacar(state, i);
		}
	}
	ai_puntaje_sort_for_attack();

	i = robot_state().ai_sorted[0];
	j = robot_state().attack_dst[i];
	if(robot_state().ai_puntaje[i] > 0) {

		c::attack_init(state);
		c::attack_click(state, &state.game.common.countries[i]);
		c::attack_click(state, &state.game.common.countries[j]);
		return TEG_STATUS_SUCCESS;
	}
	return TEG_STATUS_ERROR;
}

TEG_STATUS __ai_reagrupe(c::ClientState& state, int p, int cant)
{
	std::size_t i;
	int own = ai_own_continent(state, state.game.common.countries[p].continente);

	if(own) {
		int new_cant = cant;

		/* try to defend, undefended frontiers */
		for(i=0; i<COUNTRIES_CANT; i++) {
			if(state.game.common.countries[i].numjug != c::who_am_i(state) &&
			        countries_eslimitrofe(i, p) &&
			        state.game.common.countries[i].ejercitos > state.game.common.countries[p].ejercitos) {

				c::reagrupe_out(state, p, i, cant);
				return TEG_STATUS_SUCCESS;
			}
		}

		/* I will leave 3 armies in border country of the continent */
		if(state.game.common.countries[p].ejercitos - state.game.common.countries[p].ejer_reagrupe - cant < 3) {
			new_cant -= 2;
		}

		for(i=0; i<COUNTRIES_CANT && (new_cant>0); i++) {
			if(state.game.common.countries[i].numjug == c::who_am_i(state) && countries_eslimitrofe(p, i) &&
			        state.game.common.countries[i].continente != state.game.common.countries[p].continente) {

				c::reagrupe_out(state, p, i, new_cant);
				return TEG_STATUS_SUCCESS;
			}
		}
	}

	/*... si falla, los muevo a una frontera */
	for(i=0; i<COUNTRIES_CANT; i++) {
		if(state.game.common.countries[i].numjug == c::who_am_i(state) && countries_eslimitrofe(p, i) &&
		        state.game.common.countries[i].continente == state.game.common.countries[p].continente) {

			if(ai_is_country_border(state, i)) {
				c::reagrupe_out(state, p, i, cant);
				return TEG_STATUS_SUCCESS;
			}
		}
	}

	/*... y si no hay frontera lo mando al 1ro que tenga a mano */
	for(i=0; i<COUNTRIES_CANT; i++) {
		if(state.game.common.countries[i].numjug == c::who_am_i(state) && countries_eslimitrofe(p, i) &&
		        state.game.common.countries[i].continente == state.game.common.countries[p].continente) {

			c::reagrupe_out(state, p, i, cant);
			return TEG_STATUS_SUCCESS;
		}
	}

	return TEG_STATUS_ERROR;
}

TEG_STATUS ai_reagrupe(c::ClientState& state)
{
	std::size_t i;
	int p;
	c::reagrupe_init(state);

	for(i=0; i < COUNTRIES_CANT ; i++) {
		if(state.game.common.countries[i].numjug == c::who_am_i(state)) {
			if(ai_many_country_enemigo(state, i) == 0) {
				p = state.game.common.countries[i].ejercitos - state.game.common.countries[i].ejer_reagrupe;

				if(p < 2) {
					continue;
				}

				__ai_reagrupe(state, i, p-1);
			}
		}
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS ai_tropas(c::ClientState& state, int src, int dst, int cant)
{
	int i;
	int c=0;

	ai_puntaje_clean();
	ai_fichas_calc_puntaje(state, src);
	ai_fichas_calc_puntaje(state, dst);

	for(i=0; i<cant; i++) {
		if(robot_state().ai_puntaje[dst] > robot_state().ai_puntaje[src] + i) {
			c++;
		}
	}

	if(c > 0) {
		c::out_tropas(state, src, dst, c);
	}

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS ai_puedocanje(c::ClientState& state, int *p1, int *p2, int *p3)
{
	return c::canje_puedo(state, p1, p2, p3);
}

}
