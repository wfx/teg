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

#ifdef __cplusplus
extern "C" {
#endif

#include "../../common/common.h"
#include "../../common/country.h"

/* constructor */
TEG_STATUS locate_country_init();


/* adds a country to view */
TEG_STATUS locate_country_add_country(PCOUNTRY p);

/* adds the country's army to view */
TEG_STATUS locate_country_add_army(PCOUNTRY p);

#ifdef __cplusplus
}
#endif
