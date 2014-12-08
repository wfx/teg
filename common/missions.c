/*	$Id: missions.c,v 1.4 2007/07/07 19:16:19 nordi Exp $	*/
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
/**
 * @file missions.c
 */

#include <stdio.h>
#include <string.h>

#include "all.h"

MISSIONS g_missions[]= {
	{ 0,	/* Default mission for a game without missions */
		N_("Conquer the world"),
		{CONT_AMERICASUR_CANT, CONT_AMERICANORTE_CANT, CONT_AFRICA_CANT, CONT_OCEANIA_CANT, CONT_EUROPA_CANT, CONT_ASIA_CANT},
		{0, 0, 0, 0, 0, 0},
		0,
		0 },
	{ 0,	/* Objetivo comun */
		N_("Conquer 30 countries"),
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		0,
		30 },
	{ -1,
		N_("Conquer &2,\n5 countries of &1 and\n4 countries of &4"),
		{0, 5, CONT_AFRICA_CANT, 0, 4, 0},
		{0, 0, 0, 0, 0, 0},
		0,
		0 },

#ifdef WITH_ALL_MISSIONS
	/* 
	 * esta mision es dificil de soportar porque los countries limitrofes
	 * tiene que estar fuera de los continentes, y el manual no dice nada,
	 * etc, etc, encontes para evitar malos entendidos la desabilito
	 */
	{ -1,
		N_("Conquer &0,\n7 countries of &4 and\n3 frontier countries"),
		{CONT_AMERICASUR_CANT, 0, 0, 0, 7, 0},
		{0, 0, 0, 0, 0, 0},
		3,
		0},
#endif /* WITH_ALL_MISSIONS */

	{ -1,
		N_("Conquer &5 and\n2 countries of &0"),
		{2, 0, 0, 0, 0, CONT_ASIA_CANT},
		{0, 0, 0, 0, 0, 0},
		0,
		0},

	{ -1,
		N_("Conquer &4,\n4 countries of &5 and,\n2 of &0"),
		{2, 0, 0, 0, CONT_EUROPA_CANT, 4},
		{0, 0, 0, 0, 0, 0},
		0,
		0},

	{ -1,
		N_("Conquer &1,\n2 countries of &3 and\n4 countries of &5"),
		{0, CONT_AMERICANORTE_CANT, 0, 2, 0, 4},
		{0, 0, 0, 0, 0, 0},
		0,
		0},

	{ -1,
		N_("Conquer 2 countries of &3,\n2 of &2, 2 of &0,\n3 of &4, 4 of &1,\nand 3 of &5"),
		{2, 4, 2, 2, 3, 3},
		{0, 0, 0, 0, 0, 0},
		0,
		0},

	{ -1,
		N_("Conquer &3,\nconquer &1 and,\n2 countries of &4"),
		{0, CONT_AMERICANORTE_CANT, 0, CONT_OCEANIA_CANT, 2, 0},
		{0, 0, 0, 0, 0, 0},
		0,
		0},

	{ -1,
		N_("Conquer &0,\nconquer &2 and,\n4 countries of &5"),
		{CONT_AMERICASUR_CANT, 0, CONT_AFRICA_CANT, 0, 0, 4},
		{0, 0, 0, 0, 0, 0},
		0,
		0},

	{ -1,
		N_("Conquer &3,\nconquer &2 and,\n5 countries of &1"),
		{0, 5, CONT_AFRICA_CANT, CONT_OCEANIA_CANT, 0, 0},
		{0, 0, 0, 0, 0, 0},
		0,
		0},
};
#define NRMISSIONS (sizeof(g_missions) /sizeof(g_missions[0]))

int missions_cant()
{
	return NRMISSIONS;
}

/* WARNING: it returns a pointer static buffer */
char * missions_get_name( int number )
{
	static char *_unknown = N_("Unknown");
	static char mission[1024];
	char buf_tmp[1024];
	char old_buffer[1024];
	int i,len;
	char *tmp_ptr;

	if( number < 0 || number >= NRMISSIONS )
		return _(_unknown);

	snprintf( buf_tmp, sizeof(buf_tmp) -1, "%s", _( g_missions[number].name ) );

	/* find &0 - &5 and replace for the correct name */
	len = strlen(buf_tmp);

	memset( mission, 0,  sizeof(mission) );

	tmp_ptr = &buf_tmp[0];

	for(i=0;i<len; )
	{
		char *ptr;

		ptr = strchr( tmp_ptr, '&' );

		if ( ptr ) {
			if( strlen(ptr)>1 && ptr[1] >= '0' && ptr[1] <= '5' ) {
				int real_n;
				char n[4];

				/* XXX: yuck, this is ugly */
				n[0] = ptr[1];
				n[1] = 0;

				real_n = atoi( &n[0] );
				ptr[0] = 0;
				strncpy( old_buffer, mission, sizeof(old_buffer) -1 );
				snprintf( mission, sizeof(mission) -1,  "%s%s%s", old_buffer, tmp_ptr, _(g_conts[real_n].name) );

				tmp_ptr = &ptr[2];
			} else {
				snprintf( mission, sizeof(mission) -1,  "Error: Unknown secret mission.\nPlease check translations!" );
				break;
			}
		} else {
			strncpy( old_buffer, mission, sizeof(old_buffer) -1 );
			snprintf( mission, sizeof(mission) -1,  "%s%s", old_buffer, tmp_ptr );
			break;
		}
	}

	return mission;
}
