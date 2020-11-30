/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <riq@core-sdi.com>
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
#include "cont.h"

namespace teg::robot
{

extern int ai_puntaje[COUNTRIES_CANT*10];
extern int ai_sorted[COUNTRIES_CANT*10];

/// \brief Initialize / reset the per country attack score
TEG_STATUS ai_init();

/**
 * \brief Tries to perform one attack.
 *
 * \note The next attack will be started by the timeout in gui_main and the
 *       out_loque() call there.
 */
TEG_STATUS ai_turno();

/// Move the armies after the attack turn.
TEG_STATUS ai_reagrupe();

/// Move armies from my counry to the new conquered country
TEG_STATUS ai_tropas(int src, int dst, int cant);

/// Try to exchange cards
TEG_STATUS ai_puedocanje(int *p1, int *p2, int *p3);

/// Do I own the continent number \p c?
bool ai_own_continent(CONTINENTE c);

/// \todo Find out what this function does
TEG_STATUS ai_puntaje_sort(int cant);

/// Reset the sorting mechanism
void ai_puntaje_clean();

}
