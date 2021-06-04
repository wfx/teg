// SPDX-License-Identifier: GPL-2.0-or-later

#include "parser_private.h"

#include <gtest/gtest.h>

TEST(Parser, belongs_to_class)
{
	DELIM const delim_null{0};
	DELIM const delim_invalid{.valid=false};
	EXPECT_FALSE(parser_belongs_to_class('x', delim_invalid)) << "Must not crash";
	EXPECT_FALSE(parser_belongs_to_class('x', delim_null));
	EXPECT_TRUE(parser_belongs_to_class(0, delim_null));

	DELIM const abc{'a'};
	EXPECT_FALSE(parser_belongs_to_class('x', abc));
	EXPECT_TRUE(parser_belongs_to_class('a', abc));
}

TEST(Parser, character_class)
{
	DELIM const invalid{.valid=false};
	EXPECT_EQ(ccEnd, parser_character_class(0, invalid, invalid));
	EXPECT_EQ(ccEnd, parser_character_class('\n', invalid, invalid));
	EXPECT_EQ(ccEnd, parser_character_class('\r', invalid, invalid));

	const DELIM a{'a'};
	const DELIM z{'z'};
	EXPECT_EQ(ccData, parser_character_class('a', invalid, invalid));
	EXPECT_EQ(ccData, parser_character_class('I', a, z));
	EXPECT_EQ(ccEquals, parser_character_class('a', a, invalid));
	EXPECT_EQ(ccEquals, parser_character_class('a', a, z));
	EXPECT_EQ(ccEquals, parser_character_class('a', a, a)) << "Precedence wrong";
	EXPECT_EQ(ccSeparators, parser_character_class('z', a, z));
	EXPECT_EQ(ccSeparators, parser_character_class('z', invalid, z));
}


TEST(Parser, analyze)
{
	char out[PARSER_TOKEN_MAX+1];

	int pos;
	auto const pat = [&pos, &out](char const* line, int end_offset = 1) {
		pos = -1;
		memset(out, '#', sizeof(out));
		DELIM const equals{'='}, separators{';'};
		return parser_analyze_token(&pos, line, out, equals, separators,
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
	const DELIM invalid{.valid=false};

	{
		// single token parse
		char const*const fb = "foobar";
		PARSER foobar{fb, invalid, invalid};
		EXPECT_TRUE(foobar.parse());
		EXPECT_FALSE(foobar.can_continue());
		EXPECT_STREQ(fb, foobar.token);
		EXPECT_EQ(0, foobar.value[0]);
		EXPECT_EQ(nullptr, foobar.remainder());
	}

	{
		// single assignment
		PARSER assignment{"foo=bar", eql, invalid};
		EXPECT_TRUE(assignment.parse());
		EXPECT_FALSE(assignment.can_continue());
		EXPECT_STREQ("foo", assignment.token);
		EXPECT_STREQ("bar", assignment.value);
		EXPECT_EQ(nullptr, assignment.remainder());
	}

	{
		// non-exhaustive single token
		char const*const input = "foo;bar";
		// single assignment
		PARSER multitoken{input, invalid, sep};
		EXPECT_TRUE(multitoken.parse());
		EXPECT_TRUE(multitoken.can_continue());
		EXPECT_STREQ("foo", multitoken.token);
		EXPECT_EQ(0, multitoken.value[0]);
		EXPECT_EQ(input+4, multitoken.remainder());
	}

	{
		// non-exhaustive assignment
		char const*const input = "foo=bar;baz";
		PARSER multiassignment{input, eql, sep};
		EXPECT_TRUE(multiassignment.parse());
		EXPECT_TRUE(multiassignment.can_continue());
		EXPECT_STREQ("foo", multiassignment.token);
		EXPECT_STREQ("bar", multiassignment.value);
		EXPECT_EQ(8+input, multiassignment.remainder());
	}
	{
		// defect input string.
		char buf[PARSER_TOKEN_MAX+1] {};
		strcpy(buf, "err\"or");
		PARSER error{buf, invalid, invalid};
		EXPECT_FALSE(error.parse());
	}
}

TEST(Parser, op_int)
{
	PARSER p{"123,456"};
	int a{0}, b{0}, c{23};

	p >> a;
	EXPECT_FALSE(p.finished());
	EXPECT_TRUE(p.ok());
	EXPECT_EQ(123, a);

	p >> b;
	EXPECT_TRUE(p.finished());
	EXPECT_TRUE(p.ok());
	EXPECT_EQ(456, b);

	p >> c;
	EXPECT_FALSE(p.finished());
	EXPECT_FALSE(p.ok());
	EXPECT_EQ(23, c);
}

TEST(Parser, op_limit)
{
	PARSER p{"1,2,3"};

	{
		// values in between the limits
		int a{0}, b{0};
		p >> Limited{a, 0, 5, 100} >> Limited{b, 0, 5, 100};
		EXPECT_FALSE(p.finished());
		EXPECT_TRUE(p.ok());
		EXPECT_EQ(1, a);
		EXPECT_EQ(2, b);
	}

	{
		// value above upper limit
		int c{0};
		p >> Limited{c, 0, 2, 100};
		EXPECT_FALSE(p.finished());
		EXPECT_FALSE(p.ok());
		EXPECT_EQ(100, c);
	}

	{
		// value below lower limit
		int d{0};
		p.reset("4");
		p >> Limited{d, 5, 6, 100};
		EXPECT_FALSE(p.finished());
		EXPECT_FALSE(p.ok());
		EXPECT_EQ(100, d);
	}

	{
		// value exact on the upper/lower limit
		int e{0}, f{0};
		p.reset("23,42");
		p >> Limited{e, 23, 42, 100} >> Limited{f, 23, 42, 100};
		EXPECT_TRUE(p.finished());
		EXPECT_TRUE(p.ok());
		EXPECT_EQ(23, e);
		EXPECT_EQ(42, f);
	}
}

TEST(Parser, op_str)
{
	PARSER p{"qwert,asdfg"};

	char a[100], b[4];

	p >> CString{a} >> CString{b};
	EXPECT_TRUE(p.finished());
	EXPECT_TRUE(p.ok());
	EXPECT_STREQ("qwert", a);
	EXPECT_STREQ("asd", b);
}
