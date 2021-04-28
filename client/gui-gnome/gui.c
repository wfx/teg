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
 * gui-gnome functions
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include "gui.h"
#include "../client.h"

#include "priv.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "chatline.h"
#include "g_country.h"
#include "stock.h"
#include "cards.h"
#include "dices.h"
#include "status.h"
#include "mission.h"
#include "connect.h"
#include "armies.h"
#include "colors.h"
#include "../themes.h"
#include "g_scores.h"
#include "locate_country.h"


TTheme	gui_theme;

static TEG_STATUS get_default_values(void);

/* shows your secret mission */
TEG_STATUS gui_mission()
{
	mission_view();
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_habilitado(int numjug)
{
	if(numjug == WHOAMI()) {
		set_sensitive_tb();
		ministatus_update();
	}

	mainstatus_update();

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_winner(int numjug, int mission)
{
	armies_unview();

	teg_dialog_gameover(numjug, mission);

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_lost(int numjug)
{
	char buf[400];
	PCPLAYER pJ;

	memset(buf, 0, sizeof(buf));
	armies_unview();

	if(player_whois(numjug, &pJ) != TEG_STATUS_SUCCESS) {
		/* */

	} else if(numjug == WHOAMI()) {
		snprintf(buf, sizeof(buf)-1, _("You lost the game :("));
		teg_dialog(_("You lost"), _("Game Over"), buf);

	} else {
		snprintf(buf, sizeof(buf)-1, _("Player %s(%s) lost the game"), pJ->name, _(g_colores[pJ->color]));
		teg_dialog(_("A player lost the game"), _("Player lost the game"), buf);
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_surrender(int numjug)
{
	char buf[400];
	PCPLAYER pJ;

	memset(buf, 0, sizeof(buf));
	armies_unview();

	if(player_whois(numjug, &pJ) != TEG_STATUS_SUCCESS) {
		/* */

	} else if(numjug == WHOAMI()) {
		/* */

	} else {
		snprintf(buf, sizeof(buf)-1, _("Coward %s(%s) has surrendered"), pJ->name, _(g_colores[pJ->color]));
		teg_dialog(_("A player has surrendered"), _("A player has surrendered"), buf);
	}
	return TEG_STATUS_SUCCESS;
}


static gchar *msg = NULL;

/* Migrate old GConf settings to GSettings.  As it relies on gconf2
   being available, that is almost certain to fail for users upgrading
   from stretch to buster.  See
   https://alioth-lists.debian.net/pipermail/pkg-gnome-maintainers/2018-August/145477.html */
static void migrate_gconf_settings(const gchar *name)
{
	gboolean needed = TRUE;
	GError *error = NULL;
	GKeyFile *kf;
	gchar **list;
	gsize i, n;

	kf = g_key_file_new();

	g_key_file_load_from_data_dirs(kf, "gsettings-data-convert",
	                               NULL, G_KEY_FILE_NONE, NULL);
	list = g_key_file_get_string_list(kf, "State", "converted", &n, NULL);

	if(list) {
		for(i = 0; i < n; i++)
			if(!g_strcmp0(list[i], name)) {
				needed = FALSE;
				break;
			}

		g_strfreev(list);
	}

	g_key_file_free(kf);

	if(needed) {
		g_spawn_command_line_sync("gsettings-data-convert",
		                          NULL, NULL, NULL, &error);

		if(error) {
			msg = g_strdup_printf(_("Warning: Could not migrate old GConf "
			                        "settings: %s\nPlease make sure that GConf "
			                        "is installed and the "
			                        "gsettings-data-convert tool is in your "
			                        "PATH. Alternatively, ignore this message "
			                        "and convert your old settings manually."),
			                      error->message);
			g_error_free(error);
		} else {
			msg = g_strdup(_("Old GConf settings were either missing or "
			                 "migrated successfully."));

			/* Allow some time for the GSettings backend to record the
			   changes, otherwise get_default_values is called before
			   the old settings are in effect.  */
			sleep(1);
		}
	}
}

TEG_STATUS gui_init(int argc, char **argv)
{
#ifdef ENABLE_NLS
	/*
	 * I'm not sure if I must call setlocale() or gtk_set_locale() or nothing, since the gnome-hello
	 * example does not use them
	 */
	/* setlocale(LC_ALL,""); */
	bindtextdomain(PACKAGE, GNOMELOCALEDIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(PACKAGE);
#endif

	gtk_init(&argc, &argv);

	settings = g_settings_new("net.sf.teg");

	stock_init();

	migrate_gconf_settings("teg.convert");
	get_default_values();

	theme_load(g_game.theme);

	if(theme_giveme_theme(&gui_theme) != TEG_STATUS_SUCCESS) {
		fprintf(stderr, "Error loading theme!\n");
		return TEG_STATUS_ERROR;
	}

	main_window = create_mainwin();
	if(!main_window) {
		return TEG_STATUS_ERROR;
	}

	if(colors_allocate() != TEG_STATUS_SUCCESS) {
		return TEG_STATUS_ERROR;
	}

	priv_init();

	locate_country_init();

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_main(void)
{

	/* show welcome message */
	textmsg(M_ALL, _("Tenes Empanadas Graciela - Gnome client v%s - by Ricardo Quesada"), VERSION);
	textmsg(M_ALL, _("Using theme '%s - v%s' by %s\n"), g_game.theme, gui_theme.version, gui_theme.author);

	if(msg) {
		textmsg(M_ALL, msg);
		g_free(msg);
	}
	/* put the buttons in 'sensitive'*/
	set_sensitive_tb();

	/* shows connection window */
	connect_view();

	gtk_main();
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_disconnect(void)
{
	set_sensitive_tb();

	dices_unview();
	armies_unview();
	ministatus_update();
	mainstatus_update();

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_connected(char *c)
{
	set_sensitive_tb();
	if(!g_game.observer) {
		colortype_view(c);
	}

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_reconnected()
{
	set_sensitive_tb();
	ministatus_update();
	return TEG_STATUS_SUCCESS;
}

static void get_settings_into(char* dest, size_t dest_len,
                              char const* name, char const* default_)
{
	gchar *string = g_settings_get_string(settings, name);
	char const * source = g_ascii_strcasecmp(string, "") != 0
	                      ? string
	                      : default_;
	string_copy(dest, dest_len, source);
	g_free(string);
}

static TEG_STATUS get_default_values(void)
{
	get_settings_into(g_game.myname, sizeof(g_game.myname),
	                  "playername", getenv("LOGNAME"));

	g_game.mycolor = g_settings_get_int(settings, "color");

	get_settings_into(g_game.sername, sizeof(g_game.sername),
	                  "servername", "localhost");

	g_game.msg_show = g_settings_get_int(settings, "msgshow");
	gui_private.msg_show_colors
	    = g_settings_get_boolean(settings, "msgshow-with-color");
	gui_private.dialog_show = g_settings_get_int(settings, "dialog-show");
	get_settings_into(g_game.theme, sizeof(g_game.theme),
	                  "theme", "m2");

	g_game.robot_in_server = g_settings_get_boolean(settings,
	                         "robot-in-server");

	gui_private.status_show = g_settings_get_int(settings, "status-show");
	g_game.serport = g_settings_get_int(settings, "port");

	g_settings_set_string(settings, "playername", g_game.myname);

	return TEG_STATUS_SUCCESS;
}

/* the game is started */
TEG_STATUS gui_start(void)
{
	cards_flush();
	set_sensitive_tb();
	mainstatus_update_colors();

	if(status_dialog!= NULL) {
		out_status();
	}
	return TEG_STATUS_SUCCESS;
}

/* update the Players Status window */
TEG_STATUS gui_status()
{
	mainstatus_update_colors();
	return status_update_dialog();
}

/* update the scores */
TEG_STATUS gui_scores()
{
	gui_scores_view();
	return TEG_STATUS_SUCCESS;
}

/* tells the number of armies to place */
TEG_STATUS gui_fichas(int cant, int conts)
{
	char buf[500];
	PLAYER_STATUS e;

	set_sensitive_tb();

	if(status_dialog != NULL) {
		out_status();
	}

	e = ESTADO_GET();

	armies_view(cant, conts);
	dices_unview();

	switch(e) {
	case PLAYER_STATUS_FICHAS:
		sprintf(buf, _("1st part:\nPlace %d armies in your countries"), cant);
		break;
	case PLAYER_STATUS_FICHAS2:
		sprintf(buf, _("2nd part:\nPlace %d armies in your countries"), cant);
		break;
	case PLAYER_STATUS_FICHASC: {
		int i, p1, p2, p3;
		char buf2[500];
		int hubo_cont=0;

		buf[0]=0;
		for(i=0; i<CONT_CANT; i++) {
			if(conts & 1) {
				snprintf(buf2, sizeof(buf2)-1, _("Place %d armies in %s\n"), g_conts[i].fichas_x_cont, cont_get_name(i));
				buf2[ sizeof(buf2) -1 ] = 0;
				strcat(buf, buf2);
				hubo_cont=1;
			}
			conts >>= 1;
		}
		if(hubo_cont) {
			snprintf(buf2, sizeof(buf2)-1, _("And place another %d armies in your countries\n"), cant);
			buf2[ sizeof(buf2) -1 ] = 0;
			strcat(buf, buf2);
		} else {
			snprintf(buf2, sizeof(buf2)-1, _("Place %d armies in your countries\n"), cant);
			buf2[ sizeof(buf2) -1 ] = 0;
			strcat(buf, buf2);
		}

		if(canje_puedo(&p1, &p2, &p3) == TEG_STATUS_SUCCESS) {
			strcat(buf, _("\nIf you want to EXCHANGE your cards for armies,\nDO IT NOW!"));
			cards_select(p1, p2, p3);
		}
		break;
	}
	default:
		textmsg(M_ERR, "Error in out_fichas()");
		return TEG_STATUS_ERROR;
	}
	if(gui_private.dialog_show & (1 << DIALOG_PLACE_ARMIES)) {
		teg_dialog(_("Place armies"), _("Place armies"), buf);
	}
	textmsg(M_IMP, "%s", buf);

	return TEG_STATUS_SUCCESS;
}

/* someone has the turn */
TEG_STATUS gui_turn(PCPLAYER pJ)
{
	if(pJ->numjug == WHOAMI()) {
		set_sensitive_tb();

		if(status_dialog != NULL) {
			out_status();
		}

		if(gui_private.dialog_show & (1 << DIALOG_ATTACK_TURN)) {
			teg_dialog(_("Its your turn"), _("Its your turn to attack"),
			           _("Select two countries:\n1st: click on the source country\n2nd: click on the destination country"));
		}
	}

	mainstatus_update();

	return TEG_STATUS_SUCCESS;
}

/* shows one country */
TEG_STATUS gui_country(int p)
{
//	if( status_dialog != NULL) out_status();

	if(p < COUNTRIES_CANT) {
		G_country_draw_ejer(p);
	}

	return TEG_STATUS_SUCCESS;
}


/* the number of armies to move to the conquered country */
TEG_STATUS gui_tropas(int src, int dst, int cant)
{
	tropas_window(src, dst, cant);
	return TEG_STATUS_SUCCESS;
}

/* regrouping armies from one country to anohter */
TEG_STATUS gui_reagrupe(int src, int dst, int cant)
{
	reagrupe_window(src, dst, cant);
	return TEG_STATUS_SUCCESS;
}

/* a card was received */
TEG_STATUS gui_tarjeta(int country)
{
	cards_view(country);

	if(tarjeta_es_usada(&g_countries[country].tarjeta)) {
		locate_country_add_army(&g_countries[country]);
	}

	set_sensitive_tb();
	return TEG_STATUS_SUCCESS;
}

/* dices result */
TEG_STATUS gui_dados()
{
	dices_view();
	return TEG_STATUS_SUCCESS;
}

/* update buttons' sensitivity */
TEG_STATUS gui_sensi()
{
	set_sensitive_tb();
	cards_update();
	cards_update_para_canje();
	mainstatus_update();
	return TEG_STATUS_SUCCESS;
}

/* success exchange of cards, and the quantity of armies to place after the exchange */
TEG_STATUS gui_canje(int cant, int p1, int p2, int p3)
{
	char buf[128];
	snprintf(buf, sizeof(buf)-1, _("Now you can add %d extra armies in your countries!"), cant);
	buf[ sizeof(buf) -1 ]=0;
	teg_dialog(_("Extra armies"), _("Extra armies"), buf);
	armies_view_more(cant);
	cards_delete(p1, p2, p3);
	return TEG_STATUS_SUCCESS;
}

/* country 'src' is attacking country 'dst' */
TEG_STATUS gui_attack(int src, int dst)
{
	gui_country_select(src);
	gui_country_select(dst);
	return TEG_STATUS_SUCCESS;
}
