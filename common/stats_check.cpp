// SPDX-License-Identifier: GPL-2.0-or-later

#include "stats.h"

#include <gtest/gtest.h>

std::ostream& operator <<(std::ostream& os, PLAYER_STATS const& ps)
{
	os << "PLAYER_STATS{"
	   << ps.countries_won << ", "
	   << ps.armies_killed
	   << ", {";
	const char* sep="";
	for(auto const turn: ps.continents_turn) {
		os << sep << turn;
		sep = ", ";
	}
	os << "}, "
	   << ps.players_killed << ", "
	   << ps.score << "}";
	return os;
}

bool operator == (const PLAYER_STATS &a, const PLAYER_STATS& b)
{
	return memcmp(&a, &b, sizeof(a)) == 0;
}

TEST(Stats, init)
{
	PLAYER_STATS ps{.countries_won = 1,
		            .armies_killed=  2,
		            .continents_turn = {1, 2, 3, 4, 5, 6},
		            .players_killed = 4,
		            .score = 5};
	stats_init(&ps);
	EXPECT_EQ(PLAYER_STATS{}, ps);
}

TEST(Stats, score)
{
	PLAYER_STATS ps{.countries_won = 1,
		            .armies_killed=  2,
		            .continents_turn = {1, 2, 3, 4, 5, 6},
		            .players_killed = 4,
		            .score = 5};
	CONT const continents[CONTINENTE_LAST]{
		[CONTINENTE_AMERICASUR] = {.fichas_x_cont=23},
		[CONTINENTE_AMERICANORTE] = {.fichas_x_cont=42},
		[CONTINENTE_AFRICA] = {.fichas_x_cont=17},
		[CONTINENTE_OCEANIA] = {.fichas_x_cont=5},
		[CONTINENTE_EUROPA] = {.fichas_x_cont=99},
		[CONTINENTE_ASIA] = {.fichas_x_cont=8},
	};
	PLAYER_STATS const expected{.countries_won = 1,
		                        .armies_killed=  2,
		                        .continents_turn = {1, 2, 3, 4, 5, 6},
		                        .players_killed = 4,
		                        .score = 1*5 // won countries*5
		                                 + 2*2 // kiled armies * 2

		                                 // turns a continent was owned
		                                 // * extra armies per continent
		                                 + 1*23 + 2*42 + 3*17 + 4*5 + 5*99 + 6*8};

	stats_score(&ps, continents);
	EXPECT_EQ(expected, ps);
}
