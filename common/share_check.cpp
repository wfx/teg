// SPDX-License-Identifier: GPL-2.0-or-later

#include "share.h"

#include <type_traits>

#include <gtest/gtest.h>

TEST(Share, is_option)
{
	EXPECT_TRUE(is_option("--foo", "--foo"));
	EXPECT_TRUE(is_option("--foo", "-f"));
	EXPECT_TRUE(is_option("--foo", "-fx"));
	EXPECT_FALSE(is_option("--foo", "--fooabr"));
	EXPECT_FALSE(is_option("--foo", "-x"));
	EXPECT_FALSE(is_option("--foo", "-xf"));
}

TEST(Share, get_option)
{
	char const* argv[] = {
	    "/foo/bar",
	    "--bar=42",
	    "--baz", "23",
	    "--invalid=",
	    "-x=y",
	    "-z", "z",
	    "-E"
	};
	constexpr auto argc = std::extent<decltype(argv)>::value;
	int i = 1;

	// not present
	EXPECT_EQ(nullptr, get_option("--abc", argv, &i, argc));
	EXPECT_EQ(i, 1);

	// not the correct start position
	i = 1;
	EXPECT_EQ(nullptr, get_option("--baz", argv, &i, argc));
	EXPECT_EQ(i, 1);

	// first argument
	i = 1;
	EXPECT_STREQ("42", get_option("--bar", argv, &i, argc));
	EXPECT_EQ(i, 1);

	// second argument
	i = 2;
	EXPECT_STREQ("23", get_option("--baz", argv, &i, argc));
	EXPECT_EQ(i, 3);

	// third/defect argument
	i = 3;
	EXPECT_EQ(nullptr, get_option("--invalid", argv, &i, argc));
	EXPECT_EQ(i, 3);

	// short option with equal sign
	i = 5;
	EXPECT_STREQ("y", get_option("--xxx", argv, &i, argc));
	EXPECT_EQ(i, 5);

	// short option with space delimiter
	i = 6;
	EXPECT_STREQ("z", get_option("--zzz", argv, &i, argc));
	EXPECT_EQ(i, 7);

	// defect last option
	i = 7;
	EXPECT_EQ(nullptr, get_option("--End", argv, &i, argc));
	EXPECT_EQ(i, 7);
}
