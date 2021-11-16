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

#include <gtk/gtk.h>

#include "../../common/common.h"

namespace teg::client::callbacks
{

enum {
	DIALOG_ATTACK_TURN,
	DIALOG_REGROUP_ARMIES,
	DIALOG_PLACE_ARMIES
};

struct _gui_private {
	bool msg_show_colors;
	unsigned int status_show;	/* what columns to show in the status window */
	unsigned int dialog_show;	/* what dialogs to show */
	int country_i_am;		/* what country i'm pointing */
};

extern struct _gui_private gui_private;

extern GSettings *settings;

TEG_STATUS priv_init(void);

}
