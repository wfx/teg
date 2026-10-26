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

#include <functional>
#include <list>

#include "../common/common.h"
#include "player.h"

namespace teg::client
{

struct ClientState;

namespace callbacks
{

extern TEG_STATUS gui_init(ClientState& state, int argc, char **argv);
extern TEG_STATUS gui_main(ClientState& state);
extern TEG_STATUS gui_textmsg(ClientState& state, char *aString);
extern TEG_STATUS gui_disconnect(ClientState& state);
extern TEG_STATUS gui_habilitado(ClientState& state, int numjug);
extern TEG_STATUS gui_connected(ClientState& state, const int *c);
extern TEG_STATUS gui_reconnected(ClientState& state);
extern TEG_STATUS gui_status(ClientState& state);
extern TEG_STATUS gui_start(ClientState& state);
extern TEG_STATUS gui_fichas(ClientState& state, int cant, int conts);
extern TEG_STATUS gui_turn(ClientState& state, PCPLAYER pJ);
extern TEG_STATUS gui_country(ClientState& state, int p);
extern TEG_STATUS gui_country_select(ClientState& state, int country);
extern TEG_STATUS gui_tropas(ClientState& state, int src, int dst, int cant);
extern TEG_STATUS gui_tarjeta(ClientState& state, int country);
extern TEG_STATUS gui_dados(ClientState& state);
extern TEG_STATUS gui_reagrupe(ClientState& state, int src, int dst, int cant);
extern TEG_STATUS gui_sensi(ClientState& state);
extern TEG_STATUS gui_attack(ClientState& state, int src, int dst);
extern TEG_STATUS gui_canje(ClientState& state, int cant, int p1, int p2, int p3);
extern TEG_STATUS gui_lost(ClientState& state, int numjug);
extern TEG_STATUS gui_surrender(ClientState& state, int numjug);
extern TEG_STATUS gui_winner(ClientState& state, int numjug, int mission);
extern TEG_STATUS gui_mission(ClientState& state);
extern TEG_STATUS gui_textplayermsg(ClientState& state, char const *n, int nj, char const *m);
extern TEG_STATUS gui_scores(ClientState& state);

}

}
