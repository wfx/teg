// SPDX-License-Identifier: GPL-2.0-or-later

#include "state.h"
#include "player.h"
#include "helper.h"

#include <gtest/gtest.h>

namespace teg::server
{

TEST(ServerState, empty_state_is_waiting_for_players)
{
	ServerState state;

	EXPECT_EQ(JUEGO_ESTADO_ESPERANDO, state.game.estado);
	EXPECT_FALSE(game_started(state));
}

TEST(ServerState, started_game_is_detected)
{
	ServerState state;

	state.game.estado = JUEGO_ESTADO_FICHAS;

	EXPECT_TRUE(game_started(state));
}

TEST(ServerState, player_lists_are_independent)
{
	ServerState first;
	ServerState second;

	SPLAYER player{};
	player.is_player = true;

	PSPLAYER inserted = player_ins(first, &player, true);
	ASSERT_NE(nullptr, inserted);

	EXPECT_EQ(1u, first.players.size());
	EXPECT_EQ(0u, second.players.size());
	EXPECT_EQ(1, first.game.players);
	EXPECT_EQ(0, second.game.players);
}

TEST(ServerState, colors_are_independent)
{
	ServerState first;
	ServerState second;

	int color = 1;
	ASSERT_TRUE(color_libre(first, &color));
	EXPECT_EQ(1, color);

	char first_colors[maximum_player_count] = {0};
	char second_colors[maximum_player_count] = {0};
	colores_libres(first, first_colors);
	colores_libres(second, second_colors);

	EXPECT_EQ(1, first_colors[1]);
	EXPECT_EQ(0, second_colors[1]);

	color_del(first, 1);
	colores_libres(first, first_colors);
	colores_libres(second, second_colors);

	EXPECT_EQ(0, first_colors[1]);
	EXPECT_EQ(0, second_colors[1]);
}

}
