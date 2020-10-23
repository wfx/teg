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
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <glib.h>
#include <time.h>

#include "../common/xml_support.h"
#include "server.h"
#include "xmlscores.h"
#include "scores.h"

#ifndef xmlChildrenNode
#define xmlChildrenNode childs
#define xmlRootNode root
#endif


static bool parseScore(xmlDocPtr doc, xmlNodePtr cur, PSCORES ret)
{
	xmlChar *val;

	memset(ret, 0, sizeof(*ret));

	val = xmlGetProp(cur, (const xmlChar *) "name");
	if (val == NULL) {
		fprintf(stderr, ("Score has no name\n"));
		return false;
	} else
		strncpy( (char*)ret->name, (char*)val, sizeof(ret->name) -1 );

	val =  xmlGetProp(cur, (const xmlChar *) "color");
	if ( val == NULL) {
		fprintf(stderr, ("Score has no color\n"));
		return false;
	} else
		ret->color = atoi( (char*)val );

	val = xmlGetProp(cur, (const xmlChar *) "points");
	if ( val == NULL) {
		fprintf(stderr, ("Score has no score\n"));
		return false;
	} else
		ret->score = atoi( (char*)val );

	val = xmlGetProp(cur, (const xmlChar *) "date");
	if (val == NULL) {
		fprintf(stderr, ("Score has no date\n"));
		return false;
	} else
		strncpy( ret->date, (char*)val, sizeof(ret->date) -1 );

	val = xmlGetProp(cur, (const xmlChar *) "human");
	if (val == NULL) {
		fprintf(stderr, ("Score has no human\n"));
		return false;
	} else
		ret->human = atoi( (char*)val );

	return true;
}

TEG_STATUS xmlscores_load( void )
{
	char filename[512];
	xmlDocPtr doc;
	xmlNodePtr cur;

	/*
	 * build an XML tree from a the file;
	 */

	snprintf( filename, sizeof(filename)-1,"%s/%s/server_scores.xml",g_get_home_dir(),TEG_DIRRC);
	filename[ sizeof(filename)-1 ] = 0;

	doc = xmlParseFile( filename );

	if (doc == NULL)
		return TEG_STATUS_ERROR;

	/*
	 * Check the document is of the right kind
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
			SCORES score;
			if(parseScore(doc, cur, &score)) {
				insert_score(&score);
			}
		} else {
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

static void xmlscores_add(xmlNodePtr parent, PSCORES pS)
{
	xmlNodePtr child = xmlNewTextChild( parent , NULL, (xmlChar*)"score", NULL );

	xmlSetProp( child, (xmlChar*)"name", (xmlChar*)pS->name );
	add_numeric_attribute(child, "points", pS->score);
	add_numeric_attribute(child, "color", pS->color);
	xmlSetProp( child, (xmlChar*)"date", (xmlChar*)pS->date );
	add_numeric_attribute(child, "human", pS->human);
}

static void save_single_score(PSCORES pS, void* user)
{
	xmlNodePtr node = (xmlNodePtr) user;
	xmlscores_add(node, pS);
}

void xmlscores_save(void)
{
	xmlDocPtr doc;
	xmlNodePtr child;
	char filename[512];

	doc = xmlNewDoc((xmlChar*)"1.0");

	child = xmlNewDocRawNode( doc, NULL, (xmlChar*)"teg_scores", NULL );

	xmlDocSetRootElement( doc, child );
	scores_map(save_single_score, (void*) child);

	snprintf( filename, sizeof(filename)-1,"%s/%s/server_scores.xml",g_get_home_dir(),TEG_DIRRC);
	filename[ sizeof(filename)-1 ] = 0;

	xmlSaveFile( filename , doc );
	xmlFreeDoc(doc);
}

/* creates a new node with the correct values */
static void new_score_node(PSPLAYER pJ, PSCORES pS)
{
	time_t tt;
	struct tm *t;

	memset( pS,0,sizeof(*pS) );

	pS->score = pJ->player_stats.score;
	pS->color = pJ->color;
	strncpy( pS->name, pJ->name, sizeof( pS->name ) -1 );
	pS->human = pJ->human;

	time(&tt);
	t = localtime(&tt);

	snprintf( pS->date, sizeof(pS->date) -1, "%.2d/%.2d/%.2d %.2d:%.2d"
			,t->tm_mon +1
			,t->tm_mday
			,t->tm_year + 1900
			,t->tm_hour
			,t->tm_min
		);

	pS->date[ sizeof(pS->date) -1 ] = '\0';
}

TEG_STATUS scores_insert_player( PSPLAYER pJ )
{
	if( ! pJ->is_player )
		return TEG_STATUS_ERROR;

	stats_score( &pJ->player_stats, g_conts );

	if( pJ->player_stats.score == 0 && pJ->player_stats.armies_killed == 0 )
		return TEG_STATUS_ERROR;

	SCORES score;
	new_score_node(pJ, &score);
	insert_score(&score);

	xmlscores_save();

	return TEG_STATUS_SUCCESS;
}

struct AppendString
{
	char* dest;
	size_t storage_remaining;
	char const* delim;
	bool valid;
};

void appendScoreString(PSCORES pS, void* user)
{
	struct AppendString* as = (struct AppendString*) user;
	if(!as->valid)
		return;
	int printed = snprintf(as->dest, as->storage_remaining,
	                       "%s%s,%d,%s,%d,%d",
	                       as->delim,
	                       pS->name, pS->color, pS->date, pS->score, pS->human);
	if((printed < 0) // some printf error
	   || (printed >= as->storage_remaining)) // string truncation
		as->valid = false;
	as->dest += printed;
	as->storage_remaining -= printed;
	as->delim = "\\";
}

TEG_STATUS scores_dump(char *strout, size_t len )
{
	strout[0]=0; // zero out the string if there are no scores.
	struct AppendString as = {
		.dest = strout, .storage_remaining=len-1, .delim="", .valid=true
	};
	scores_map(appendScoreString, &as);
	if(!as.valid) {
		strout[0]=0; // something bad happened, so zero out the result
		return TEG_STATUS_ERROR;
	} else {
		strout[len-1] = 0; // add terminator just in case.
	}

	return TEG_STATUS_SUCCESS;
}
