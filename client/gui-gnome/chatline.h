/**********************************************************************
 Freeciv - Copyright (C) 1996 - A Kjeldberg, L Gregersen, P Unold
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/

#pragma once

#include <gtk/gtk.h>

#include "../../common/common.h"

namespace teg::client::callbacks
{

void inputline_return(GtkEntry *w, gpointer data);
void output_window_clear(void);
void set_output_window_text(const char *text);
TEG_STATUS gui_textmsg(char *astring);
TEG_STATUS chatline_init();

}
