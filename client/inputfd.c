/*	$Id: inputfd.c,v 1.98 2002/10/14 03:26:36 riq Exp $	*/
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
 * @file inputfd.c
 * manages the 'in' tokens
 */
#include <string.h>
#include <stdio.h>

#include "client.h"

TEG_STATUS clitok_rem(char *str);
TEG_STATUS clitok_status(char *str);
TEG_STATUS clitok_start(char *str);
TEG_STATUS clitok_playerid(char *str);
TEG_STATUS clitok_newplayer(char *str);
TEG_STATUS clitok_message(char *str);
TEG_STATUS clitok_countries(char *str);
TEG_STATUS clitok_fichas(char *str);
TEG_STATUS clitok_fichas2(char *str);
TEG_STATUS clitok_fichasc(char *str);
TEG_STATUS clitok_turno(char *str);
TEG_STATUS clitok_attack(char *str);
TEG_STATUS clitok_dados(char *str);
TEG_STATUS clitok_country(char *str);
TEG_STATUS clitok_error(char *str);
TEG_STATUS clitok_ok(char *str);
TEG_STATUS clitok_tropas(char *str);
TEG_STATUS clitok_tarjeta(char *str);
TEG_STATUS clitok_exchange(char *str);
TEG_STATUS clitok_modalidad(char *str);
TEG_STATUS clitok_mission(char *str);
TEG_STATUS clitok_winner( char *str );
TEG_STATUS clitok_lost( char *str );
TEG_STATUS clitok_exit( char *str );
TEG_STATUS clitok_pversion( char *str );
TEG_STATUS clitok_sversion( char *str );
TEG_STATUS clitok_ggz( char *str );
TEG_STATUS clitok_serverfull(void);
TEG_STATUS clitok_surrender(char *str);
TEG_STATUS clitok_started(void);
TEG_STATUS clitok_kick(char *str);
TEG_STATUS clitok_scores(char *str);
TEG_STATUS clitok_reconnect(char *str);
TEG_STATUS clitok_enum_cards( char *str );
TEG_STATUS clitok_playersscores( char *str );
TEG_STATUS clitok_new_round( char *str );

struct {
	char *label;
	TEG_STATUS (*func) ();
} tokens[] = {
	{ TOKEN_REM,		clitok_rem 	},
	{ TOKEN_STATUS,		clitok_status 	},
	{ TOKEN_START,		clitok_start 	},
	{ TOKEN_PLAYERID,	clitok_playerid	},
	{ TOKEN_NEWPLAYER,	clitok_newplayer},
	{ TOKEN_MESSAGE,	clitok_message	},
	{ TOKEN_COUNTRIES,		clitok_countries	},
	{ TOKEN_FICHAS,		clitok_fichas	},
	{ TOKEN_FICHAS2,	clitok_fichas2	},
	{ TOKEN_FICHASC,	clitok_fichasc	},
	{ TOKEN_TURNO,		clitok_turno	},
	{ TOKEN_ATAQUE,		clitok_attack	},
	{ TOKEN_DADOS,		clitok_dados	},
	{ TOKEN_COUNTRY,		clitok_country	},
	{ TOKEN_ERROR,		clitok_error	},
	{ TOKEN_OK,		clitok_ok	},
	{ TOKEN_TROPAS,		clitok_tropas	},
	{ TOKEN_TARJETA,	clitok_tarjeta	},
	{ TOKEN_CANJE,		clitok_exchange	},
	{ TOKEN_MODALIDAD,	clitok_modalidad},
	{ TOKEN_MISSION,	clitok_mission	},
	{ TOKEN_WINNER,		clitok_winner	},
	{ TOKEN_LOST,		clitok_lost	},
	{ TOKEN_EXIT,		clitok_exit	},
	{ TOKEN_PVERSION,	clitok_pversion	},
	{ TOKEN_SVERSION,	clitok_sversion	},
	{ TOKEN_SURRENDER,	clitok_surrender},
#ifdef WITH_GGZ
	{ TOKEN_GGZ,		clitok_ggz	},
#endif /* WITH_GGZ */
	{ TOKEN_SERVERFULL,	clitok_serverfull},
	{ TOKEN_GAMEINPROGRESS,	clitok_started	},
	{ TOKEN_KICK,		clitok_kick	},
	{ TOKEN_SCORES,		clitok_scores	},
	{ TOKEN_RECONNECT,	clitok_reconnect},
	{ TOKEN_ENUM_CARDS,	clitok_enum_cards},
	{ TOKEN_NEW_ROUND,	clitok_new_round},

};
#define	NRCLITOKENS  (sizeof(tokens)/sizeof(tokens[0]))

struct {
	char *label;
	TEG_STATUS (*func) ();
} err_tokens[] = {
	{ TOKEN_FICHAS,		fichas_restore_from_error },
	{ TOKEN_FICHAS2,	fichas2_restore_from_error },
	{ TOKEN_FICHASC,	fichasc_restore_from_error },
	{ TOKEN_REAGRUPE,	reagrupe_restore_from_error },
	{ TOKEN_EJER2,		ejer2_restore_from_error },
	{ TOKEN_PLAYERID,	playerid_restore_from_error },
	{ TOKEN_START,		aux_start_error },
};
#define	NRERRCLITOKENS  (sizeof(err_tokens)/sizeof(err_tokens[0]))

/* the server is full */
TEG_STATUS clitok_serverfull( void )
{
	textmsg( M_ERR, _("The server is full. Try connecting as an observer"));
	teg_disconnect();
	return TEG_STATUS_SUCCESS;
}

/* the game is already started. you cant join it */
TEG_STATUS clitok_started( void )
{
	textmsg( M_ERR, _("The game has already started. Try connecting as an observer."));
	teg_disconnect();
	return TEG_STATUS_SUCCESS;
}

/* a player has been kicked */
TEG_STATUS clitok_kick( char *name )
{
	if( name && strlen(name) )
		textmsg( M_IMP, _("Player %s was kicked from the game"),name);
	return TEG_STATUS_SUCCESS;
}


/* Informs that a player has surrender */
TEG_STATUS clitok_surrender( char *str )
{
	int numjug;
	PCPLAYER pJ;

	numjug = atoi( str );
	if( player_whois( numjug, &pJ) != TEG_STATUS_SUCCESS) {
		/* no lo tengo en la base */
		textmsg( M_IMP,_("Player %d abandoned the game"), numjug );
		return TEG_STATUS_SUCCESS;
	}

	textmsg( M_IMP,_("Player %s(%s) abandoned the game"),
				pJ->name,
				_(g_colores[pJ->color])
				);

	if( pJ->numjug == WHOAMI() ) {
		ESTADO_SET(PLAYER_STATUS_GAMEOVER);
		gui_sensi();
	}

	out_countries();

	gui_surrender(numjug);

	return TEG_STATUS_SUCCESS;
}

/* a players lost the connection of just quit the game */
TEG_STATUS clitok_exit( char *str )
{
	int numjug;
	PCPLAYER pJ;

	numjug = atoi( str );
	if( player_whois( numjug, &pJ) != TEG_STATUS_SUCCESS) {
		/* no lo tengo en la base */
		textmsg( M_IMP,_("Player %d exit the game"), numjug );
		return TEG_STATUS_SUCCESS;
	}

	textmsg( M_IMP,_("Player %s(%s) exit the game"),
				pJ->name,
				_(g_colores[pJ->color])
				);

	/* dont delete the player, I need the status */
	/* player_del( pJ ); */

	if( WHOAMI() == numjug ) {
		/* por alguna razon el server quiere que abandone el game */
		teg_disconnect();
	}

	return TEG_STATUS_SUCCESS;
}

/* a player won the game */
TEG_STATUS clitok_winner( char *str )
{
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };
	int numjug;
	int mission;
	PCPLAYER pJ;

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	if( strlen(str)==0 )
		goto error;

	if( parser_call( &p ) && p.hay_otro ) {
		numjug = atoi( p.token );
	} else goto error;

	if( parser_call( &p ) && !p.hay_otro ) {
		mission = atoi( p.token );
	} else goto error;


	if( player_whois( numjug, &pJ) != TEG_STATUS_SUCCESS)
		goto error;

	if( mission == -1 ) mission = 0;

	if( mission < 0 || mission >= missions_cant() )
		goto error;

	textmsg( M_IMP,_("Player %s(%s) is the WINNER"),
				pJ->name,
				_(g_colores[pJ->color])
				);

	gui_winner( pJ->numjug, mission );

	ESTADO_SET( PLAYER_STATUS_HABILITADO );
	gui_sensi();

	game_finalize();
	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_winner()");
	return TEG_STATUS_ERROR;
}

/* a player lost the game */
TEG_STATUS clitok_lost( char *str )
{
	int numjug;
	PCPLAYER pJ;

	numjug = atoi( str );
	if( player_whois( numjug, &pJ) != TEG_STATUS_SUCCESS)
		goto error;

	textmsg( M_IMP,_("Player %s(%s) lost the game\n"),
				pJ->name,
				_(g_colores[pJ->color])
				);

	if( pJ->numjug == WHOAMI() )
		ESTADO_SET(PLAYER_STATUS_GAMEOVER);

	gui_lost( pJ->numjug );

	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_lost()");
	return TEG_STATUS_ERROR;
}

/* you can move some armies to the conquered country */
TEG_STATUS clitok_tropas( char *str)
{
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };
	int src,dst,cant;

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	if( strlen(str)==0 )
		goto error;

	if( parser_call( &p ) && p.hay_otro ) {
		src = atoi( p.token );
	} else goto error;

	if( parser_call( &p ) && p.hay_otro ) {
		dst = atoi( p.token );
	} else goto error;

	if( parser_call( &p ) && !p.hay_otro ) {
		cant = atoi( p.token );
	} else goto error;

	ESTADO_SET(PLAYER_STATUS_TROPAS);

	gui_tropas( src,dst,cant);

	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_tropas()");
	return TEG_STATUS_ERROR;
}

/* ok */
TEG_STATUS clitok_ok( char *str)
{
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	if( strlen(str)==0 )
		goto error;

	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_ok()");
	return TEG_STATUS_ERROR;
}

/* your last request has an error */
TEG_STATUS clitok_error( char *str)
{
	int i;
	if( strlen(str)==0 )
		goto error;


	for(i = 0; i < NRERRCLITOKENS; i++) {
		if(strcasecmp( str, err_tokens[i].label )==0 ){
			if (err_tokens[i].func)
				return( (err_tokens[i].func)());
			textmsg(M_ERR,_("The server report an error in '%s'"),str);
			return TEG_STATUS_TOKENNULL;
		}
	}
	return TEG_STATUS_SUCCESS;

error:
	textmsg(M_ERR,"Error in clitok_error()");
	return TEG_STATUS_ERROR;
}

/* update just one country */
TEG_STATUS clitok_country( char *str)
{
	int country;
	int jug;
	int ejer;

	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	if( strlen(str)==0 )
		goto error;

	if( parser_call( &p ) && p.hay_otro ) {
		country = atoi( p.token );
	} else goto error;

	if( parser_call( &p ) && p.hay_otro ) {
		jug = atoi( p.token );
	} else goto error;

	if( parser_call( &p ) && !p.hay_otro ) {
		ejer = atoi( p.token );
	} else goto error;

	g_countries[country].numjug = jug;
	g_countries[country].ejercitos = ejer;

	gui_country( country );

	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_country()");
	return TEG_STATUS_ERROR;
}

/* dices result */
TEG_STATUS clitok_dados( char *str)
{
	int i;

	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	if( strlen(str)==0 )
		goto error;

	/* src and dst country can be -1 */

	/* attacker */
	if( parser_call( &p ) && p.hay_otro ) {
		g_game.dados_srccountry = atoi( p.token );
		if( g_game.dados_srccountry >= COUNTRIES_CANT || g_game.dados_srccountry < -1 ) {
			g_game.dados_srccountry = -1;
			goto error;
		}
	}

	for(i=0;i<3;i++) {
		if( parser_call( &p ) && p.hay_otro ) {
			g_game.dados_src[i] = atoi( p.token );
		} else goto error;
	}

	/* defender */
	if( parser_call( &p ) && p.hay_otro ) {
		g_game.dados_dstcountry = atoi( p.token );
		if( g_game.dados_dstcountry >= COUNTRIES_CANT || g_game.dados_dstcountry < -1 ) {
			g_game.dados_dstcountry = -1;
			goto error;
		}
	}

	for(i=0;i<2;i++) {
		if( parser_call( &p ) && p.hay_otro ) {
			g_game.dados_dst[i] = atoi( p.token );
		} else goto error;
	}

	if( parser_call( &p ) && !p.hay_otro ) {
		g_game.dados_dst[2] = atoi( p.token );
	} else goto error;

	textmsg(M_INF,_("Dices: %s: %d %d %d vs. %s: %d %d %d")
			,countries_get_name(g_game.dados_srccountry)
			,g_game.dados_src[0]
			,g_game.dados_src[1]
			,g_game.dados_src[2]
			,countries_get_name(g_game.dados_dstcountry)
			,g_game.dados_dst[0]
			,g_game.dados_dst[1]
			,g_game.dados_dst[2] );

	gui_dados();

	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_dados()");
	return TEG_STATUS_ERROR;
}

/* src is attacking dst */
TEG_STATUS clitok_attack( char *str)
{
	int src,dst;
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };
	PCPLAYER pJsrc, pJdst;

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	if( strlen(str)==0 )
		goto error;

	if( parser_call( &p ) && p.hay_otro ) {
		src = atoi( p.token );
	} else goto error;

	if( parser_call( &p ) && !p.hay_otro ) {
		dst = atoi( p.token );
	} else goto error;

	/* countries are allowed to be -1 (unknown in fog of war) */
	if( src<-1 || src>=COUNTRIES_CANT || dst<-1 || dst>=COUNTRIES_CANT )
		goto error;

	pJsrc = NULL;
	if( src >=0 )
		player_whois( g_countries[src].numjug, &pJsrc );

	pJdst = NULL;
	if( dst >=0 )
		player_whois( g_countries[dst].numjug, &pJdst );

	if( src >= 0 && g_countries[src].numjug == WHOAMI() ) {
		attack_reset();
	} else {
		attack_show( src, dst );
	}

	if( pJsrc && pJdst )
		textmsg(M_INF,_("%s(%s) is attacking %s(%s)"), countries_get_name(src), _(g_colores[pJsrc->color])
				,countries_get_name(dst), _(g_colores[pJdst->color]) );
	else
		textmsg(M_INF,_("%s is attacking %s"),countries_get_name(src), countries_get_name(dst) );

	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_attack()");
	return TEG_STATUS_ERROR;
}

/* it's someone turn */
TEG_STATUS clitok_turno( char *str)
{
	int numjug;
	PCPLAYER pJ;
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	if( strlen(str)==0 )
		goto error;

	if( parser_call( &p ) && !p.hay_otro ) {
		numjug = atoi( p.token );
	} else goto error;

	if( player_whois( numjug, &pJ) != TEG_STATUS_SUCCESS)
		goto error;

	g_game.whos_turn = numjug;

	attack_unshow();
	out_countries();

	if( numjug == g_game.numjug ) {
		ESTADO_SET(PLAYER_STATUS_ATAQUE);
		/* cosas a hacer cuando yo comienzo un nuevo turno */
		reagrupe_bigreset();
		attack_init();
		textmsg(M_INF,_("Its your turn to attack!!"));
	} else  {
		/* No es mi turno, entonces yo tengo que estar en idle */
		textmsg(M_IMP,_("Player %s(%s) has the turn to attack!"),
				pJ->name,
				_(g_colores[pJ->color]) );
		ESTADO_SET(PLAYER_STATUS_IDLE);
	}
	gui_turn( pJ );

	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_turno()");
	return TEG_STATUS_ERROR;
}

/* someone is placing the initial armies */
TEG_STATUS clitok_fichas( char *str)
{
	int numjug;
	int cant;
	PCPLAYER j;
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	if( strlen(str)==0 )
		goto error;

	if( parser_call( &p ) && p.hay_otro ) {
		numjug = atoi( p.token );
	} else goto error;

	if( parser_call( &p ) && !p.hay_otro ) {
		cant = atoi( p.token );
	} else goto error;

	if( player_whois( numjug, &j) != TEG_STATUS_SUCCESS)
		goto error;

	out_countries();

	g_game.whos_turn = numjug;

	gui_sensi();

	if( numjug == g_game.numjug ) {
		ESTADO_SET(PLAYER_STATUS_FICHAS);
		fichas_init( cant, 0 );
		gui_fichas(cant,0);
	} else {
		textmsg(M_INF,_("Player %s(%s) is placing %d armies for 1st time"),
				j->name,
				_(g_colores[j->color]),
				cant);
	}

	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_fichas()");
	return TEG_STATUS_ERROR;
}

/* someone is placing the initial armies (2nd time) */
TEG_STATUS clitok_fichas2( char *str)
{
	int numjug;
	int cant;
	PCPLAYER j;
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	if( strlen(str)==0 )
		goto error;

	if( parser_call( &p ) && p.hay_otro ) {
		numjug = atoi( p.token );
	} else goto error;

	if( parser_call( &p ) && !p.hay_otro ) {
		cant = atoi( p.token );
	} else goto error;

	if( player_whois( numjug, &j) != TEG_STATUS_SUCCESS)
		goto error;

	out_countries();

	g_game.whos_turn = numjug;
	gui_sensi();

	if( numjug == g_game.numjug ) {
		ESTADO_SET(PLAYER_STATUS_FICHAS2);
		fichas_init( cant, 0 );
		gui_fichas(cant,0);
	} else {
		textmsg(M_INF,_("Player %s(%s) is placing %d armies for 2nd time"),
				j->name,
				_(g_colores[j->color]),
				cant);
	}

	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_fichas2()");
	return TEG_STATUS_ERROR;
}

/* someone is placing the continents armies */
TEG_STATUS clitok_fichasc( char *str)
{
	int numjug;
	int cant,tot_cant;
	unsigned long conts;
	PCPLAYER j;
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	if( strlen(str)==0 )
		goto error;

	if( parser_call( &p ) && p.hay_otro ) {
		numjug = atoi( p.token );
	} else goto error;

	if( parser_call( &p ) && p.hay_otro ) {
		conts = atoi( p.token );
	} else goto error;

	if( parser_call( &p ) && !p.hay_otro ) {
		cant = atoi( p.token );
	} else goto error;

	if( player_whois( numjug, &j) != TEG_STATUS_SUCCESS)
		goto error;

	attack_unshow();

	g_game.whos_turn = numjug;
	gui_sensi();

	tot_cant = cont_tot( conts ) + cant;

	out_countries();

	if( numjug == g_game.numjug ) {
		ESTADO_SET(PLAYER_STATUS_FICHASC);
		fichas_init( tot_cant, conts );
		gui_fichas(cant,conts);
	} else {
		textmsg(M_INF,_("Player %s(%s) is placing %d armies"),
				j->name,
				_(g_colores[j->color]),
				cant);
	}

	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_fichasc()");
	return TEG_STATUS_ERROR;
}


/* all the countries that a players has */
TEG_STATUS clitok_countries( char *str)
{
	int numjug;
	PCPLAYER j;
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ '/', '/', '/' };


	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	if( strlen(str)==0 )
		goto error;

	if( parser_call( &p ) && p.hay_otro )
		numjug = atoi( p.token );
	else 
		goto error;

	/* XXX: numjug == -1 in fog of war. Need to check if in FOW */
	if( numjug == -1 || player_whois( numjug, &j) == TEG_STATUS_SUCCESS)
		return aux_countries( numjug, p.data );
error:
	textmsg(M_ERR,"Error in clitok_countries()");
	return TEG_STATUS_ERROR;
}

/* who am i, and available colors */
TEG_STATUS clitok_playerid( char *str)
{
	char c[TEG_MAX_PLAYERS];
	PARSER p;
	int i;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	if( strlen(str)==0 )
		goto error;

	if( parser_call( &p ) && p.hay_otro ) {
		strncpy( g_game.myname, p.token, sizeof(g_game.myname)-1);
		g_game.myname[sizeof(g_game.myname)-1]=0;
	} else goto error;

	if( parser_call( &p ) && p.hay_otro ) {
		g_game.numjug = atoi( p.token );
	} else goto error;

	for(i=0;i<TEG_MAX_PLAYERS-1;i++ ) {

		if( parser_call( &p ) && p.hay_otro ) {
			c[i] = atoi( p.token );	
		} else goto error;
	}

	if( parser_call( &p ) && !p.hay_otro ) {
		c[i] = atoi( p.token );	
	} else goto error;


	ESTADO_SET(PLAYER_STATUS_CONNECTED);

	out_status();
	if( g_game.observer )
		out_countries();

	gui_connected( c );

	textmsg( M_IMP,_("I'm player number:%d"),g_game.numjug );
	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_playerid()");
	return TEG_STATUS_ERROR;
}


/* I reconnected to the server */
TEG_STATUS clitok_reconnect( char *str)
{
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	if( strlen(str)==0 )
		goto error;

	if( parser_call( &p ) && p.hay_otro ) {
		strncpy( g_game.myname, p.token, sizeof(g_game.myname)-1);
		g_game.myname[sizeof(g_game.myname)-1]=0;
	} else goto error;

	if( parser_call( &p ) && p.hay_otro ) {
		g_game.numjug = atoi( p.token );
	} else goto error;

	if( parser_call( &p ) && !p.hay_otro ) {
		g_game.mycolor  = atoi( p.token );	
	} else goto error;


	ESTADO_SET(PLAYER_STATUS_IDLE);

	textmsg( M_IMP,_("Successful reconnection. I'm player number:%d"),g_game.numjug );

	{
		/* insert myself in the list of players */
		CPLAYER j;
		PCPLAYER pJ;

		if( player_whois( g_game.numjug, &pJ ) != TEG_STATUS_SUCCESS ) {
			memset(&j,0,sizeof(j));
			j.color = g_game.mycolor;
			j.numjug = g_game.numjug;
			strncpy(j.name,g_game.myname,sizeof(j.name)-1);
			j.name[sizeof(j.name)-1]=0;
			player_ins(&j);
		}
	}

	out_status();
	out_countries();
	out_enum_cards();
	out_get_typeofgame();
	out_new_round();

	gui_reconnected();

	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_reconnect()");
	return TEG_STATUS_ERROR;
}

/* a new player enters the game */
TEG_STATUS clitok_newplayer( char *str)
{
	char name[PLAYERNAME_MAX_LEN];
	int color,numjug;
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };
	CPLAYER j;
	PCPLAYER pJ;

	if( strlen(str)==0 )
		goto error;

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	if( parser_call( &p ) && p.hay_otro ) {
		strncpy( name, p.token, sizeof(name)-1 );
		name[sizeof(name)-1]=0;
	} else goto error;

	if( parser_call( &p ) && p.hay_otro ) {
		numjug = atoi( p.token );		
	} else goto error;

	if( parser_call( &p ) && !p.hay_otro ) {
		color = atoi( p.token );
	} else goto error;

	if( player_whois( numjug, &pJ ) != TEG_STATUS_SUCCESS ) {
		memset(&j,0,sizeof(j));
		j.color = color;
		j.numjug = numjug;
		strncpy(j.name,name,sizeof(j.name)-1);
		j.name[sizeof(j.name)-1]=0;
		player_ins(&j);
	} else
		pJ->color =  color;

	if( numjug == WHOAMI() ) {
		g_game.mycolor = color;
		ESTADO_SET( PLAYER_STATUS_HABILITADO );
		textmsg( M_IMP,_("My color is: %s"),_(g_colores[color]) );
	} else {
		textmsg(M_IMP,_("Player[%d] '%s' is connected with color %s"),numjug,name,_(g_colores[color]));
	}
	gui_habilitado( numjug );

	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_newplayer()");
	return TEG_STATUS_ERROR;
}

/* a new message has arrived */
TEG_STATUS clitok_message( char *str)
{
	char name[PLAYERNAME_MAX_LEN];
	int numjug;
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };

	if( strlen(str) == 0 )
		goto error;

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	if( parser_call( &p ) && p.hay_otro ) {
		strncpy( name, p.token, sizeof(name)-1);
		name[sizeof(name)-1]=0;
	} else goto error;

	if( parser_call( &p ) && p.hay_otro ) {
		numjug = atoi( p.token );		
	} else goto error;

	/* I dont care if there is one more or not */

	if( g_game.msg_show & M_MSG ) {
		gui_textplayermsg(name,numjug,p.data);
	}
	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_message()");
	return TEG_STATUS_ERROR;
}

/* ignore this. used for sending comments in help */
TEG_STATUS clitok_rem( char *str)
{
	if(strlen(str)>0)
		textmsg( M_IMP,str );
	return TEG_STATUS_SUCCESS;
}

/* shows the status of the players */
TEG_STATUS clitok_status( char *str)
{
	CPLAYER j,*j_tmp;
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ '/', '/', '/' };

	int i;

	player_flush();

	if( strlen(str)==0 )
		goto ok;

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;


	do {
		if( (i=parser_call( &p )) ) {
			if( aux_status( &j, p.token ) != TEG_STATUS_SUCCESS )
				goto error;

			if( player_whois( j.numjug, &j_tmp ) == TEG_STATUS_SUCCESS )
				player_update( &j );
			else
				player_ins( &j );
		}
	} while( i && p.hay_otro);
ok:
	gui_status();
	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_status()");
	return TEG_STATUS_PARSEERROR;
}

/* receives the players scores */
TEG_STATUS clitok_scores( char *str)
{
	PSCORES pS;
	SCORES score;
	PARSER p;
	DELIM igualador = DELIM_NULL;
	DELIM separador={ '\\', '\\', '\\' };

	int i;

	if( strlen(str)==0 )
		goto ok;

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	scores_flush();

	do {
		if( (i=parser_call( &p )) ) {
			if( aux_scores( &score, p.token ) != TEG_STATUS_SUCCESS )
				goto error;

			pS = malloc( sizeof(*pS));
			if( ! pS )
				goto error;

			*pS = score;
			scores_insert_score( pS );
		}
	} while( i && p.hay_otro);

ok:
	gui_scores();
	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_scores()");
	return TEG_STATUS_PARSEERROR;
}


/* the game have just started */
TEG_STATUS clitok_start(char *str)
{
	CPLAYER j;
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ '/', '/', '/' };
	int i;

	if( strlen(str)==0 )
		goto error;

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	player_flush();
	do {
		if( (i=parser_call( &p )) ) {
			if( aux_status( &j, p.token ) != TEG_STATUS_SUCCESS )
				goto error;

			player_ins( &j );
		}
	} while( i && p.hay_otro);

	ESTADO_SET(PLAYER_STATUS_START);

	out_countries();
	gui_start();
	return TEG_STATUS_SUCCESS;

error:
	textmsg(M_ERR,"Error in clitok_start()");
	return TEG_STATUS_ERROR;
}


/* what cards do I have */
TEG_STATUS clitok_enum_cards( char *str )
{
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };
	int country, used;
	PLIST_ENTRY ltmp;

	g_game.tarjetas_cant = 0;

	while( ! IsListEmpty( &g_game.tarjetas_list ) )
		ltmp = RemoveHeadList( &g_game.tarjetas_list );

	if(  ! str || strlen(str) == 0 )
		goto ok;

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	do {
		if( parser_call( &p ) ) {
			country = atoi( p.token );
			used = atoi( p.value );
		} else 
			goto error;

		if( country < 0 || country >= COUNTRIES_CANT )
			goto error;

		InsertTailList( &g_game.tarjetas_list, (PLIST_ENTRY) &g_countries[ country ].tarjeta );
		g_game.tarjetas_cant++;

		if( used ) tarjeta_usar( &g_countries[ country ].tarjeta );
		g_countries[ country ].tarjeta.numjug = WHOAMI();

	} while ( p.hay_otro );

ok:
	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_enum_cards()");
	return TEG_STATUS_ERROR;
}

/* someone has exchanged cards for armies. maybe its me */
TEG_STATUS clitok_exchange(char *str)
{
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };
	int p1,p2,p3;
	int numjug,cant;
	PCPLAYER pJ;

	if( strlen(str)==0 )
		goto error;

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	if( parser_call( &p ) && p.hay_otro ) {
		numjug = atoi( p.token );
	} else goto error;

	if( parser_call( &p ) && p.hay_otro ) {
		cant = atoi( p.token );		
	} else goto error;

	if( parser_call( &p ) && p.hay_otro ) {
		p1 = atoi( p.token );		
	} else goto error;

	if( parser_call( &p ) && p.hay_otro ) {
		p2 = atoi( p.token );		
	} else goto error;

	if( parser_call( &p ) && !p.hay_otro ) {
		p3 = atoi( p.token );		
	} else goto error;

	if( player_whois( numjug, &pJ) != TEG_STATUS_SUCCESS)
		goto error;

	if( numjug == WHOAMI() ) {
		PLIST_ENTRY pL = g_game.tarjetas_list.Flink;

		while( !IsListEmpty( &g_game.tarjetas_list ) && (pL != &g_game.tarjetas_list )) {
			PCOUNTRY pP;
			PTARJETA pT = (PTARJETA) pL;
			pP = (PCOUNTRY ) COUNTRY_FROM_TARJETA( pT );

			if( pP->id == p1 || pP->id == p2 || pP->id == p3 ) {
				PLIST_ENTRY l;

				g_countries[ pP->id ].tarjeta.numjug = -1;
				l = RemoveHeadList( pL->Blink );
				g_game.tarjetas_cant--;

			}
			pL = LIST_NEXT( pL );
		}

		fichas_add_wanted( cant );
		textmsg( M_IMP,_("Exchanged approved. Now you can place %d more armies!"),cant);
		gui_canje(cant,p1,p2,p3);
	} else {
		textmsg(M_IMP,_("Player %s(%s) exchanged 3 cards for %d armies"),
				pJ->name,
				_(g_colores[pJ->color]),
				cant);
	}

	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_tarjeta()");
	return TEG_STATUS_ERROR;
}


/* tells the rules of the game */
TEG_STATUS clitok_modalidad(char *str)
{
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };
	int with_secret_mission, with_common_mission, with_fog_of_war, rules;

	if( strlen(str)==0 )
		goto error;

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	if( parser_call( &p ) && p.hay_otro ) {
		with_secret_mission = atoi( p.token );
	} else goto error;

	if( parser_call( &p ) && p.hay_otro ) {
		with_common_mission = atoi( p.token );
	} else goto error;

	if( parser_call( &p ) && p.hay_otro ) {
		with_fog_of_war = atoi( p.token );
	} else goto error;

	/* not used */
	if( parser_call( &p ) && !p.hay_otro ) {
		rules = atoi( p.token );		
	} else goto error;

	g_game.rules = rules;
	g_game.with_common_mission = with_common_mission;
	g_game.with_secret_mission = with_secret_mission;
	g_game.with_fog_of_war = with_fog_of_war;

	out_missions();

	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_tarjeta()");
	return TEG_STATUS_ERROR;
}

/* tells what is your secret mission */
TEG_STATUS clitok_mission(char *str)
{
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };
	int mission;

	if( strlen(str)==0 )
		goto error;

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	if( parser_call( &p ) && !p.hay_otro ) {
		mission = atoi( p.token );
	} else goto error;

	if( mission < 0 || mission >= missions_cant() )
		goto error;

	g_game.secret_mission = mission;
	gui_mission();
	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_mission()");
	return TEG_STATUS_ERROR;
}

/* I'm receiving the card I've requested after finishing my turn */
TEG_STATUS clitok_tarjeta(char *str)
{
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };
	int country,used;

	if( strlen(str)==0 )
		goto error;

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;


	if( parser_call( &p ) && p.hay_otro ) {
		country = atoi( p.token );
	} else goto error;

	if( parser_call( &p ) && !p.hay_otro ) {
		used = atoi( p.token );		
	} else goto error;

	if( country < 0 || country >= COUNTRIES_CANT )
		goto error;

	ESTADO_SET(PLAYER_STATUS_TARJETA);

	InsertTailList( &g_game.tarjetas_list, (PLIST_ENTRY) &g_countries[ country ].tarjeta );
	g_game.tarjetas_cant++;

	if( used )
		tarjeta_usar( &g_countries[ country ].tarjeta );
	g_countries[ country ].tarjeta.numjug = WHOAMI();

	if( used ) {
		textmsg(M_IMP,_("You received card: '%s' and 2 armies where placed there"), countries_get_name( country ) );
	} else {
		textmsg(M_IMP,_("You received card: '%s'"), countries_get_name( country ) );
	}

	gui_tarjeta( country );
	return TEG_STATUS_SUCCESS;

error:
	textmsg(M_ERR,"Error in clitok_tarjeta()");
	return TEG_STATUS_ERROR;
}

/* Servers's Protocol version. HIVER MUST be equal, otherwise wont work */
TEG_STATUS clitok_pversion( char *str)
{
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };
	int hi,lo;

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	if( strlen(str)==0 )
		goto error;

	if( parser_call( &p ) && p.hay_otro ) {
		hi = atoi( p.token );
	} else goto error;

	if( parser_call( &p ) && !p.hay_otro ) {
		lo = atoi( p.token );
	} else goto error;

	if( hi != PROTOCOL_HIVER ) {
		textmsg(M_ERR,_("Aborting: Different protocols version. Server:%d Client:%d"),hi,PROTOCOL_HIVER);
		teg_disconnect();
		return TEG_STATUS_ERROR;
	}

	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_pversion()");
	return TEG_STATUS_ERROR;
}

/* server version */
TEG_STATUS clitok_sversion( char *str)
{
	if( strlen(str)==0 )
		goto error;

	textmsg(M_ALL,"%s",str);
	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_pversion()");
	return TEG_STATUS_ERROR;
}

/* says who starts the round and the round number */
TEG_STATUS clitok_new_round( char *str )
{
	PARSER p;
	DELIM igualador={ ':', ':', ':' };
	DELIM separador={ ',', ',', ',' };
	int numjug, round_number;
	PCPLAYER pJ;

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	if( strlen(str)==0 )
		goto error;

	if( parser_call( &p ) && p.hay_otro ) {
		numjug = atoi( p.token );
	} else goto error;

	if( parser_call( &p ) && !p.hay_otro ) {
		round_number = atoi( p.token );
	} else goto error;

	if( round_number >=0 && numjug >= 0 ) {
		g_game.round_number = round_number;
		g_game.who_started_round = numjug;
	} else goto error;

	if( player_whois( numjug, &pJ) != TEG_STATUS_SUCCESS) {
		/* no lo tengo en la base */
		textmsg( M_IMP,_("Player %d started round number: %d"), numjug, round_number );
	} else {
		textmsg( M_IMP,_("Player %s(%s) started round number: %d"),
				pJ->name,
				_(g_colores[pJ->color]),
				round_number
				);
	}

	gui_sensi();

	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_new_round()");
	return TEG_STATUS_ERROR;
}

/* avoids perdig effect */
#ifdef WITH_GGZ
TEG_STATUS clitok_ggz( char *str)
{
	int is_human=1;

	if( !g_game.with_ggz )
		goto error;

	if( str && str[0] ) {
		is_human = atoi(str);

		g_game.observer = ! is_human;
	}

	out_pversion();
	out_id();
	return TEG_STATUS_SUCCESS;
error:
	textmsg(M_ERR,"Error in clitok_ggz()");
	return TEG_STATUS_ERROR;
}
#endif /* WITH_GGZ */



/*
 * 	code that interprets which fn to call
 */
static TEG_STATUS client_lookup( int fd, PARSER *p )
{
	int i;

	for(i = 0; i < NRCLITOKENS; i++) {
		if(strcasecmp( p->token, tokens[i].label )==0 ){
			if (tokens[i].func)
				return( (tokens[i].func)(p->value));
			return TEG_STATUS_TOKENNULL;
		}
	}
	textmsg( M_ERR, _("Token '%s' not found"),p->token);
	return TEG_STATUS_TOKENNOTFOUND;
}

/* read from the fd, then parse the data */
TEG_STATUS client_recv( int fd )
{
	int i,j;
	PARSER p;
	char str[PROT_MAX_LEN];
	DELIM igualador={ '=', '=', '=' };
	DELIM separador={ ';', ';', ';' };

	p.igualador = &igualador;
	p.separador = &separador;

	memset(str,0,sizeof(str));
	j=net_readline( fd, str, PROT_MAX_LEN );

	if( j<1 ) {
		teg_disconnect();
		return TEG_STATUS_CONNCLOSED;
	}

	p.data = str;

	do {
		if( (i=parser_call( &p )) ) {
			if( client_lookup( fd,&p ) == TEG_STATUS_CONNCLOSED ) {
				return TEG_STATUS_CONNCLOSED;
			}
		}
	} while( i && p.hay_otro);

	return TEG_STATUS_SUCCESS;
}
