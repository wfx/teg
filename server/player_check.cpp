// SPDX-License-Identifier: GPL-2.0-or-later

#include "player.h"
#include "../common/country.h"

#include <gtest/gtest.h>

TEST(player, init_player)
{
	SPLAYER player{.hizo_canje=true, .turno_conq=12, .tot_countries=5,
	               .tot_armies=323, .tot_cards=8, .tot_exchanges=7,
	               .fichasc_armies=17, .fichasc_conts = 9};
	player_initplayer(&player);
	EXPECT_FALSE(player.hizo_canje);
	EXPECT_EQ(0, player.tot_exchanges);
	EXPECT_EQ(0, player.tot_countries);
	EXPECT_EQ(0, player.tot_cards);
	EXPECT_EQ(0, player.tot_armies);
	EXPECT_EQ(0, player.turno_conq);
	EXPECT_EQ(-1, player.country_src);
	EXPECT_EQ(-1, player.country_dst);
	EXPECT_EQ(-1, player.mission);
	EXPECT_EQ(0, player.fichasc_armies);
	EXPECT_EQ(0, player.fichasc_conts);
	EXPECT_EQ(&player.countries, player.countries.Blink);
	EXPECT_EQ(&player.countries, player.countries.Flink);
	EXPECT_EQ(&player.deals, player.deals.Blink);
	EXPECT_EQ(&player.deals, player.deals.Flink);
}

TEST(player, is_playing)
{
	{
		SPLAYER player{.is_player = false, .estado=PLAYER_STATUS_POSTFICHAS};
		EXPECT_FALSE(player_is_playing(&player));
	}

	{
		SPLAYER player{.is_player = true, .estado=PLAYER_STATUS_HABILITADO};
		EXPECT_FALSE(player_is_playing(&player));
	}

	{
		SPLAYER player{.is_player = true, .estado=PLAYER_STATUS_START};
		EXPECT_TRUE(player_is_playing(&player));
	}

	{
		SPLAYER player{.is_player = true, .estado=PLAYER_STATUS_TURNOEND};
		EXPECT_TRUE(player_is_playing(&player));
	}

	{
		SPLAYER player{.is_player = true, .estado=PLAYER_STATUS_LAST};
		EXPECT_FALSE(player_is_playing(&player));
	}
}

TEST(player, lister_countries)
{
	COUNTRY countries[] = {
		[0] = {
			.next = {.Flink=&countries[1].next},
			.continente = CONTINENTE_AMERICASUR
		},
		[1] = {
			.next = {.Flink=&countries[2].next},
			.continente=CONTINENTE_AMERICASUR
		},
		[2] = {
			.next = {.Flink=nullptr},
			.continente=CONTINENTE_ASIA
		}
	};

	SPLAYER player{.countries={.Flink=&countries[0].next}};
	countries[2].next.Flink = &player.countries;
	int continent_counts[CONTINENTE_LAST] = {0};

	player_listar_countries(&player, continent_counts);

	int const ref[CONTINENTE_LAST] = {
		[CONTINENTE_AMERICASUR] = 2,
		[CONTINENTE_AMERICANORTE] = 0,
		[CONTINENTE_AFRICA] = 0,
		[CONTINENTE_OCEANIA] = 0,
		[CONTINENTE_EUROPA] = 0,
		[CONTINENTE_ASIA] = 1,
	};
	for(std::size_t i=0; i<CONTINENTE_LAST; i++) {
		EXPECT_EQ(ref[i], continent_counts[i]) << "Failed at index " << i;
	}
}

void check_player_count(int countries, int armies)
{
	SPLAYER player{.tot_countries = countries};
	EXPECT_EQ(armies, player_fichasc_cant(&player));
}

TEST(player, army_count)
{
	check_player_count(0, 3);
	check_player_count(6, 3);
	check_player_count(7, 3);
	check_player_count(42, 21);
}

TEST(player, clear_turn)
{
	const int player_number = 42;
	SPLAYER player{.numjug = player_number, .turno_conq = 23,
	               .estado=PLAYER_STATUS_POSTFICHAS2,
	               .country_src=15, .country_dst=51};

	auto const belongs_to_player = [](int country_number)->bool {
		return (country_number==3) || (country_number==7) || (country_number==9);
	};

	for(std::size_t i=0; i<COUNTRIES_CANT; i++) {
		g_countries[i].numjug = belongs_to_player(i)
		                        ? 42
		                        : 14;
		g_countries[i].ejer_reagrupe = i;
	}

	player_clear_turn(&player);
	EXPECT_EQ(0, player.turno_conq);
	EXPECT_EQ(PLAYER_STATUS_IDLE, player.estado);
	EXPECT_EQ(-1, player.country_src);
	EXPECT_EQ(-1, player.country_dst);

	for(std::size_t i=0; i<COUNTRIES_CANT; i++) {
		if(belongs_to_player(i)) {
			EXPECT_EQ(0, g_countries[i].ejer_reagrupe) << i;
		} else {
			EXPECT_EQ(i, g_countries[i].ejer_reagrupe) << i;
		}
	}
}
