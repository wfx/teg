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
/*
 * La mayoria de estas funciones fueron basadas en codigo de:
 *       Unix Network Programming vol1 2nd Ed. (W.Richard Stevens)
 *       Linux Application Development (M.K.Johnson, E.K.Troan)
 *       y del archivo gnome-net (que ya no existe mas)
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <netdb.h>

#include "all.h"


static ssize_t
writen(int fd, const void *vptr, size_t n )
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = vptr;
	nleft = n;
	while( nleft > 0 ) {
		if( ( nwritten = write(fd, ptr, nleft) ) <= 0 ) {
			if( errno == EINTR )
				nwritten = 0;
			else
				return (-1 );
		}
		nleft -= nwritten ;
		ptr += nwritten;
	}
	return(n);
}

ssize_t
net_readline( int fd, void *vptr, size_t maxlen )
{
	ssize_t n, rc;
	char c, *ptr;

	ptr = vptr ;
	for( n =1 ; n < maxlen; n++ ) {
again:
		if( (rc= read(fd, &c, 1 )) ==1 ) {
			*ptr++ = c;
			if( c=='\n' )
				break;
		} else if( rc== 0 ) {
			return 0;
		} else {
			if( errno == EINTR )
				goto again;
			return(-1);
		}
	}

	*ptr = 0;
	return( n );
}

ssize_t
net_read( int fd, void *vptr, size_t maxlen )
{
	return read( fd, vptr, maxlen );
}


int
net_connect_tcp( const char *host, int port )
{
	int sockfd, n;
	struct addrinfo hints, *res, *ressave;
	char serv[30];

	snprintf(serv,sizeof(serv)-1,"%d",(unsigned int) port );
	serv[sizeof(serv)-1]=0;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((n = getaddrinfo( host,serv,&hints,&res)) != 0) {
		fprintf(stderr,"net_connect_tcp error for %s, %s: %s\n", host, serv, gai_strerror(n));
		return -1;
	}

	ressave = res;

	do {
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if( sockfd < 0)
			continue;
		if (connect(sockfd,res->ai_addr,res->ai_addrlen) == 0)
			break;

		close(sockfd);

	} while( (res = res->ai_next)!=NULL);

	if (res == NULL) { 	/* errno set from final connect() */
		perror("net_listen:");
		freeaddrinfo(ressave);
		return -1;
	}

	freeaddrinfo(ressave);

	return(sockfd);
}

int
net_listen(const char *host, int port )
{
	int listenfd, n;
	const int on = 1;
	struct addrinfo	hints, *res, *ressave;
	char serv[30];

	snprintf(serv,sizeof(serv)-1,"%d",(unsigned int) port );
	serv[sizeof(serv)-1]=0;
	
	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0) {
		fprintf(stderr,"net_listen error for %s, %s: %s", host, serv, gai_strerror(n));
		return -1;
	}

	ressave = res;

	do {
		listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (listenfd < 0)
			continue;		/* error, try next one */

		setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
		if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
			break;			/* success */

		close(listenfd);	/* bind error, close and try next one */
	} while ( (res = res->ai_next) != NULL);

	if (res == NULL) {
		perror("net_listen:");
		freeaddrinfo(ressave);
		return -1;
	}

	listen(listenfd, 50);

	freeaddrinfo(ressave);

	return(listenfd);
}



int
net_printf(int sock, char *format, ...)
{
        va_list args;
	char buf[PROT_MAX_LEN];

	va_start(args, format);
	vsnprintf(buf, sizeof(buf), format, args);
	va_end(args);

	return writen(sock, buf, strlen(buf));
}

int
net_print(int sock, char *msg)
{
	return writen(sock, msg, strlen(msg));
}

int
net_connect_unix(char *path )
{
	int sock;
	struct sockaddr_un address;
	size_t addrLenght;

	if((sock= socket(AF_UNIX, SOCK_STREAM,0)) < 0 )
		return -1;

	address.sun_family = AF_UNIX ;
	strncpy(address.sun_path,path,sizeof(address.sun_path)-1);
	address.sun_path[sizeof(address.sun_path)-1]=0;

	addrLenght = sizeof( address.sun_family) + strlen(address.sun_path);

	if( connect(sock, (struct sockaddr *) &address, addrLenght ) <0) 
		return -1;

        return sock;
}

int
net_close( int fd )
{
	if( fd >= 0 )
		return close( fd );
	return -1;
}
