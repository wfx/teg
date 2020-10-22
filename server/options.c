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
 * @file options.c
 * Manages the server options
 */

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "server.h"
#include "fow.h"

TEG_STATUS option_conqworld(int fd, char *str);
TEG_STATUS option_conqworld_view( void );
TEG_STATUS option_cmission( int fd, char *str );
TEG_STATUS option_cmission_view( void );
TEG_STATUS option_fog_of_war( int fd, char *str );
TEG_STATUS option_fog_of_war_view( void );
TEG_STATUS option_armies(int fd, char *str);
TEG_STATUS option_armies_view( void );
TEG_STATUS option_seed(int fd, char *str);
TEG_STATUS option_seed_view( void );
TEG_STATUS option_help( int fd, char *str );

TEG_STATUS option_lookup( int fd, PARSER *p );

struct {
	char *label;
	TEG_STATUS (*func) ();
	TEG_STATUS (*func_view) ();
	char *help;
} options[] = {
	{ OPTION_CONQWORLD, option_conqworld, option_conqworld_view, N_("Conquer the world, or play with secret missions") },
	{ OPTION_CMISSION, option_cmission, option_cmission_view, N_("Enable/Disable common secret mission") },
	{ OPTION_ARMIES, option_armies, option_armies_view, N_("Set the initial number of armies to place") },
	{ OPTION_SEED, option_seed, option_seed_view, N_("Set the seed for random") },
	{ OPTION_FOG_OF_WAR, option_fog_of_war, option_fog_of_war_view, N_("Enable/Disable the Fog of War") },
	{ OPTION_HELP, option_help, NULL, N_("Shows the set options") },
};
#define	NOPTIONS  (sizeof(options)/sizeof(options[0]))

TEG_STATUS option_conqworld_view( void )
{
	con_text_out_wop(M_INF,_("'%s' variable is set to: %s\n"),
		OPTION_CONQWORLD,
		(!g_game.mission?_("TRUE"):_("FALSE"))
		);
	return TEG_STATUS_SUCCESS;
}
TEG_STATUS option_conqworld( int fd, char *str )
{
	int a,b;

	if( str && strlen(str) !=0 ) {
		a = atoi(str);
		b = !g_game.mission;
		if( mission_set( !a ) == TEG_STATUS_SUCCESS ) {
			con_text_out_wop(M_INF,_("Setting %s variable to: %s -> %s\n"),
				OPTION_CONQWORLD,
				( b?_("TRUE"):_("FALSE")),
				( a?_("TRUE"):_("FALSE"))
				);
		} else goto error;
	} else {
		option_conqworld_view();
	}
	return TEG_STATUS_SUCCESS;

error:
	if(fd==CONSOLE_FD)
		con_text_out_wop(M_ERR,_("Error setting %s variable.\n"),OPTION_CONQWORLD);

	return TEG_STATUS_ERROR;
}

TEG_STATUS option_cmission_view( void )
{
	con_text_out_wop(M_INF,_("'%s' variable is set to: %s\n"),
		OPTION_CMISSION,
		(g_game.cmission?_("TRUE"):_("FALSE"))
		);
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS option_cmission( int fd, char *str )
{
	int a,b;

	if( str && strlen(str) !=0 ) {
		a = atoi(str);
		b = g_game.cmission;
		if( mission_common_mission( a ) == TEG_STATUS_SUCCESS ) {
			con_text_out_wop(M_INF,_("Setting %s variable to: %s -> %s\n"),
				OPTION_CMISSION,
				( b?_("TRUE"):_("FALSE")),
				( a?_("TRUE"):_("FALSE"))
				);
		} else goto error;
	} else {
		option_cmission_view();
	}
	return TEG_STATUS_SUCCESS;

error:
	if(fd==CONSOLE_FD)
		con_text_out_wop(M_ERR,_("Error setting %s variable.\n"),OPTION_CMISSION);

	return TEG_STATUS_ERROR;
}

TEG_STATUS option_fog_of_war_view( void )
{
	con_text_out_wop(M_INF,_("'%s' variable is set to: %s\n"),
		OPTION_FOG_OF_WAR,
		(g_game.fog_of_war?_("TRUE"):_("FALSE"))
		);
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS option_fog_of_war( int fd, char *str )
{
	int a,b;

	if( str && strlen(str) !=0 ) {
		a = atoi(str);
		b = g_game.fog_of_war;
		if( fow_set_mode( a ) == TEG_STATUS_SUCCESS ) {
			con_text_out_wop(M_INF,_("Setting %s variable to: %s -> %s\n"),
				OPTION_FOG_OF_WAR,
				( b?_("TRUE"):_("FALSE")),
				( a?_("TRUE"):_("FALSE"))
				);
		} else goto error;
	} else {
		option_fog_of_war_view();
	}
	return TEG_STATUS_SUCCESS;

error:
	if(fd==CONSOLE_FD)
		con_text_out_wop(M_ERR,_("Error setting %s variable.\n"),OPTION_CMISSION);

	return TEG_STATUS_ERROR;
}

TEG_STATUS option_armies_view( void )
{
	con_text_out_wop(M_INF,_("'%s' variable is set to: (%d,%d)\n"),OPTION_ARMIES,g_game.fichas,g_game.fichas2);
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS option_armies( int fd, char *str )
{
	int armies1;
	int armies2;
	PARSER p;
	DELIM igualador={ '|', '|', '|' };
	DELIM separador={ ',', ',', ',' };

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	if(str && strlen(str)!=0) {

		if( JUEGO_EMPEZADO )
			goto error;

		if( parser_call( &p ) && p.hay_otro ) {
			armies1 = atoi( p.token );		
		} else goto error;

		if( parser_call( &p ) && !p.hay_otro ) {
			armies2 = atoi( p.token );		
		} else goto error;

		if(armies1 && armies2 ) {
			g_game.fichas = armies1;
			g_game.fichas2 = armies2;
		} else goto error;
	}
	option_armies_view();
	return TEG_STATUS_SUCCESS;
error:
	return TEG_STATUS_ERROR;
}

TEG_STATUS option_seed_view( void )
{
	con_text_out_wop(M_INF,_("'%s' variable is set to: %u\n"),OPTION_SEED,g_game.seed);
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS option_seed( int fd, char *str )
{
	unsigned int seed;

	if(str && strlen(str)!=0) {

		if(fd!=CONSOLE_FD)
			goto error;

		seed = atoi(str);
		g_game.seed = seed;
		srandom(seed);
	}

	option_seed_view();
	return TEG_STATUS_SUCCESS;
error:
	return TEG_STATUS_ERROR;
}

TEG_STATUS option_help( int fd, char *str )
{
	int i;
	for(i=0;i<NOPTIONS;i++) {
		if(options[i].func)
			net_printf(fd, TOKEN_REM"='%s' %s\n",options[i].label,_(options[i].help));
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS option_lookup( int fd, PARSER *p )
{
	int i;

	for(i = 0; i < NOPTIONS; i++) {
		if(strcasecmp( p->token, options[i].label )==0 ){
			if (options[i].func)
				return( (options[i].func)(fd,p->value));

			return TEG_STATUS_TOKENNULL;
		}
	}
	net_printf(fd,_("Unknow option. Try using `%s %s' for help\n"),TOKEN_SET,OPTION_HELP);
	return TEG_STATUS_TOKENNOTFOUND;
}

TEG_STATUS option_view( int fd, char *str)
{
	int i;

	for(i = 0; i < NOPTIONS; i++) {
		if (options[i].func_view)
			(options[i].func_view)();
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS option_parse( int fd, char *str)
{
	int i;
	PARSER p;
	DELIM igualador={ '=', ' ', ':' };

	p.igualador = &igualador;
	p.separador = NULL;
	p.data = str;

	do {
		if( (i=parser_call( &p )) ) {
			if( option_lookup( fd,&p ) == TEG_STATUS_CONNCLOSED ) {
				return TEG_STATUS_CONNCLOSED;
			}
		}
	} while( i && p.hay_otro);

	return TEG_STATUS_SUCCESS;
}
