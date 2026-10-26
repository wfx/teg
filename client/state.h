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

#include <array>
#include <list>
#include <memory>

#include "../common/common.h"
#include "../common/cont.h"
#include "../common/country.h"
#include "../common/state.h"
#include "../common/transport.h"

#include "player.h"
#include "themes.h"

namespace teg::client
{

typedef struct _cgame {
	PLAYER_STATUS estado;		/**< estado del game */
	int numjug;			/**< numero de player (yo) */
	int human;			/**< Am I a human ? */
	int observer;			/**< si soy observer o player */
	int playeres;			/**< cantidad de playeres */
	char myname[max_playername_length];
	int mycolor;			/**< color preferido */
	char sername[maximum_servername_length];	/**< server name */
	int serport;			/**< server port */
	int tarjetas_cant;		/**< cantidad de tarjetas */
	int secret_mission;		/**< which secret mission to accomplish */
	int dados_srccountry;		/**< utimo country que ataco */
	int dados_src[3];		/**< ultimos dados del atacante */
	int dados_dstcountry;		/**< ultimo country que se defendio */
	int dados_dst[3];		/**< ultimos dados del defensor */
	int msg_show;			/**< mascara de los mensajes que hay que mostrar */
	int already_connected;		/**< already connected to the server */
	int robot_in_server;		/**< the robot will be launched in the server */
	char theme[THEME_MAX_NAME];	/**< theme que se esta usando */
	int whos_turn;			/**< playernumber who has the turn */
	int who_started_round;		/**< player who started the round */
	int round_number;		/**< round number */
	bool	with_secret_mission;	/**< play with secret missions */
	bool	with_common_mission;	/**< if playing with secret mission, includes the common mission */
	bool	with_fog_of_war;	/**< playing in Fog of War type of game */

	CommonState common;	/**< state shared with the server and the robot */
} CJUEGO, *PCJUEGO;

/// \brief All mutable state of one client (or robot) instance.
struct ClientState {
	CJUEGO game{};
	ClientPlayerList players{};
	bool first_time{true};

	/// \brief The connection to the game server
	std::unique_ptr<Transport> connection;

	/// State of the client side attack handling
	struct {
		int country_origen{-1};
		int country_destino{-1};
		int country_origen_bak{-1};
		int country_destino_bak{-1};
		int show_src{-1};
		int show_dst{-1};
	} attack;

	/// State of the army placement
	struct {
		std::array<int, COUNTRIES_CANT> countries{};
		std::array<int, CONT_CANT> continents{};
		int total{0};
		int wanted{0};
		int wanted_continents{0};
	} fichas;

	/// State of the two-army card exchange
	struct {
		int last_country{-1};
	} ejer2;

	/// State of the army movement
	struct {
		int country_origen{-1};
		int country_destino{-1};
		int last_origen{-1};
		int last_destino{-1};
		int last_cant{-1};
	} reagrupe;

	/// State of the theme handling
	struct {
		pTheme current{nullptr};
		ThemeDirectories available{};
	} theme;
};

}
