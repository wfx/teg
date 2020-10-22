// SPDX-License-Identifier: GPL-2.0-or-later

#include "parser_private.h"

#include <gtest/gtest.h>

TEST(Parser, belongs_to_class)
{
	EXPECT_FALSE(parser_belongs_to_class('x', nullptr)) << "Must not crash";
	EXPECT_FALSE(parser_belongs_to_class('x', &delim_null));
	EXPECT_TRUE(parser_belongs_to_class(0, &delim_null));
	DELIM const abc{'a', 'b', 'c'};
	EXPECT_FALSE(parser_belongs_to_class('x', &abc));
	EXPECT_TRUE(parser_belongs_to_class('a', &abc));
	EXPECT_TRUE(parser_belongs_to_class('b', &abc));
	EXPECT_TRUE(parser_belongs_to_class('c', &abc));
}

TEST(Parser, character_class)
{
	EXPECT_EQ(PARSER_FIN, parser_character_class(0, nullptr, nullptr));
	EXPECT_EQ(PARSER_FIN, parser_character_class('\n', nullptr, nullptr));
	EXPECT_EQ(PARSER_FIN, parser_character_class('\r', nullptr, nullptr));

	const DELIM a__{'a', 'b', 'c'};
	const DELIM abc{'a', 'b', 'c'};
	const DELIM xyz{'x', 'y', 'z'};
	EXPECT_EQ(PARSER_DATA, parser_character_class('a', &delim_null, &delim_null));
	EXPECT_EQ(PARSER_IGUAL, parser_character_class('a', &a__, &delim_null));
	EXPECT_EQ(PARSER_IGUAL, parser_character_class('a', &abc, &delim_null));
	EXPECT_EQ(PARSER_IGUAL, parser_character_class('b', &abc, &delim_null));
	EXPECT_EQ(PARSER_IGUAL, parser_character_class('c', &abc, &delim_null));
	EXPECT_EQ(PARSER_IGUAL, parser_character_class('a', &a__, &xyz));
	EXPECT_EQ(PARSER_IGUAL, parser_character_class('a', &a__, &a__))
	        << "Precedence wrong";
	EXPECT_EQ(PARSER_SEPARADOR, parser_character_class('a', &xyz, &a__));
}
