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
/*
 * Handles the metaserver's commands
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef HAVE_PTHREADS
#include <pthread.h>
#endif

#include "server.h"

TEG_STATUS metaserver_status(int fd, char *str);
TEG_STATUS metaserver_on(int fd, char *str);
TEG_STATUS metaserver_off(int fd, char *str);
TEG_STATUS metaserver_address(int fd, char *str);
TEG_STATUS metaserver_help(int fd, char *str);

TEG_STATUS metaserver_lookup( int fd, PARSER *p );

static int g_just_once = 0;
static char *g_comment;

struct {
	char *label; TEG_STATUS (*func) ();
	char *help;
} meta_options[] = {
	{ OPTION_META_STATUS, metaserver_status, N_("Shows status of this server this the metaserver") },
	{ OPTION_META_ON, metaserver_on, N_("Add this server to the metaserver, with an optional message") },
	{ OPTION_META_OFF, metaserver_off, N_("Delete this server from the metaserver") },
	{ OPTION_META_ADDRESS, metaserver_address, N_("Show/Change the metaserver address") },
	{ OPTION_HELP, metaserver_help, N_("Shows the metaserver options") },
};
#define	NOPTIONS  (sizeof(meta_options)/sizeof(meta_options[0]))

/* threaded funcions ! */
void * thread_meta_on( void * commentptr )
{
	int s;
	char *comment;
	
	comment = (char*)commentptr;

	if( ! g_just_once ) {
		con_text_out_wop(M_INF,_("Adding %s:%d to metaserver %s:%d...\n"),
				g_server.name, g_server.port,
				g_server.metaserver_name, g_server.metaserver_port
				);
	}

	s = net_connect_tcp( g_server.metaserver_name, g_server.metaserver_port);
	if( s < 0 ) {
		con_text_out(M_INF,_("Error\n"));
		goto error;
	}

	net_printf(s, "GET /addServer/%s/%d/%s/%s HTTP/1.0\r\n\r\n",
			g_server.name,
			g_server.port,
			VERSION,
			comment);

	if( ! g_just_once ) {
		con_text_out(M_INF,_("OK\n"));
		g_just_once = 1;
	}

	net_close(s);
/*	g_server.metaserver_on = 1; */
	return NULL;

error:
/*	g_server.metaserver_on = 0; */
	return NULL;
}

void * thread_meta_off( void * unused )
{
	int s;
	con_text_out_wop(M_INF,_("Deleting %s:%d from metaserver %s:%d...\n"),
			g_server.name, g_server.port,
			g_server.metaserver_name, g_server.metaserver_port
			);

	s = net_connect_tcp( g_server.metaserver_name, g_server.metaserver_port);
	if( s < 0 ) {
		con_text_out(M_INF,_("Error\n"));
		goto error;
	}

	net_printf(s, "GET /delServer/%s/%d HTTP/1.0\r\n\r\n",
			g_server.name,
			g_server.port );
	con_text_out(M_INF,_("OK\n"));
	net_close(s);

error:
	g_just_once = 0;
	g_server.metaserver_on = 0;
	return NULL;
}

/* main thread functions */
static TEG_STATUS view_current_server( void )
{
	con_text_out_wop(M_INF,_("Current metaserver: %s  port: %d\n"), g_server.metaserver_name, g_server.metaserver_port);
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS metaserver_address( int fd, char *str )
{
	char *ptr;

	if( ! str || ! str[0] )
		return view_current_server();

	if( (ptr = strchr(str,' ')) == NULL  ) {
		con_text_out_wop(M_ERR,_("Invalid address. Examples:\n   metaserver address\n   metaserver address teg.game-server.cc 2002\n"));
		goto error;
	}

	g_server.metaserver_port = atoi( ptr+1);
	*ptr = '\0';
	strncpy( g_server.metaserver_name, str, sizeof(g_server.metaserver_name)-1);

	view_current_server();
	return TEG_STATUS_SUCCESS;
error:
	return TEG_STATUS_ERROR;
}

TEG_STATUS metaserver_status( int fd, char *str )
{
	con_text_out_wop(M_INF,_("Metaserver status:\n"));
	con_text_out_wop(M_INF,_("  Metaserver is: %s\n"), ( g_server.metaserver_on ? _("ON") : _("OFF") ));
	view_current_server();
	return TEG_STATUS_SUCCESS;
}

/* str is an optional message */
TEG_STATUS metaserver_on( int fd, char *str )
{
	/* translate spaces into '+' */
	if( str && str[0] )
	{
		int i;
		int l = strlen(str);

		for(i=0;i<l;i++) {
			if (str[i] == ' ' )
				str[i] = '+';
		}
	} else {
		str = "[empty+description]";
	}

	if( g_comment )
		free( g_comment );
	g_comment = strdup( str );


#ifdef HAVE_PTHREADS
	{
		pthread_t tid;
		pthread_create( &tid, NULL, thread_meta_on, g_comment );
	}
#else
	thread_meta_on( str );
#endif
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS metaserver_off( int fd, char *str )
{
#ifdef HAVE_PTHREADS
	pthread_t tid;
	pthread_create( &tid, NULL, thread_meta_off, NULL);
#else
	thread_meta_off( str );
#endif
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS metaserver_help( int fd, char *str )
{
	int i;
	for(i=0;i<NOPTIONS;i++) {
		if(meta_options[i].func)
			net_printf(fd, "'%s' %s\n",meta_options[i].label,_(meta_options[i].help));
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS metaserver_lookup( int fd, PARSER *p )
{
	int i;

	for(i = 0; i < NOPTIONS; i++) {
		if(strcasecmp( p->token, meta_options[i].label )==0 ){
			if (meta_options[i].func)
				return( (meta_options[i].func)(fd,p->value));

			return TEG_STATUS_TOKENNULL;
		}
	}
	net_printf(fd,_("Unknow option. Try using `%s %s' for help\n"),TOKEN_METASERVER,OPTION_HELP);
	return TEG_STATUS_TOKENNOTFOUND;
}

TEG_STATUS metaserver_parse( int fd, char *str)
{
	int i;
	PARSER p;
	DELIM igualador={ '=', ' ', ':' };
	DELIM separador=DELIM_NULL;

	p.igualador = &igualador;
	p.separador = &separador;
	p.data = str;

	do {
		if( (i=parser_call( &p )) ) {
			if( metaserver_lookup( fd,&p ) == TEG_STATUS_CONNCLOSED ) {
				return TEG_STATUS_CONNCLOSED;
			}
		}
	} while( i && p.hay_otro);

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS metaserver_init()
{
	strncpy( g_server.metaserver_name, "teg.game-server.cc", sizeof( g_server.metaserver_name) -1 );
	g_server.metaserver_port = 2002;
	g_server.metaserver_on = 0;

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS metaserver_publish()
{
	if( ! g_comment )
		g_comment = strdup( "[default+description]" );

	if( g_server.metaserver_on )
	{
#ifdef HAVE_PTHREADS
		pthread_t tid;
		pthread_create( &tid, NULL, thread_meta_on, g_comment );
#else
		thread_meta_on( g_comment );
#endif
	}
	return TEG_STATUS_SUCCESS;
}
