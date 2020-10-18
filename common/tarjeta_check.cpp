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
