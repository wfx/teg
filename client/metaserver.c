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
 * client side of the metaserver 
 */

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include <unistd.h>
#include <string.h>

#include "client.h"

LIST_ENTRY g_list_metaserver;
/* metaserver helpers */
static void metaserver_flush_list()
{
	PLIST_ENTRY tmp;

	while( !IsListEmpty( &g_list_metaserver ) ) {
		tmp = RemoveHeadList( &g_list_metaserver );
		free( tmp );
	}
	return;
}

static TEG_STATUS parse_server_entry(xmlDocPtr doc, xmlNodePtr cur)
{
	char *name=NULL, *port=NULL, *version=NULL, *comment=NULL;
	PMETASERVER entry = NULL;
	
	cur = xml_get_element_children( cur );

	while (cur != NULL) {

		/* name */
		if (!xmlStrcmp(cur->name, (const xmlChar *) "name") ) {
			name = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if( ! name ) {
				textmsg( M_ERR, "Metaserver XML: 'entry' has no 'name'\n");
				goto error;
			}
		}

		/* port */
		else if (!xmlStrcmp(cur->name, (const xmlChar *) "port") ) {
			port = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if( ! port ) {
				textmsg( M_ERR, "Metaserver XML: 'entry' has no 'port'\n");
				goto error;
			}
		}

		/* version */
		else if (!xmlStrcmp(cur->name, (const xmlChar *) "version") ) {
			version = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if( ! version ) {
				textmsg( M_ERR, "Metaserver XML: 'entry' has no 'version'\n");
				goto error;
			}
		}

		/* comment */
		else if (!xmlStrcmp(cur->name, (const xmlChar *) "comment") ) {
			comment = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if( ! comment ) {
				textmsg( M_ERR, "Metaserver XML: 'entry' has no 'comment'\n");
				goto error;
			}
		}

		cur = xml_get_element_next( cur );
	}

	if( !name || !port || !version || !comment )
		goto error;

	entry = malloc (sizeof (*entry) );
	if( ! entry )
		goto error;

	memset( entry, 0, sizeof(*entry) );

	strncpy( entry->name, name, META_MAX_NAME );
	strncpy( entry->version, version, META_MAX_NAME );
	strncpy( entry->comment, comment, META_MAX_COMMENT );
	entry->port = atoi( port );

	InsertTailList( &g_list_metaserver, (PLIST_ENTRY) entry );

	return TEG_STATUS_SUCCESS;
error:
	if( entry )
		free( entry );
	return TEG_STATUS_ERROR;
}

static TEG_STATUS metaserver_parse_xml( xmlDocPtr doc )
{
	xmlNodePtr cur;
	char *theme_version;

	/*
	 * Check the document is of the right kind
	 */
	
	cur = xmlDocGetRootElement(doc);

	if (xmlStrcmp(cur->name, (const xmlChar *) "teg_metaserver")) {
		fprintf(stderr,"Wrong type. root node != teg_metaserver\n");
		goto error;
	}

	/* find metaserver version */
	theme_version = xmlGetProp(cur, (const xmlChar *) "version");
	if ( ! theme_version ) {
		fprintf(stderr, "XML: 'teg_metaserver' has no 'version'\n");
		goto error;
	}
	/* compare versions */
	{
		int ver_major, ver_minor;
		char *ret;

		ret = strchr( theme_version, '.' );

		if( ret ) {
			ver_minor = atoi( ret+1);
			*ret = 0;
		} else
			ver_minor = 0;

		ver_major = atoi( theme_version );

#ifdef _DEBUG
		fprintf( stderr, "major:%d, minor: %d\n", ver_major, ver_minor );
#endif

		if( ver_major != META_VERSION_MAJOR || ver_minor > META_VERSION_MINOR )
		{
			fprintf(stderr, "XML: The version of the XML is incompatible.\n . Supported metaserver version are %d.x where x is <= %d\n . Current metaserver version is:%d.%d\n"
					,META_VERSION_MAJOR
					,META_VERSION_MINOR
					,ver_major
					,ver_minor );
			goto error;
		}
	}

	/* Now, walk the tree */
	cur = xml_get_element_children( cur );

	if ( cur == NULL )
		goto error;

	while (cur != NULL) {

		/* entry */
		if (!xmlStrcmp(cur->name, (const xmlChar *) "entry") ) {
			if( parse_server_entry( doc, cur ) != TEG_STATUS_SUCCESS )
				goto error;

		} else {
			fprintf(stderr,"Metaserver XML: Wrong type (%s). 'entry' was expected\n", cur->name);
			goto error;
		}

		cur = xml_get_element_next( cur );
	}

	return TEG_STATUS_SUCCESS;

error:
	return TEG_STATUS_ERROR;
}

TEG_STATUS metaserver_init()
{
	InitializeListHead( &g_list_metaserver );
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS metaserver_get_servers( void )
{
	int s = -1;
	int ret;
	xmlParserCtxtPtr ctxt;
	xmlDocPtr doc = NULL;

	metaserver_flush_list();

	s = net_connect_tcp("teg.game-server.cc",2002);
	if( s < 0 )
		goto error;

	if( net_printf( s, "GET /listServers_xml HTTP/1.1\r\n\r\n") < 0 )
		goto error;


	ctxt = xmlCreateIOParserCtxt(NULL, NULL,
		    (xmlInputReadCallback) net_read,
		    (xmlInputCloseCallback) net_close,
		    (void *)s, XML_CHAR_ENCODING_NONE);
	xmlParseDocument(ctxt);

	ret = ctxt->wellFormed;
	doc = ctxt->myDoc;
	xmlFreeParserCtxt(ctxt);

	if (!ret)
		goto error;

	if( metaserver_parse_xml( doc ) != TEG_STATUS_SUCCESS )
		goto error;

	return TEG_STATUS_SUCCESS;

error:
	if( doc )
		xmlFreeDoc(doc);

	if( s >= 0 )
		net_close( s );

	return TEG_STATUS_ERROR;
}
