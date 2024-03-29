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
TEG_STATUS turno_2nextplayer(PSPLAYER *ppJ)
{
	PSPLAYER next = find_next_player(*ppJ,
	[](SPLAYER &p) {
		return player_is_playing(&p);
	});
	if(!next) {
		con_text_out_wop(M_ERR, "Abnormal error in turno_2nextplayer\n");
		return TEG_STATUS_PLAYERNOTFOUND;
	}

	TURNO_DEBUG("Old turn: '%s'\n", (*ppJ)->name);
	g_game.old_turn = *ppJ;

	TURNO_DEBUG("New turn: '%s'\n", next->name);
	*ppJ = next;
	return TEG_STATUS_SUCCESS;
}

/* Ends the player turn */
TEG_STATUS turno_end(PSPLAYER pJ)
{
	assert(pJ);

	/* FIXME: Creo que igual este chequeo no sirve, porque se chequea cuando
	 * se conquista un country. Solo sirve si uno gana sin conquistar algun country
	 * Puede pasar ???
	 */
	if(mission_chequear(pJ) == TEG_STATUS_GAMEOVER) {
		game_end(pJ);
		return TEG_STATUS_GAMEOVER;
	}

	player_clear_turn(pJ);
	return TEG_STATUS_SUCCESS;
}

/* Gives turn to the next player */
TEG_STATUS turno_next(void)
{
	assert(g_game.turno);

	/* give turn to the next one */
	if(turno_2nextplayer(&g_game.turno) == TEG_STATUS_SUCCESS) {

		/* the one who has the turn is the one that started ? */
		if(turno_is_round_complete()) {

			turno_initialize_new_round();

			aux_token_fichasc(g_game.turno);

		} else {
			g_game.turno->estado = PLAYER_STATUS_ATAQUE;
			netall_printf(TOKEN_TURNO"=%d\n", g_game.turno->numjug);
		}
		return TEG_STATUS_SUCCESS;
	}

	return TEG_STATUS_ERROR;
}

/* initializes the round. Called before the game starts */
TEG_STATUS turno_init(void)
{
	int i;
	int real_i;
	PSPLAYER j;

	i = random_between(0, g_game.playing-1);

	player_from_indice(i, &real_i);
	if(player_whois(real_i, &j) != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_ERROR;
	}

	g_game.old_turn = NULL;
	g_game.turno = j;
	g_game.empieza_turno = j;

	return TEG_STATUS_SUCCESS;
}

/* return true if the round is complete */
bool turno_is_round_complete()
{
	/* I want to know if the round is over. It is not enought to know
	 * if newturn == started because if a player with the turn exit the game
	 * he will never receive the turn again, but the started turn will point
	 * to him
	 */

	if(g_game.old_turn == NULL) {
		return false;
	}

	if(g_game.empieza_turno == g_game.turno) {
		return true;
	}

	/*
	 * if the previous playing player of 'empieza turn' is 'old turn' then
	 * the round is over
	 */
	auto *next = find_next_player(g_game.old_turn,
	[](SPLAYER& player) {
		return player.is_player;
	});
	if(next) {
		if(next == g_game.empieza_turno) {
			return true;
		} else if(next == g_game.turno) {
			return false;
		}
	}

	/* abnormal error */
	fprintf(stderr, "Abnormal error in turno_is_round_complete()\n");
	return false;
}

/* initialize variables for the new round */
void turno_initialize_new_round(void)
{
	/* So, a new round is started */
	turno_2nextplayer(&g_game.empieza_turno);
	g_game.turno = g_game.empieza_turno;

	g_game.round_number++;

	/* add the continents that it defend */
	player_map(
	[](PSPLAYER pJ) {
		unsigned long conts;
		player_listar_conts(pJ, &conts);
		int i;
		for(i=0; i<CONT_CANT; i++) {
			if(conts & 1) {
				pJ->player_stats.continents_turn[i]++;
			}
			conts >>= 1;
		}

		/* update the score */
		stats_score(&pJ->player_stats, g_conts);
	});

	netall_printf(TOKEN_NEW_ROUND"=%d,%d\n", g_game.turno->numjug, g_game.round_number);

}

}
