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
/* highlights a country in the map */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <goocanvas.h>
#include <assert.h>

#include "client.h"

#include "locate_country.h"
#include "g_country.h"

namespace teg::client::callbacks
{

static guint timeout_id = -1;

struct _struct_countries {
	int country;
	int is_hidden;
	int number_times_refreshed;
};


#define MAX_LOCATE_COUNTRIES (16)
struct _struct_countries list_locate_countries[ MAX_LOCATE_COUNTRIES ];
struct _struct_countries list_locate_armies[ MAX_LOCATE_COUNTRIES ];


#define MAX_REFRESHES_COUNTRY (5)
#define MAX_REFRESHES_ARMY (7)


static void locate_country_init_entry(struct _struct_countries *s)
{
	s->country = -1;
	s->is_hidden = FALSE;
	s->number_times_refreshed = 0;
}

static gint timeout_cb(gpointer data)
{
	int i;

	/* iterate over the country's list */
	for(i=0; i<MAX_LOCATE_COUNTRIES; i++) {
		if(list_locate_countries[i].country == -1) {
			continue;
		}

		if(list_locate_countries[i].is_hidden) {
			list_locate_countries[i].is_hidden = FALSE;
			g_object_set(G_countries[ list_locate_countries[i].country ].country_item, "visibility", GOO_CANVAS_ITEM_VISIBLE, NULL);

			if(++list_locate_countries[i].number_times_refreshed == MAX_REFRESHES_COUNTRY) {
				locate_country_init_entry(&list_locate_countries[i]);
			}
		} else {
			list_locate_countries[i].is_hidden = TRUE;
			g_object_set(G_countries[ list_locate_countries[i].country ].country_item, "visibility", GOO_CANVAS_ITEM_INVISIBLE, NULL);
		}

	}

	/* iterate over the army's list */
	for(i=0; i<MAX_LOCATE_COUNTRIES; i++) {
		if(list_locate_armies[i].country == -1) {
			continue;
		}

		if(list_locate_armies[i].is_hidden) {
			list_locate_armies[i].is_hidden = FALSE;
			g_object_set(G_countries[ list_locate_armies[i].country ].ellip_item, "visibility", GOO_CANVAS_ITEM_VISIBLE, NULL);
			g_object_set(G_countries[ list_locate_armies[i].country ].text_item, "visibility", GOO_CANVAS_ITEM_VISIBLE, NULL);

			if(++list_locate_armies[i].number_times_refreshed == MAX_REFRESHES_ARMY) {
				locate_country_init_entry(&list_locate_armies[i]);
			}
		} else {
			list_locate_armies[i].is_hidden = TRUE;
			g_object_set(G_countries[ list_locate_armies[i].country ].ellip_item, "visibility", GOO_CANVAS_ITEM_INVISIBLE, NULL);
			g_object_set(G_countries[ list_locate_armies[i].country ].text_item, "visibility", GOO_CANVAS_ITEM_INVISIBLE, NULL);
		}

	}
	return TRUE;
}

/* constructor */
TEG_STATUS locate_country_init()
{
	int i;
	timeout_id = g_timeout_add(300, timeout_cb, NULL);

	for(i=0; i<MAX_LOCATE_COUNTRIES; i++) {
		list_locate_countries[i].country = -1;
		list_locate_armies[i].country = -1;
	}

	return TEG_STATUS_SUCCESS;
}

/* add a country to the list of countries to be shown */
TEG_STATUS locate_country_add_country(PCOUNTRY p)
{
	int i;

	for(i=0; i<MAX_LOCATE_COUNTRIES; i++) {
		if(list_locate_countries[i].country == -1) {

			locate_country_init_entry(&list_locate_countries[i]);
			list_locate_countries[i].country = p->id;
			break;
		}

		if(list_locate_countries[i].country == p->id) {
			list_locate_countries[i].number_times_refreshed = 0;
			break;
		}
	}

	if(i==MAX_LOCATE_COUNTRIES) {
		return TEG_STATUS_ERROR;
	}
	return TEG_STATUS_SUCCESS;
}


/* add the country's armies to the list of armies to be shown */
TEG_STATUS locate_country_add_army(PCOUNTRY p)
{
	int i;

	for(i=0; i<MAX_LOCATE_COUNTRIES; i++) {
		if(list_locate_armies[i].country == -1) {

			locate_country_init_entry(&list_locate_armies[i]);
			list_locate_armies[i].country = p->id;
			break;
		}

		if(list_locate_armies[i].country == p->id) {
			list_locate_armies[i].number_times_refreshed = 0;
			break;
		}
	}

	if(i==MAX_LOCATE_COUNTRIES) {
		return TEG_STATUS_ERROR;
	}
	return TEG_STATUS_SUCCESS;
}

}
