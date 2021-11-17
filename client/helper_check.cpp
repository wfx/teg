// SPDX-License-Identifier: GPL-2.0-or-later

#include "helper.h"

#include <gtest/gtest.h>

namespace teg::client
{

TEST(ClientHelper, aux_status)
{
	Player player{.name={'x'}, .addr={'y'}, .color=23, .score=42, .numjug=17,
	               .estado=PLAYER_STATUS_CONNECTED, .tot_countries=9,
	               .tot_cards=3, .empezo_turno=5, .human=12};
	const char*const input = "name,2,3,1,7,5,4,9,7,8,addr";
	EXPECT_EQ(TEG_STATUS_SUCCESS, aux_status(&player, input));
	EXPECT_STREQ("name", player.name);
	EXPECT_EQ(2, player.color);
	EXPECT_EQ(3, player.score);
	EXPECT_EQ(1, player.numjug);
	static_assert(unsigned(PLAYER_STATUS_FICHAS2) == 7,
	              "The value in the input string does not match the reference");
	EXPECT_EQ(PLAYER_STATUS_FICHAS2, player.estado);
	EXPECT_EQ(5, player.tot_countries);
	EXPECT_EQ(4, player.tot_armies);
	EXPECT_EQ(9, player.tot_cards);
	EXPECT_EQ(7, player.empezo_turno);
	EXPECT_EQ(8, player.human);
	EXPECT_STREQ("addr", player.addr);
}

TEST(ClientHelper, aux_scores)
{
	SCORES sc{.name={'a'}, .color=6, .date={'#'}, .human=99, .score=1337};
	char const*const input = "foobar,13,insertDateHere,23,42";
	EXPECT_EQ(TEG_STATUS_SUCCESS, aux_scores(&sc, input));
	EXPECT_STREQ("foobar", sc.name);
	EXPECT_EQ(13, sc.color);
	EXPECT_STREQ("insertDateHere", sc.date);
	EXPECT_EQ(42, sc.human);
	EXPECT_EQ(23, sc.score);
}

}
