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
/**
 * @file turno.c
 * Manejo de turnos
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../common/missions.h"
#include "server.h"
#include "tegdebug.h"
#include "missions.h"

#define TURNO_DEBUG PDEBUG

namespace teg::server
{

/* Gives turn to the next player */
TEG_STATUS turno_2nextplayer(ServerState& state, PSPLAYER *ppJ)
{
	PSPLAYER next = find_next_player(state, *ppJ,
	[](SPLAYER &p) {
		return player_is_playing(&p);
	});
	if(!next) {
		con_text_out_wop(state, M_ERR, "Abnormal error in turno_2nextplayer\n");
		return TEG_STATUS_PLAYERNOTFOUND;
	}

	TURNO_DEBUG("Old turn: '%s'\n", (*ppJ)->name);
	state.game.old_turn = *ppJ;

	TURNO_DEBUG("New turn: '%s'\n", next->name);
	*ppJ = next;
	return TEG_STATUS_SUCCESS;
}

/* Ends the player turn */
TEG_STATUS turno_end(ServerState& state, PSPLAYER pJ)
{
	assert(pJ);

	/* FIXME: Creo que igual este chequeo no sirve, porque se chequea cuando
	 * se conquista un country. Solo sirve si uno gana sin conquistar algun country
	 * Puede pasar ???
	 */
	if(mission_chequear(state, pJ) == TEG_STATUS_GAMEOVER) {
		game_end(state, pJ);
		return TEG_STATUS_GAMEOVER;
	}

	player_clear_turn(state, pJ);
	return TEG_STATUS_SUCCESS;
}

/* Gives turn to the next player */
TEG_STATUS turno_next(ServerState& state)
{
	assert(state.game.turno);

	/* give turn to the next one */
	if(turno_2nextplayer(state, &state.game.turno) == TEG_STATUS_SUCCESS) {

		/* the one who has the turn is the one that started ? */
		if(turno_is_round_complete(state)) {

			turno_initialize_new_round(state);

			aux_token_fichasc(state, state.game.turno);

		} else {
			state.game.turno->estado = PLAYER_STATUS_ATAQUE;
			netall_printf(state, TOKEN_TURNO"=%d\n", state.game.turno->numjug);
		}
		return TEG_STATUS_SUCCESS;
	}

	return TEG_STATUS_ERROR;
}

/* initializes the round. Called before the game starts */
TEG_STATUS turno_init(ServerState& state)
{
	int i;
	int real_i;
	PSPLAYER j;

	i = random_between(0, state.game.playing-1);

	player_from_indice(state, i, &real_i);
	if(player_whois(state, real_i, &j) != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_ERROR;
	}

	state.game.old_turn = NULL;
	state.game.turno = j;
	state.game.empieza_turno = j;

	return TEG_STATUS_SUCCESS;
}

/* return true if the round is complete */
bool turno_is_round_complete(ServerState& state)
{
	if(state.game.old_turn == NULL || state.game.empieza_turno == NULL) {
		return false;
	}

	/*
	 * The player who started the round may have been eliminated (or left the
	 * game) in the meantime. In that case the round ends when the first player
	 * that is still playing after the starter gets the turn.
	 */
	PSPLAYER first = state.game.empieza_turno;
	if(!player_is_playing(first)) {
		first = find_next_player(state, first,
		[](SPLAYER& player) {
			return player_is_playing(&player);
		});
	}

	return first && state.game.turno == first;
}

/* initialize variables for the new round */
void turno_initialize_new_round(ServerState& state)
{
	/* So, a new round is started */
	turno_2nextplayer(state, &state.game.empieza_turno);
	state.game.turno = state.game.empieza_turno;

	state.game.round_number++;

	/* add the continents that it defend */
	player_map(state,
	[&state](PSPLAYER pJ) {
		unsigned long conts;
		player_listar_conts(state, pJ, &conts);
		int i;
		for(i=0; i<CONT_CANT; i++) {
			if(conts & 1) {
				pJ->player_stats.continents_turn[i]++;
			}
			conts >>= 1;
		}

		/* update the score */
		stats_score(&pJ->player_stats, state.game.common.conts.data());
	});

	netall_printf(state, TOKEN_NEW_ROUND"=%d,%d\n", state.game.turno->numjug, state.game.round_number);

}

}
