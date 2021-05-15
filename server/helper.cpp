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
 * helper functions used by play*
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "../common/parser.h"
#include "../common/tegdebug.h"
#include "../common/fcintl.h"
#include "../common/execute.hpp"

#include "server.h"
#include "fow.h"

using namespace teg;

static char colors[TEG_MAX_PLAYERS];

#ifdef MACOSX
#define socklen_t int
#endif

/* initialize the colors */
void color_init()
{
	memset(colors, 0, sizeof(colors));
}

/* frees on color */
void color_del(int i)
{
	colors[i]=0;
}

/* sets and set if the color is free */
BOOLEAN color_libre(int *color)
{
	int i;
	if((*color) < 0 || (*color) > (TEG_MAX_PLAYERS-1)) {
		return FALSE;
	}
	if(colors[*color]==0) {
		colors[*color]=1;
		return TRUE;
	}
	/* tries to find a free color */
	for(i=0; i<TEG_MAX_PLAYERS; i++) {
		if(colors[i]==0) {
			colors[i]=1;
			*color = i;
			return TRUE;
		}
	}
	return FALSE;
}

/* tells which colors are availables */
TEG_STATUS colores_libres(char *c)
{
	int i;
	for(i=0; i<TEG_MAX_PLAYERS; i++) {
		c[i] = colors[i];
	}
	return TEG_STATUS_SUCCESS;
}

/* order-insert a char in a array */
void ins_orden(char d, char *array, int len)
{
	int i, j;

	for(i=0; i < len ; i++) {
		if(d > array[i]) {
			break;
		}
	}

	if(i==len) {
		return;
	}

	if(array[i] == 0) {
		array[i] = d;
		return;
	}
	j=i;
	for(i=len-1; i>j; i--) {
		array[i] = array[i-1];
	}
	array[j]=d;
}

/* Validates the total number of armies placed */
TEG_STATUS aux_token_fichas(int fd, char *str, int maximo, unsigned long conts)
{
	PARSER p;
	DELIM igualador= { ':', ':', ':' };
	DELIM separador= { ',', ',', ',' };
	PSPLAYER j;
	int fichas, country, cant, real;
	char *copia;
	int cptr[CONT_CANT];			/**< continent ptr */

	if(player_whoisfd(fd, &j) != TEG_STATUS_SUCCESS) {
		goto error;
	}

	if(g_game.turno != j) {
		goto error;
	}

	if(strlen(str)==0) {
		goto error;
	}

	copia = str;

	p.equals = &igualador;
	p.separators = &separador;
	p.data = str;

	memset(cptr, 0, sizeof(cptr));
	fichas=0;
	real=0;

do_real:
	do {
		if(parser_parse(&p)) {

			country = atoi(p.token);
			cant = atoi(p.value);

			if(country<0 || country >= COUNTRIES_CANT || cant<0 ||
			        cant+fichas > maximo || g_countries[country].numjug != j->numjug) {
				goto error;
			}

			/* cuando real==1 se asignas los ejercitos, sino hace el test */
			if(real) {
				g_countries[country].ejercitos += cant;
				j->tot_armies += cant;
			}

			if(conts) {
				cptr[ g_countries[country].continente ] += cant;
			}

			fichas += cant;
		} else {
			goto error;
		}

	} while(p.can_continue);

	if(fichas != maximo) {
		goto error;
	}

	/* Did I have to place armies in continents ? */
	if(conts) {
		unsigned long conts_tmp = conts;
		int i;

		for(i=0; i<CONT_CANT; i++) {
			if(conts_tmp & 1) {
				if(cptr[i] < g_conts[i].fichas_x_cont) {
					goto error;
				}
			}
			conts_tmp >>= 1;
		}
	}

	if(real==0) {
		fichas=0;
		real=1;
		memset(cptr, 0, sizeof(cptr));
		p.data = copia;
		goto do_real; /// \todo Remove this
	}

	return TEG_STATUS_SUCCESS;
error:
	return TEG_STATUS_PARSEERROR;
}


/* simulates the attack between 2 countries telling the result of the dices */
TEG_STATUS aux_token_attack(int src, int dst, int *src_lost, int *dst_lost, char *dados_src, char *dados_dst)
{
	int i, tmp;
	char src_d[4];
	char dst_d[4];

	memset(src_d, 0, sizeof(src_d));
	memset(dst_d, 0, sizeof(dst_d));

	(*dst_lost) = 0;
	(*src_lost) = 0;

	if(dst > 3) {
		dst =3 ;
	}
	src--;
	if(src > 3) {
		src = 3;
	}

	for(i=0; i<src; i++) {
		tmp = RANDOM_MAX(1, 6);
		ins_orden((char) tmp, src_d, 3);
	}

	for(i=0; i<dst; i++) {
		tmp = RANDOM_MAX(1, 6);
		ins_orden((char) tmp, dst_d, 3);
	}

	for(i=0; i< 3 && src_d[i]!=0 && dst_d[i]!=0; i++) {
		if(src_d[i] > dst_d[i]) {
			(*dst_lost)++;
		} else {
			(*src_lost)++;
		}
	}
	memcpy(dados_src, src_d, 3);
	memcpy(dados_dst, dst_d, 3);

	return TEG_STATUS_SUCCESS;
}

/* generates the status in a given array */
TEG_STATUS aux_token_stasta(char *strout, size_t len)
{
	int n;
	char strtmp[ PLAYERNAME_MAX_LEN + 200];

	PLIST_ENTRY l = g_list_player.Flink;
	PSPLAYER j;

	strout[0]=0;

	n=0;
	while(!IsListEmpty(&g_list_player) && (l != &g_list_player)) {
		j = (PSPLAYER) l;

		if(j->is_player) {
			int color = (j->color==-1) ? TEG_MAX_PLAYERS : j->color;
			if(n==0) {
				snprintf(strtmp, sizeof(strtmp)-1, "%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s"
				         , j->name
				         , color
				         , j->player_stats.score
				         , j->numjug
				         , j->estado
				         , (! g_game.fog_of_war) ? j->tot_countries : -1
				         , (! g_game.fog_of_war) ? j->tot_armies : -1
				         , j->tot_cards
				         , (g_game.empieza_turno && (g_game.empieza_turno->numjug==j->numjug))
				         , j->human
				         , j->addr);
				n=1;
			} else
				snprintf(strtmp, sizeof(strtmp)-1, "/%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s"
				         , j->name
				         , color
				         , j->player_stats.score
				         , j->numjug
				         , j->estado
				         , (! g_game.fog_of_war) ? j->tot_countries : -1
				         , (! g_game.fog_of_war) ? j->tot_armies : -1
				         , j->tot_cards
				         , (g_game.empieza_turno && (g_game.empieza_turno->numjug==j->numjug))
				         , j->human
				         , j->addr);

			strtmp[ sizeof(strtmp) -1 ] = 0;

			strncat(strout, strtmp, len - strlen(strout));
		}

		l = l->Flink;
	}
	return TEG_STATUS_SUCCESS;
}


/* find by random a country */
PCOUNTRY get_random_country(get_random_func func)
{
	int i;

again:
	i = RANDOM_MAX(0, COUNTRIES_CANT-1);
	if(func(i)) {
		return &g_countries[i];
	} else {
		int r = RANDOM_MAX(0, 2);
		switch(r) {
		/* search going down */
		case 0:
			for(; i>=0; i--) {
				if(func(i)) {
					return &g_countries[i];
				}
			}
			for(i=COUNTRIES_CANT-1; i>=0; i--) {
				if(func(i)) {
					return &g_countries[i];
				}
			}
			PDEBUG("uncountryelibre(down): Abnormal error\n");
			break;
		/* search going up */
		case 1:
			for(; i<COUNTRIES_CANT; i++) {
				if(func(i)) {
					return &g_countries[i];
				}
			}
			for(i=0; i<COUNTRIES_CANT; i++) {
				if(func(i)) {
					return &g_countries[i];
				}
			}
			PDEBUG("uncountrylibre(up):Abnormal error\n");
			break;
		/* search again */
		case 2:
		default:
			goto again;
		}
	}

	/* to prevent warnings at compile time */
	return &g_countries[0];
}

/* Sends the player the total number of armies it must place, and the continents he has conquered */
void aux_token_fichasc(PSPLAYER pJ)
{
	unsigned long conts=0;
	int armies;
	int x_canje=0;

	assert(pJ);

	if(pJ->fichasc_armies) {
		conts = pJ->fichasc_conts;
		armies = pJ->fichasc_armies;
	} else {
		player_listar_conts(pJ, &conts);
		armies = player_fichasc_cant(pJ);

		pJ->fichasc_conts = conts;
		pJ->fichasc_armies = armies;
	}

	if(pJ->hizo_canje) {
		x_canje = cards_for_this_exchange(pJ->tot_exchanges);
	}

	pJ->estado = PLAYER_STATUS_FICHASC;
	netall_printf(TOKEN_FICHASC"=%d,%d,%d\n", pJ->numjug, conts, armies + x_canje);
}


/* give the 'token fichas' to the next player */
TEG_STATUS fichas_next(void)
{
	turno_2nextplayer(&g_game.turno);
	if(turno_is_round_complete()) {
		/* ya di la vuelta */
		g_game.turno->estado = PLAYER_STATUS_FICHAS2;
		netall_printf(TOKEN_FICHAS2"=%d,%d\n", g_game.turno->numjug, g_game.fichas2);

	} else {
		/* paso el estado fichas al siguiente */
		g_game.turno->estado = PLAYER_STATUS_FICHAS;
		netall_printf(TOKEN_FICHAS"=%d,%d\n", g_game.turno->numjug, g_game.fichas);
	}
	return TEG_STATUS_SUCCESS;
}

/* give the 'token fichas 2' to the next player */
TEG_STATUS fichas2_next(void)
{
	turno_2nextplayer(&g_game.turno);
	if(turno_is_round_complete()) {
		/* ya di la vuelta, enconces a empezar a jugar  */
		g_game.turno->estado = PLAYER_STATUS_ATAQUE;
		netall_printf(TOKEN_TURNO"=%d\n", g_game.turno->numjug);

	} else {
		/* paso el estado fichas2 al siguiente */
		g_game.turno->estado = PLAYER_STATUS_FICHAS2;
		netall_printf(TOKEN_FICHAS2"=%d,%d\n", g_game.turno->numjug, g_game.fichas2);
	}
	return TEG_STATUS_SUCCESS;
}

/* give the 'token fichas c' to the next player */
TEG_STATUS fichasc_next(void)
{
	turno_2nextplayer(&g_game.turno);

	if(turno_is_round_complete()) {
		/* ya di la vuelta */
		g_game.turno->estado = PLAYER_STATUS_ATAQUE;
		netall_printf(TOKEN_TURNO"=%d\n", g_game.turno->numjug);

	} else {
		aux_token_fichasc(g_game.turno);
	}
	return TEG_STATUS_SUCCESS;
}

/* find the internet address of a player */
TEG_STATUS aux_find_inaddr(PSPLAYER pJ)
{
	union {
		sockaddr addr;
		char buf[128+sizeof(sockaddr)];
	} addr;

	assert(pJ);

	strncpy(pJ->addr, _("Unknown"), sizeof(pJ->addr)-1);

	if(pJ->fd <= 0) {
		return TEG_STATUS_ERROR;
	}

	socklen_t length=sizeof(addr);
	if(getpeername(pJ->fd, &addr.addr, &length) == -1) {
		con_text_out(M_ERR, "Error in getpeername()\n");
		pJ->addr[sizeof(pJ->addr)-1]=0;

		return TEG_STATUS_ERROR;
	}

	switch(addr.addr.sa_family) {
	case AF_INET: {
		sockaddr_in *sin = (struct sockaddr_in*) &addr.addr;
		inet_ntop(AF_INET, &sin->sin_addr, pJ->addr, sizeof(pJ->addr)-1);
		break;
	}
	case AF_INET6: {
		sockaddr_in6 *sin6 = (struct sockaddr_in6*) &addr.addr;
		inet_ntop(AF_INET6, &sin6->sin6_addr, pJ->addr, sizeof(pJ->addr)-1);
		break;
	}
	default:
		break;
	}

	pJ->addr[sizeof(pJ->addr)-1]=0;

	strip_invalid(pJ->addr);

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS aux_token_countries(PSPLAYER pJ, char *buf, int buflen)
{
	PLIST_ENTRY pL;
	PCOUNTRY country;
	int i, n=0;
	char strtmp[2048];

	assert(buf);

	memset(buf, 0, buflen);

	/* special case where I want to send all countries that he CAN'T see */
	if(g_game.fog_of_war && pJ->numjug == -1) {
		for(i=0; i<COUNTRIES_CANT; i++) {
			if(! fow_can_player_see_country(g_game.player_fow, &g_countries[i])) {
				if(n==0) {
					snprintf(strtmp, sizeof(strtmp)-1, "%i:%d", i, -1);
					n=1;
				} else {
					snprintf(strtmp, sizeof(strtmp)-1, ",%i:%d", i, -1);
				}

				strtmp[ sizeof(strtmp) -1 ] = 0;

				strncat(buf, strtmp, buflen);
			}
		}
		return TEG_STATUS_SUCCESS;
	}

	/* the other case */
	pL = pJ->countries.Flink;
	while(!IsListEmpty(&pJ->countries) && (pL != &pJ->countries)) {
		country = (PCOUNTRY) pL;

		if(
		    (! g_game.fog_of_war) ||
		    (g_game.fog_of_war && fow_can_player_see_country(g_game.player_fow, country))) {

			if(n==0) {
				snprintf(strtmp, sizeof(strtmp)-1, "%i:%d", country->id, country->ejercitos);
				n=1;
			} else {
				snprintf(strtmp, sizeof(strtmp)-1, ",%i:%d", country->id, country->ejercitos);
			}

			strtmp[ sizeof(strtmp) -1 ] = 0;

			strncat(buf, strtmp, buflen);
		}

		pL = LIST_NEXT(pL);
	}
	buf[buflen]=0;

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS launch_robot(int *robot_socket, char const *mode)
{
	pid_t pid;
	char const *args[5];

	int sockets[2];

	// sockets[0] is the client side of the robot
	// sockets[1] is the server side
	sockets[0] = 0;
	while(sockets[0] < 3) {
		int r = socketpair(AF_LOCAL, SOCK_STREAM, 0, sockets);
		if(r != 0) {
			return TEG_STATUS_ERROR;
		}
	}

	// launch the server connected
	if((pid = fork()) < 0) {
		perror("tegserver: launch_bot() ");
		return TEG_STATUS_ERROR;
	} else if(pid == 0) {

		close(sockets[1]);
		args[0] = BINDIR"/tegrobot";
		args[1] = mode;
		args[2] = NULL;

		if(dup2(sockets[0], 3) == -1) {
			return TEG_STATUS_ERROR;
		}

		con_text_out(M_ERR, _("Launching robot with options: %s %s\n"), args[0], args[1]);
		if(!execute_program(args)) {
			fprintf(stderr, "Launching robot failed. Does the file `%s' exists ?", args[0]);
			perror("exe:");
		}
		exit(1);
	} else {

		close(sockets[0]);
	}

	// return the robot fd
	*robot_socket = sockets[1];

	return TEG_STATUS_SUCCESS;
}
