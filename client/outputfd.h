/*	$Id: outputfd.h,v 1.30 2002/08/31 17:45:57 riq Exp $	*/
/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
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
/**
 * @file outputfd.h
 */
#ifndef __TEGC_OUTPUTFD_H
#define __TEGC_OUTPUTFD_H

#include "client.h"

TEG_STATUS out_mensaje( char *msg );
TEG_STATUS out_id();
TEG_STATUS out_exit();
TEG_STATUS out_countries();
TEG_STATUS out_status();
TEG_STATUS out_start();
TEG_STATUS out_tropas( int src, int dst, int cant);
TEG_STATUS out_endturn();
TEG_STATUS out_tarjeta();
TEG_STATUS out_missions();
TEG_STATUS out_set_typeofgame(int a, int b, int c, int ar1, int ar2);
TEG_STATUS out_get_typeofgame();
TEG_STATUS out_color();
TEG_STATUS out_loque();
TEG_STATUS out_echo(char *msg);
TEG_STATUS out_pversion();
TEG_STATUS out_surrender();

/**! request the high scores from the server */
TEG_STATUS out_scores();

/**! sends the clients version */
TEG_STATUS out_cversion();

/**! sends the request to enumerate all the cards i have */
TEG_STATUS out_enum_cards();

/**! tell the server to launch a robot there */
TEG_STATUS out_robot();

/**! request who started the round, and the round number */
TEG_STATUS out_new_round();

#endif /* __TEGC_OUTPUTFD_H */
