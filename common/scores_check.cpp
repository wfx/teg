// SPDX-License-Identifier: GPL-2.0-or-later

#include "scores.h"

#include <cstring>

#include <gtest/gtest.h>

template <std::size_t N>
bool strneq(char const(&a)[N], char const(&b)[N])
{
	return std::strncmp(a, b, N) == 0;
}

bool operator == (const SCORES& a, const SCORES& b)
{
	return strneq(a.name, b.name)
	       && (a.color == b.color)
	       && strneq(a.date, b.date)
	       && (a.human == b.human)
	       && (a.score == b.score);
}

std::ostream& operator << (std::ostream& os, const SCORES& score)
{
	os << "Score{" << score.name
	   << ", " << score.color
	   << ", " << score.date
	   << ", " << score.human
	   << ", " << score.score << "}";
	return os;
}

using SCS = std::initializer_list<SCORES>;
void check_scores(SCS scores, HighScores const& hs, unsigned linenumber)
{
	EXPECT_EQ(scores.size(), hs.count) << " origin: " << linenumber;
	size_t i=0;
	for(const SCORES score: scores) {
		EXPECT_EQ(score, hs.highscores[i])
		        << "Failed for dataset #" << i
		        << " origin: " << linenumber;
		i++;
	}
}

TEST(Scores, insert)
{
	HighScores hs {
		.count = 0,
		.highscores = {}
	};

	SCORES n{.name={'n'}, .color=1, .date={'D', 'n'}, .human=1, .score=0};
	SCORES a{.name={'a'}, .color=2, .date={'D', 'a'}, .human=0, .score=23};
	SCORES b{.name={'b'}, .color=7, .date={'D', 'b'}, .human=2, .score=42};
	SCORES c{.name={'c'}, .color=4, .date={'D', 'c'}, .human=5, .score=99};

	// variant: random insert and appends
	insert_highscore(&hs, &b);
	check_scores({b}, hs, __LINE__);

	insert_highscore(&hs, &a);
	check_scores({b, a}, hs, __LINE__);

	insert_highscore(&hs, &c);
	check_scores({c, b, a}, hs, __LINE__);

	// Fill the array completely
	insert_highscore(&hs, &a);
	check_scores({c, b, a, a}, hs, __LINE__);
	insert_highscore(&hs, &b);
	check_scores({c, b, b, a, a}, hs, __LINE__);
	insert_highscore(&hs, &a);
	check_scores({c, b, b, a, a, a}, hs, __LINE__);
	insert_highscore(&hs, &c);
	check_scores({c, c, b, b, a, a, a}, hs, __LINE__);
	insert_highscore(&hs, &b);
	check_scores({c, c, b, b, b, a, a, a}, hs, __LINE__);
	insert_highscore(&hs, &n);
	check_scores({c, c, b, b, b, a, a, a, n}, hs, __LINE__);
	insert_highscore(&hs, &n);
	check_scores({c, c, b, b, b, a, a, a, n, n}, hs, __LINE__);
	// Now the highscore list is full -> only replacements
	insert_highscore(&hs, &n);
	check_scores({c, c, b, b, b, a, a, a, n, n}, hs, __LINE__);
	insert_highscore(&hs, &b);
	check_scores({c, c, b, b, b, b, a, a, a, n}, hs, __LINE__);
	insert_highscore(&hs, &a);
	check_scores({c, c, b, b, b, b, a, a, a, a}, hs, __LINE__);
	insert_highscore(&hs, &c);
	check_scores({c, c, c, b, b, b, b, a, a, a}, hs, __LINE__);

	// variant: Move the first element on second insert
	hs.count = 0;
	insert_highscore(&hs, &n);
	check_scores({n}, hs, __LINE__);
	insert_highscore(&hs, &a);
	check_scores({a, n}, hs, __LINE__);
	insert_highscore(&hs, &b);
	check_scores({b, a, n}, hs, __LINE__);
	insert_highscore(&hs, &c);
	check_scores({c, b, a, n}, hs, __LINE__);

	// variant: append only
	hs.count = 0;
	insert_highscore(&hs, &c);
	check_scores({c}, hs, __LINE__);
	insert_highscore(&hs, &b);
	check_scores({c, b}, hs, __LINE__);
	insert_highscore(&hs, &a);
	check_scores({c, b, a}, hs, __LINE__);
	insert_highscore(&hs, &n);
	check_scores({c, b, a, n}, hs, __LINE__);
}
