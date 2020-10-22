/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <riq@corest.com>
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
/*
 * Controls all the messages that receives the server
 */

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "server.h"
#include "xmlscores.h"
#include "fow.h"

#undef DEBUG_PLAY
#define PLAY_DEBUG PDEBUG

#ifdef DEBUG_PLAY
# define STATIC
#else
# define STATIC static
#endif


STATIC TEG_STATUS token_status(int, char *);
STATIC TEG_STATUS token_test(int, char *);
STATIC TEG_STATUS token_sversion(int, char *);
STATIC TEG_STATUS token_cversion(int, char *);
STATIC TEG_STATUS token_pversion(int, char *);
STATIC TEG_STATUS token_playerid(int, char *);
STATIC TEG_STATUS token_help(int, char *);
STATIC TEG_STATUS token_countries(int, char *);
STATIC TEG_STATUS token_fichas(int, char* );
STATIC TEG_STATUS token_fichas2(int, char *);
STATIC TEG_STATUS token_fichasc(int, char *);
STATIC TEG_STATUS token_attack(int , char *);
STATIC TEG_STATUS token_qumm(int fd, char *);
STATIC TEG_STATUS token_rem( int fd, char *);
STATIC TEG_STATUS token_message(int fd, char *);
STATIC TEG_STATUS token_tropas( int fd, char *str);
STATIC TEG_STATUS token_turn( int fd, char *str);
STATIC TEG_STATUS token_card( int fd, char *str );
STATIC TEG_STATUS token_regroup(int, char *);
STATIC TEG_STATUS token_canje(int, char*);
STATIC TEG_STATUS token_ejer2(int, char *);
STATIC TEG_STATUS token_mission(int, char *);
STATIC TEG_STATUS token_color(int, char*);
STATIC TEG_STATUS token_loque(int, char*);
STATIC TEG_STATUS token_echo(int, char*);
STATIC TEG_STATUS token_surrender(int, char*);
STATIC TEG_STATUS token_set(int, char*);
STATIC TEG_STATUS token_scores(int, char*);
STATIC TEG_STATUS token_enum_cards(int, char*);
STATIC TEG_STATUS token_robot(int, char*);
STATIC TEG_STATUS token_new_round( int fd, char *str );
STATIC TEG_STATUS token_typeofgame( int fd, char *str );

struct {
	char *label;
	TEG_STATUS (*func) ();
	char *help;
} tokens[] = {
	{ TOKEN_START,		token_start,	N_("to start the game") },
	{ TOKEN_STATUS,		token_status,	N_("shows the status of the players") },
	{ TOKEN_MESSAGE,	token_message,	N_("to send a message") },
	{ TOKEN_EXIT,		token_exit,	N_("to exit the game") },
	{ TOKEN_TEST,		token_test,	N_("internal use. Dont use it.") },
	{ TOKEN_CVERSION,	token_cversion,	N_("client version") },
	{ TOKEN_SVERSION,	token_sversion,	N_("server version") },
	{ TOKEN_PVERSION,	token_pversion,	N_("protocol version") },
	{ TOKEN_PLAYERID,	token_playerid,	N_("to register as a player") },
	{ TOKEN_HELP,		token_help,	N_("to ask for help") },
	{ TOKEN_REM,		token_rem,	N_("to comment a command") },
	{ TOKEN_QUMM,		token_qumm,	N_("It makes you more happy") },
	{ TOKEN_COUNTRIES,		token_countries,N_("It shows info about the countries") },
	{ TOKEN_FICHAS,		token_fichas,	N_("to place the initials 5 armies") },
	{ TOKEN_FICHAS2,	token_fichas2,	N_("to place the initials 3 armies") },
	{ TOKEN_FICHASC,	token_fichasc,	N_("to place the armies after a turn have finished") },
	{ TOKEN_ATAQUE,		token_attack,	N_("to attack a country") },
	{ TOKEN_TROPAS,		token_tropas,	N_("to send armies to a conquered country") },
	{ TOKEN_TARJETA,	token_card,	N_("to pick up a country-card") },
	{ TOKEN_REAGRUPE,	token_regroup,	N_("to reorder your armies") },
	{ TOKEN_TURNO,		token_turn,	N_("to finish your turn") },
	{ TOKEN_CANJE,		token_canje,	N_("to exchange your cards for armies") },
	{ TOKEN_EJER2,		token_ejer2,	N_("to place 2 armies in a country. You must have that card") },
	{ TOKEN_MISSION,	token_mission,	N_("request a mission") },
	{ TOKEN_COLOR,		token_color,	N_("to select a color") },
	{ TOKEN_LOQUE,		token_loque,	N_("to remind me what to do") },
	{ TOKEN_ECHO,		token_echo,	N_("to set an async callback") },
	{ TOKEN_SURRENDER,	token_surrender,N_("to surrender") },
	{ TOKEN_SET,		token_set,	N_("to set options") },
	{ TOKEN_SCORES,		token_scores,	N_("to show the highscores") },
	{ TOKEN_ENUM_CARDS,	token_enum_cards,N_("to show the cards a player has") },
	{ TOKEN_ROBOT,		token_robot,	N_("to play with a robot") },
	{ TOKEN_NEW_ROUND,	token_new_round,N_("to know who started the round, and the round number") },
	{ TOKEN_MODALIDAD,	token_typeofgame,N_("to know the type of game it is being played") },
};
#define	NTOKENS  (sizeof(tokens)/sizeof(tokens[0]))


/* Sets a server option */
STATIC TEG_STATUS token_set( int fd, char *str )
{
	if( strlen(str)==0 )
		goto error;

	if( option_parse(fd, str) == TEG_STATUS_SUCCESS )
		return TEG_STATUS_SUCCESS;

error:
	net_print(fd,TOKEN_ERROR"="TOKEN_SET"\n");
	return TEG_STATUS_PARSEERROR;
}

/* Sends the player the message he requested. Usefull as callback */
STATIC TEG_STATUS token_echo( int fd, char *msg )
{
	PSPLAYER pJ;
	if( player_whoisfd(fd, &pJ )!=TEG_STATUS_SUCCESS || strlen(msg)==0 )
		goto error;

	net_printf(fd,"%s\n",msg);
	return TEG_STATUS_SUCCESS;

error:
	net_print(fd,TOKEN_ERROR"="TOKEN_ECHO"\n");
	return TEG_STATUS_PARSEERROR;
}

/* Sends the player the actions he must do. Usefull as callback */
STATIC TEG_STATUS token_loque( int fd, char *unused )
{
	PSPLAYER pJ;
	PLAY_DEBUG("token_loque()");
	if( player_whoisfd( fd, &pJ ) != TEG_STATUS_SUCCESS ) {
		goto error;
	}

	switch( pJ->estado ) {
	case PLAYER_STATUS_FICHAS:
		net_printf( fd, "%s=%d,%d\n", TOKEN_FICHAS, g_game.turno->numjug,g_game.fichas);
		break;
	case PLAYER_STATUS_FICHAS2:
		net_printf( fd, "%s=%d,%d\n", TOKEN_FICHAS2, g_game.turno->numjug,g_game.fichas);
		break;
	case PLAYER_STATUS_FICHASC:
	case PLAYER_STATUS_CANJE:
		aux_token_fichasc( g_game.turno );
		break;
	case PLAYER_STATUS_ATAQUE:
	case PLAYER_STATUS_TROPAS:
		net_printf( fd, "%s=%d\n", TOKEN_TURNO, g_game.turno->numjug );
		break;
	default:
		break;
	}

	return TEG_STATUS_SUCCESS;
error:
	net_print(fd,TOKEN_ERROR"="TOKEN_LOQUE"\n");
	return TEG_STATUS_PARSEERROR;
}

/* Assigns the player a color */
STATIC TEG_STATUS token_color( int fd, char *str )
{
	int color;
	int a;
	PSPLAYER pJ;
	PLAY_DEBUG("token_color()");

	if( player_whoisfd( fd, &pJ ) != TEG_STATUS_SUCCESS ) {
		goto error;
	}

	if(strlen(str)==0)
		goto error;

	if( pJ->estado != PLAYER_STATUS_CONNECTED )
		goto error;

	if( pJ->is_player == FALSE )
		goto error;

	a = atoi( str );
	if (a < 0 ||  a >= TEG_MAX_PLAYERS )
		goto error;

	color = a;
	if ( color_libre( &color )  == FALSE )
		goto error;

	pJ->estado = PLAYER_STATUS_HABILITADO;
	pJ->color = color;
	con_text_out(M_INF,_("Player %s(%d) has color %s\n"),pJ->name,pJ->numjug,_(g_colores[color]));

	netall_printf( "%s=%s,%d,%d\n", TOKEN_NEWPLAYER, pJ->name, pJ->numjug, pJ->color );
	return TEG_STATUS_SUCCESS;
error:
	net_print(fd,TOKEN_ERROR"="TOKEN_COLOR"\n");
	return TEG_STATUS_PARSEERROR;
}


/* The player is asking a mission */
STATIC TEG_STATUS token_mission( int fd, char *unused )
{
	PSPLAYER pJ;
	PLAY_DEBUG("token_mission()");

	if( player_whoisfd( fd, &pJ ) != TEG_STATUS_SUCCESS ) {
		goto error;
	}

	if( pJ->estado < PLAYER_STATUS_START ) {
		goto error;
	}

	if( mission_asignar( pJ ) != TEG_STATUS_SUCCESS )
		goto error;

	net_printf( fd, "%s=%d\n", TOKEN_MISSION, pJ->mission);
	return TEG_STATUS_SUCCESS;
error:
	net_print(fd,TOKEN_ERROR"="TOKEN_MISSION"\n");
	return TEG_STATUS_PARSEERROR;
}

/* Called when a player finish his turn */
STATIC TEG_STATUS token_turn( int fd, char *unused )
{
	PSPLAYER pJ;
	PLAY_DEBUG("token_turn()");

	if( player_whoisfd( fd, &pJ ) != TEG_STATUS_SUCCESS ) {
		goto error;
	}

	if( pJ->estado < PLAYER_STATUS_TURNOSTART || pJ->estado > PLAYER_STATUS_TURNOEND )
		goto error;

	if( pJ != g_game.turno ) {
		con_text_out(M_ERR,_("BUG: The server believes that player `%s' does not have the turn"),pJ->name);
		goto error;
	}

	pJ->estado = PLAYER_STATUS_IDLE;

	if( turno_end( pJ ) != TEG_STATUS_SUCCESS )
		goto error;

	/* give turn to the next player */
	if( turno_next() != TEG_STATUS_SUCCESS )
		goto error;

	return TEG_STATUS_SUCCESS;
error:
	net_print(fd,TOKEN_ERROR"="TOKEN_TURNO"\n");
	return TEG_STATUS_PARSEERROR;
}

/* Sends all the player a message. An observer may use this function */
STATIC TEG_STATUS token_message( int fd, char *msg )
{
	PSPLAYER j;
	PLAY_DEBUG("token_message()\n");

	if( player_whoisfd(fd, &j )!=TEG_STATUS_SUCCESS || strlen(msg)==0 )
		goto error;

	strip_invalid_msg(msg);

	if( j->is_player )
		netall_printf("%s=%s,%d,\"%s\"\n", TOKEN_MESSAGE, j->name,j->numjug,msg);
	else
		netall_printf("%s=observer-%s,%d,\"%s\"\n", TOKEN_MESSAGE, j->name,j->numjug,msg);
	return TEG_STATUS_SUCCESS;
error:
	net_print(fd,TOKEN_ERROR"="TOKEN_MESSAGE"\n");
	return TEG_STATUS_PARSEERROR;
}

/* mostly-Useless function */
STATIC TEG_STATUS token_rem( int fd, char *unused )
{
	PLAY_DEBUG("token_rem()\n");

	if( !SPLAYER_HABILITADO(fd) )
		goto error;

	net_printf(fd,"%s=%s\n",TOKEN_REM,_("Para que me envias un rem?"));
	return TEG_STATUS_SUCCESS;
error:
	return TEG_STATUS_PARSEERROR;
}

/* I want a better world "Quiero Un Mundo Mejor" */
STATIC TEG_STATUS token_qumm( int fd, char *str )
{
	PLAY_DEBUG("token_qumm()\n");

	if( !SPLAYER_HABILITADO(fd) )
		goto error;

	net_printf(fd,"%s=%s\n",TOKEN_REM,_("Yo tambien quiero un mundo mejor!"));
	return TEG_STATUS_SUCCESS;
error:
	return TEG_STATUS_PARSEERROR;
}

/* Creates a Player */
STATIC TEG_STATUS token_playerid( int fd, char *str )
{
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };
	SPLAYER j, *pJ;
	char c[TEG_MAX_PLAYERS];
	char colores[100];
	int i;
	int reconnect = FALSE;

	PLAY_DEBUG("token_playerid( fd=%d)\n",fd);

	/* si existe entonces da error, porque no tiene que existir */
	if( player_whoisfd(fd, &pJ ) == TEG_STATUS_SUCCESS )
		goto error;

	if( strlen(str)==0 )
		goto error;

	p.equals = &igualador;
	p.separators = &separador;
	p.data = str;

	memset( &j, 0, sizeof(SPLAYER));

	/* averigua el name */
	if( parser_parse( &p ) && p.hay_otro ) {
			player_fillname( &j, p.token );
	} else goto error;

	if( parser_parse( &p ) && p.hay_otro ) {
		j.is_player = atoi( p.token );		
	} else
		goto error;

	if( parser_parse( &p ) && !p.hay_otro ) {
		j.human = atoi( p.token );		
	} else
		goto error;


	if( j.is_player ) {
		if( JUEGO_EMPEZADO ) {
			if( ! (reconnect = player_is_disconnected(&j)) ) {
				net_print(fd,TOKEN_GAMEINPROGRESS"\n");
				fd_remove(fd);
				return TEG_STATUS_CONNCLOSED;
			}
		}
		if( reconnect )
			pJ = player_return_disconnected( &j );
		else
			pJ = player_ins_player( &j );
	} else
		pJ = player_ins_ro( &j );


	if( pJ == NULL ) {
		net_print(fd,TOKEN_SERVERFULL "\n");
		fd_remove(fd);
		return TEG_STATUS_CONNCLOSED;
	}

	pJ->fd = fd;

	aux_find_inaddr( pJ );

	if( reconnect ) {

		pJ->estado = pJ->status_before_discon;

		net_printf(fd,"%s=%s,%d,%d\n", TOKEN_RECONNECT, pJ->name,pJ->numjug,pJ->color);
		con_text_out(M_INF,_("Player %s(%d) is re-connected from %s\n"),pJ->name,pJ->numjug,pJ->addr);

	} else {
		colores_libres( c );
		memset(colores,0,sizeof(colores));

		for(i=0;i<TEG_MAX_PLAYERS;i++) {
			char buf[100];
			sprintf( buf, ",%d",c[i] );
			strncat( colores, buf, sizeof(colores)-1 );
		}

		net_printf(fd,"%s=%s,%d%s\n", TOKEN_PLAYERID, pJ->name,pJ->numjug,colores);

		con_text_out(M_INF,_("Player %s(%d) is connected from %s\n"),pJ->name,pJ->numjug,pJ->addr);
	}
	return TEG_STATUS_SUCCESS;
error:
	net_print(fd,TOKEN_ERROR"="TOKEN_PLAYERID"\n");
	return TEG_STATUS_PARSEERROR;
}

STATIC TEG_STATUS token_cversion( int fd, char *str )
{
	con_text_out(M_INF,_("Using client version: %s\n"),str);
	return TEG_STATUS_SUCCESS;
}

/* Parses the armies the player must place for the 1st time */
STATIC TEG_STATUS token_fichas( int fd, char *str )
{
	PLAY_DEBUG("token_fichas()\n");

	if( !SPLAYER_FICHAS(fd) )
		goto error;

	if( aux_token_fichas( fd, str, g_game.fichas, 0) == TEG_STATUS_SUCCESS ) {

		g_game.turno->estado = PLAYER_STATUS_POSTFICHAS;

		fichas_next();
		return TEG_STATUS_SUCCESS;
	}

error:
	net_print(fd,TOKEN_ERROR"="TOKEN_FICHAS"\n");
	return TEG_STATUS_PARSEERROR;
}

/* Parses the armies the player must place for the 2nd time */
STATIC TEG_STATUS token_fichas2( int fd, char *str )
{
	PLAY_DEBUG("token_fichas2()\n");

	if( !SPLAYER_FICHAS2(fd) )
		goto error;
	
	if( aux_token_fichas( fd, str, g_game.fichas2, 0) == TEG_STATUS_SUCCESS ) {

		g_game.turno->estado = PLAYER_STATUS_POSTFICHAS2;

		fichas2_next();
		return TEG_STATUS_SUCCESS;
	}
error:
	net_print(fd,TOKEN_ERROR"="TOKEN_FICHAS2"\n");
	return TEG_STATUS_PARSEERROR;
}

/* Parses the armies the player must place the rest of the times */
STATIC TEG_STATUS token_fichasc( int fd, char *str )
{
	PSPLAYER pJ;
	int total_armies;
	PLAY_DEBUG("token_fichasc()\n");

	if( player_whoisfd( fd, &pJ ) != TEG_STATUS_SUCCESS )
		goto error;

	if( pJ->estado != PLAYER_STATUS_FICHASC && pJ->estado != PLAYER_STATUS_CANJE )
		goto error;

	total_armies = pJ->fichasc_armies + cont_tot(pJ->fichasc_conts);

	if( pJ->hizo_canje )
		total_armies += cuantos_x_canje( pJ->tot_exchanges );

	if( aux_token_fichas( fd, str, total_armies, pJ->fichasc_conts) == TEG_STATUS_SUCCESS ) {

		pJ->estado = PLAYER_STATUS_IDLE;

		fichasc_next();

		pJ->hizo_canje = FALSE;
		pJ->fichasc_armies = 0;
		pJ->fichasc_conts = 0;

		return TEG_STATUS_SUCCESS;
	}

error:
	net_print(fd,TOKEN_ERROR"="TOKEN_FICHASC"\n");
	return TEG_STATUS_PARSEERROR;
}

/* a player request to play with a robot. the robot will be launched in the server-side */
STATIC TEG_STATUS token_robot( int fd, char *str )
{
	PSPLAYER pJ;
	int newfd;

	PLAY_DEBUG("token_robot()\n");

	if( player_whoisfd(fd, &pJ ) != TEG_STATUS_SUCCESS )
		goto error;

	if( ! pJ->is_player )
		goto error;

	if( JUEGO_EMPEZADO )
		goto error;

	if( launch_robot(&newfd, "--connected") != TEG_STATUS_SUCCESS )
		goto error;

	fd_add( newfd );

	return TEG_STATUS_SUCCESS;

error:
	net_print(fd,TOKEN_ERROR"="TOKEN_ROBOT"\n");
	return TEG_STATUS_PARSEERROR;
}


/* A player is attacking from src to dst */
STATIC TEG_STATUS token_attack( int fd, char *str )
{
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };
	int src,dst,src_lost,dst_lost;
	char d_src[3],d_dst[3];
	PSPLAYER pJ_src, pJ_dst;
	int conq = 0;
	int tropas = 0;
	char buffer[4096];

	PLAY_DEBUG("token_attack()\n");

	if( strlen(str)==0)
		goto error;

	if( !SPLAYER_ATAQUE_P(fd,&pJ_src)) {
		if( SPLAYER_TROPAS_P(fd,&pJ_src)) {
			pJ_src->estado=PLAYER_STATUS_ATAQUE;
			pJ_src->country_src = pJ_src->country_dst = -1;
		} else goto error;
	}

	p.equals = &igualador;
	p.separators = &separador;
	p.data = str;

	if( parser_parse( &p ) && p.hay_otro ) {
		src = atoi( p.token );		
	} else goto error;

	if( parser_parse( &p ) && !p.hay_otro ) {
		dst = atoi( p.token );		
	} else goto error;

	if( src >= COUNTRIES_CANT || src < 0 || dst >= COUNTRIES_CANT || dst < 0) {
		goto error;
	}

	if( pJ_src->numjug != g_countries[src].numjug || pJ_src->numjug == g_countries[dst].numjug )  {
		goto error;
	}

	if( g_countries[src].ejercitos < 2 || !countries_eslimitrofe( src, dst) ) {
		goto error;
	}

	if( player_whois( g_countries[dst].numjug, &pJ_dst ) != TEG_STATUS_SUCCESS ){
		goto error;
	}

	/* aviso a todos que hay un attack */
	if( ! g_game.fog_of_war )
		netall_printf( "%s=%d,%d\n",TOKEN_ATAQUE,src,dst );
	else {
		fow_2_netall_printf( src, dst, "%s=%s,%s\n",TOKEN_ATAQUE,"%d","%d" );
	}

	/* so far, attack... */
	aux_token_attack( g_countries[src].ejercitos, g_countries[dst].ejercitos, &src_lost, &dst_lost, d_src, d_dst );

	g_countries[src].ejercitos -= src_lost;
	g_countries[dst].ejercitos -= dst_lost;
	pJ_src->tot_armies -= src_lost;
	pJ_dst->tot_armies -= dst_lost;

	/* updated statistics */
	pJ_src->player_stats.armies_killed += dst_lost;
	pJ_dst->player_stats.armies_killed += src_lost;

	/* conquisto el country */
	if( g_countries[dst].ejercitos == 0) {
		PLIST_ENTRY l;

		conq = 1;

		pJ_src->turno_conq++;
		pJ_src->tot_countries++;

		
		g_countries[dst].numjug = pJ_src->numjug;

		g_countries[dst].ejercitos++;		/* se pasa automaticamente */
		g_countries[src].ejercitos--;		/* un ejercito */

		tropas = g_countries[src].ejercitos - 1;	/* cantidad que se pueden pasar */
		if( tropas > 2 )			/* En verdad son 3, pero ya se le paso 1 */
			tropas =2;

		pJ_src->estado = PLAYER_STATUS_TROPAS;
		pJ_src->country_src = src;
		pJ_src->country_dst = dst;
	
		pJ_dst->tot_countries--;

		l= RemoveHeadList( g_countries[dst].next.Blink );
		InsertTailList( &pJ_src->countries, l);

		/* updated statistics */
		pJ_src->player_stats.countries_won ++;
	}

	/* update the scores */
	stats_score( &pJ_src->player_stats, g_conts );
	stats_score( &pJ_dst->player_stats, g_conts );

	/* tell everybody the result of the attack */

	memset( buffer, 0, sizeof(buffer) );

	if( ! g_game.fog_of_war ) {
		netall_printf( "%s=%d,%d,%d,%d,%d,%d,%d,%d\n", TOKEN_DADOS,
			src,d_src[0],d_src[1],d_src[2], dst,d_dst[0],d_dst[1],d_dst[2] );
	} else {
		fow_2_netall_printf( src, dst, "%s=%s,%d,%d,%d,%s,%d,%d,%d\n"
			, TOKEN_DADOS
			, "%d",d_src[0],d_src[1],d_src[2]
			, "%d",d_dst[0],d_dst[1],d_dst[2] );
	}

	if( ! g_game.fog_of_war ) {
		netall_printf( "%s=%d,%d,%d;%s=%d,%d,%d\n",
			TOKEN_COUNTRY, src, g_countries[src].numjug, g_countries[src].ejercitos,
			TOKEN_COUNTRY, dst, g_countries[dst].numjug, g_countries[dst].ejercitos
			);
	} else {
		fow_netall_printf( src, "%s=%d,%d,%d\n", TOKEN_COUNTRY,
				src, g_countries[src].numjug, g_countries[src].ejercitos );

		fow_netall_printf( dst, "%s=%d,%d,%d\n", TOKEN_COUNTRY,
				dst, g_countries[dst].numjug, g_countries[dst].ejercitos );
	}

	if( conq == 1 ) {

		/* Did 'dst' player lose the game ? */
		if( player_is_lost( pJ_dst ) ) {
			con_text_out(M_INF,_("Player %s(%d) lost the game\n"),pJ_dst->name,pJ_dst->numjug);
			netall_printf( "%s=%d\n",TOKEN_LOST, pJ_dst->numjug );
			player_poner_perdio(pJ_dst);
		}

		/* Did 'src' player win the game ? */
		if( mission_chequear( pJ_src ) == TEG_STATUS_GAMEOVER || game_is_finished() ) {
			con_text_out(M_INF,_("Player %s(%d) is the winner! Game Over\n"),pJ_src->name,pJ_src->numjug);
			pJ_src->estado = PLAYER_STATUS_GAMEOVER;
			game_end( pJ_src );
			return TEG_STATUS_SUCCESS;
		}

		net_printf(fd,"%s=%d,%d,%d\n", TOKEN_TROPAS, src,dst,tropas);


		/* in FOW show the boundaries countries */
		if( g_game.fog_of_war ) {
			char buffer[2048];

			memset( buffer, 0, sizeof(buffer) );
			if( fow_fill_with_boundaries( dst, buffer, sizeof(buffer) ) == TEG_STATUS_SUCCESS )
				net_printf( fd, "%s\n", buffer );
		}
	}

	return TEG_STATUS_SUCCESS;
error:
	net_print(fd,TOKEN_ERROR"="TOKEN_ATAQUE"\n");
	return TEG_STATUS_PARSEERROR;
}

/* Place armies in the conquered country */
STATIC TEG_STATUS token_tropas( int fd, char *str )
{
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };
	int src,dst,cant;
	PSPLAYER pJ;

	PLAY_DEBUG("token_tropas()\n");

	if( !SPLAYER_TROPAS_P(fd,&pJ) || strlen(str)==0 )
		goto error;

	p.equals = &igualador;
	p.separators = &separador;
	p.data = str;

	if( parser_parse( &p ) && p.hay_otro ) {
		src = atoi( p.token );		
	} else goto error;

	if( parser_parse( &p ) && p.hay_otro ) {
		dst = atoi( p.token );		
	} else goto error;

	if( parser_parse( &p ) && !p.hay_otro ) {
		cant = atoi( p.token );		
	} else goto error;

	if( src >= COUNTRIES_CANT || src < 0 || dst >= COUNTRIES_CANT || dst < 0) {
		goto error;
	}

	if( src != pJ->country_src || dst != pJ->country_dst )
		goto error;

	if( cant > g_countries[src].ejercitos-1 || cant > 3)
		goto error;

	if( cant > 0 ) {

		g_countries[src].ejercitos -= cant;
		g_countries[dst].ejercitos += cant;

		pJ->estado=PLAYER_STATUS_ATAQUE;
		pJ->country_src = pJ->country_dst = -1;

		if( ! g_game.fog_of_war )
			netall_printf( "%s=%d,%d,%d;%s=%d,%d,%d\n",
					TOKEN_COUNTRY, src,g_countries[src].numjug,g_countries[src].ejercitos,
					TOKEN_COUNTRY, dst,g_countries[dst].numjug,g_countries[dst].ejercitos
					);
		else {
			fow_netall_printf( src, "%s=%d,%d,%d\n",
					TOKEN_COUNTRY, src,g_countries[src].numjug,g_countries[src].ejercitos );
			fow_netall_printf( dst, "%s=%d,%d,%d\n",
					TOKEN_COUNTRY, dst,g_countries[dst].numjug,g_countries[dst].ejercitos );
		}
	}

	return TEG_STATUS_SUCCESS;
error:
	net_print(fd,TOKEN_ERROR"="TOKEN_TROPAS"\n");
	return TEG_STATUS_PARSEERROR;
}

/* Ask for a card after the attack */
STATIC TEG_STATUS token_card( int fd, char *str )
{
	PSPLAYER pJ;
	PCOUNTRY pP;

	PLAY_DEBUG("token_card()\n");


	/* Veo si puede sacar una tarjeta... */
	if( player_whoisfd( fd, &pJ ) != TEG_STATUS_SUCCESS )
		goto error;

	if( pJ->estado < PLAYER_STATUS_TURNOSTART || pJ->estado >= PLAYER_STATUS_TARJETA)
		goto error;

	if( pJ->tot_cards >= TEG_MAX_TARJETAS )
		goto error;

	if( pJ->turno_conq < 1 )
		goto error;

	if( pJ->tot_exchanges > 3 && pJ->turno_conq < 2 )
		goto error;


	/* Puede sacar tarjeta */
	pJ->tot_cards++;
	pP = get_random_country( tarjeta_es_libre );

	pJ->estado = PLAYER_STATUS_TARJETA;

	tarjeta_sacar( &pP->tarjeta, pJ->numjug );

	/*
	 * Me fijo si el player es dueño del país que dice la tarjeta. Si es así
	 * le agrego 2 fichas automaticamente como dice el reglamento.
	 */
	if( pP->numjug == pJ->numjug ) {
		pP->ejercitos += 2;
		pJ->tot_armies += 2;
		tarjeta_usar( &pP->tarjeta );
		if( ! g_game.fog_of_war )
			netall_printf( "%s=%d,%d,%d\n", TOKEN_COUNTRY,pP->id,pP->numjug,pP->ejercitos);
		else
			fow_netall_printf( pP->id, "%s=%d,%d,%d\n", TOKEN_COUNTRY,pP->id,pP->numjug,pP->ejercitos);
	} 
	net_printf(fd,"%s=%d,%d\n", TOKEN_TARJETA, pP->id,pP->tarjeta.usada);

	return TEG_STATUS_SUCCESS;
error:
	net_print(fd,TOKEN_ERROR"="TOKEN_TARJETA"\n");
	return TEG_STATUS_ERROR;
}

/* request which cards do the players has */
STATIC TEG_STATUS token_enum_cards( int fd, char *str )
{
	PSPLAYER pJ;
	int i,first;
	char buffer[1024];

	PLAY_DEBUG("token_enum_cards()\n");


	if( player_whoisfd( fd, &pJ ) != TEG_STATUS_SUCCESS )
		goto error;

	memset( buffer, 0, sizeof(buffer) );

	first=1;
	for(i=0;i<COUNTRIES_CANT;i++) {
		if( g_countries[i].tarjeta.numjug == pJ->numjug ) {
			char buffy[512];

			memset(buffy,0,sizeof(buffy));

			if( first ) {
				snprintf( buffy, sizeof(buffy)-1, "%d:%d", i, g_countries[i].tarjeta.usada );
				strncat( buffer, buffy, sizeof(buffer)-1 );
				first = 0;
			} else {
				snprintf( buffy, sizeof(buffy)-1, ",%d:%d", i, g_countries[i].tarjeta.usada );
				strncat( buffer, buffy, sizeof(buffer)-1 );
			}
		}
	}

	net_printf(fd,"%s=%s\n", TOKEN_ENUM_CARDS, buffer);

	return TEG_STATUS_SUCCESS;
error:
	net_print(fd,TOKEN_ERROR"="TOKEN_ENUM_CARDS"\n");
	return TEG_STATUS_ERROR;
}

/* request who started the round, and which round number */
STATIC TEG_STATUS token_new_round( int fd, char *str )
{
	PSPLAYER pJ;

	PLAY_DEBUG("token_new_round()\n");

	if( !JUEGO_EMPEZADO )
		goto error;

	if( player_whoisfd( fd, &pJ ) != TEG_STATUS_SUCCESS )
		goto error;

	net_printf(fd,"%s=%d,%d\n", TOKEN_NEW_ROUND
			,g_game.turno->numjug 	/* who starts the new turn */
			,g_game.round_number	/* the round number */
		  );

	return TEG_STATUS_SUCCESS;
error:
	net_print(fd,TOKEN_ERROR"="TOKEN_NEW_ROUND"\n");
	return TEG_STATUS_ERROR;
}

/* Place 2 armies in the card's country. The player must own the country */
STATIC TEG_STATUS token_ejer2( int fd, char *str )
{
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };
	int country;
	PSPLAYER j;

	PLAY_DEBUG("token_ejer2()\n");

	if( strlen(str)==0)
		goto error;

	if( player_whoisfd( fd, &j ) != TEG_STATUS_SUCCESS )
		goto error;

	if( j->estado != PLAYER_STATUS_TARJETA )
		goto error;

	p.equals = &igualador;
	p.separators = &separador;
	p.data = str;

	if( parser_parse( &p ) && !p.hay_otro ) {
		country = atoi( p.token );		
	} else goto error;

	if( country >= COUNTRIES_CANT || country < 0 ) {
		goto error;
	}

	if( tarjeta_es_usada( &g_countries[ country ].tarjeta ))
		goto error;

	if( g_countries[ country ].numjug == j->numjug && g_countries[ country ].tarjeta.numjug == j->numjug ) {
		g_countries[ country ].ejercitos += 2;
		j->tot_armies += 2;
		tarjeta_usar( &g_countries[ country ].tarjeta );

		if( ! g_game.fog_of_war )
			netall_printf("%s=%d,%d,%d\n", TOKEN_COUNTRY
				,country ,g_countries[ country ].numjug ,g_countries[ country ].ejercitos);
		else
			fow_netall_printf( country, "%s=%d,%d,%d\n", TOKEN_COUNTRY
				,country ,g_countries[ country ].numjug ,g_countries[ country ].ejercitos);
	} 

	return TEG_STATUS_SUCCESS;
error:
	net_print(fd,TOKEN_ERROR"="TOKEN_EJER2"\n");
	return TEG_STATUS_PARSEERROR;
}


/* To exchange cards for armies */
STATIC TEG_STATUS token_canje( int fd, char *str )
{
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };
	PSPLAYER pJ;
	int t1,t2,t3;
	int canj_ejer;

	PLAY_DEBUG("token_canje()\n");

	if( player_whoisfd( fd, &pJ ) != TEG_STATUS_SUCCESS )
		goto error;

	if( pJ->estado != PLAYER_STATUS_FICHASC )
		goto error;

	p.equals = &igualador;
	p.separators = &separador;
	p.data = str;

	if( parser_parse( &p ) && p.hay_otro ) {
		t1 = atoi( p.token );		
	} else goto error;

	if( parser_parse( &p ) && p.hay_otro ) {
		t2 = atoi( p.token );		
	} else goto error;

	if( parser_parse( &p ) && !p.hay_otro ) {
		t3 = atoi( p.token );		
	} else goto error;

	/* se puede hacer el canje ? */
	if( !tarjeta_puedocanje( pJ->numjug, t1, t2, t3 ) )
		goto error;

	pJ->estado = PLAYER_STATUS_CANJE;

	pJ->hizo_canje = TRUE;
	pJ->tot_exchanges++;
	pJ->tot_cards -= 3;

	canj_ejer = cuantos_x_canje( pJ->tot_exchanges );

	/* quitarle las tarjetas al player | mark the cards as unused*/
	tarjeta_poner( &g_countries[t1].tarjeta );
	tarjeta_poner( &g_countries[t2].tarjeta );
	tarjeta_poner( &g_countries[t3].tarjeta );


	netall_printf("%s=%d,%d,%d,%d,%d\n", TOKEN_CANJE,
			pJ->numjug,canj_ejer,t1,t2,t3);
	return TEG_STATUS_SUCCESS;
error:
	net_print(fd,TOKEN_ERROR"="TOKEN_CANJE"\n");
	return TEG_STATUS_PARSEERROR;
}


/* Player is regrouping its armies */
STATIC TEG_STATUS token_regroup( int fd, char *str )
{
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };
	int src,dst,cant;
	int ejer_disp;
	PSPLAYER pJ;

	PLAY_DEBUG("token_regroup()\n");


	if( strlen(str)==0)
		goto error;

	if( player_whoisfd( fd, &pJ ) != TEG_STATUS_SUCCESS )
		goto error;

	if( pJ->estado < PLAYER_STATUS_TURNOSTART || pJ->estado > PLAYER_STATUS_REAGRUPE)
		goto error;

	p.equals = &igualador;
	p.separators = &separador;
	p.data = str;


	if( parser_parse( &p ) && p.hay_otro ) {
		src = atoi( p.token );
	} else goto error;

	if( parser_parse( &p ) && p.hay_otro ) {
		dst = atoi( p.token );
	} else goto error;

	if( parser_parse( &p ) && !p.hay_otro ) {
		cant = atoi( p.token );
	} else goto error;

	if( src >= COUNTRIES_CANT || src < 0 || dst >= COUNTRIES_CANT || dst < 0 || cant <= 0) {
		goto error;
	}

	if( g_countries[src].numjug != pJ->numjug || g_countries[dst].numjug != pJ->numjug ) {
		goto error;
	}

	if( !countries_eslimitrofe( src, dst ))
		goto error;

	ejer_disp = g_countries[src].ejercitos - g_countries[src].ejer_reagrupe - 1;
	if( cant > ejer_disp )
		goto error;

	pJ->estado = PLAYER_STATUS_REAGRUPE;

	g_countries[dst].ejercitos += cant;
	g_countries[dst].ejer_reagrupe += cant;
	g_countries[src].ejercitos -= cant;

	if( ! g_game.fog_of_war )
	{
		netall_printf("%s=%d,%d,%d;%s=%d,%d,%d\n",
			TOKEN_COUNTRY, src,g_countries[src].numjug,g_countries[src].ejercitos,
			TOKEN_COUNTRY, dst,g_countries[dst].numjug,g_countries[dst].ejercitos );
	} else {
		fow_netall_printf( src, "%s=%d,%d,%d\n",
			TOKEN_COUNTRY, src,g_countries[src].numjug,g_countries[src].ejercitos );
		fow_netall_printf( dst, "%s=%d,%d,%d\n",
			TOKEN_COUNTRY, dst,g_countries[dst].numjug,g_countries[dst].ejercitos );
	}

	return TEG_STATUS_SUCCESS;
error:
	net_print(fd,TOKEN_ERROR"="TOKEN_REAGRUPE"\n");
	return TEG_STATUS_PARSEERROR;
}

/* Show the status of all the players */
STATIC TEG_STATUS token_status( int fd, char *unused )
{
	char strout[PROT_MAX_LEN + PLAYERNAME_MAX_LEN * TEG_MAX_PLAYERS + 200];

	PLAY_DEBUG("token_status()\n");

	if( !SPLAYER_CONNECTED( fd ))
		goto error;

	if( aux_token_stasta(strout, sizeof(strout) -1 ) != TEG_STATUS_SUCCESS )
		goto error;

	net_printf(fd,"%s=%s\n", TOKEN_STATUS, strout);

	return TEG_STATUS_SUCCESS;

error:
	net_print(fd,TOKEN_ERROR"="TOKEN_STATUS"\n");
	return TEG_STATUS_PARSEERROR;
}

/* send the hi-scores to the player */
STATIC TEG_STATUS token_scores( int fd, char *unused )
{
	char strout[PROT_MAX_LEN + PLAYERNAME_MAX_LEN * TEG_MAX_PLAYERS + 200];

	PLAY_DEBUG("token_scores()\n");

	if( !SPLAYER_CONNECTED( fd ))
		goto error;

	if( scores_dump(strout,sizeof(strout) -1 ) != TEG_STATUS_SUCCESS )
		goto error;

	net_printf(fd,"%s=%s\n", TOKEN_SCORES, strout);

	return TEG_STATUS_SUCCESS;

error:
	net_print(fd,TOKEN_ERROR"="TOKEN_SCORES"\n");
	return TEG_STATUS_PARSEERROR;
}


/* Enums the players'countries , or of all players if player is -1 */
STATIC TEG_STATUS token_countries( int fd, char *str )
{
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };
	int i;
	PSPLAYER pJ;
	char strout[PROT_MAX_LEN];

	PLAY_DEBUG("token_countries()\n");

	if( !JUEGO_EMPEZADO )
		goto error;

	if( strlen(str)==0 )
		goto error;

	p.equals = &igualador;
	p.separators = &separador;
	p.data = str;

	if( parser_parse( &p ) && !p.hay_otro ) {
		i = atoi( p.token );		
	} else
		goto error;

	if( i != -1 && player_whois( i, &pJ ) != TEG_STATUS_SUCCESS )
		goto error;

	if(i!=-1) {
		/* ask just for 1 player */
		if( aux_token_countries(pJ,strout,sizeof(strout)-1) !=TEG_STATUS_SUCCESS )
			goto error;
		net_printf(fd,"%s=%d/%s\n", TOKEN_COUNTRIES, pJ->numjug,strout);
	} else  {
		/* ask for all the players */
		PLIST_ENTRY pL = g_list_player.Flink;

		while( !IsListEmpty( &g_list_player ) && (pL != &g_list_player) ) {
			pJ = (PSPLAYER) pL;
			if( pJ->is_player ) {
				if( aux_token_countries(pJ,strout,sizeof(strout)-1) !=TEG_STATUS_SUCCESS )
					goto error;
				net_printf(fd,"%s=%d/%s\n", TOKEN_COUNTRIES, pJ->numjug,strout);
			}
			pL = LIST_NEXT(pL);
		}

		if( g_game.fog_of_war ) {
			SPLAYER j;

			j.numjug = -1;

			if( aux_token_countries(&j,strout,sizeof(strout)-1) == TEG_STATUS_SUCCESS )
				net_printf(fd,"%s=%d/%s\n",TOKEN_COUNTRIES,j.numjug,strout);
		}
	}
	return TEG_STATUS_SUCCESS;

error:
	net_print(fd,TOKEN_ERROR"="TOKEN_COUNTRIES"\n");
	return TEG_STATUS_PARSEERROR;
}

/* To start the game */
TEG_STATUS token_start( int fd )
{
	char strout[PROT_MAX_LEN + PLAYERNAME_MAX_LEN  * TEG_MAX_PLAYERS + 200];
	PSPLAYER pJ;
	PLAY_DEBUG("token_start()\n");

	if( JUEGO_EMPEZADO || g_game.players < 2 )
		goto error;

	if( g_server.with_console && fd != CONSOLE_FD) {
		if( !SPLAYER_HABILITADO_P(fd,&pJ) || !pJ->is_player )
			goto error;
	}

	JUEGO_EN_EMPEZAR;

	g_game.playing = g_game.players;

	con_text_out(M_INF,_("Starting game number: %d with seed: %u\n"),g_game.gamenumber,g_game.seed);

	player_all_set_status ( PLAYER_STATUS_START );
	countries_repartir();

	if(turno_init() != TEG_STATUS_SUCCESS ) {
		con_text_out(M_ERR,_("Error, cant initilize a new turn\n"));
		goto error;
	}

	JUEGO_EN_FICHAS;

	g_game.turno->estado = PLAYER_STATUS_FICHAS;

	aux_token_stasta(strout, sizeof(strout) -1 );

	netall_printf( "%s=%s;%s=%d,%d;%s=%d,%d,%d,%d;%s=%d,%d\n"
			,TOKEN_START
			,strout			/* available players */
			,TOKEN_NEW_ROUND
			,g_game.turno->numjug 	/* who starts the new turn */
			,g_game.round_number	/* the round number */
			,TOKEN_MODALIDAD
			,g_game.mission	/* play with missions ? */
			,g_game.cmission	/* play with common mission */
			,g_game.fog_of_war	/* play with fog of war */
			,g_game.reglas		/* which rules ? */
			,TOKEN_FICHAS
			,g_game.turno->numjug,	/* who starts ? */
			g_game.fichas );	/* how many armies to place */
	return TEG_STATUS_SUCCESS;
error:
	net_print(fd,TOKEN_ERROR"="TOKEN_START"\n");
	return TEG_STATUS_PARSEERROR;
}

/* sends a player the type of game it is being played */
TEG_STATUS token_typeofgame( int fd, char *str )
{
	PSPLAYER pJ;
	PLAY_DEBUG("token_typeofgame()\n");

	if( JUEGO_EMPEZADO )
		goto error;

	if( !SPLAYER_HABILITADO_P(fd,&pJ) || !pJ->is_player )
		goto error;

	netall_printf( "%s=%d,%d,%d,%d;\n"
			,TOKEN_MODALIDAD
			,g_game.mission	/* play with missions ? */
			,g_game.cmission	/* play with common mission */
			,g_game.fog_of_war	/* play with fog of war */
			,g_game.reglas		/* which rules ? */
		     );
	return TEG_STATUS_SUCCESS;
error:
	net_print(fd,TOKEN_ERROR"="TOKEN_MODALIDAD"\n");
	return TEG_STATUS_PARSEERROR;
}

/* For testing only */
STATIC TEG_STATUS token_test( int fd, char *unused )
{
	PLAY_DEBUG("token_test()\n");

	return TEG_STATUS_SUCCESS;
}

/* Server version */
STATIC TEG_STATUS token_sversion( int fd, char *unused )
{
	PLAY_DEBUG("token_sversion()\n");

	net_printf(fd,"%s=%s %s\n", TOKEN_SVERSION, _("TEG server version "),VERSION);
	return TEG_STATUS_SUCCESS;
}

/* Protocol version */
STATIC TEG_STATUS token_pversion( int fd, char *str )
{
	PSPLAYER pJ;
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };
	int hi,lo;

	PLAY_DEBUG("token_pversion()\n");


	if( strlen(str)==0 )
		goto error;

	p.equals = &igualador;
	p.separators = &separador;
	p.data = str;

	if( parser_parse( &p ) && p.hay_otro ) {
		hi = atoi( p.token );		
	} else goto error;

	if( parser_parse( &p ) && !p.hay_otro ) {
		lo = atoi( p.token );		
	} else goto error;

	net_printf(fd,"%s=%i,%i\n", TOKEN_PVERSION, PROTOCOL_HIVER,PROTOCOL_LOVER);

	if( hi != PROTOCOL_HIVER ) {
		con_text_out(M_ERR,_("Client with incompatible protocol version (server:%d , client:%d)\n"),PROTOCOL_HIVER,hi);

		if( player_whoisfd( fd, &pJ ) == TEG_STATUS_SUCCESS )
			player_del_hard( pJ );
		else
			fd_remove(fd);
		return TEG_STATUS_CONNCLOSED;
	}

	return TEG_STATUS_SUCCESS;

error:
	net_print(fd,TOKEN_ERROR"="TOKEN_PVERSION"\n");
	return TEG_STATUS_PARSEERROR;
}

/* Quit TEG */
TEG_STATUS token_exit( int fd )
{
	PSPLAYER pJ;
	PLAY_DEBUG("token_exit\n");

	if( player_whoisfd( fd, &pJ ) == TEG_STATUS_SUCCESS )
		player_del_hard( pJ );
	else
		fd_remove(fd);

	return TEG_STATUS_CONNCLOSED;
}

/* Puts the player in Game Over state */
TEG_STATUS token_surrender( int fd, char *unused )
{
	PSPLAYER pJ;
	PLAY_DEBUG("token_surrender\n");

	if( player_whoisfd( fd, &pJ ) != TEG_STATUS_SUCCESS )
		goto error;

	if( !pJ->is_player )
		goto error;

	if( pJ->estado < PLAYER_STATUS_HABILITADO )
		goto error;

	con_text_out(M_INF,_("Player %s(%d) abandoned the game\n"),pJ->name,pJ->numjug);

	netall_printf("%s=%d\n", TOKEN_SURRENDER, pJ->numjug );

	player_del_soft( pJ );

	return TEG_STATUS_SUCCESS;
error:
	net_print(fd,TOKEN_ERROR"="TOKEN_SURRENDER"\n");
	return TEG_STATUS_PARSEERROR;
}

/* Ask for help */
STATIC TEG_STATUS token_help ( int fd, char *unused )
{
	int i;
	for(i=0;i<NTOKENS;i++) {
		if(tokens[i].func)
			net_printf(fd, "%s='%s' %s\n", TOKEN_REM, tokens[i].label,_(tokens[i].help));
	}
	return TEG_STATUS_SUCCESS;
}

/* Parses the tokens */
STATIC TEG_STATUS token_lookup( int fd, PARSER *p )
{
	int i;

	for(i = 0; i < NTOKENS; i++) {
		if(strcasecmp( p->token, tokens[i].label )==0 ){
			if (tokens[i].func)
				return( (tokens[i].func)(fd,p->value));
			return TEG_STATUS_TOKENNULL;
		}
	}
	PLAY_DEBUG("Token '%s' no encontrado\n",p->token);
	return TEG_STATUS_TOKENNOTFOUND;
}

/* Read the file descriptor and call the apropiate function */
TEG_STATUS play_teg( int fd )
{
	int i,j;
	PARSER p;
	char str[PROT_MAX_LEN];
	DELIM igualador={ '=', '=', '=' };
	DELIM separador={ ';', ';', ';' };

	p.equals = &igualador;
	p.separators = &separador;

	str[0]=0;

	if( g_game.fog_of_war )
		g_game.player_fow = NULL;

	j=net_readline( fd, str, PROT_MAX_LEN );

	if( j<1 ) {
		PSPLAYER pJ;
		if( player_whoisfd( fd, &pJ ) == TEG_STATUS_SUCCESS )
			player_del_hard( pJ );
		else
			fd_remove(fd);

		return TEG_STATUS_CONNCLOSED;
	}

	if( g_game.fog_of_war ) {
		if( player_whoisfd( fd, &g_game.player_fow ) != TEG_STATUS_SUCCESS )
			g_game.player_fow = NULL;
	}

	p.data = str;

	do {
		if( (i=parser_parse( &p )) ) {
			if( token_lookup( fd,&p ) == TEG_STATUS_CONNCLOSED ) {
				return TEG_STATUS_CONNCLOSED;
			}
		}
	} while( i && p.hay_otro);

	return TEG_STATUS_SUCCESS;
}
