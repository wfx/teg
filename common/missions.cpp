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

#include <type_traits>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "fcintl.h"
#include "missions.h"


MISSIONS g_missions[]= {
	// common mission, all time active: conquer the whole world
	[MISSION_CONQWORLD] = {
		.player_number=0,
		.name=N_("Conquer the world"),
		.continents={
			[CONTINENTE_AMERICASUR]=CONT_AMERICASUR_CANT,
			[CONTINENTE_AMERICANORTE]=CONT_AMERICANORTE_CANT,
			[CONTINENTE_AFRICA]=CONT_AFRICA_CANT,
			[CONTINENTE_OCEANIA]=CONT_OCEANIA_CANT,
			[CONTINENTE_EUROPA]=CONT_EUROPA_CANT,
			[CONTINENTE_ASIA]=CONT_ASIA_CANT
		},
		.frontiering_countries=0,
		.tot_countries=0
	},
	// optional common mission: conquer 30 countries
	[MISSION_COMMON] = {
		.player_number=0,
		.name=N_("Conquer 30 countries"),
		.continents={0, 0, 0, 0, 0, 0},
		.frontiering_countries=0,
		.tot_countries=30
	},
	{
		.player_number=-1,
		.name=N_("Conquer &2,\n5 countries of &1 and\n4 countries of &4"),
		.continents={
			[CONTINENTE_AMERICASUR]=0,
			[CONTINENTE_AMERICANORTE]=5,
			[CONTINENTE_AFRICA]=CONT_AFRICA_CANT,
			[CONTINENTE_OCEANIA]=0,
			[CONTINENTE_EUROPA]=4,
			[CONTINENTE_ASIA]=0
		},
		.frontiering_countries=0,
		.tot_countries=0
	},

#ifdef WITH_ALL_MISSIONS
	/*
	 * esta mision es dificil de soportar porque los countries limitrofes
	 * tiene que estar fuera de los continentes, y el manual no dice nada,
	 * etc, etc, encontes para evitar malos entendidos la desabilito
	 */
	{
		.player_number=-1,
		.name=N_("Conquer &0,\n7 countries of &4 and\n3 frontier countries"),
		.continents={
			[CONTINENTE_AMERICASUR]=CONT_AMERICASUR_CANT,
			[CONTINENTE_AMERICANORTE]=0,
			[CONTINENTE_AFRICA]=0,
			[CONTINENTE_OCEANIA]=0,
			[CONTINENTE_EUROPA]=7,
			[CONTINENTE_ASIA]=0
		},
		.frontiering_countries=3,
		.tot_countries=0
	},
#endif /* WITH_ALL_MISSIONS */

	{
		.player_number=-1,
		.name=N_("Conquer &5 and\n2 countries of &0"),
		.continents={
			[CONTINENTE_AMERICASUR]=2,
			[CONTINENTE_AMERICANORTE]=0,
			[CONTINENTE_AFRICA]=0,
			[CONTINENTE_OCEANIA]=0,
			[CONTINENTE_EUROPA]=0,
			[CONTINENTE_ASIA]=CONT_ASIA_CANT
		},
		.frontiering_countries=0,
		.tot_countries=0
	},

	{
		.player_number=-1,
		.name=N_("Conquer &4,\n4 countries of &5 and,\n2 of &0"),
		.continents={
			[CONTINENTE_AMERICASUR]=2,
			[CONTINENTE_AMERICANORTE]=0,
			[CONTINENTE_AFRICA]=0,
			[CONTINENTE_OCEANIA]=0,
			[CONTINENTE_EUROPA]=CONT_EUROPA_CANT,
			[CONTINENTE_ASIA]=4
		},
		.frontiering_countries=0,
		.tot_countries=0
	},

	{
		.player_number=-1,
		.name=N_("Conquer &1,\n2 countries of &3 and\n4 countries of &5"),
		.continents={
			[CONTINENTE_AMERICASUR]=0,
			[CONTINENTE_AMERICANORTE]=CONT_AMERICANORTE_CANT,
			[CONTINENTE_AFRICA]=0,
			[CONTINENTE_OCEANIA]=2,
			[CONTINENTE_EUROPA]=0,
			[CONTINENTE_ASIA]=4
		},
		.frontiering_countries=0,
		.tot_countries=0
	},

	{
		.player_number=-1,
		.name=N_("Conquer 2 countries of &3,\n2 of &2, 2 of &0,\n3 of &4, 4 of &1,\nand 3 of &5"),
		.continents={
			[CONTINENTE_AMERICASUR]=2,
			[CONTINENTE_AMERICANORTE]=4,
			[CONTINENTE_AFRICA]=2,
			[CONTINENTE_OCEANIA]=2,
			[CONTINENTE_EUROPA]=3,
			[CONTINENTE_ASIA]=3
		},
		.frontiering_countries=0,
		.tot_countries=0
	},

	{
		.player_number=-1,
		.name=N_("Conquer &3,\nconquer &1 and,\n2 countries of &4"),
		.continents={
			[CONTINENTE_AMERICASUR]=0,
			[CONTINENTE_AMERICANORTE]=CONT_AMERICANORTE_CANT,
			[CONTINENTE_AFRICA]=0,
			[CONTINENTE_OCEANIA]=CONT_OCEANIA_CANT,
			[CONTINENTE_EUROPA]=2,
			[CONTINENTE_ASIA]=0
		},
		.frontiering_countries=0,
		.tot_countries=0
	},

	{
		.player_number=-1,
		.name=N_("Conquer &0,\nconquer &2 and,\n4 countries of &5"),
		.continents={
			[CONTINENTE_AMERICASUR]=CONT_AMERICASUR_CANT,
			[CONTINENTE_AMERICANORTE]=0,
			[CONTINENTE_AFRICA]=CONT_AFRICA_CANT,
			[CONTINENTE_OCEANIA]=0,
			[CONTINENTE_EUROPA]=0,
			[CONTINENTE_ASIA]=4
		},
		.frontiering_countries=0,
		.tot_countries=0
	},

	{
		.player_number=-1,
		.name=N_("Conquer &3,\nconquer &2 and,\n5 countries of &1"),
		.continents={
			[CONTINENTE_AMERICASUR]=0,
			[CONTINENTE_AMERICANORTE]=5,
			[CONTINENTE_AFRICA]=CONT_AFRICA_CANT,
			[CONTINENTE_OCEANIA]=CONT_OCEANIA_CANT,
			[CONTINENTE_EUROPA]=0,
			[CONTINENTE_ASIA]=0
		},
		.frontiering_countries=0,
		.tot_countries=0
	},
};
constexpr auto NRMISSIONS = std::extent<decltype(g_missions)>::value;

int missions_cant()
{
	return NRMISSIONS;
}

/// \todo Refactor the following code into ideomatic c++ code, so that this
/// hack is not needed anymore.
#pragma GCC diagnostic ignored "-Wformat-truncation"

/* WARNING: it returns a pointer static buffer */
const char *missions_get_name(int number)
{
	static char const *_unknown = N_("Unknown");
	static char mission[1024];
	char buf_tmp[1024];
	char old_buffer[1024];
	int i, len;
	char *tmp_ptr;

	if((number < 0) || (((unsigned)number) >= NRMISSIONS)) {
		return _(_unknown);
	}

	snprintf(buf_tmp, sizeof(buf_tmp) -1, "%s", _(g_missions[number].name));

	/* find &0 - &5 and replace for the correct name */
	len = strlen(buf_tmp);

	memset(mission, 0,  sizeof(mission));

	tmp_ptr = &buf_tmp[0];

	int printf_result = -1;
	for(i=0; i<len;) {
		char *ptr;

		ptr = strchr(tmp_ptr, '&');

		if(ptr) {
			if(strlen(ptr)>1 && ptr[1] >= '0' && ptr[1] <= '5') {
				int real_n;
				char n[4];

				/* XXX: yuck, this is ugly */
				n[0] = ptr[1];
				n[1] = 0;

				real_n = atoi(&n[0]);
				ptr[0] = 0;
				strncpy(old_buffer, mission, sizeof(old_buffer) -1);
				printf_result = snprintf(mission, sizeof(mission),  "%s%s%s", old_buffer, tmp_ptr, _(g_conts[real_n].name));

				tmp_ptr = &ptr[2];
			} else {
				printf_result = snprintf(mission, sizeof(mission),  "Error: Unknown secret mission.\nPlease check translations!");
				break;
			}
		} else {
			strncpy(old_buffer, mission, sizeof(old_buffer) -1);
			printf_result = snprintf(mission, sizeof(mission),  "%s%s", old_buffer, tmp_ptr);
			break;
		}
	}

	if(printf_result<0) { // printf signalled an error
		return "Failure during mission printing";
	}

	if(((unsigned)printf_result)>=sizeof(mission)) { // string got truncated
		mission[sizeof(mission)-1] = 0;
	}

	return mission;
}
