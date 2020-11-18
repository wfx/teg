// SPDX-License-Identifier: GPL-2.0-or-later

#include "parser_private.h"

#include <gtest/gtest.h>

TEST(Parser, belongs_to_class)
{
	DELIM const delim_null{0};
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
	EXPECT_EQ(ccData, parser_character_class('a', nullptr, nullptr));
	EXPECT_EQ(ccData, parser_character_class('I', &a, &z));
	EXPECT_EQ(ccEquals, parser_character_class('a', &a, nullptr));
	EXPECT_EQ(ccEquals, parser_character_class('a', &a, &z));
	EXPECT_EQ(ccEquals, parser_character_class('a', &a, &a)) << "Precedence wrong";
	EXPECT_EQ(ccSeparators, parser_character_class('z', &a, &z));
	EXPECT_EQ(ccSeparators, parser_character_class('z', nullptr, &z));
}


TEST(Parser, analyze)
{
	char out[PARSER_TOKEN_MAX+1];

	int pos;
	auto const pat = [&pos, &out](char const* line, int end_offset = 1) {
		pos = -1;
		memset(out, '#', sizeof(out));
		DELIM const equals{'='}, separators{';'};
		return parser_analyze_token(&pos, line, out, &equals, &separators,
		                            strlen(line)+end_offset);
	};
	EXPECT_EQ(PARSER_FIN, pat(""));
	EXPECT_EQ(0, pos);
	EXPECT_STREQ("", out);

	EXPECT_EQ(PARSER_FIN, pat("abcdefg"));
	EXPECT_EQ(7, pos);
	EXPECT_STREQ("abcdefg", out);

	// end of input string -> error
	EXPECT_EQ(PARSER_ERROR, pat("", 0));
	EXPECT_EQ(-1, pos);

	EXPECT_EQ(PARSER_SEPARADOR, pat("abcdefg;hij"));
	EXPECT_EQ(7, pos);
	EXPECT_STREQ("abcdefg", out);

	EXPECT_EQ(PARSER_IGUAL, pat("abcdefg=hij"));
	EXPECT_EQ(7, pos);
	EXPECT_STREQ("abcdefg", out);

	EXPECT_EQ(PARSER_IGUAL, pat("a\"bcde\"fg=hij"));
	EXPECT_EQ(9, pos);
	EXPECT_STREQ("abcdefg", out);

	EXPECT_EQ(PARSER_ERROR, pat("a\"bcdefg=hij"));
	EXPECT_EQ(-1, pos);
	EXPECT_STREQ("abcdefg=hij", out);
}

TEST(Parser, parse)
{
	const DELIM eql{'='};
	const DELIM sep{';'};

	{
		// single token parse
		char const*const fb = "foobar";
		PARSER foobar{.data=fb, .can_continue=true, .token={'x'}, .value={'y'}};
		EXPECT_TRUE(parser_parse(&foobar));
		EXPECT_FALSE(foobar.can_continue);
		EXPECT_STREQ(fb, foobar.token);
		EXPECT_EQ(0, foobar.value[0]);
		EXPECT_EQ(nullptr, foobar.data);
	}

	{
		// single assignment
		PARSER assignment{.data="foo=bar", .can_continue=true,
		                  .token={'x'}, .value={'y'}, .equals=&eql};
		EXPECT_TRUE(parser_parse(&assignment));
		EXPECT_FALSE(assignment.can_continue);
		EXPECT_STREQ("foo", assignment.token);
		EXPECT_STREQ("bar", assignment.value);
		EXPECT_EQ(nullptr, assignment.data);
	}

	{
		// non-exhaustive single token
		char const*const input = "foo;bar";
		// single assignment
		PARSER multitoken{.data=input, .can_continue=false,
		                  .token={'x'}, .value={'y'}, .separators=&sep};
		EXPECT_TRUE(parser_parse(&multitoken));
		EXPECT_TRUE(multitoken.can_continue);
		EXPECT_STREQ("foo", multitoken.token);
		EXPECT_EQ(0, multitoken.value[0]);
		EXPECT_EQ(input+4, multitoken.data);
	}

	{
		// non-exhaustive assignment
		char const*const input = "foo=bar;baz";
		PARSER multiassignment{.data=input, .can_continue=false,
		                       .token={'x'}, .value={'y'},
		                       .equals=&eql, .separators=&sep};
		EXPECT_TRUE(parser_parse(&multiassignment));
		EXPECT_TRUE(multiassignment.can_continue);
		EXPECT_STREQ("foo", multiassignment.token);
		EXPECT_STREQ("bar", multiassignment.value);
		EXPECT_EQ(8+input, multiassignment.data);
	}
	{
		// defect input string.
		PARSER error{.data="err\"or"};
		EXPECT_FALSE(parser_parse(&error));
	}
}
