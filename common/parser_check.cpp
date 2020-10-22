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
	EXPECT_EQ(ccEnd, parser_character_class(0, nullptr, nullptr));
	EXPECT_EQ(ccEnd, parser_character_class('\n', nullptr, nullptr));
	EXPECT_EQ(ccEnd, parser_character_class('\r', nullptr, nullptr));

	const DELIM a{'a'};
	const DELIM z{'z'};
	EXPECT_EQ(ccData, parser_character_class('a', &delim_null, &delim_null));
	EXPECT_EQ(ccEquals, parser_character_class('a', &a, nullptr));
	EXPECT_EQ(ccEquals, parser_character_class('a', &a, &z));
	EXPECT_EQ(ccEquals, parser_character_class('a', &a, &a)) << "Precedence wrong";
	EXPECT_EQ(ccSeparators, parser_character_class('z', &a, &z));
	EXPECT_EQ(ccSeparators, parser_character_class('z', nullptr, &z));
}
