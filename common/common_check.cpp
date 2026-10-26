// SPDX-License-Identifier: GPL-2.0-or-later

#include "common.h"

#include <cstring>

#include <gtest/gtest.h>

TEST(Common, color_name)
{
	EXPECT_STREQ("red", color_name(ARMY_ROJO));
	EXPECT_STREQ("yellow", color_name(ARMY_AMARILLO));
	EXPECT_STREQ("blue", color_name(ARMY_AZUL));
	EXPECT_STREQ("black", color_name(ARMY_NEGRO));
	EXPECT_STREQ("pink", color_name(ARMY_ROSA));
	EXPECT_STREQ("green", color_name(ARMY_VERDE));
	EXPECT_STREQ("unknown", color_name(-1));
	EXPECT_STREQ("unknown", color_name(maximum_player_count));
}

TEST(Common, player_status_name)
{
	EXPECT_STREQ("disconnected", player_status_name(PLAYER_STATUS_DESCONECTADO));
	EXPECT_STREQ("connected", player_status_name(PLAYER_STATUS_CONNECTED));
	EXPECT_STREQ("game over", player_status_name(PLAYER_STATUS_GAMEOVER));
	EXPECT_STREQ("ending turn", player_status_name(PLAYER_STATUS_TURNOEND));
	EXPECT_STREQ("unknown", player_status_name(-1));
	EXPECT_STREQ("unknown", player_status_name(PLAYER_STATUS_LAST));
}

TEST(Common, cards_for_this_exchange)
{
	EXPECT_EQ(0, cards_for_this_exchange(0));
	EXPECT_EQ(4, cards_for_this_exchange(1));
	EXPECT_EQ(7, cards_for_this_exchange(2));
	EXPECT_EQ(10, cards_for_this_exchange(3));
	EXPECT_EQ(15, cards_for_this_exchange(4));
	EXPECT_EQ(20, cards_for_this_exchange(5));
	EXPECT_EQ(45, cards_for_this_exchange(10));
	EXPECT_EQ(495, cards_for_this_exchange(100));
}

TEST(Common, strip_invalid)
{
	char string[] = "a=b:c\\d,e:f/g%h\"i";
	strip_invalid(string);
	EXPECT_STREQ("a.b.c.d.e.f.g.h\"i", string);
}

TEST(Common, strip_invalid_msg)
{
	char string[] = "a=b:c\\d,e:f/g%h\"i";
	strip_invalid_msg(string);
	EXPECT_STREQ("a=b:c\\d,e:f/g%h\'i", string);
}

TEST(Common, my_atoi)
{
	EXPECT_EQ(-1, my_atoi(nullptr));
	EXPECT_EQ(-50, my_atoi("-50"));
	EXPECT_EQ(-1, my_atoi("-1"));
	EXPECT_EQ(0, my_atoi("0"));
	EXPECT_EQ(1, my_atoi("1"));
	EXPECT_EQ(23, my_atoi("23"));
}

TEST(Common, string_copy)
{
	char dest[5];

	std::memset(dest, '#', sizeof(dest));
	string_copy(dest+2, sizeof(dest)-2, "asdfg");
	EXPECT_STREQ("##as", dest);

	std::memset(dest, '#', sizeof(dest));
	string_copy(dest, sizeof(dest), "abc");
	EXPECT_STREQ("abc", dest);

	std::memset(dest, '#', sizeof(dest));
	string_copy(dest, sizeof(dest), "abcdefg");
	EXPECT_STREQ("abcd", dest);

	std::memset(dest, '#', sizeof(dest));
	string_copy(dest, 1, "abcdefg");
	EXPECT_STREQ("", dest);
}

TEST(Common, replace_continents)
{
	ContinentNames const replacements{
		"Foo",
		"Bar",
		"Baz",
		"BigBigBigContinentName",
		"!'§$%&/()=?",
		"No. 6"
	};
	EXPECT_EQ("", replace_continents("", replacements));
	EXPECT_EQ("Foo", replace_continents("&0", replacements));
	EXPECT_EQ("Bar", replace_continents("&1", replacements));
	EXPECT_EQ("Baz", replace_continents("&2", replacements));
	EXPECT_EQ("BigBigBigContinentName", replace_continents("&3", replacements));
	EXPECT_EQ("!'§$%&/()=?", replace_continents("&4", replacements));
	EXPECT_EQ("No. 6", replace_continents("&5", replacements));
	EXPECT_EQ("", replace_continents("&6", replacements));

	EXPECT_EQ("x Bar y", replace_continents("x &1 y", replacements));
	EXPECT_EQ("Conquer BigBigBigContinentName, !'§$%&/()=?, Bar, Baz, Foo "
	          "with the focus on BigBigBigContinentName -- No. 6",
	          replace_continents(
	              "Conquer &3, &4, &1, &2, &0 with the focus on &3 -- &5",
	              replacements));
}
