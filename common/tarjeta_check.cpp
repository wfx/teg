// SPDX-License-Identifier: GPL-2.0-or-later

#include "tarjeta.h"

#include <gtest/gtest.h>

#include "country.h"


TEST(Tarjeta, card_belongs_to_player)
{
	const std::size_t checkCountry = 15;
	const int myself = 23;
	const int other = 42;
	static_assert(checkCountry < COUNTRIES_CANT);

	g_countries[checkCountry].tarjeta.numjug = other;
	EXPECT_FALSE(card_belongs_to_player(myself, 0)) << "Players differ";

	EXPECT_FALSE(card_belongs_to_player(myself, COUNTRIES_CANT))
	        << "Country number out of range";
	g_countries[checkCountry].tarjeta.numjug = myself;
	EXPECT_TRUE(card_belongs_to_player(myself, checkCountry));

	g_countries[checkCountry].tarjeta.numjug = -1;
}

TEST(TARJETA, can_trade_cards)
{
	struct TestCase {
		bool expectation;
		TARJTIPO a, b, c;
	};

	TestCase const testTable[] = {
		{ true,  TARJ_GALEON,  TARJ_GALEON,  TARJ_GALEON}, // only one kind of cards
		{false,  TARJ_GALEON,  TARJ_GALEON,  TARJ_CANION}, // no match
		{false,  TARJ_GALEON,  TARJ_GALEON,   TARJ_GLOBO}, // no match
		{ true,  TARJ_GALEON,  TARJ_GALEON, TARJ_COMODIN}, // joker
		{false,  TARJ_GALEON,  TARJ_CANION,  TARJ_GALEON}, // no match
		{false,  TARJ_GALEON,  TARJ_CANION,  TARJ_CANION}, // no match
		{ true,  TARJ_GALEON,  TARJ_CANION,   TARJ_GLOBO}, // 3 different cards
		{ true,  TARJ_GALEON,  TARJ_CANION, TARJ_COMODIN}, // joker
		{false,  TARJ_GALEON,   TARJ_GLOBO,  TARJ_GALEON}, // no match
		{ true,  TARJ_GALEON,   TARJ_GLOBO,  TARJ_CANION}, // 3 different cards
		{false,  TARJ_GALEON,   TARJ_GLOBO,   TARJ_GLOBO}, // no match
		{ true,  TARJ_GALEON,   TARJ_GLOBO, TARJ_COMODIN}, // joker
		{ true,  TARJ_GALEON, TARJ_COMODIN,  TARJ_GALEON}, // joker
		{ true,  TARJ_GALEON, TARJ_COMODIN,  TARJ_CANION}, // joker
		{ true,  TARJ_GALEON, TARJ_COMODIN,   TARJ_GLOBO}, // joker
		{ true,  TARJ_GALEON, TARJ_COMODIN, TARJ_COMODIN}, // joker
		{false,  TARJ_CANION,  TARJ_GALEON,  TARJ_GALEON}, // no match
		{false,  TARJ_CANION,  TARJ_GALEON,  TARJ_CANION}, // no match
		{ true,  TARJ_CANION,  TARJ_GALEON,   TARJ_GLOBO}, // 3 different cards
		{ true,  TARJ_CANION,  TARJ_GALEON, TARJ_COMODIN}, // joker
		{false,  TARJ_CANION,  TARJ_CANION,  TARJ_GALEON}, // no match
		{ true,  TARJ_CANION,  TARJ_CANION,  TARJ_CANION}, // only one kind of cards
		{false,  TARJ_CANION,  TARJ_CANION,   TARJ_GLOBO}, // no match
		{ true,  TARJ_CANION,  TARJ_CANION, TARJ_COMODIN}, // joker
		{ true,  TARJ_CANION,   TARJ_GLOBO,  TARJ_GALEON}, // 3 different cards
		{false,  TARJ_CANION,   TARJ_GLOBO,  TARJ_CANION}, // no match
		{false,  TARJ_CANION,   TARJ_GLOBO,   TARJ_GLOBO}, // no match
		{ true,  TARJ_CANION,   TARJ_GLOBO, TARJ_COMODIN}, // joker
		{ true,  TARJ_CANION, TARJ_COMODIN,  TARJ_GALEON}, // joker
		{ true,  TARJ_CANION, TARJ_COMODIN,  TARJ_CANION}, // joker
		{ true,  TARJ_CANION, TARJ_COMODIN,   TARJ_GLOBO}, // joker
		{ true,  TARJ_CANION, TARJ_COMODIN, TARJ_COMODIN}, // joker
		{false,   TARJ_GLOBO,  TARJ_GALEON,  TARJ_GALEON}, // no match
		{ true,   TARJ_GLOBO,  TARJ_GALEON,  TARJ_CANION}, // 3 different cards
		{false,   TARJ_GLOBO,  TARJ_GALEON,   TARJ_GLOBO}, // no match
		{ true,   TARJ_GLOBO,  TARJ_GALEON, TARJ_COMODIN}, // joker
		{ true,   TARJ_GLOBO,  TARJ_CANION,  TARJ_GALEON}, // 3 different cards
		{false,   TARJ_GLOBO,  TARJ_CANION,  TARJ_CANION}, // no match
		{false,   TARJ_GLOBO,  TARJ_CANION,   TARJ_GLOBO}, // no match
		{ true,   TARJ_GLOBO,  TARJ_CANION, TARJ_COMODIN}, // joker
		{false,   TARJ_GLOBO,   TARJ_GLOBO,  TARJ_GALEON}, // no match
		{false,   TARJ_GLOBO,   TARJ_GLOBO,  TARJ_CANION}, // no match
		{ true,   TARJ_GLOBO,   TARJ_GLOBO,   TARJ_GLOBO}, // only one kind of cards
		{ true,   TARJ_GLOBO,   TARJ_GLOBO, TARJ_COMODIN}, // joker
		{ true,   TARJ_GLOBO, TARJ_COMODIN,  TARJ_GALEON}, // joker
		{ true,   TARJ_GLOBO, TARJ_COMODIN,  TARJ_CANION}, // joker
		{ true,   TARJ_GLOBO, TARJ_COMODIN,   TARJ_GLOBO}, // joker
		{ true,   TARJ_GLOBO, TARJ_COMODIN, TARJ_COMODIN}, // joker
		{ true, TARJ_COMODIN,  TARJ_GALEON,  TARJ_GALEON}, // joker
		{ true, TARJ_COMODIN,  TARJ_GALEON,  TARJ_CANION}, // joker
		{ true, TARJ_COMODIN,  TARJ_GALEON,   TARJ_GLOBO}, // joker
		{ true, TARJ_COMODIN,  TARJ_GALEON, TARJ_COMODIN}, // joker
		{ true, TARJ_COMODIN,  TARJ_CANION,  TARJ_GALEON}, // joker
		{ true, TARJ_COMODIN,  TARJ_CANION,  TARJ_CANION}, // joker
		{ true, TARJ_COMODIN,  TARJ_CANION,   TARJ_GLOBO}, // joker
		{ true, TARJ_COMODIN,  TARJ_CANION, TARJ_COMODIN}, // joker
		{ true, TARJ_COMODIN,   TARJ_GLOBO,  TARJ_GALEON}, // joker
		{ true, TARJ_COMODIN,   TARJ_GLOBO,  TARJ_CANION}, // joker
		{ true, TARJ_COMODIN,   TARJ_GLOBO,   TARJ_GLOBO}, // joker
		{ true, TARJ_COMODIN,   TARJ_GLOBO, TARJ_COMODIN}, // joker
		{ true, TARJ_COMODIN, TARJ_COMODIN,  TARJ_GALEON}, // joker
		{ true, TARJ_COMODIN, TARJ_COMODIN,  TARJ_CANION}, // joker
		{ true, TARJ_COMODIN, TARJ_COMODIN,   TARJ_GLOBO}, // joker
		{ true, TARJ_COMODIN, TARJ_COMODIN, TARJ_COMODIN}, // only one kind of cards
	};

	for(const auto& check: testTable) {
		EXPECT_EQ(check.expectation, can_trade_cards(check.a, check.b, check.c))
		        << "Failed for " << check.a << check.b << check.c;
	}
}
