/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2002 Ricardo Quesada
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
/* Fog of War */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "../common/net.h"
#include "server.h"
#include "fow.h"

namespace teg::server
{

TEG_STATUS fow_set_mode(bool b)
{
	if(JUEGO_EMPEZADO) {
		return TEG_STATUS_ERROR;
	}

	if(b!=0) {
		b=1;
	}
	g_game.fog_of_war = b;
	return TEG_STATUS_SUCCESS;
}

bool fow_can_player_see_country(PSPLAYER pJ, PCOUNTRY pP)
{
	if(! pJ  || ! pP) {
		return false;
	}

	bool result{false};
	countries_map_int(pJ->numjug, [pP, &result](COUNTRY& country) {
		if(pP->id == country.id) {
			result = true;
			return false;
		}

		if(countries_eslimitrofe(pP->id, country.id)) {
			result = true;
			return false;
		}
		return true;
	});

	return result;
}

TEG_STATUS fow_fill_with_boundaries(int country, char *buffer, int buf_len)
{
	int i;
	int first_time=1;

	if(! g_game.player_fow) {
		return TEG_STATUS_ERROR;
	}

	for(i=0; i < COUNTRIES_CANT; i++) {
		if(i != country && g_countries[i].numjug != g_game.player_fow->numjug && countries_eslimitrofe(country, i)) {
			char buf[1024];

			if(first_time) {
				snprintf(buf, sizeof(buf)-1, "%s=%d,%d,%d", TOKEN_COUNTRY, i, g_countries[i].numjug, g_countries[i].ejercitos);
				first_time = 0;
			} else {
				snprintf(buf, sizeof(buf)-1, ";%s=%d,%d,%d", TOKEN_COUNTRY, i, g_countries[i].numjug, g_countries[i].ejercitos);
			}

			strncat(buffer, buf, buf_len);
		}
	}


	/* dont send, since I didnt modify the buffer */
	if(first_time) {
		return TEG_STATUS_ERROR;
	}

	return TEG_STATUS_SUCCESS;
}

int fow_netall_printf(int country, char const *format, ...)
{
	va_list args;
	char buf[PROT_MAX_LEN];

	if(country < 0 || country >= COUNTRIES_CANT) {
		return -1;
	}

	va_start(args, format);
	vsnprintf(buf, sizeof(buf) -1, format, args);
	va_end(args);

	buf[ sizeof(buf) -1 ] = 0;

	player_map([buf, country](PSPLAYER pJ) {
		if(pJ->fd > 0) {
			/* check if he has a boundry country wih 'country' */
			countries_map_int(
			pJ->numjug, [country, &buf, pJ](COUNTRY& country_to_inspect) -> bool {
				CountryId const cid{country_to_inspect.id};
				if((cid == country) || countries_eslimitrofe(cid, country))
				{
					net_print(pJ->fd, buf);
					return false;
				}
				return true;
			});
		}
	});
	return 0;
}

/*
 * IMPORTANT: it is very important the user can't control the format
 * in this function to prevent a format string vulnerability
 *
 * After format is parsed, 2 "%d" should be present, because they will
 * be filled with src & dst
 */
int fow_2_netall_printf(int src, int dst, char const *format, ...)
{
	va_list args;
	char buf[PROT_MAX_LEN];

	if(src<0 || src>=COUNTRIES_CANT || dst<0 || dst>=COUNTRIES_CANT) {
		return -1;
	}

	va_start(args, format);
	vsnprintf(buf, sizeof(buf) -1, format, args);
	va_end(args);

	buf[ sizeof(buf) -1 ] = 0;

	player_map([buf, src, dst](PSPLAYER pJ) {
		if(pJ->fd > 0) {
			/* check if he has a boundry country wih src && dst */

			int src_country = -1;
			int dst_country = -1;
			countries_map_int(pJ->numjug,
			[src, dst, &src_country, &dst_country](COUNTRY& country) {

				if(country.id == src || countries_eslimitrofe(country.id, src)) {
					src_country = src;
				}

				if(country.id == dst || countries_eslimitrofe(country.id, dst)) {
					dst_country = dst;
				}

				return (dst_country == -1) && (src_country == -1);
			});

			net_printf(pJ->fd, buf, src_country, dst_country);
		}
	});
	return 0;
}

}
