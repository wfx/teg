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

typedef enum {
	CONTINENTE_AMERICASUR,
	CONTINENTE_AMERICANORTE,
	CONTINENTE_AFRICA,
	CONTINENTE_OCEANIA,
	CONTINENTE_EUROPA,
	CONTINENTE_ASIA,
	CONTINENTE_LAST,
} CONTINENTE, *PCONTINENTE;

#define		CONT_CANT	CONTINENTE_LAST

#define CONT_AMERICASUR_CANT	6
#define CONT_AMERICANORTE_CANT	10
#define CONT_AFRICA_CANT	6
#define CONT_OCEANIA_CANT	4
#define CONT_EUROPA_CANT	9
#define CONT_ASIA_CANT		15

typedef struct _cont {
	char const *name;		/**< name del continenete */
	CONTINENTE id;			/**< id del continente */
	int	cant_countries;		/**< cantidad de countries que tiene el continenete */
	int	fichas_x_cont;		/**< cantidad de fichas adicionales por tener el cont*/
} CONT, *PCONT;

extern CONT g_conts[];

/**
 * \brief Calculates the sum of all additional armies given for completely
 * conquered continents.
 *
 * \param conts A bitfield, where the bit for each completely conquered
 *              continent is set to 1.
 *
 *              Example: if the player conquered oceania and asia, the bitfield
 *              equals to (1 << CONTINENTE_OCEANIA) | (1 << CONTINENTE_ASIA)
 *                      = (1 << 3)                  | (1 << 5)
 *                      =     8                     |    32
 *                      =                          40
 *
 * \return The total number of additional armies. For oceania and asia, this
 *         would be 9 (2 for oceania + 7 for asia).
 */
int cont_tot(unsigned long conts);

/// \return the translated name of the continent with number \p cont_number
char const* cont_get_name(unsigned cont_number);
