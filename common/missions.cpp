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
#include "common.h"


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

std::string missions_get_name(int number)
{
	if((number < 0) || (((unsigned)number) >= NRMISSIONS)) {
		return _("Unknown");
	}
	ContinentNames names{
		_(g_conts[0].name),
		_(g_conts[1].name),
		_(g_conts[2].name),
		_(g_conts[3].name),
		_(g_conts[4].name),
		_(g_conts[5].name),
	};
	return replace_continents(_(g_missions[number].name), names);
}
