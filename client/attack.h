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
 * attack functions
 */

#ifndef __TEGC_ATAQUE_H
#define __TEGC_ATAQUE_H

TEG_STATUS attack_init();
TEG_STATUS attack_click( PCOUNTRY p );
TEG_STATUS attack_finish( int *ori, int *dst );
TEG_STATUS attack_init();
void attack_restore();
void attack_backup();
void attack_reset();
TEG_STATUS attack_pre_reset();
TEG_STATUS attack_out();
TEG_STATUS attackre_out();
TEG_STATUS attack_enter( PCOUNTRY p );
TEG_STATUS attack_leave( PCOUNTRY p );
TEG_STATUS attack_show( int src, int dst );
TEG_STATUS attack_unshow();

#endif /* __TEGC_ATAQUE_H */
