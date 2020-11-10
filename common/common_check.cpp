// SPDX-License-Identifier: GPL-2.0-or-later

#include "common.h"

#include <cstring>

#include <gtest/gtest.h>

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
