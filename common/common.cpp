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

#include <type_traits>

#include "common.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include "fcintl.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

char const *g_colores[] = {
	[ARMY_ROJO]     = N_("red"),
	[ARMY_AMARILLO] = N_("yellow"),
	[ARMY_AZUL]     = N_("blue"),
	[ARMY_NEGRO]    = N_("black"),
	[ARMY_ROSA]     = N_("pink"),
	[ARMY_VERDE]    = N_("green"),
};
static_assert(std::extent<decltype(g_colores)>::value == maximum_player_count);

char const *g_estados[] = {
	[PLAYER_STATUS_DESCONECTADO] = N_("disconnected"),
	[PLAYER_STATUS_CONNECTED]    = N_("connected"),
	[PLAYER_STATUS_GAMEOVER]     = N_("game over"),
	[PLAYER_STATUS_HABILITADO]   = N_("enabled"),
	[PLAYER_STATUS_START]        = N_("started"),
	[PLAYER_STATUS_FICHAS]       = N_("placing armies"),
	[PLAYER_STATUS_POSTFICHAS]   = N_("postarmies"),
	[PLAYER_STATUS_FICHAS2]      = N_("placing armies 2"),
	[PLAYER_STATUS_POSTFICHAS2]  = N_("postarmies 2"),
	[PLAYER_STATUS_IDLE]         = N_("idle"),
	[PLAYER_STATUS_FICHASC]      = N_("placing armies 3"),
	[PLAYER_STATUS_CANJE]        = N_("exchanging cards"),
	[PLAYER_STATUS_POSTFICHASC]  = N_("postarmies 3"),
	[PLAYER_STATUS_PACTO]        = N_("making a pact"),
	[PLAYER_STATUS_TURNOSTART]   = N_("starting turn"),
	[PLAYER_STATUS_ATAQUE]       = N_("attacking"),
	[PLAYER_STATUS_TROPAS]       = N_("moving armies"),
	[PLAYER_STATUS_REAGRUPE]     = N_("regrouping"),
	[PLAYER_STATUS_TARJETA]      = N_("getting card"),
	[PLAYER_STATUS_TURNOEND]     = N_("ending turn")
};

/* returns an integer from /dev/random or 0 if it can't */
int get_int_from_dev_random(void)
{
	char const*const RANDOM_DEVICE="/dev/urandom";

	int const fd = open(RANDOM_DEVICE, O_RDONLY);
	if(fd < 0) {
		fprintf(stderr, "Couldn't open '%s'\n", RANDOM_DEVICE);
		return 0;
	}
	int result;
	const int l = read(fd, &result, sizeof(result));
	if(l != sizeof(result)) {
		fprintf(stderr, "Returning a not so random number. Read: %d\n", l);
	}

	close(fd);

	return result;
}

/* given the number of exchange, it says the numer of armies he deserves */
unsigned cards_for_this_exchange(unsigned exchanges)
{
	switch(exchanges) {
	case 0:
		return 0;
	case 1:
		return 4;
	case 2:
		return 7;
	case 3:
		return 10;
	default:
		return (exchanges-1) * 5;
	}
}

void strip_invalid(char *n)
{
	int l = strlen(n);
	int i;

	assert(n);

	for(i=0; i<l; i++) {
		if(n[i]=='=' || n[i]==';' || n[i]=='\\' || n[i]==',' || n[i]==':' || n[i]=='/' || n[i]=='%') {
			n[i] = '.';
		}
	}
}

void strip_invalid_msg(char *n)
{
	int l = strlen(n);
	int i;

	assert(n);

	for(i=0; i<l; i++) {
		if(n[i]=='"') {
			n[i]='\'';
		}
	}
}

int my_atoi(char const *s)
{
	if(! s) {
		return -1;
	}
	return atoi(s);
}

void string_copy(char* dest, size_t destlen, char const* source)
{
	if(destlen == 0) {
		return;
	}
	strncpy(dest, source, destlen-1);
	dest[destlen-1] = 0;
}

std::string replace_continents(std::string const& format,
                               ContinentNames const& country_names)
{
	std::string result;
	std::string::size_type last_start{0};
	std::string::size_type start{0};
	for(start=format.find('&');
	        start < format.size();
	        last_start = start+1, start=format.find('&', last_start)) {
		result += std::string(format, last_start, start-last_start);

		/* jump over the & to the next input character, which should be the
		 * continent number */
		start++;

		if(start < format.size()) {
			auto const code=format[start];
			if((code >= '0') && (code <= '5')) {
				unsigned country_index = code - '0';
				result += country_names[country_index];
			}
		}
	}
	if(last_start < format.size()) {
		result += std::string(format, last_start);
	}
	return result;
}
