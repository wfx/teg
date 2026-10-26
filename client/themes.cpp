/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2001 Ricardo Quesada
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
 * File that parse the theme stuff
 * File based in the job example from libxml
 */

#define _XOPEN_SOURCE 500

#include <vector>
#include <algorithm>

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <glib.h>

#include "../common/tegdebug.h"
#include "../common/xml_support.h"
#include "../common/execute.hpp"
#include "themes.h"
#include "globals.h"
#include "state.h"
#include "common.h"

#ifndef xmlChildrenNode
#define xmlChildrenNode childs
#define xmlRootNode root
#endif

#undef _DEBUG

namespace teg::client
{

#define TEG_THEME_VER_MAJOR (1)
#define TEG_THEME_VER_MINOR (5)


std::filesystem::path theme_dir()
{
	return installation_directory() / "share" / "teg" / "themes";
}

static pCountry parseCountry(xmlDocPtr doc, xmlNodePtr cur)
{
	pCountry ret = NULL;

	ret = (pCountry) malloc(sizeof(Country));

	if(ret == NULL) {
		fprintf(stderr, "Out of memory\n");
		return(NULL);
	}

	memset(ret, 0, sizeof(*ret));

	ret->name = xmlGetProp(cur, (const xmlChar *) "name");
	if(ret->name == NULL) {
		fprintf(stderr, "XML: 'country' has no 'name'\n");
		goto error;
	}

	ret->file = xmlGetProp(cur, (const xmlChar *) "file");
	if(ret->file == NULL) {
		fprintf(stderr, "XML: 'country' has no 'file'\n");
		goto error;
	}

	ret->pos_x = xmlGetProp(cur, (const xmlChar *) "pos_x");
	if(ret->pos_x == NULL) {
		fprintf(stderr, "XML: 'country' has no 'pos_x'\n");
		goto error;
	}

	ret->pos_y = xmlGetProp(cur, (const xmlChar *) "pos_y");
	if(ret->pos_y == NULL) {
		fprintf(stderr, "XML: 'country' has no 'pos_y'\n");
		goto error;
	}

	ret->army_x = xmlGetProp(cur, (const xmlChar *) "army_x");
	if(ret->army_x == NULL) {
		fprintf(stderr, "XML: 'country' has no 'army_x'\n");
		goto error;
	}

	ret->army_y = xmlGetProp(cur, (const xmlChar *) "army_y");
	if(ret->army_y == NULL) {
		fprintf(stderr, "XML: 'country' has no 'army_y'\n");
		goto error;
	}

	cur = xml_get_element_children(cur);

	if(cur != NULL) {
		fprintf(stderr, "Wrong type (%s). Nothing was expected\n", cur->name);
		free(ret);
		return(NULL);
	}

	return(ret);

error:
	free(ret);
	return NULL;
}

static pContinent parseContinent(xmlDocPtr doc, xmlNodePtr cur)
{
	pContinent ret = NULL;
	pCountry curcountry = NULL;

	ret = (pContinent) malloc(sizeof(Continent));

	if(ret == NULL) {
		fprintf(stderr, "Out of memory\n");
		return(NULL);
	}

	memset(ret, 0, sizeof(*ret));

	ret->name = xmlGetProp(cur, (const xmlChar *) "name");
	if(ret->name == NULL) {
		fprintf(stderr, "XML: 'continent' has no 'name'\n");
		goto error;
	}

	ret->pos_x = xmlGetProp(cur, (const xmlChar *) "pos_x");
	if(ret->pos_x == NULL) {
		fprintf(stderr, "XML: 'continent' has no 'pos_x'\n");
		goto error;
	}

	ret->pos_y = xmlGetProp(cur, (const xmlChar *) "pos_y");
	if(ret->pos_y == NULL) {
		fprintf(stderr, "XML: 'continent' has no 'pos_y'\n");
		goto error;
	}

	cur = xml_get_element_children(cur);
	while(cur != NULL) {
		/* country */
		if(!xmlStrcmp(cur->name, (const xmlChar *) "country")) {
			curcountry = parseCountry(doc, cur);
			if(curcountry != NULL) {
				ret->countries[ret->i_country++] = curcountry;
				if(ret->i_country >= THEME_COUNTRY_MAX) {
					break;
				}
			}
		}

		/* else */
		else {
			fprintf(stderr, "Wrong type (%s). country was expected\n", cur->name);
			free(ret);
			return(NULL);
		}

		cur = xml_get_element_next(cur);
	}

	return(ret);
error:
	free(ret);
	return NULL;
}

static pCards parseCards(xmlDocPtr doc, xmlNodePtr cur)
{
	pCards ret = NULL;

	ret = (pCards) malloc(sizeof(Cards));

	if(ret == NULL) {
		fprintf(stderr, "Out of memory\n");
		return(NULL);
	}

	memset(ret, 0, sizeof(*ret));

	cur = xml_get_element_children(cur);
	while(cur != NULL) {
		/* card */
		if(!xmlStrcmp(cur->name, (const xmlChar *) "card")) {

			xmlChar * type = xmlGetProp(cur, (const xmlChar *) "type");
			if(type == NULL) {
				fprintf(stderr, "Card has no type\n");
			}

			if(strcmp((char*)type, "jocker") == 0) {
				ret->jocker = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			} else if(strcmp((char*)type, "balloon") == 0) {
				ret->balloon= xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			} else if(strcmp((char*)type, "cannon") == 0) {
				ret->cannon = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			} else if(strcmp((char*)type, "ship") == 0) {
				ret->ship = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			} else {
				fprintf(stderr, "Unsupported type (%s) in cards\n", (char*)type);
			}
		} else if(!xmlStrcmp(cur->name, (const xmlChar *) "pos_y")) {
			ret->pos_y = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
		}

		/* else */
		else {
			fprintf(stderr, "Wrong name(%s). card was expected\n", cur->name);
		}

		cur = xml_get_element_next(cur);
	}

	return(ret);
}

static pTheme parseTheme(std::string const& filename)
{
	xmlDocPtr doc = NULL;
	pContinent curcontinent = NULL;
	pCards curcards = NULL;
	xmlNodePtr cur;
	pTheme ret = NULL;
	xmlChar *theme_version;

	/*
	 * build an XML tree from a the file;
	 */
	doc = xmlParseFile(filename.c_str());
	if(doc == NULL) {
		return(NULL);
	}

	/*
	 * Check the document is of the right kind
	 */

	cur = xmlDocGetRootElement(doc);
	if(cur == NULL) {
		fprintf(stderr, "Empty document\n");
		goto error;
	}

	if(xmlStrcmp(cur->name, (const xmlChar *) "teg_theme")) {
		fprintf(stderr, "Wrong type. root node != teg_theme\n");
		goto error;
	}

	/* find theme version */
	theme_version = xmlGetProp(cur, (const xmlChar *) "version");
	if(theme_version== NULL) {
		fprintf(stderr, "XML: 'teg_theme' has no 'version'\n");
		goto error;
	}
	/* compare versions */
	{
		int ver_major, ver_minor;
		char *ret;

		ret = strchr((char*)theme_version, '.');

		if(ret) {
			ver_minor = atoi(ret+1);
			*ret = 0;
		} else {
			ver_minor = 0;
		}

		ver_major = atoi((char*)theme_version);

		if(ver_major != TEG_THEME_VER_MAJOR || ver_minor > TEG_THEME_VER_MINOR) {
			fprintf(stderr, "XML: The version of the XML is incompatible.\n . Supported theme version are %d.x where x is <= %d\n . Current theme version is:%d.%d\n"
			        , TEG_THEME_VER_MAJOR
			        , TEG_THEME_VER_MINOR
			        , ver_major
			        , ver_minor);
			goto error;
		}
	}


	/*
	 * Allocate the structure to be returned.
	 */
	ret = (pTheme) malloc(sizeof(Theme));
	if(ret == NULL) {
		fprintf(stderr, "Out of memory\n");
		goto error;
	}
	memset(ret, 0, sizeof(*ret));

	/*
	 * Now, walk the tree.
	 */
	cur = xml_get_element_children(cur);
	if(cur == 0) {
		goto error;
	}

	while(cur != NULL) {

		/* author */
		if(!xmlStrcmp(cur->name, (const xmlChar *) "author")) {
			ret->author = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if(ret->author== NULL) {
				fprintf(stderr, "Theme:Author is empty\n");
				goto error;
			}
		}

		/* email */
		else if(!xmlStrcmp(cur->name, (const xmlChar *) "email")) {
			ret->email = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if(ret->email== NULL) {
				fprintf(stderr, "Theme:Email is empty\n");
				goto error;
			}
		}


		/* version */
		else if(!xmlStrcmp(cur->name, (const xmlChar *) "version")) {
			ret->version= xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if(ret->version == NULL) {
				fprintf(stderr, "Theme:version is empty\n");
				goto error;
			}
		}


		/* continents */
		else if(!xmlStrcmp(cur->name, (const xmlChar *) "continent")) {
			curcontinent = parseContinent(doc, cur);
			if(curcontinent != NULL) {
				ret->continents[ret->i_continent++] = curcontinent;
				if(ret->i_continent>= THEME_CONTINENT_MAX) {
					break;
				}
			}
		}

		/* cards */
		else if(!xmlStrcmp(cur->name, (const xmlChar *) "cards")) {
			curcards = parseCards(doc, cur);
			if(curcards != NULL) {
				ret->cards = curcards;
			}
		}

		/* board */
		else if(!xmlStrcmp(cur->name, (const xmlChar *) "board")) {
			ret->board = xmlGetProp(cur, (const xmlChar *) "file");
			if(ret->board == NULL) {
				fprintf(stderr, "XML: 'board' has no name\n");
				goto error;
			}
		}

		/* armies pos */
		else if(!xmlStrcmp(cur->name, (const xmlChar *) "armies_pos")) {

			ret->armies_x = xmlGetProp(cur, (const xmlChar *) "x");
			if(ret->armies_x== NULL) {
				fprintf(stderr, "XML: 'armies_pos' has no x\n");
				goto error;
			}
			ret->armies_y = xmlGetProp(cur, (const xmlChar *) "y");
			if(ret->armies_y== NULL) {
				fprintf(stderr, "XML: 'armies_pos' has no y\n");
				goto error;
			}

			/* optional */
			ret->armies_background = xmlGetProp(cur, (const xmlChar *) "background");
			ret->armies_dragable = xmlGetProp(cur, (const xmlChar *) "dragable");
		}

		/* dices pos */
		else if(!xmlStrcmp(cur->name, (const xmlChar *) "dices_pos")) {

			ret->dices_x = xmlGetProp(cur, (const xmlChar *) "x");
			if(ret->dices_x == NULL) {
				fprintf(stderr, "XML: 'dices_pos' has no x\n");
				goto error;
			}
			ret->dices_y = xmlGetProp(cur, (const xmlChar *) "y");
			if(ret->dices_y == NULL) {
				fprintf(stderr, "XML: 'dices_pos' has no y\n");
				goto error;
			}
		}


		/* dices text */
		else if(!xmlStrcmp(cur->name, (const xmlChar *) "dices_text")) {

			int i;
			char buf[16];

			ret->dices_color = xmlGetProp(cur, (const xmlChar *) "color");
			if(ret->dices_color== NULL) {
				fprintf(stderr, "XML: 'dices_text' has no color\n");
				goto error;
			}

			/* extended dices support for text. It is optional in dices_text */
			for(i=0; i<2; i++) {
				/* x */
				sprintf(buf, "x%d", i+1);
				ret->dices_ext_text_x[i] = xmlGetProp(cur, (const xmlChar *) buf);
				/* y */
				sprintf(buf, "y%d", i+1);
				ret->dices_ext_text_y[i] = xmlGetProp(cur, (const xmlChar *) buf);
			}
		}

		/* extended dices support */
		else if(!xmlStrcmp(cur->name, (const xmlChar *) "dices_ext_pos")) {
			int i;
			char buf[16];

#ifdef _DEBUG
			fprintf(stderr, "Found extended dices support\n");
#endif
			for(i=0; i<6; i++) {
				/* x */
				sprintf(buf, "x%d", i+1);
				ret->dices_ext_x[i] = xmlGetProp(cur, (const xmlChar *) buf);
				if(ret->dices_ext_x[i] == NULL) {
					fprintf(stderr, "XML: 'dices_ext_pos' has no '%s'\n", buf);
					goto error;
				}

				/* y */
				sprintf(buf, "y%d", i+1);
				ret->dices_ext_y[i] = xmlGetProp(cur, (const xmlChar *) buf);
				if(ret->dices_ext_y[i] == NULL) {
					fprintf(stderr, "XML: 'dices_ext_pos' has no '%s'\n", buf);
					goto error;
				}
			}
		}

		/* choose_colors (optional) */
		else if(!xmlStrcmp(cur->name, (const xmlChar *) "choose_colors")) {

			/* custom */
			ret->choose_colors_custom = xmlGetProp(cur, (const xmlChar *) "custom");

			/* prefix */
			ret->choose_colors_prefix = xmlGetProp(cur, (const xmlChar *) "prefix");
		}

		/* toolbar (optional) */
		else if(!xmlStrcmp(cur->name, (const xmlChar *) "toolbar")) {

			/* custom */
			ret->toolbar_custom = xmlGetProp(cur, (const xmlChar *) "custom");

			/* name */
			ret->toolbar_name = xmlGetProp(cur, (const xmlChar *) "name");

			/* text_color */
			ret->toolbar_text_color = xmlGetProp(cur, (const xmlChar *) "text_color");

			/* offset left */
			ret->toolbar_offset_left = xmlGetProp(cur, (const xmlChar *) "offset_left");

			/* offset right */
			ret->toolbar_offset_right = xmlGetProp(cur, (const xmlChar *) "offset_right");
		}



		/* screenshot */
		else if(!xmlStrcmp(cur->name, (const xmlChar *) "screenshot")) {
			ret->screenshot = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if(ret->screenshot== NULL) {
				fprintf(stderr, "XML: 'screenshot' is empty\n");
				goto error;
			}
		}

		/* screen */
		else if(!xmlStrcmp(cur->name, (const xmlChar *) "screen")) {
			ret->screen_size_x = xmlGetProp(cur, (const xmlChar *) "size_x");
			if(ret->screen_size_x == NULL) {
				fprintf(stderr, "XML: 'screen' has no size_x\n");
				goto error;
			}
			ret->screen_size_y = xmlGetProp(cur, (const xmlChar *) "size_y");
			if(ret->screen_size_x == NULL) {
				fprintf(stderr, "XML: 'screen' has no size_y\n");
				goto error;
			}
		}

		/* map */
		else if(!xmlStrcmp(cur->name, (const xmlChar *) "map")) {
			ret->board = xmlGetProp(cur, (const xmlChar *) "file");
			if(ret->board == NULL) {
				fprintf(stderr, "XML: 'board' has no name\n");
				goto error;
			}
			ret->board_x = xmlGetProp(cur, (const xmlChar *) "offset_x");
			if(ret->board_x == NULL) {
				ret->board_x = (xmlChar*)"0";
			}
			ret->board_y = xmlGetProp(cur, (const xmlChar *) "offset_y");
			if(ret->board_y == NULL) {
				ret->board_y = (xmlChar*)"0";
			}
		} else {
			fprintf(stderr, "Wrong type (%s). 'continent', 'board' or 'map', 'screen' and 'dices' were expected\n", cur->name);
			free(ret);
			goto error;
		}

		cur = xml_get_element_next(cur);
	}

	xmlFreeDoc(doc);
	return(ret);

error:
	if(doc) {
		xmlFreeDoc(doc);
	}
	if(ret) {
		free(ret);
	}
	return(NULL);
}

static TEG_STATUS theme_fill_country_name(ClientState& state)
{
	int j, k, l;

	if(!state.theme.current) {
		return TEG_STATUS_ERROR;
	}

	l=0;
	for(j=0; j<state.theme.current->i_continent; j++) {
		for(k=0; k<state.theme.current->continents[j]->i_country; k++) {
			state.game.common.countries[l++].name = (char*)state.theme.current->continents[j]->countries[k]->name;
		}
	}

	return TEG_STATUS_SUCCESS;
}

static TEG_STATUS theme_fill_continent_name(ClientState& state)
{
	int i;

	if(!state.theme.current) {
		return TEG_STATUS_ERROR;
	}

	for(i=0; i<state.theme.current->i_continent; i++) {
		state.game.common.conts[i].name = (char*)state.theme.current->continents[i]->name;
	}

	return TEG_STATUS_SUCCESS;
}

/* Loads the 'name' theme */
TEG_STATUS theme_load(ClientState& state, char *name)
{
	using p = std::filesystem::path;
	/* themes/%s/teg_theme.xml */
	state.theme.current = parseTheme(p("themes") / name /"teg_theme.xml");

	/* ~/.teg/themes/%s/teg_theme.xml */
	if(state.theme.current == NULL) {
		state.theme.current = parseTheme(p(g_get_home_dir()) / rc_directory_name / "themes" / name / "teg_theme.xml");
	}

	/* /usr/local/share/teg/themes/%s/teg_theme.xml */
	if(state.theme.current == NULL) {
		auto const path = (theme_dir() / name / "teg_theme.xml");
		state.theme.current = parseTheme(path.c_str());
	}

	if(state.theme.current == NULL) {
		fprintf(stderr, "Unable to load theme `%s'\n", name);
		return TEG_STATUS_THEMEERROR;
	}

	/* place new name for continents */
	theme_fill_continent_name(state);

	/* place new name of the countries */
	theme_fill_country_name(state);

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS theme_giveme_cards(ClientState& state, pTCards pC)
{
	if(state.theme.current == NULL) {
		return TEG_STATUS_ERROR;
	}

	pC->jocker= (char*)state.theme.current->cards->jocker;
	pC->balloon= (char*)state.theme.current->cards->balloon;
	pC->ship= (char*)state.theme.current->cards->ship;
	pC->cannon= (char*)state.theme.current->cards->cannon;
	pC->pos_y = atoi((char*)state.theme.current->cards->pos_y);

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS theme_giveme_continent(ClientState& state, pTContinent pC, int n)
{
	if(state.theme.current == NULL) {
		return TEG_STATUS_ERROR;
	}

	if(n <0 || n >= state.theme.current->i_continent) {
		return TEG_STATUS_ERROR;
	}

	pC->pos_x = atoi((char*)state.theme.current->continents[n]->pos_x);
	pC->pos_y = atoi((char*)state.theme.current->continents[n]->pos_y);

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS theme_giveme_country(ClientState& state, pTCountry pC, int cont, int n)
{
	if(state.theme.current == NULL) {
		return TEG_STATUS_ERROR;
	}

	if(cont <0 || cont >= state.theme.current->i_continent) {
		return TEG_STATUS_ERROR;
	}

	if(n <0 || n >= state.theme.current->continents[cont]->i_country) {
		return TEG_STATUS_ERROR;
	}

	pC->name = (char*)state.theme.current->continents[cont]->countries[n]->name;
	pC->file = (char*)state.theme.current->continents[cont]->countries[n]->file;
	pC->pos_x= atoi((char*)state.theme.current->continents[cont]->countries[n]->pos_x);
	pC->pos_y= atoi((char*)state.theme.current->continents[cont]->countries[n]->pos_y);
	pC->army_x = atoi((char*)state.theme.current->continents[cont]->countries[n]->army_x);
	pC->army_y = atoi((char*)state.theme.current->continents[cont]->countries[n]->army_y);

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS theme_giveme_theme(ClientState& state, pTTheme pT)
{
	int i;

	if(state.theme.current == NULL) {
		return TEG_STATUS_ERROR;
	}

	pT->author= (char*)state.theme.current->author;
	pT->email = (char*)state.theme.current->email;
	pT->version = (char*)state.theme.current->version;
	pT->screen_size_x = my_atoi((char*)state.theme.current->screen_size_x);
	if(pT->screen_size_x == -1) {
		pT->screen_size_x = 640;
	}
	pT->screen_size_y = my_atoi((char*)state.theme.current->screen_size_y);
	if(pT->screen_size_y == -1) {
		pT->screen_size_y = 480;
	}
	pT->board = (char*)state.theme.current->board;
	pT->board_x = my_atoi((char*)state.theme.current->board_x);
	pT->board_y = my_atoi((char*)state.theme.current->board_y);

	pT->dices_x = my_atoi((char*)state.theme.current->dices_x);
	pT->dices_y = my_atoi((char*)state.theme.current->dices_y);
	pT->armies_x = atoi((char*)state.theme.current->armies_x);
	pT->armies_y = atoi((char*)state.theme.current->armies_y);

	pT->armies_background = 1;	/* default */
	if(state.theme.current->armies_background) {
		pT->armies_background = (strcasecmp((char*)state.theme.current->armies_background, "true") == 0);
	}

	pT->armies_dragable = 1;	/* default */
	if(state.theme.current->armies_background) {
		pT->armies_dragable = (strcasecmp((char*)state.theme.current->armies_background, "true") == 0);
	}

	for(i=0; i<static_cast<int>(sides_on_the_dice); i++) {
		pT->dices_file[i] = (char*)state.theme.current->dices_file[i];
	}

	pT->dices_color = (char*)state.theme.current->dices_color;
	pT->screenshot = (char*)state.theme.current->screenshot;

	/* optional features */

	/* extended dices support. This is an optional feature. See Draco Theme */
	for(i=0; i<6; i++) {
		pT->dices_ext_x[i] = my_atoi((char*)state.theme.current->dices_ext_x[i]);
		pT->dices_ext_y[i] = my_atoi((char*)state.theme.current->dices_ext_y[i]);
	}

	for(i=0; i<2; i++) {
		pT->dices_ext_text_x[i] = my_atoi((char*)state.theme.current->dices_ext_text_x[i]);
		pT->dices_ext_text_y[i] = my_atoi((char*)state.theme.current->dices_ext_text_y[i]);
	}

	pT->choose_colors_custom = 0;
	pT->choose_colors_prefix = NULL;
	if(state.theme.current->choose_colors_custom && strcasecmp((char*)state.theme.current->choose_colors_custom, "true") == 0) {
		pT->choose_colors_custom = 1;
	}
	pT->choose_colors_prefix = (char*)state.theme.current->choose_colors_prefix;

	pT->toolbar_custom = 0;
	pT->toolbar_name = NULL;
	pT->toolbar_text_color = NULL;
	if(state.theme.current->toolbar_custom && strcasecmp((char*)state.theme.current->toolbar_custom, "true") == 0) {
		pT->toolbar_custom = 1;
	}
	pT->toolbar_name = (char*)state.theme.current->toolbar_name;
	pT->toolbar_text_color = (char*)state.theme.current->toolbar_text_color;
	pT->toolbar_offset_left = my_atoi((char*)state.theme.current->toolbar_offset_left);
	pT->toolbar_offset_right = my_atoi((char*)state.theme.current->toolbar_offset_right);

	return TEG_STATUS_SUCCESS;
}

ThemeDirectories const& theme_enum_themes(ClientState& state)
{
	if(state.theme.available.size() != 0) {
		return state.theme.available;
	}

	// the search paths for theme files
	std::string dirnames[3] {
		"themes",
		std::string(g_get_home_dir()) + "/" + rc_directory_name + "/themes",
		theme_dir().string()
	};
	/* /usr/local/share/teg/themes */

	for(std::string const &dir_name: dirnames) {
		DIR * const dir = opendir(dir_name.c_str());
		if(dir==NULL) {
			PDEBUG("Failed for dir %s\n", dir_name.c_str());
			continue;
		}

		/* scan for directories with file teg_theme.xml */
		struct dirent *e;
		while((e = readdir(dir)) != NULL) {
			FILE *fp;
			const auto theme_file_name =
			    dir_name + '/' + e->d_name + "/teg_theme.xml";

			if((fp = fopen(theme_file_name.c_str(), "r"))) {
				fclose(fp);
				state.theme.available.insert(e->d_name);
				PDEBUG("Insert %s\n", theme_file_name.c_str());
			} else {
				PDEBUG("Failed to open %s\n", theme_file_name.c_str());
			}
		}
		closedir(dir);
	}

	return state.theme.available;
}

/* Finds the path for a filename */
/// \todo make this return std::string
std::optional<std::filesystem::path> theme_load_file(ClientState& state, char const *name)
{
	return theme_load_fake_file(name, state.game.theme);
}

std::optional<std::filesystem::path> valid_filename(std::initializer_list<char const*> parts)
{
	std::filesystem::path to_check;
	for(auto const*const part: parts) {
		to_check /= part;
	}
	if(std::filesystem::exists(to_check)) {
		return to_check;
	} else {
		return std::nullopt;
	}
}

/* Loads a pixmap of a not loaded theme */
std::optional<std::filesystem::path> theme_load_fake_file(char const *name, char const *theme)
{
	std::optional<std::filesystem::path> result;
	using P = std::initializer_list<char const*>;

	result = valid_filename(P{"themes", theme, name});
	if(result) {
		return result;
	}

	result = valid_filename(P{g_get_home_dir(), rc_directory_name, "themes", theme, name});
	if(result) {
		return result;
	}

	return valid_filename(P{theme_dir().c_str(), theme, name});
}

int theme_using_extended_dices(ClientState& state)
{
	if(state.theme.current == NULL) {
		fprintf(stderr, "theme_using_extended() cant be called before loading a theme!\n");
		return 0;
	}

	return (my_atoi((char*)state.theme.current->dices_ext_x[0]) != -1);
}

}
