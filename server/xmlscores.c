/*	$Id: xmlscores.c,v 1.16 2007/07/07 19:36:13 nordi Exp $	*/
/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2001 Ricardo Quesada
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
/**
 * @file xmlscores.c
 * server scores from/to xml
 */

/*
 * File based in the job example from libxml
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <glib.h>
#include <time.h>

#include "server.h"
#include "xmlscores.h"
#include "scores.h"

#ifndef xmlChildrenNode
#define xmlChildrenNode childs
#define xmlRootNode root
#endif


static PSCORES parseScore(xmlDocPtr doc, xmlNodePtr cur)
{
	PSCORES ret = NULL;
	xmlChar *val;

	ret = (PSCORES) malloc(sizeof(SCORES));

	if (ret == NULL) {
		fprintf(stderr,_("Out of memory\n"));
		return(NULL);
	}
	
	memset(ret, 0, sizeof(*ret));

	val = xmlGetProp(cur, (const xmlChar *) "name");
	if (val == NULL)
		fprintf(stderr, ("Score has no name\n"));
	else
		strncpy( (char*)ret->name, (char*)val, sizeof(ret->name) -1 );

	val =  xmlGetProp(cur, (const xmlChar *) "color");
	if ( val == NULL)
		fprintf(stderr, ("Score has no color\n"));
	else
		ret->color = atoi( (char*)val );

	val = xmlGetProp(cur, (const xmlChar *) "points");
	if ( val == NULL)
		fprintf(stderr, ("Score has no score\n"));
	else
		ret->stats.score = atoi( (char*)val );

	val = xmlGetProp(cur, (const xmlChar *) "date");
	if (val == NULL)
		fprintf(stderr, ("Score has no date\n"));
	else
		strncpy( ret->date, (char*)val, sizeof(ret->date) -1 );

	val = xmlGetProp(cur, (const xmlChar *) "human");
	if (val == NULL)
		fprintf(stderr, ("Score has no human\n"));
	else
		ret->human = atoi( (char*)val );

	return(ret);
}

TEG_STATUS xmlscores_load( void )
{
	char filename[512];
	xmlDocPtr doc;
	xmlNodePtr cur;
	PSCORES curscore;

	/*
	 * build an XML tree from the file;
	 */

	snprintf( filename, sizeof(filename)-1,"%s/%s/server_scores.xml",g_get_home_dir(),TEG_DIRRC);
	filename[ sizeof(filename)-1 ] = 0;

	doc = xmlParseFile( filename );

	if (doc == NULL)
		return TEG_STATUS_ERROR;

	/*
	 * Check if the document is of the right kind
	 */
	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		fprintf(stderr,("Empty document\n"));
		goto error;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *) "teg_scores")) {
		fprintf(stderr,("Wrong type. root node != teg_scores\n"));
		goto error;
	}

	/*
	 * Allocate the structure to be returned.
	 */

	cur = xml_get_element_children( cur );

	while( cur != NULL ) {
		if ( !xmlStrcmp(cur->name, (const xmlChar *) "score") ) {
			curscore = parseScore( doc, cur );
			if (curscore != NULL)
				scores_insert_score(curscore);
		}

		else {
			fprintf(stderr,("Wrong type (%s). score was expected\n"), cur->name);
			goto error;
		}

		cur = xml_get_element_next( cur );
	}

	xmlFreeDoc(doc);
	return TEG_STATUS_SUCCESS;
error:
	xmlFreeDoc(doc);
	return TEG_STATUS_ERROR;
}

static TEG_STATUS xmlscores_add( xmlNodePtr parent, PSCORES pS )
{
	xmlNodePtr child;
	char buffer[512];

	child = xmlNewTextChild( parent , NULL, (xmlChar*)"score", NULL ); 

	xmlSetProp( child, (xmlChar*)"name", (xmlChar*)pS->name );

	snprintf(buffer,sizeof(buffer)-1,"%d", pS->stats.score );
	buffer[ sizeof(buffer) -1 ] = 0;
	xmlSetProp( child, (xmlChar*)"points", (xmlChar*)buffer);

	snprintf(buffer,sizeof(buffer)-1,"%d", pS->color);
	buffer[ sizeof(buffer) -1 ] = 0;
	xmlSetProp( child, (xmlChar*)"color", (xmlChar*)buffer );

	xmlSetProp( child, (xmlChar*)"date", (xmlChar*)pS->date );

	snprintf(buffer,sizeof(buffer)-1,"%d", pS->human);
	buffer[ sizeof(buffer) -1 ] = 0;
	xmlSetProp( child, (xmlChar*)"human", (xmlChar*)buffer );


	return TEG_STATUS_SUCCESS;
}

TEG_STATUS xmlscores_save( PLIST_ENTRY pL_orig )
{
	xmlDocPtr doc;
	xmlNodePtr child;
	PLIST_ENTRY pL = pL_orig->Flink;
	char filename[512];

	PSCORES pS;

	doc = xmlNewDoc((xmlChar*)"1.0");

	child = xmlNewDocRawNode( doc, NULL, (xmlChar*)"teg_scores", NULL );

	/* whats the difference between xmlNewDocRawNode & SetRootElement */
	xmlDocSetRootElement( doc, child );

	while( !IsListEmpty( pL_orig ) && (pL != pL_orig) ) {
		pS = (PSCORES) pL;

		xmlscores_add( child, pS );

		pL = LIST_NEXT(pL);
	}


	snprintf( filename, sizeof(filename)-1,"%s/%s/server_scores.xml",g_get_home_dir(),TEG_DIRRC);
	filename[ sizeof(filename)-1 ] = 0;

	xmlSaveFile( filename , doc );

	return TEG_STATUS_SUCCESS;
}


/* creates a new node with the correct values */
static PSCORES new_score_node( PSPLAYER pJ )
{
	PSCORES pS = NULL;
	time_t tt;
/*	char *char_time; */
/*	char *newline; */
	struct tm *t;

	pS = (PSCORES) malloc( sizeof(*pS) );
	if( !pS )
		return NULL;

	memset( pS,0,sizeof(*pS) );
	InitializeListHead( &pS->next );

	/* copy the stats */
	pS->stats = pJ->player_stats;

	pS->color = pJ->color;
	strncpy( pS->name, pJ->name, sizeof( pS->name ) -1 );
	pS->human = pJ->human;

	time(&tt);
	t = localtime(&tt);

#if 0
	char_time = ctime(&tt);

	/* remove newline */
	if( (newline = strchr( char_time, (int) '\n' )) != NULL )
		*newline = '\0';

	snprintf( pS->date, sizeof(pS->date) -1, "%s", char_time );
#else
	snprintf( pS->date, sizeof(pS->date) -1, "%.2d/%.2d/%.2d %.2d:%.2d"
			,t->tm_mon +1
			,t->tm_mday
			,t->tm_year + 1900
			,t->tm_hour
			,t->tm_min
		);
#endif

	pS->date[ sizeof(pS->date) -1 ] = '\0';
	return pS;
}

TEG_STATUS scores_insert_player( PSPLAYER pJ )
{
	PSCORES pS_new;
	TEG_STATUS s;

	if( ! pJ->is_player )
		return TEG_STATUS_ERROR;

	stats_score( &pJ->player_stats );

	if( pJ->player_stats.score == 0 && pJ->player_stats.armies_killed == 0 )
		return TEG_STATUS_ERROR;

	if( (pS_new = new_score_node( pJ )) == NULL)
		return TEG_STATUS_NOMEM;

	s = scores_insert_score( pS_new );

	xmlscores_save( scores_get_list() );

	return s;
}

TEG_STATUS scores_dump(char *strout, size_t len )
{
	int n;
	int max;
	char strtmp[ PLAYERNAME_MAX_LEN + 200];

	PLIST_ENTRY l = scores_get_list()->Flink;
	PSCORES pS;

	strout[0]=0;

	n=0;
	max=0;
	while( !IsListEmpty( scores_get_list() ) && (l != scores_get_list()) ) {
		pS = (PSCORES) l;

		if(n==0) {
			snprintf(strtmp,sizeof(strtmp)-1,"%s,%d,%s,%d,%d",pS->name,pS->color,pS->date,pS->stats.score,pS->human );
			n=1;
		} else
			snprintf(strtmp,sizeof(strtmp)-1,"\\%s,%d,%s,%d,%d",pS->name,pS->color,pS->date,pS->stats.score,pS->human );

		strtmp[ sizeof(strtmp) -1 ] = 0;

		strncat( strout, strtmp, len - strlen(strout) );

		/* only the top ten */
		if( ++max == 10 )
			break;

		l = LIST_NEXT(l);
	}
	return TEG_STATUS_SUCCESS;
}
