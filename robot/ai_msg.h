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
 * @file ai_msg.h
 * Mensaje que puede decir el robot ;)
 */

#ifndef __TEGR_AI_MSG_H
#define __TEGR_AI_MSG_H

typedef struct _aimsg {
	char *msg;
	int args;
} aimsg_t, *paimsg_t;

enum {
	AI_MSG_HI,
	AI_MSG_BYE,
	AI_MSG_MISC,
	AI_MSG_ANSWER,

	AI_MSG_LAST,
};

TEG_STATUS ai_msg( int type, char *n );
char * ai_name();
TEG_STATUS ai_findname( char *name );

#endif /* __TEGR_AI_MSG_H */
