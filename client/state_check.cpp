// SPDX-License-Identifier: GPL-2.0-or-later

#include "state.h"
#include "misc.h"
#include "player.h"
#include "fichas.h"
#include "attack.h"
#include "reagrupe.h"

#include <gtest/gtest.h>

namespace teg::client
{

TEST(ClientState, player_lists_are_independent)
{
	ClientState first;
	ClientState second;

	Player player{};
	player.numjug = 3;
	player_ins(first, &player);

	EXPECT_EQ(1u, first.players.size());
	EXPECT_TRUE(second.players.empty());
	EXPECT_EQ(1, first.game.playeres);
	EXPECT_EQ(0, second.game.playeres);
}

TEST(ClientState, module_states_are_independent)
{
	ClientState first;
	ClientState second;

	fichas_init(first, 5, 1);
	EXPECT_EQ(5, first.fichas.wanted);
	EXPECT_EQ(0, second.fichas.wanted);

	EXPECT_TRUE(is_status(first, PLAYER_STATUS_DESCONECTADO));
	EXPECT_TRUE(is_status(second, PLAYER_STATUS_DESCONECTADO));

	first.game.fd = 42;
	EXPECT_EQ(42, first.game.fd);
	EXPECT_EQ(0, second.game.fd);
}

TEST(ClientState, default_module_values)
{
	ClientState state;

	EXPECT_EQ(-1, state.attack.country_origen);
	EXPECT_EQ(-1, state.attack.show_src);
	EXPECT_EQ(-1, state.ejer2.last_country);
	EXPECT_EQ(-1, state.reagrupe.last_origen);
	EXPECT_EQ(0, state.fichas.total);
	EXPECT_EQ(nullptr, state.theme.current);
}

}
