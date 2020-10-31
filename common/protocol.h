/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <riq@corest.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; only version 2 of the License.
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
/*
 *	Definicion del protocolo
 */

#ifndef __TEG_PROTOCOL_H
#define __TEG_PROTOCOL_H

#define PROTOCOL_HIVER	6
#define PROTOCOL_LOVER	0
#define PROT_MAX_LEN	(5000)

/* Poner todo en minuscula */

/* tokens */
#define TOKEN_CVERSION		"client_version"
#define TOKEN_DISCON		"disconnect"
#define TOKEN_EXIT		"exit"
#define TOKEN_GAMEOVER		"game_over"
#define TOKEN_HELP		"help"
#define TOKEN_LOST		"loser"
#define TOKEN_MESSAGE		"msg"
#define TOKEN_PLAYERID		"player_id"
#define TOKEN_RECONNECT		"reconnect"
#define TOKEN_NEWPLAYER		"newplayer"
#define TOKEN_PVERSION		"protocol_version"
#define TOKEN_QUMM		"quiero_un_mundo_mejor"
#define TOKEN_REM		"*"
#define TOKEN_ROBOT		"quiero_jugar_con_un_robot"
#define TOKEN_SCORES		"scores"
#define TOKEN_STATS		"stats"
#define TOKEN_SERVERFULL	"el_servidor_esta_lleno"
#define TOKEN_GAMEINPROGRESS	"game_in_progress"
#define TOKEN_SVERSION		"server_version"
#define TOKEN_ALONE		"solo_a_ningun_lado"
#define TOKEN_START		"start"
#define TOKEN_STATUS		"status"
#define TOKEN_TEST		"eva_test"
#define	TOKEN_TURNO		"turno"
#define	TOKEN_WAIT		"wait"
#define	TOKEN_WINNER		"winner"
#define TOKEN_ERROR		"error"
#define TOKEN_OK		"ok"
#define TOKEN_COUNTRIES		"paises"
#define TOKEN_FICHAS		"fichas"
#define TOKEN_FICHAS2		"fichas2"
#define TOKEN_FICHASC		"fichasc"
#define TOKEN_FICHAS3		"fichas3"
#define TOKEN_ATAQUE		"ataque"
#define TOKEN_TROPAS		"tropas"
#define TOKEN_COUNTRY		"pais"
#define TOKEN_REAGRUPE		"reagrupe"
#define TOKEN_TARJETA		"tarjeta"
#define TOKEN_DADOS		"dados"
#define TOKEN_CANJE		"canje"
#define TOKEN_EJER2		"ejer2"
#define TOKEN_MODALIDAD		"modalidad"
#define TOKEN_MISSION		"mission"
#define TOKEN_SET		"set"
#define TOKEN_VIEW		"view"
#define TOKEN_COLOR		"color"
#define TOKEN_SAVE		"save"
#define TOKEN_LOQUE		"que_tengo_que_hacer"
#define TOKEN_PACTO_REQUEST	"pacto_request"
#define TOKEN_PACTO_REPLY	"pacto_reply"
#define TOKEN_SURRENDER		"soy_cobarde"
#define TOKEN_KICK		"kick"
#define TOKEN_ENUM_CARDS	"enum_cards"
#define TOKEN_NEW_ROUND		"new_round"
#define TOKEN_METASERVER	"metaserver"

/* options */
#define OPTION_CONQWORLD	"conquer_world"
#define OPTION_ARMIES		"armies"
#define OPTION_SEED		"seed"
#define OPTION_HELP		"help"
#define OPTION_CMISSION		"common_mission"
#define OPTION_FOG_OF_WAR	"fog_of_war"

/* metaserver */
#define OPTION_META_LIST	"list"
#define OPTION_META_ON		"on"
#define OPTION_META_OFF		"off"
#define OPTION_META_ADDRESS	"address"
#define OPTION_META_STATUS	"status"

#endif /* __TEG_PROTOCOL_H */
