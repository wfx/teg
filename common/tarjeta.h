/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <rquesada@core-sdi.com>
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

#pragma once

#include <type_traits>

#include "common.h"

/** \brief The data type for country numbers.
 *
 * This is defined here to break a dependency cycle between the Country and
 * Tarjeta structures. */
using CountryId = int;

#define TARJETA_NULL { 0, true, false }
#define TARJETA_NULL_COMODIN TARJ_COMODIN
#define TARJETA_NULL_GALEON TARJ_GALEON
#define TARJETA_NULL_CANION TARJ_CANION
#define TARJETA_NULL_GLOBO TARJ_GLOBO

enum CardKind: unsigned {
	TARJ_GALEON = 1,		/**< valor del galeon */
	TARJ_CANION = 4,		/**< valor del canion */
	TARJ_GLOBO = 8,			/**< valor del globo */
	TARJ_COMODIN = 16,		/**< valor del comodin */
};
using TARJTIPO = std::underlying_type<CardKind>::type;

struct TARJETA {
	TARJTIPO tarjeta; /**< tipo de tarjeta */
	int	numjug;			/**< si la tiene algun jugador y que jugador */
	bool usada;			/**< si se pusieron las 2 fichas del country */
	CountryId country; ///< The country id for this card
};
using PTARJETA = TARJETA*;

/*
 * Funciones y varialbes exportadas
 */
bool card_belongs_to_player(int player, int country);
bool can_trade_cards(TARJTIPO a, TARJTIPO b, TARJTIPO c);

bool tarjeta_puedocanje(int numjug, int t1, int t2, int t3);
void tarjeta_init(void);
void tarjeta_inittarj(PTARJETA t);
void tarjeta_poner(PTARJETA t);
void tarjeta_sacar(PTARJETA t, int numjug);
void tarjeta_usar(PTARJETA pT);
void tarjeta_desusar(PTARJETA pT);
bool tarjeta_es_libre(int i);
bool tarjeta_es_usada(PTARJETA pT);
