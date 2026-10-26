// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "cont.h"
#include "country.h"
#include "scores.h"

/// State that is shared by the client, the server and the robot.
struct CommonState {
	Continents conts{make_conts()}; ///< the continent definitions
	Countries countries{make_countries()}; ///< the country definitions and state
	HighScores highscores{}; ///< the high score table
};
