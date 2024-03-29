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

#pragma once

#include <libxml/tree.h>

xmlNodePtr xml_get_element_children(xmlNodePtr);
xmlNodePtr xml_get_element_next(xmlNodePtr);

/**
 * @brief Add an attribute with a number to an existing xml node.
 * @param node The xml node to add the attribute to
 * @param name the name of the attribute
 * @param number the number to store
 */
void add_numeric_attribute(xmlNodePtr node, char const* name, int number);
