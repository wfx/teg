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

#pragma once

enum DisconnectReason {
	/// the client decided to terminate the connection (for example with the
	/// disconnect button
	DR_NORMAL_DISCONNECT,

	/// There was an unrecoverable IO error
	DR_IO_ERROR,
};

/**
 * Shut down the connection to the server, reset the GUI and print a message
 * about the disconnect.
 *
 * \param reason Why was the connectio shut down?
 */
void disconnect(enum DisconnectReason reason);

void connect_view();
void gametype_view();
void colortype_view(char *colores);
