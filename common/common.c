/*	$Id: common.c,v 1.30 2007/07/07 19:16:19 nordi Exp $	*/
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
/**
 * @file common.c
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include "all.h"

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


/* XXX sync with common.h */
char *g_reglas[] = {
	N_("TEG"),
	N_("Risk"),
	N_("1914"),
	N_("other"),
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
	N_("gettting card"),
	N_("ending turn")
};

/* returns an integer from /dev/random or 0 if it can't */
int get_int_from_dev_random( void )
{
	int fd;
	char buf[sizeof(int)];
	int l;
	int *ret;

	fd = open( RANDOM_DEVICE, O_RDONLY);
	if( fd < 0) {
		fprintf(stderr,"Couldn't open '%s'\n", RANDOM_DEVICE);
		return 0;
	}
	l = read( fd, buf, sizeof(buf));
	if( l != sizeof(buf) )
		fprintf(stderr,"Returning a not so random number. Read: %d\n",l);

	ret = (int *)&buf;

	close(fd);

	return *ret;
}

/* When exchanging cards for armies, given the number of exchange
   it says the numer of armies he deserves */
int cuantos_x_canje( int c )
{
	if( c < 1 )
		return 0;
	switch( c ) {
		case 0:
			return 0;
		case 1:
			return 4;
		case 2:
			return 7;
		case 3:
			return 10;
		default:
			return (c-1) * 5;
	}

}

/* Removes invalid characters =;\,:/%. from string n and replaces them with "." */
TEG_STATUS strip_invalid( char *n )
{
	int l = strlen(n);
	int i;

	assert(n);

	for(i=0;i<l;i++) {
		if( n[i]=='=' || n[i]==';' || n[i]=='\\' || n[i]==',' || n[i]==':' || n[i]=='/' || n[i]=='%')
			n[i] = '.';
	}

	return TEG_STATUS_SUCCESS;
}

/* Removes invalid character " from string n and replaces it with ' */
TEG_STATUS strip_invalid_msg( char *n )
{
	int l = strlen(n);
	int i;

	assert(n);

	for(i=0;i<l;i++) {
		if( n[i]=='"' )
			n[i]='\'';
	}

	return TEG_STATUS_SUCCESS;
}

int my_atoi( char *s )
{
	if( ! s )
		return -1;
	return atoi( s );
}
