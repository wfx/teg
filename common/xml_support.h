/*	$Id: xml_support.h,v 1.2 2002/09/09 14:10:46 riq Exp $	*/
/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2002 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <rquesada@core-sdi.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; only version 2 of the License.
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

#ifndef __TEG_XML_SUPPORT_H
#define __TEG_XML_SUPPORT_H

/* I don't know, but I tried with forward declarations and didn't work either */
void* xml_get_element_children( void* );
void* xml_get_element_next( void* );

#endif /* __TEG_XML_SUPPORT_H */
