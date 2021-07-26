/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <rquesada@core-sdi.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; only version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#pragma once

#include "../common/common.h"

typedef struct _aimsg {
	char const *msg;
	int args;
} aimsg_t, *paimsg_t;

enum {
	AI_MSG_HI,
	AI_MSG_BYE,
	AI_MSG_MISC,
	AI_MSG_ANSWER,

	AI_MSG_LAST,
};

/// Creates a random message, with a random frequency
TEG_STATUS ai_msg(int type, const char *n);

/// Choose a random name
const char *ai_name();

/// Checks if the name is a name the robot would use
TEG_STATUS ai_findname(const char *name);
