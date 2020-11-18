// SPDX-License-Identifier: GPL-2.0-or-later

#include "helper.h"

#include <initializer_list>
#include <string>

#include <gtest/gtest.h>

std::string printable_list(char const* input, std::size_t length)
{
	std::string result{'['};
	result.reserve(2 // brackets
	               + 4*length // maximum 4 chars per number (-128)
	               + 2*(length-1) // ", " - separators
	              );
	char const*sep = "";
	for(std::size_t i=0; i<length; i++) {
		result += sep;
		result += std::to_string(input[i]);
		sep = ", ";
	}
	result += ']';
	return result;
}

using CIOPattern = std::initializer_list<std::pair<char, char>>;
void check_ins_orden(CIOPattern pre_post, char value)
{
	char precondition[pre_post.size()];
	char postcondition[pre_post.size()];

	{
		std::size_t i = 0;
		for(auto const& element: pre_post) {
			precondition[i] = element.first;
			postcondition[i] = element.second;
			i++;
		}
	}
	std::string const pre_printable{printable_list(precondition, pre_post.size())};
	std::string const post_printable{printable_list(postcondition, pre_post.size())};
	ins_orden(value, precondition, sizeof(precondition));
	std::string const processed_printable{printable_list(precondition, pre_post.size())};
	for(std::size_t i=0; i<sizeof(precondition); i++) {
		EXPECT_EQ(postcondition[i], precondition[i])
		        << "Failed at position " << i << ", "
		        "input=" << pre_printable << ", "
		        "wanted=" << post_printable << ", "
		        "got=" << processed_printable << ", "
		        "inserted value=" << int(value);
	}

}

TEST(ServerHelper, ins_orden)
{
	check_ins_orden(CIOPattern{{2, 2}, {1, 1}, {0, 0}}, -1);
	check_ins_orden(CIOPattern{{2, 2}, {1, 1}, {0, 0}}, 0);
	check_ins_orden(CIOPattern{{2, 2}, {1, 1}, {0, 1}}, 1);
	check_ins_orden(CIOPattern{{2, 2}, {1, 2}, {0, 1}}, 2);
	check_ins_orden(CIOPattern{{2, 3}, {1, 2}, {0, 1}}, 3);
	check_ins_orden(CIOPattern{{40, 40}, {30, 30}, {20, 25}, {10, 20}, {0, 10}}, 25);
}
