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

char *g_colores[] = {
	N_("red"),
	N_("yellow"),
	N_("blue"),
	N_("black"),
	N_("pink"),
	N_("green"),
	N_("n/a")			/* color is not assigned yet */
};

/* XXX: sync these status with the one in common.h */
char *g_estados[] = {
	N_("disconnected"),
	N_("connected"),
	N_("game over"),
	N_("enabled"),
	N_("started"),
	N_("placing armies"),
	N_("postarmies"),
	N_("placing armies 2"),
	N_("postarmies 2"),
	N_("idle"),
	N_("placing armies 3"),
	N_("exchanging cards"),
	N_("postarmies 3"),
	N_("making a pact"),
	N_("starting turn"),
	N_("attacking"),
	N_("moving armies"),
	N_("regrouping"),
	N_("getting card"),
	N_("ending turn")
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
