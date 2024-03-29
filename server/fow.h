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
#include "player.h"

namespace teg::server
{

/// enables or disables FOW
TEG_STATUS fow_set_mode(bool b);

/// returns true if player pJ can see country pP
bool fow_can_player_see_country(PSPLAYER pJ, PCOUNTRY pP);

/// fills in buffer the boundaries countries of 'country'
TEG_STATUS fow_fill_with_boundaries(int country, char *buffer, int buf_len);

/// send the message to all players who has a country boundary with 'country'
int fow_netall_printf(int country, const char *format, ...);

/// send the message to all players who has a country boundary with  country1 and country2
int fow_2_netall_printf(int country1, int country2, const char *format, ...);

}
