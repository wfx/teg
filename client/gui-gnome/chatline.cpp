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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../common/net.h"
#include "client.h"
#include "interface.h"
#include "chatline.h"
#include "colors.h"
#include "priv.h"
#include "support.h"


static void update_scrollbar(GtkTextBuffer *buf)
{
	GtkTextMark *mark;
	GtkTextIter i;

	/* have to use a mark, or this won't work properly */
	gtk_text_buffer_get_end_iter(buf, &i);
	mark = gtk_text_buffer_create_mark(buf, NULL, &i, FALSE);
	gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(main_message_area), mark);
	gtk_text_buffer_delete_mark(buf, mark);
}

void inputline_return(GtkEntry *w, gpointer data)
{
	const char *theinput;

	if(ESTADO_ES(PLAYER_STATUS_DESCONECTADO)) {
		gtk_entry_set_text(w, "");
		textmsg(M_INF, _("You need to be connected"));
		return;
	}

	theinput = gtk_entry_get_text(w);

	if(*theinput) {
		if(theinput[0]=='/') {
			net_printf(g_game.fd, "%s\n", &theinput[1]);
		} else	{
			char *d = g_strdup(theinput);
			out_mensaje(d);
			g_free(d);
		}
	}
	gtk_entry_set_text(w, "");
}

/* funcion exportada a cliente */
TEG_STATUS gui_textmsg(char *astring)
{
	gchar *ptr;
	GtkTextIter iter;
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(main_message_area));

	ptr = translate_to_utf8(astring);

	gtk_text_buffer_get_end_iter(buffer, &iter);
	gtk_text_buffer_insert(buffer, &iter, ptr, -1);
	gtk_text_buffer_insert(buffer, &iter, "\n", -1);

	free(ptr);
	update_scrollbar(buffer);

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_textplayermsg(char const *n, int num, char const *msg)
{
	GtkTextIter iter;
	char name[PLAYERNAME_MAX_LEN+30];
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(main_message_area));
	gchar *ptr_name, *ptr_msg;

	snprintf(name, sizeof(name)-1, "<%s>", n);
	name[sizeof(name)-1] = 0;

	gtk_text_buffer_get_end_iter(buffer, &iter);

	ptr_name = translate_to_utf8(name);
	ptr_msg = translate_to_utf8(msg);

	if(gui_private.msg_show_colors) {
		gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, ptr_name, -1, get_tag_for_color(num), NULL);
	} else {
		gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, " ", -1, get_tag_for_color(num), NULL);
		gtk_text_buffer_insert(buffer, &iter, ptr_name, -1);
	}
	gtk_text_buffer_insert(buffer, &iter, " ", -1);
	gtk_text_buffer_insert(buffer, &iter, ptr_msg, -1);
	gtk_text_buffer_insert(buffer, &iter, "\n", -1);


	free(ptr_name);
	free(ptr_msg);


	update_scrollbar(buffer);

	return TEG_STATUS_SUCCESS;
}

void output_window_clear(void)
{
	GtkTextBuffer *buf;

	buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(main_message_area));
	gtk_text_buffer_set_text(buf, _("Cleared output window"), -1);
}

void set_output_window_text(const char *text)
{
	GtkTextBuffer *buf;

	buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(main_message_area));
	gtk_text_buffer_set_text(buf, text, -1);
}

TEG_STATUS chatline_init()
{
	GtkTextBuffer *buffer;
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(main_message_area));

	gtk_text_buffer_create_tag(buffer, "red_tag", "background", "red", "foreground", "white", NULL);
	gtk_text_buffer_create_tag(buffer, "yellow_tag", "background", "yellow", "foreground", "black", NULL);
	gtk_text_buffer_create_tag(buffer, "blue_tag", "background", "blue", "foreground", "white",  NULL);
	gtk_text_buffer_create_tag(buffer, "black_tag", "background", "grey27", "foreground", "white",  NULL);
	gtk_text_buffer_create_tag(buffer, "pink_tag", "background", "PaleVioletRed1", "foreground", "black",  NULL);
	gtk_text_buffer_create_tag(buffer, "green_tag", "background", "green", "foreground", "black",  NULL);
	gtk_text_buffer_create_tag(buffer, "default_tag", "background", "grey", "foreground", "black",  NULL);

	return TEG_STATUS_SUCCESS;
}
