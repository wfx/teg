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

#pragma once

#include <set>
#include <string>

#include <libxml/parser.h>

#include "../common/common.h"

#define THEME_CONTINENT_MAX	(30)
#define THEME_COUNTRY_MAX	(30)
#define THEME_MAX_NAME		(50)


/* country */
typedef struct _theme_country {
	xmlChar *name;		/**< name of the country */
	xmlChar *file;		/**< name of the file .png */
	xmlChar *pos_x;
	xmlChar *pos_y;
	xmlChar *army_x;
	xmlChar *army_y;
} Country, *pCountry;

typedef struct _tcountry {
	char	*name;		/**< name of the country */
	char	*file;		/**< name of the file .png */
	int	pos_x;
	int	pos_y;
	int	army_x;
	int	army_y;
} TCountry, *pTCountry;

/* continent */
typedef struct _theme_continent {
	xmlChar *name;
	xmlChar *pos_x;
	xmlChar *pos_y;
	int	i_country;
	pCountry countries[THEME_COUNTRY_MAX];
} Continent, *pContinent;

typedef struct _tcontinent {
	char	*name;
	int	pos_x;
	int	pos_y;
} TContinent, *pTContinent;

/* cards */
typedef struct _theme_cards {
	xmlChar *jocker;	/**< the jocker figure */
	xmlChar *balloon;	/**< the balloon ship */
	xmlChar *cannon;	/**< the cannon ship */
	xmlChar *ship;		/**< the ship figure */
	xmlChar *pos_y;		/**< position Y of the name in the card */
} Cards, *pCards;

typedef struct _tcards {
	char	* jocker;
	char	* balloon;
	char	* cannon;
	char	* ship;
	int	pos_y;
} TCards, *pTCards;

/* theme */
typedef struct _theme {
	xmlChar	*author;	/**< author of the theme */
	xmlChar	*email;		/**< email of the author */
	xmlChar	*version;	/**< version of the theme */
	xmlChar *screen_size_x;	/**< screen height */
	xmlChar *screen_size_y;	/**< screen width */
	xmlChar	*board;		/**< board file name */
	xmlChar *board_x;	/**< board x position */
	xmlChar *board_y;	/**< board y position */
	xmlChar	*armies_x;
	xmlChar	*armies_y;
	xmlChar *armies_background;
	xmlChar *armies_dragable;
	xmlChar *choose_colors_custom;
	xmlChar *choose_colors_prefix;
	xmlChar *toolbar_custom;
	xmlChar *toolbar_name;
	xmlChar *toolbar_text_color;
	xmlChar	*toolbar_offset_right;
	xmlChar	*toolbar_offset_left;
	xmlChar	*dices_x;
	xmlChar	*dices_y;
	xmlChar	*dices_ext_x[6];	/**< extended dices support */
	xmlChar	*dices_ext_y[6];
	xmlChar	*dices_ext_text_x[2];
	xmlChar	*dices_ext_text_y[2];
	xmlChar *dices_file[DICES_CANT];	/**< 6 dices */
	xmlChar	*dices_color;
	xmlChar	*screenshot;
	int	i_continent;
	pCards	cards;
	pContinent continents[THEME_CONTINENT_MAX];
} Theme, *pTheme;

typedef struct _ttheme {
	char	*author;	/**< author of the theme */
	char	*email;		/**< email of the author */
	char	*version;	/**< version of the theme */
	int	screen_size_x;	/**< screen height */
	int	screen_size_y;	/**< screen width */
	char	*board;		/**< board file name */
	int	board_x;	/**< board x position */
	int	board_y;	/**< board y position */
	int	armies_x;
	int	armies_y;
	int	armies_background;
	int	armies_dragable;
	int	choose_colors_custom;
	char	*choose_colors_prefix;
	int	toolbar_custom;
	char	*toolbar_name;
	char	*toolbar_text_color;
	int	toolbar_offset_right;
	int	toolbar_offset_left;
	int	dices_x;
	int	dices_y;
	int	dices_ext_x[6];	/**< extended dices support */
	int	dices_ext_y[6];
	int	dices_ext_text_x[2];
	int	dices_ext_text_y[2];
	char	*dices_file[DICES_CANT];
	char	*dices_color;
	char	*screenshot;
} TTheme, *pTTheme;

using ThemeDirectories = std::set<std::string>;

/// \brief Tries to load the theme named \p name from different directories
TEG_STATUS theme_load(char *name);

/// \brief Fill in the card details from the theme dataset
TEG_STATUS theme_giveme_cards(pTCards pC);

/// \brief Fill in the coordinates of continent \p n into the structure \p pC
TEG_STATUS theme_giveme_continent(pTContinent pC, int n);

/// \brief Fill in the details of country \p n of continent \p cont into \p pC
TEG_STATUS theme_giveme_country(pTCountry pC, int cont, int n);

/// \brief Copy the theme data into \p pT
TEG_STATUS theme_giveme_theme(pTTheme pT);

/// \brief Return the list of all available themes
ThemeDirectories const& theme_enum_themes();

/// \brief Tries to load the theme \p name.
char * theme_load_file(const char *name);

/// Tries to load the preview image
char * theme_load_fake_file(const char *name, char *theme);

/**! returns 1 if dices extended are being used */
int theme_using_extended_dices();
