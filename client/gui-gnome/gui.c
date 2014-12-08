/*	$Id: gui.c,v 1.127 2007/09/09 21:52:59 nordi Exp $	*/
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
#include <gnome.h>

#include "gui.h"
#include "client.h"

#include "priv.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "chatline.h"
#include "priv.h"
#include "g_country.h"
#include "stock.h"
#include "cards.h"
#include "dices.h"
#include "status.h"
#include "mission.h"
#include "connect.h"
#include "armies.h"
#include "colors.h"
#include "themes.h"
#include "g_scores.h"
#include "locate_country.h"


struct _gui_private gui_private;

TTheme	gui_theme;

static TEG_STATUS get_default_values( void );

static const struct poptOption options[] = {
#ifdef WITH_GGZ
	{"ggz", '\0', POPT_ARG_NONE, &g_game.with_ggz, 0, N_("Enables GGZ mode"), NULL},
#endif /* WITH_GGZ */
	{"observe", '\0', POPT_ARG_NONE, &g_game.observer, 0, N_("Observe the game, dont play it"), NULL},
	{NULL, '\0', POPT_ARG_NONE, 0, 0, NULL, NULL}
};

/* shows your secret mission */
TEG_STATUS gui_mission()
{
	mission_view();
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_habilitado( int numjug )
{
	if( numjug == WHOAMI() ) {
		set_sensitive_tb();
		ministatus_update();
	}

	mainstatus_update();

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_winner( int numjug, int mission )
{
	armies_unview();

	teg_dialog_gameover( numjug, mission );

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_lost(int numjug)
{
	char buf[400];
	PCPLAYER pJ;

	memset(buf,0,sizeof(buf));
	armies_unview();

	if( player_whois( numjug, &pJ) != TEG_STATUS_SUCCESS) {
		/* */

	} else if( numjug == WHOAMI() ) {
		snprintf(buf,sizeof(buf)-1,_("You lost the game :("));
		teg_dialog(_("You lost"),_("Game Over"),buf);

	} else {
		snprintf(buf,sizeof(buf)-1,_("Player %s(%s) lost the game"),pJ->name,_(g_colores[pJ->color]));
		teg_dialog(_("A player lost the game"),_("Player lost the game"),buf);
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_surrender(int numjug)
{
	char buf[400];
	PCPLAYER pJ;

	memset(buf,0,sizeof(buf));
	armies_unview();

	if( player_whois( numjug, &pJ) != TEG_STATUS_SUCCESS) {
		/* */

	} else if( numjug == WHOAMI() ) {
		/* */

	} else {
		snprintf(buf,sizeof(buf)-1,_("Coward %s(%s) has surrendered"),pJ->name,_(g_colores[pJ->color]));
		teg_dialog(_("A player has surrendered"),_("A player has surrendered"),buf);
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_exit( char *str)
{
	locate_country_exit();

	gtk_main_quit();
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_init( int argc, char **argv)
{
	GnomeClient *client;

#ifdef ENABLE_NLS
	/*
	 * I'm not sure if I must call setlocale() or gtk_set_locale() or nothing, since the gnome-hello
	 * example does not use them
	 */
	/* setlocale(LC_ALL,""); */
	bindtextdomain(PACKAGE, GNOMELOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain(PACKAGE);
#endif

	gnome_program_init (PACKAGE, VERSION,
			LIBGNOMEUI_MODULE,
			argc, argv,
			GNOME_PARAM_POPT_TABLE, options,
			GNOME_PARAM_APP_DATADIR, DATADIR, NULL);

	/* Get the default GConfClient */
	g_conf_client = gconf_client_get_default ();

	stock_init();

	client = gnome_master_client();

	get_default_values();

	theme_load(g_game.theme);

	if( theme_giveme_theme(&gui_theme) != TEG_STATUS_SUCCESS ) {
		// Could not load theme from config, try "m2" as default value
		strncpy(g_game.theme, "m2", THEME_MAX_NAME-1);
		g_game.theme[THEME_MAX_NAME-1] = 0;
		theme_load(g_game.theme);
		if( theme_giveme_theme(&gui_theme) != TEG_STATUS_SUCCESS ) {
			// Could not load theme m2. Last chance: Try the sentimental theme.
			strncpy(g_game.theme, "sentimental", THEME_MAX_NAME-1);
			g_game.theme[THEME_MAX_NAME-1] = 0;
			theme_load(g_game.theme);
			if( theme_giveme_theme(&gui_theme) != TEG_STATUS_SUCCESS ) {
				// Could not load anything. Giving up...
				fprintf(stderr,"Error loading theme!\n");
				return TEG_STATUS_ERROR;
			}
		}
	}

	main_window = create_mainwin();
	if( !main_window )
		return TEG_STATUS_ERROR;

	if( colors_allocate() != TEG_STATUS_SUCCESS )
		return TEG_STATUS_ERROR;

	priv_init();

	locate_country_init();

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_main(void)
{

	/* show welcome message */
	textmsg(M_ALL,_("Tenes Empanadas Graciela - Gnome client v%s - by Ricardo Quesada"),VERSION);
	textmsg(M_ALL,_("Using theme '%s - v%s' by %s\n"),g_game.theme, gui_theme.version,gui_theme.author);

	/* put the buttons in 'sensitive'*/
	set_sensitive_tb();

	/* shows connection window */
	connect_view();

#ifdef WITH_GGZ
	if( g_game.with_ggz && g_game.fd < 0)
		return TEG_STATUS_ERROR;
#endif /* WITH_GGZ */

	gtk_main();
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_disconnect(void)
{
	if( gui_private.tag >=0 )
		gdk_input_remove( gui_private.tag );
	if( gui_private.tag_ggz >=0 )
		gdk_input_remove( gui_private.tag_ggz );

	gui_private.tag=-1;
	gui_private.tag_ggz=-1;
	set_sensitive_tb();

	dices_unview();
	armies_unview();

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_connected( char *c)
{
	set_sensitive_tb();
	if( !g_game.observer)
		colortype_view( c );

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS gui_reconnected()
{
	set_sensitive_tb();
	ministatus_update();
	return TEG_STATUS_SUCCESS;
}

static TEG_STATUS get_default_values( void )
{
	gchar *string;

	string = gconf_client_get_string( g_conf_client, "/apps/teg/playername",NULL);
	if( string && string[0] )
		strncpy(g_game.myname,string,PLAYERNAME_MAX_LEN);
	else
		strncpy(g_game.myname,getenv("LOGNAME"),PLAYERNAME_MAX_LEN);


	g_game.mycolor = gconf_client_get_int( g_conf_client, "/apps/teg/color", NULL);

	string = gconf_client_get_string( g_conf_client, "/apps/teg/servername",NULL);
	if( string )
		strncpy(g_game.sername,string,SERVER_NAMELEN);
	else
		strncpy(g_game.sername,"localhost",SERVER_NAMELEN);

	g_game.msg_show = gconf_client_get_int( g_conf_client, "/apps/teg/msgshow",NULL);
	gui_private.msg_show_colors =  gconf_client_get_bool( g_conf_client, "/apps/teg/msgshow_with_color",NULL);
	gui_private.msg_show_robot =  gconf_client_get_bool( g_conf_client, "/apps/teg/msgshow_robot",NULL);
	gui_private.dialog_show =  gconf_client_get_int( g_conf_client, "/apps/teg/dialog_show",NULL);
	string = gconf_client_get_string( g_conf_client, "/apps/teg/theme",NULL);
	if( string )
		strncpy( g_game.theme, string ,sizeof(g_game.theme) );
	else
		strncpy( g_game.theme, "m2" ,sizeof(g_game.theme) );
	g_game.robot_in_server = gconf_client_get_bool( g_conf_client, "/apps/teg/robot_in_server",NULL);
	
	gui_private.status_show =  gconf_client_get_int( g_conf_client, "/apps/teg/status_show",NULL);
	g_game.serport = gconf_client_get_int( g_conf_client, "/apps/teg/port",NULL);
	// if serport is still 0 then this must be the first start of this program, so set serport and
	// status_show to reasonable default values
	if ( g_game.serport == 0 )
	{
		g_game.serport = 2000;
		gui_private.status_show = 453;
	}
	
	gconf_client_set_int   ( g_conf_client, "/apps/teg/port",  g_game.serport, NULL);
	gconf_client_set_string( g_conf_client, "/apps/teg/servername",g_game.sername, NULL);
	gconf_client_set_string( g_conf_client, "/apps/teg/playername",g_game.myname, NULL);
	gconf_client_set_string( g_conf_client, "/apps/teg/theme",g_game.theme, NULL);
	gconf_client_set_int( g_conf_client, "/apps/teg/color",g_game.mycolor, NULL);
	gconf_client_set_int( g_conf_client, "/apps/teg/msgshow",g_game.msg_show, NULL);
	gconf_client_set_bool( g_conf_client, "/apps/teg/msgshow_with_color",gui_private.msg_show_colors, NULL);
	gconf_client_set_int( g_conf_client, "/apps/teg/status_show",gui_private.status_show, NULL);
	gconf_client_set_int( g_conf_client, "/apps/teg/dialog_show",gui_private.dialog_show, NULL);
	gconf_client_set_bool( g_conf_client, "/apps/teg/robot_in_server",g_game.robot_in_server, NULL);
	return TEG_STATUS_SUCCESS;
}

/* the game is started */
TEG_STATUS gui_start(void)
{
	cards_flush();
	set_sensitive_tb();
	mainstatus_update_colors();

	if( status_dialog!= NULL)
		out_status();
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

	if( status_dialog != NULL) out_status();

	e = ESTADO_GET();

	armies_view( cant, conts );
	dices_unview();

	switch(e) {
	case PLAYER_STATUS_FICHAS:
		sprintf(buf,_("1st part:\nPlace %d armies in your countries"),cant);
		break;
	case PLAYER_STATUS_FICHAS2:
		sprintf(buf,_("2nd part:\nPlace %d armies in your countries"),cant);
		break;
	case PLAYER_STATUS_FICHASC:
		{
			int i,p1,p2,p3;
			char buf2[500];
			int hubo_cont=0;

			buf[0]=0;
			for(i=0;i<CONT_CANT;i++) {
				if( conts & 1 ) {
					snprintf(buf2,sizeof(buf2)-1,_("Place %d armies in %s\n"),g_conts[i].fichas_x_cont, cont_get_name(i) );
					buf2[ sizeof(buf2) -1 ] = 0;
					strcat(buf,buf2);
					hubo_cont=1;
				}
				conts >>= 1;
			}
			if( hubo_cont ) {
				snprintf(buf2,sizeof(buf2)-1,_("And place another %d armies in your countries\n"),cant);
				buf2[ sizeof(buf2) -1 ] = 0;
				strcat(buf,buf2);
			} else {
				snprintf(buf2,sizeof(buf2)-1,_("Place %d armies in your countries\n"),cant);
				buf2[ sizeof(buf2) -1 ] = 0;
				strcat(buf,buf2);
			}

			if( canje_puedo(&p1,&p2,&p3) == TEG_STATUS_SUCCESS ) {
				strcat(buf,_("\nIf you want to EXCHANGE your cards for armies,\nDO IT NOW!"));
				cards_select(p1,p2,p3);
			}
			break;
		}
	default:
		textmsg(M_ERR,"Error in out_fichas()");
		return TEG_STATUS_ERROR;
	}
	if (gui_private.dialog_show & (1 << DIALOG_PLACE_ARMIES) ) {
		teg_dialog(_("Place armies"),_("Place armies"),buf); }
	textmsg(M_IMP,"%s",buf);

	return TEG_STATUS_SUCCESS;
}

/* someone has the turn */
TEG_STATUS gui_turn( PCPLAYER pJ )
{
	if( pJ->numjug == WHOAMI() )
	{
		set_sensitive_tb();

		if( status_dialog != NULL)
			out_status();

		if( gui_private.dialog_show & (1 << DIALOG_ATTACK_TURN ) ) {
			teg_dialog(_("Its your turn"),_("Its your turn to attack"),
					_("Select two countries:\n1st: click on the source country\n2nd: click on the destination country"));
		}
	}

	mainstatus_update();

	return TEG_STATUS_SUCCESS;
}

/* shows one country */
TEG_STATUS gui_country( int p )
{
//	if( status_dialog != NULL) out_status();

	if( p < COUNTRIES_CANT ) {
		G_country_draw_ejer( p );
	}

	return TEG_STATUS_SUCCESS;
}


/* the number of armies to move to the conquered country */
TEG_STATUS gui_tropas( int src, int dst, int cant )
{
	tropas_window( src, dst, cant );
	return TEG_STATUS_SUCCESS;
}

/* regrouping armies from one country to another */
TEG_STATUS gui_reagrupe( int src, int dst, int cant )
{
	reagrupe_window( src, dst, cant );
	return TEG_STATUS_SUCCESS;
}

/* a card was received */
TEG_STATUS gui_tarjeta( int country )
{
	cards_view( country );

	if( tarjeta_es_usada( &g_countries[country].tarjeta ) )
		locate_country_add_army( &g_countries[country] );

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
TEG_STATUS gui_canje( int cant, int p1, int p2, int p3)
{
	char buf[128];
	snprintf(buf, sizeof(buf)-1,_("Now you can add %d extra armies in your countries!"), cant);
	buf[ sizeof(buf) -1 ]=0;
	teg_dialog(_("Extra armies"),_("Extra armies"),buf);
	armies_view_more( cant );
	cards_delete(p1,p2,p3);
	return TEG_STATUS_SUCCESS;
}

/* country 'src' is attacking country 'dst' */
TEG_STATUS gui_attack( int src, int dst )
{
	gui_country_select( src );
	gui_country_select( dst );
	return TEG_STATUS_SUCCESS;
}
