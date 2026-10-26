// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "scores.h"

/// State that is shared by the client, the server and the robot.
struct CommonState {
	HighScores highscores; ///< the high score table
};
