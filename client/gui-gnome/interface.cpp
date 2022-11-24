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
/**
 * @file interface.c
 * Maneja las ventanas que se crean, salvo algunas
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <goocanvas.h>
#include <glib/gi18n.h>

#include "client.h"
#include "chatline.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "g_country.h"
#include "g_cont.h"
#include "stock.h"
#include "status.h"
#include "armies.h"
#include "dices.h"
#include "themes.h"
#include "fonts.h"
#include "priv.h"
#include "armies.h"

#include "robot.xpm"

namespace teg::client::callbacks
{

extern TTheme gui_theme;

GtkTextView *main_message_area = NULL;
GtkWidget *main_window = NULL;
GtkWidget *toolbar_main = NULL;
GtkWidget *statusbar = NULL;
GtkWidget *canvas_map = NULL;
GtkWidget *window_mainstatus = NULL;

double canvas_map_zoom =1.00;

static GtkWidget *tropas_dialog=NULL;
static GtkWidget *tropas_hscale_cant=NULL;
static int tropas_cant;
static int tropas_src;
static int tropas_dst;

static GtkWidget *reagrupe_dialog=NULL;
static GtkWidget *reagrupe_hscale_cant=NULL;
static int reagrupe_cant=-1;
static int reagrupe_src=-1;
static int reagrupe_dst=-1;

static void canvas_map_zoom_in(GtkWidget *widget, gpointer data);
static void canvas_map_zoom_out(GtkWidget *widget, gpointer data);
static void canvas_map_zoom_1(GtkWidget *widget, gpointer data);

/*
 * IMPORTANT:
 * if you add/remove an entry in the menu, modify this
 */
enum {
	ON_CONNECT,
	ON_DISCONNECT,
	ON_GAME_SEPARATOR_1,
	ON_LAUNCHROBOT,
	ON_GAME_SEPARATOR_2,
	ON_SCORES,
	ON_GAME_SEPARATOR_3,
	ON_EXIT
};

/*
 * IMPORTANT:
 * Si se agrega o se saca algo de actions_menu_uiinfo, sacarlo de aca
 */
enum {
	ON_START,
	ON_FICHAS,
	ON_UPDATEMAP,
	ON_ATTACK_RE,
	ON_ATTACK_RESET,
	ON_REGROUP,
	ON_GETCARD,
	ON_ENDTURN,
	ON_ACTIONS_SEPARATOR,
	ON_SURRENDER,
};

/*
 * IMPORTANT:
 * Si se agrega o se saca algo de view_menu_uiinfo, sacarlo de aca
 */
enum {
	ON_STATUS,
	ON_MISSION,
	ON_CARDS,
};

struct _SensiData {
	GtkWidget *connect_button;
	GtkWidget *start_button;
	GtkWidget *enviarfichas_button;
	GtkWidget *reagrupe_button;
	GtkWidget *getcard_button;
	GtkWidget *endturn_button;
	GtkWidget *status_button;
} SensiData;


static void canvas_map_zoom_1(GtkWidget *widget, gpointer data)
{
	canvas_map_zoom = 1.0;
	goo_canvas_set_scale((GooCanvas*)canvas_map, canvas_map_zoom);
}

static void canvas_map_zoom_in(GtkWidget *widget, gpointer data)
{
	canvas_map_zoom *= 1.05;
	goo_canvas_set_scale((GooCanvas*)canvas_map, canvas_map_zoom);
}

static void canvas_map_zoom_out(GtkWidget *widget, gpointer data)
{
	canvas_map_zoom *= 0.95;
	goo_canvas_set_scale((GooCanvas*)canvas_map, canvas_map_zoom);
}

static TEG_STATUS canvas_create_countries(GooCanvasItem *group)
{
	int i=0;

	for(i=0; i<CONT_CANT; i++) {
		G_cont_create(i, group);
	}

	for(i=0; i<COUNTRIES_CANT; i++) {
		G_country_create(i);
		G_country_draw(i);
		G_country_draw_ejer(i);
	}

	return TEG_STATUS_SUCCESS;
}

static TEG_STATUS canvas_create_map(void)
{
	GooCanvasItem* root;
	GdkPixbuf *im;

	canvas_map = goo_canvas_new();
	g_object_set(canvas_map, "anchor", GOO_CANVAS_ANCHOR_CENTER, NULL);
	root = goo_canvas_get_root_item(GOO_CANVAS(canvas_map));

	auto fpath = theme_load_file(gui_theme.board);
	char const*const filename = fpath ? fpath->c_str() : nullptr;

	im = gdk_pixbuf_new_from_file(filename, NULL);

	if(im) {
		goo_canvas_image_new(
		    root,
		    im,
		    0.0,
		    0.0,
		    "width", (double) gdk_pixbuf_get_width(im),
		    "height", (double) gdk_pixbuf_get_height(im),
		    NULL);
		canvas_create_countries(root);
		armies_init(root);
		dices_init(root);
		goo_canvas_set_bounds(GOO_CANVAS(canvas_map), 0, 0,
		                      gdk_pixbuf_get_width(im),
		                      gdk_pixbuf_get_height(im));

		g_object_unref(im);

	} else {
		g_warning(_("Could not find the %s file"), gui_theme.board);
		return TEG_STATUS_ERROR;
	}

	return TEG_STATUS_SUCCESS;
}

static GtkWidget *connect_menu;
static GtkWidget *disconnect_menu;
static GtkWidget *robot_menu;
static GtkWidget *scores_menu;
static GtkWidget *start_menu;
static GtkWidget *send_menu;
static GtkWidget *update_menu;
static GtkWidget *reattack_menu;
static GtkWidget *reset_menu;
static GtkWidget *regroup_menu;
static GtkWidget *get_menu;
static GtkWidget *end_menu;
static GtkWidget *surrender_menu;
static GtkWidget *players_menu;
static GtkWidget *mission_menu;
static GtkWidget *cards_menu;

/**
 * @fn GtkWidget* create_mainwin (void)
 * Crea la ventana principal
 */
GtkWidget* create_mainwin(void)
{
	GtkWidget *app1;
	GtkWidget *dock1;
	GtkWidget *menubar;
	GtkAccelGroup *accel_group;
	GtkWidget *gamemenu;
	GtkWidget *game_menu;
	GdkPixbuf *robot;
	GtkWidget *hbox;
	GtkWidget *icon;
	GtkWidget *label;
	GtkWidget *separator;
	GtkWidget *quit_menu;
	GtkWidget *actionmenu;
	GtkWidget *action_menu;
	GtkWidget *viewmenu;
	GtkWidget *view_menu;
	GtkWidget *zoomin_menu;
	GtkWidget *zoomout_menu;
	GtkWidget *zoom1_menu;
	GtkWidget *settingsmenu;
	GtkWidget *settings_menu;
	GtkWidget *prefs_menu;
	GtkWidget *helpmenu;
	GtkWidget *help_menu;
	GtkWidget *manual_menu;
	GtkWidget *about_menu;
	GtkWidget *image;
	GtkToolItem *button1;
	GtkToolItem *button2;
	GtkToolItem *button3;
	GtkToolItem *button4;
	GtkToolItem *button5;
	GtkToolItem *button6;
	GtkToolItem *button7;
	GtkToolItem *sep;
	GtkWidget *eventbox1;
	GtkWidget *viewport1;
	GtkWidget *vbox_main;
	GtkWidget *vbox_paned;
	GtkWidget *scrolledwindow;
	GtkWidget *paned;
	GtkWidget *text;
	GtkWidget *inputline;
	GtkWidget *sw;
	GtkWidget *ahbox;

	app1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(app1), "Tenes Empanadas Graciela");
	g_object_set_data(G_OBJECT(app1), "app1", app1);
	gtk_window_set_default_size(GTK_WINDOW(app1), 664, 650);

	dock1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	g_object_ref(dock1);
	g_object_set_data_full(G_OBJECT(app1), "dock1", dock1,
	                       (GDestroyNotify) g_object_unref);
	gtk_widget_show(dock1);
	gtk_container_add(GTK_CONTAINER(app1), dock1);

	menubar = gtk_menu_bar_new();
	gtk_container_add(GTK_CONTAINER(dock1), menubar);

	accel_group = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(app1), accel_group);

	gamemenu = gtk_menu_new();
	gtk_menu_set_accel_group(GTK_MENU(gamemenu), accel_group);
	game_menu = gtk_menu_item_new_with_mnemonic(_("_Game"));
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(game_menu), gamemenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), game_menu);

	connect_menu = gtk_menu_item_new_with_mnemonic(_("_Connect"));
	gtk_widget_set_tooltip_text(connect_menu, _("Connect to server"));
	gtk_menu_shell_append(GTK_MENU_SHELL(gamemenu), connect_menu);
	g_signal_connect(connect_menu, "activate",
	                 G_CALLBACK(on_connect_activate), NULL);

	disconnect_menu = gtk_menu_item_new_with_mnemonic(_("_Disconnect"));
	gtk_widget_set_tooltip_text(disconnect_menu,
	                            _("Disconnect from server"));
	gtk_menu_shell_append(GTK_MENU_SHELL(gamemenu), disconnect_menu);
	g_signal_connect(disconnect_menu, "activate",
	                 G_CALLBACK(on_disconnect_activate), NULL);

	/* Emulate GtkImageMenuItem.  This was the author's intention
	   but it doesn't work in the original code.  */
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
	robot = gdk_pixbuf_new_from_xpm_data(robot_xpm);
	icon = gtk_image_new_from_pixbuf(robot);
	g_object_unref(robot);
	label = gtk_label_new_with_mnemonic(_("_Launch robot"));
	gtk_widget_set_tooltip_text(label, _("Launch a robot"));
	robot_menu = gtk_menu_item_new();
	gtk_container_add(GTK_CONTAINER(hbox), icon);
	gtk_container_add(GTK_CONTAINER(hbox), label);
	gtk_container_add(GTK_CONTAINER(robot_menu), hbox);
	gtk_menu_shell_append(GTK_MENU_SHELL(gamemenu), robot_menu);
	g_signal_connect(robot_menu, "activate",
	                 G_CALLBACK(on_launchrobot_activate), NULL);

	separator = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(gamemenu), separator);

	scores_menu = gtk_menu_item_new_with_mnemonic(_("_Scores..."));
	gtk_widget_set_tooltip_text(scores_menu, _("View the scores"));
	gtk_menu_shell_append(GTK_MENU_SHELL(gamemenu), scores_menu);
	g_signal_connect(scores_menu, "activate",
	                 G_CALLBACK(on_scores_activate), NULL);

	separator = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(gamemenu), separator);

	quit_menu = gtk_menu_item_new_with_mnemonic(_("_Quit"));
	gtk_widget_set_tooltip_text(quit_menu, _("Quit the application"));
	gtk_menu_shell_append(GTK_MENU_SHELL(gamemenu), quit_menu);
	gtk_widget_add_accelerator(quit_menu, "activate", accel_group,
	                           GDK_KEY_Q, GDK_CONTROL_MASK,
	                           GTK_ACCEL_VISIBLE);
	g_signal_connect(quit_menu, "activate",
	                 G_CALLBACK(on_exit_activate), NULL);

	actionmenu = gtk_menu_new();
	gtk_menu_set_accel_group(GTK_MENU(actionmenu), accel_group);
	action_menu = gtk_menu_item_new_with_mnemonic(_("_Actions"));
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(action_menu), actionmenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), action_menu);

	start_menu = gtk_menu_item_new_with_label(_("Start"));
	gtk_widget_set_tooltip_text(start_menu, _("Start the game"));
	gtk_menu_shell_append(GTK_MENU_SHELL(actionmenu), start_menu);
	g_signal_connect(start_menu, "activate",
	                 G_CALLBACK(on_start_activate), NULL);

	send_menu = gtk_menu_item_new_with_label(_("Send armies"));
	gtk_widget_set_tooltip_text(send_menu,
	                            _("Send the armies recently placed"));
	gtk_menu_shell_append(GTK_MENU_SHELL(actionmenu), send_menu);
	g_signal_connect(send_menu, "activate",
	                 G_CALLBACK(on_enviarfichas_activate), NULL);

	update_menu = gtk_menu_item_new_with_mnemonic(_("_Update Map"));
	gtk_widget_set_tooltip_text(update_menu,
	                            _("Show the armies that other players "
	                              "had placed"));
	gtk_menu_shell_append(GTK_MENU_SHELL(actionmenu), update_menu);
	g_signal_connect(update_menu, "activate",
	                 G_CALLBACK(on_updatemap_activate), NULL);

	reattack_menu = gtk_menu_item_new_with_mnemonic(_("_Reattack"));
	gtk_widget_set_tooltip_text(reattack_menu,
	                            _("Attack your enemy, again using same "
	                              "source and destination"));
	gtk_menu_shell_append(GTK_MENU_SHELL(actionmenu), reattack_menu);
	gtk_widget_add_accelerator(reattack_menu, "activate", accel_group,
	                           GDK_KEY_r, GDK_CONTROL_MASK,
	                           GTK_ACCEL_VISIBLE);
	g_signal_connect(reattack_menu, "activate",
	                 G_CALLBACK(on_enviarreattack_activate), NULL);

	reset_menu = gtk_menu_item_new_with_label(_("Reset the attack"));
	gtk_widget_set_tooltip_text(reset_menu,
	                            _("Start again selecting the source and "
	                              "destination country for the attack"));
	gtk_menu_shell_append(GTK_MENU_SHELL(actionmenu), reset_menu);
	g_signal_connect(reset_menu, "activate",
	                 G_CALLBACK(on_enviarattackreset_activate), NULL);

	regroup_menu = gtk_menu_item_new_with_mnemonic(_("Re_group armies"));
	gtk_widget_set_tooltip_text(regroup_menu,
	                            _("Regroup your armies, moving from "
	                              "one country to another"));
	gtk_menu_shell_append(GTK_MENU_SHELL(actionmenu), regroup_menu);
	g_signal_connect(regroup_menu, "activate",
	                 G_CALLBACK(on_reagrupe_activate), NULL);

	get_menu = gtk_menu_item_new_with_label(_("Get card"));
	gtk_widget_set_tooltip_text(get_menu, _("Get a country card"));
	gtk_menu_shell_append(GTK_MENU_SHELL(actionmenu), get_menu);
	g_signal_connect(get_menu, "activate",
	                 G_CALLBACK(on_pedirtarjeta_activate), NULL);

	end_menu = gtk_menu_item_new_with_mnemonic(_("_End turn"));
	gtk_widget_set_tooltip_text(end_menu,
	                            _("Pass the turn to another player"));
	gtk_menu_shell_append(GTK_MENU_SHELL(actionmenu), end_menu);
	g_signal_connect(end_menu, "activate",
	                 G_CALLBACK(on_endturn_activate), NULL);

	separator = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(actionmenu), separator);

	surrender_menu = gtk_menu_item_new_with_label(_("Surrender"));
	gtk_widget_set_tooltip_text(surrender_menu, _("I'm a coward"));
	gtk_menu_shell_append(GTK_MENU_SHELL(actionmenu), surrender_menu);
	g_signal_connect(surrender_menu, "activate",
	                 G_CALLBACK(on_surrender_activate), NULL);

	viewmenu = gtk_menu_new();
	gtk_menu_set_accel_group(GTK_MENU(viewmenu), accel_group);
	view_menu = gtk_menu_item_new_with_mnemonic(_("_View"));
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_menu), viewmenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), view_menu);

	players_menu = gtk_menu_item_new_with_mnemonic(_("View _players"));
	gtk_widget_set_tooltip_text(players_menu,
	                            _("View the status of the players"));
	gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), players_menu);
	g_signal_connect(players_menu, "activate",
	                 G_CALLBACK(on_status_activate), NULL);

	mission_menu = gtk_menu_item_new_with_mnemonic(_("View _mission"));
	gtk_widget_set_tooltip_text(mission_menu,
	                            _("View your mission of this game"));
	gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), mission_menu);
	g_signal_connect(mission_menu, "activate",
	                 G_CALLBACK(on_viewmission_activate), NULL);

	cards_menu = gtk_menu_item_new_with_mnemonic(_("View _cards"));
	gtk_widget_set_tooltip_text(cards_menu,
	                            _("View all the cards that you won"));
	gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), cards_menu);
	g_signal_connect(cards_menu, "activate",
	                 G_CALLBACK(on_viewcards_activate), NULL);

	separator = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), separator);

	zoomin_menu = gtk_menu_item_new_with_label(_("Zoom In"));
	gtk_widget_set_tooltip_text(zoomin_menu,
	                            _("Increase zoom factor by 5%"));
	gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), zoomin_menu);
	gtk_widget_add_accelerator(zoomin_menu, "activate", accel_group,
	                           GDK_KEY_equal, GDK_CONTROL_MASK,
	                           GTK_ACCEL_VISIBLE);
	g_signal_connect(zoomin_menu, "activate",
	                 G_CALLBACK(canvas_map_zoom_in), NULL);

	zoomout_menu = gtk_menu_item_new_with_label(_("Zoom out"));
	gtk_widget_set_tooltip_text(zoomout_menu,
	                            _("Decrease zoom factor by 5%"));
	gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), zoomout_menu);
	gtk_widget_add_accelerator(zoomout_menu, "activate", accel_group,
	                           GDK_KEY_minus, GDK_CONTROL_MASK,
	                           GTK_ACCEL_VISIBLE);
	g_signal_connect(zoomout_menu, "activate",
	                 G_CALLBACK(canvas_map_zoom_out), NULL);

	zoom1_menu = gtk_menu_item_new_with_mnemonic(_("Zoom _1:1"));
	gtk_widget_set_tooltip_text(zoom1_menu,
	                            _("Display the map at 1:1 scale"));
	gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), zoom1_menu);
	gtk_widget_add_accelerator(zoom1_menu, "activate", accel_group,
	                           GDK_KEY_1, GDK_CONTROL_MASK,
	                           GTK_ACCEL_VISIBLE);
	g_signal_connect(zoom1_menu, "activate",
	                 G_CALLBACK(canvas_map_zoom_1), NULL);

	settingsmenu = gtk_menu_new();
	settings_menu = gtk_menu_item_new_with_mnemonic(_("_Settings"));
	gtk_widget_set_name(settings_menu, "settings");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(settings_menu), settingsmenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), settings_menu);

	prefs_menu = gtk_menu_item_new_with_mnemonic(_("Prefere_nces"));
	gtk_widget_set_tooltip_text(prefs_menu,
	                            _("Configure the application"));
	gtk_widget_set_name(prefs_menu, "preferences");
	gtk_menu_shell_append(GTK_MENU_SHELL(settingsmenu), prefs_menu);
	g_signal_connect(prefs_menu, "activate",
	                 G_CALLBACK(on_preferences1_activate), NULL);

	helpmenu = gtk_menu_new();
	gtk_menu_set_accel_group(GTK_MENU(helpmenu), accel_group);
	help_menu = gtk_menu_item_new_with_mnemonic(_("_Help"));
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(help_menu), helpmenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), help_menu);

	manual_menu = gtk_menu_item_new_with_mnemonic(_("_Contents"));
	gtk_widget_set_tooltip_text(manual_menu,
	                            _("View help for this application"));
	gtk_menu_shell_append(GTK_MENU_SHELL(helpmenu), manual_menu);
	gtk_widget_add_accelerator(manual_menu, "activate", accel_group,
	                           GDK_KEY_F1, GDK_CONTROL_MASK,
	                           GTK_ACCEL_VISIBLE);
	g_signal_connect(manual_menu, "activate",
	                 G_CALLBACK(on_help_activate), NULL);

	about_menu = gtk_menu_item_new_with_mnemonic(_("_About"));
	gtk_widget_set_tooltip_text(about_menu, _("About this application"));
	gtk_menu_shell_append(GTK_MENU_SHELL(helpmenu), about_menu);
	g_signal_connect(about_menu, "activate",
	                 G_CALLBACK(on_about_activate), NULL);

	gtk_widget_show_all(menubar);

	toolbar_main = gtk_toolbar_new();
	g_settings_bind(settings, "toolbar-style", toolbar_main,
	                "toolbar-style", G_SETTINGS_BIND_DEFAULT);
	gtk_widget_show(toolbar_main);
	gtk_container_add(GTK_CONTAINER(dock1), toolbar_main);

	image = gtk_image_new_from_icon_name(STOCK_CONNECT,
	                                     GTK_ICON_SIZE_LARGE_TOOLBAR);
	button1 = gtk_tool_button_new(image, _("Connect"));
	gtk_tool_item_set_tooltip_text(button1, _("Connect to server"));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_main), button1, -1);
	g_signal_connect(button1, "clicked",
	                 G_CALLBACK(on_connect_activate), NULL);

	image = gtk_image_new_from_icon_name(STOCK_STARTGAME,
	                                     GTK_ICON_SIZE_LARGE_TOOLBAR);
	button2 = gtk_tool_button_new(image, _("Start"));
	gtk_tool_item_set_tooltip_text(button2, _("Start the game"));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_main), button2, -1);
	g_signal_connect(button2, "clicked",
	                 G_CALLBACK(on_start_activate), NULL);

	sep = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_main), sep, -1);

	image = gtk_image_new_from_icon_name(STOCK_SENDARMIES,
	                                     GTK_ICON_SIZE_LARGE_TOOLBAR);
	button3 = gtk_tool_button_new(image, _("Send armies"));
	gtk_tool_item_set_tooltip_text(button3, _("Send your armies"));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_main), button3, -1);
	g_signal_connect(button3, "clicked",
	                 G_CALLBACK(on_enviarfichas_activate), NULL);

	image = gtk_image_new_from_icon_name(STOCK_REGROUPARMIES,
	                                     GTK_ICON_SIZE_LARGE_TOOLBAR);
	button4 = gtk_tool_button_new(image, _("1. Regroup"));
	gtk_tool_item_set_tooltip_text(button4,
	                               _("Regroup your armies, moving from "
	                                 "one country to another"));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_main), button4, -1);
	g_signal_connect(button4, "clicked",
	                 G_CALLBACK(on_reagrupe_activate), NULL);

	image = gtk_image_new_from_icon_name(STOCK_GETCARD,
	                                     GTK_ICON_SIZE_LARGE_TOOLBAR);
	button5 = gtk_tool_button_new(image, _("2. Get card"));
	gtk_tool_item_set_tooltip_text(button5, _("Get a country card"));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_main), button5, -1);
	g_signal_connect(button5, "clicked",
	                 G_CALLBACK(on_pedirtarjeta_activate), NULL);

	image = gtk_image_new_from_icon_name(STOCK_ENDTURN,
	                                     GTK_ICON_SIZE_LARGE_TOOLBAR);
	button6 = gtk_tool_button_new(image, _("3. End turn"));
	gtk_tool_item_set_tooltip_text(button6,
	                               _("Pass the turn to another player"));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_main), button6, -1);
	g_signal_connect(button6, "clicked",
	                 G_CALLBACK(on_endturn_activate), NULL);

	sep = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_main), sep, -1);

	image = gtk_image_new_from_icon_name(STOCK_VIEWPLAYERS,
	                                     GTK_ICON_SIZE_LARGE_TOOLBAR);
	button7 = gtk_tool_button_new(image, _("View players"));
	gtk_tool_item_set_tooltip_text(button7, _("View players' status"));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_main), button7, -1);
	g_signal_connect(button7, "clicked",
	                 G_CALLBACK(on_status_activate), NULL);

	gtk_widget_show_all(toolbar_main);

	gtk_widget_realize(app1);

	g_signal_connect(G_OBJECT(app1), "destroy",
	                 G_CALLBACK(on_exit_activate), NULL);

	g_signal_connect(G_OBJECT(app1), "delete_event",
	                 G_CALLBACK(on_exit_activate), NULL);


	SensiData.connect_button = GTK_WIDGET(button1);
	SensiData.start_button = GTK_WIDGET(button2);
	/* 2 is a separator */
	SensiData.enviarfichas_button = GTK_WIDGET(button3);
	SensiData.reagrupe_button = GTK_WIDGET(button4);
	SensiData.getcard_button = GTK_WIDGET(button5);
	SensiData.endturn_button = GTK_WIDGET(button6);
	/* 7 is a separator */
	SensiData.status_button = GTK_WIDGET(button7);

	eventbox1 = gtk_event_box_new();
	g_object_ref(eventbox1);
	g_object_set_data_full(G_OBJECT(app1), "eventbox1", eventbox1,
	                       (GDestroyNotify) g_object_unref);
	gtk_widget_show(eventbox1);
	gtk_box_pack_start(GTK_BOX(dock1), eventbox1, TRUE, TRUE, 0);

	viewport1 = gtk_viewport_new(NULL, NULL);
	gtk_widget_show(viewport1);
	gtk_container_add(GTK_CONTAINER(eventbox1), viewport1);

	vbox_main = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show(vbox_main);
	gtk_container_add(GTK_CONTAINER(viewport1), vbox_main);

	/* status map */
	if(mainstatus_create(&window_mainstatus) != TEG_STATUS_SUCCESS) {
		goto error;
	}
	gtk_box_pack_start(GTK_BOX(vbox_main), window_mainstatus, FALSE, FALSE, 0);

	/* paned */
	paned = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
	gtk_box_pack_start(GTK_BOX(vbox_main), paned, TRUE, TRUE, 0);
	vbox_paned = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_paned_add1(GTK_PANED(paned), vbox_paned);
	gtk_widget_show(vbox_paned);

	/* map */
	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow),
	                               GTK_POLICY_AUTOMATIC,
	                               GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW
	        (scrolledwindow), 641);
	gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW
	        (scrolledwindow), 394);

	if(canvas_create_map() != TEG_STATUS_SUCCESS) {
		goto error;
	}

	gtk_container_add(GTK_CONTAINER(scrolledwindow), canvas_map);
	gtk_box_pack_start(GTK_BOX(vbox_paned), scrolledwindow, TRUE, TRUE, 0);
	gtk_widget_show(canvas_map);
	gtk_widget_show(scrolledwindow);


	/* text */
	ahbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	gtk_paned_pack2(GTK_PANED(paned), ahbox, TRUE, TRUE);

	sw = gtk_scrolled_window_new(NULL, NULL);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_AUTOMATIC,
	                               GTK_POLICY_ALWAYS);
	gtk_box_pack_start(GTK_BOX(ahbox), sw, TRUE, TRUE, 0);

	gtk_widget_set_size_request(sw, 620, 60);

	text = gtk_text_view_new();
	gtk_text_view_set_editable(GTK_TEXT_VIEW(text), FALSE);
	gtk_container_add(GTK_CONTAINER(sw), text);

	gtk_widget_set_name(text, "chatline");

	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text), GTK_WRAP_WORD);
	gtk_widget_realize(text);
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(text), 5);

	main_message_area = GTK_TEXT_VIEW(text);

	gtk_widget_show(text);
	gtk_widget_show(sw);
	gtk_widget_show(ahbox);
	gtk_widget_show(paned);

	/* the chat line */
	inputline = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(vbox_main), inputline, FALSE, FALSE, 0);
	gtk_widget_show(inputline);
	g_signal_connect(inputline, "activate", G_CALLBACK(inputline_return), NULL);


	chatline_init();


	/* status bar */
	statusbar = gtk_statusbar_new();
	g_object_ref(statusbar);
	g_object_set_data_full(G_OBJECT(app1), "statusbar", statusbar,
	                       (GDestroyNotify) g_object_unref);
	gtk_widget_show(statusbar);
	gtk_container_add(GTK_CONTAINER(dock1), statusbar);

	gtk_box_pack_start(GTK_BOX(statusbar),
	                   ministatus_build(), FALSE, TRUE, 0);


	gtk_statusbar_push(GTK_STATUSBAR(statusbar), 0,
	                   _("Welcome to TEG!"));

	/* icono */
	{
		GdkPixbuf *icono;
#include "../teg_pix/teg_icono.xpm"
		gtk_widget_realize(app1);
		icono = gdk_pixbuf_new_from_xpm_data(teg_icono_xpm);
		gtk_window_set_icon(GTK_WINDOW(app1), icono);
	}

	gtk_widget_show(app1);
	return app1;

error:
	g_error(_("Unable to create map!"));
	return NULL;
}

/**
 * @fn TEG_STATUS set_sensitive_tb(void)
 * 'prende' y 'apaga' los botones segun el estado del game
 */
TEG_STATUS set_sensitive_tb(void)
{
	PLAYER_STATUS e = ESTADO_GET();

	/* CONNECT & START */
	if(e == PLAYER_STATUS_DESCONECTADO) {
		gtk_widget_set_sensitive(connect_menu, TRUE);
		gtk_widget_set_sensitive(disconnect_menu, FALSE);
		gtk_widget_set_sensitive(start_menu, FALSE);
		gtk_widget_set_sensitive(SensiData.connect_button, TRUE);
		gtk_widget_set_sensitive(SensiData.start_button, FALSE);

	} else if(e == PLAYER_STATUS_HABILITADO) {
		gtk_widget_set_sensitive(connect_menu, FALSE);
		gtk_widget_set_sensitive(disconnect_menu, TRUE);
		gtk_widget_set_sensitive(start_menu, TRUE);
		gtk_widget_set_sensitive(SensiData.connect_button, FALSE);
		gtk_widget_set_sensitive(SensiData.start_button, TRUE);

	} else {
		gtk_widget_set_sensitive(connect_menu, FALSE);
		gtk_widget_set_sensitive(disconnect_menu, TRUE);
		gtk_widget_set_sensitive(start_menu, FALSE);
		gtk_widget_set_sensitive(SensiData.connect_button, FALSE);
		gtk_widget_set_sensitive(SensiData.start_button, FALSE);
	}

	if(g_game.observer) {
		gtk_widget_set_sensitive(start_menu, FALSE);
		gtk_widget_set_sensitive(SensiData.start_button, FALSE);
	}

	/* STATUS */
	if(e == PLAYER_STATUS_DESCONECTADO) {
		gtk_widget_set_sensitive(players_menu, FALSE);
		gtk_widget_set_sensitive(SensiData.status_button, FALSE);
	} else {
		gtk_widget_set_sensitive(players_menu, TRUE);
		gtk_widget_set_sensitive(SensiData.status_button, TRUE);
	}

	/* ATAQUE */
	if(e == PLAYER_STATUS_ATAQUE) {
		gtk_widget_set_sensitive(reattack_menu, TRUE);
		gtk_widget_set_sensitive(reset_menu, TRUE);
	} else {
		gtk_widget_set_sensitive(reattack_menu, FALSE);
		gtk_widget_set_sensitive(reset_menu, FALSE);
	}

	/* ENVIAR FICHAS */
	if(e==PLAYER_STATUS_FICHAS || e==PLAYER_STATUS_FICHAS2 || e==PLAYER_STATUS_FICHASC) {
		gtk_widget_set_sensitive(send_menu, TRUE);
		gtk_widget_set_sensitive(SensiData.enviarfichas_button, TRUE);
	} else {
		gtk_widget_set_sensitive(send_menu, FALSE);
		gtk_widget_set_sensitive(SensiData.enviarfichas_button, FALSE);
	}

	/* REAGRUPE */
	if(e==PLAYER_STATUS_ATAQUE || e==PLAYER_STATUS_TROPAS) {
		gtk_widget_set_sensitive(regroup_menu, TRUE);
		gtk_widget_set_sensitive(SensiData.reagrupe_button, TRUE);
	} else {
		gtk_widget_set_sensitive(regroup_menu, FALSE);
		gtk_widget_set_sensitive(SensiData.reagrupe_button, FALSE);
	}

	/* GET CARD */
	if(e==PLAYER_STATUS_ATAQUE || e==PLAYER_STATUS_TROPAS || e==PLAYER_STATUS_REAGRUPE) {
		gtk_widget_set_sensitive(get_menu, TRUE);
		gtk_widget_set_sensitive(SensiData.getcard_button, TRUE);
	} else {
		gtk_widget_set_sensitive(get_menu, FALSE);
		gtk_widget_set_sensitive(SensiData.getcard_button, FALSE);
	}

	/* END TURN */
	if(e==PLAYER_STATUS_ATAQUE || e==PLAYER_STATUS_TROPAS || e==PLAYER_STATUS_REAGRUPE || e==PLAYER_STATUS_TARJETA) {
		gtk_widget_set_sensitive(end_menu, TRUE);
		gtk_widget_set_sensitive(SensiData.endturn_button, TRUE);
	} else {
		gtk_widget_set_sensitive(end_menu, FALSE);
		gtk_widget_set_sensitive(SensiData.endturn_button, FALSE);
	}

	/* UPDATE MAP */
	if(e >= PLAYER_STATUS_START || (g_game.observer && e >= PLAYER_STATUS_CONNECTED)) {
		gtk_widget_set_sensitive(update_menu, TRUE);
	} else {
		gtk_widget_set_sensitive(update_menu, FALSE);
	}

	/* VIEW CARDS, MISSION */
	if(e >= PLAYER_STATUS_START) {
		gtk_widget_set_sensitive(mission_menu, TRUE);
		gtk_widget_set_sensitive(cards_menu, TRUE);
	} else {
		gtk_widget_set_sensitive(mission_menu, FALSE);
		gtk_widget_set_sensitive(cards_menu, FALSE);
	}

	/* LAUNCH ROBOT */
	if(e >= PLAYER_STATUS_CONNECTED && e < PLAYER_STATUS_START) {
		gtk_widget_set_sensitive(robot_menu, TRUE);
	} else {
		gtk_widget_set_sensitive(robot_menu, FALSE);
	}

	/* SURRENDER */
	if(e>=PLAYER_STATUS_START) {
		gtk_widget_set_sensitive(surrender_menu, TRUE);
	} else {
		gtk_widget_set_sensitive(surrender_menu, FALSE);
	}

	/* SCORES */
	if(e >= PLAYER_STATUS_CONNECTED) {
		gtk_widget_set_sensitive(scores_menu, TRUE);
	} else {
		gtk_widget_set_sensitive(scores_menu, FALSE);
	}


	return TEG_STATUS_SUCCESS;
}


/*
 * tropas
 */

#define GNOME_PAD 8

static void tropas_response_cb(GtkDialog *dialog, gint id, gpointer data)
{
	if(id == GTK_RESPONSE_OK) {
		if(GTK_IS_RANGE(tropas_hscale_cant))
			tropas_cant = (gint) gtk_adjustment_get_value
			              (gtk_range_get_adjustment
			               (GTK_RANGE(tropas_hscale_cant)));

		if(tropas_cant > 0) {
			out_tropas(tropas_src, tropas_dst, tropas_cant);
		}
	}

	gtk_widget_destroy(GTK_WIDGET(dialog));
}

/* shows how many armies you can move after an attack */
void tropas_window(int src, int dst, int cant)
{
	GtkWidget *label;
	GtkWidget *table;
	GtkWidget *frame;
	GtkWidget *vbox_dia, *vbox;
	GtkWidget *hbox;
	GtkAdjustment *adj;
	char buf[200];

	tropas_src = src;
	tropas_dst = dst;

	tropas_cant = 0;

	tropas_dialog = teg_dialog_new(_("A country was conquered"), _("A country was conquered"));
	gtk_dialog_add_button(GTK_DIALOG(tropas_dialog), _("_OK"),
	                      GTK_RESPONSE_OK);

	gtk_window_set_transient_for(GTK_WINDOW(tropas_dialog),
	                             GTK_WINDOW(main_window));

	g_signal_connect(G_OBJECT(tropas_dialog), "destroy",
	                 G_CALLBACK(gtk_widget_destroyed), &tropas_dialog);

	g_signal_connect(tropas_dialog, "response",
	                 G_CALLBACK(tropas_response_cb), NULL);

	vbox_dia = gtk_dialog_get_content_area(GTK_DIALOG(tropas_dialog));

	table = gtk_grid_new();
	gtk_container_set_border_width(GTK_CONTAINER(table), GNOME_PAD);
	gtk_grid_set_row_spacing(GTK_GRID(table), GNOME_PAD);
	gtk_grid_set_column_spacing(GTK_GRID(table), GNOME_PAD);

	gtk_box_pack_start(GTK_BOX(vbox_dia), table, TRUE, TRUE, 0);

	frame = gtk_frame_new(_("A new country"));
	gtk_container_set_border_width(GTK_CONTAINER(frame), 0);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), GNOME_PAD);

	snprintf(buf, sizeof(buf)-1, _("You conquered '%s' in your attack from '%s'!!\n"), countries_get_name(dst), countries_get_name(src));
	buf[ sizeof(buf) -1 ] = 0;
	label = gtk_label_new(buf);
	gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);

	if(cant >  0) {
		hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_container_add(GTK_CONTAINER(vbox), hbox);
		label = gtk_label_new(_("Armies to move"));
		gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
		adj = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, (gfloat)(cant + 1), 1.0, 1.0, 1.0);
		// tropas_spinner_cant = gtk_spin_button_new( adj, 0.0, 0);
		tropas_hscale_cant = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL,
		                                   adj);
		gtk_box_pack_start(GTK_BOX(hbox), tropas_hscale_cant, TRUE, TRUE, 0);
		gtk_scale_set_digits(GTK_SCALE(tropas_hscale_cant), 0);
		gtk_scale_set_draw_value(GTK_SCALE(tropas_hscale_cant), 1);
		/*		gtk_range_draw_trough(GTK_RANGE(tropas_hscale_cant));  */

		snprintf(buf, sizeof(buf) -1, _("[Note: The server moved 1 army to '%s' for you.\nChoose how many more armies you want to move]"), countries_get_name(dst));
		buf[ sizeof(buf) -1 ] = 0;
		label = gtk_label_new(buf);
		gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);
	}

	gtk_container_add(GTK_CONTAINER(frame), vbox);

	gtk_grid_attach(GTK_GRID(table), frame, 0, 0, 1, 1);


	if(!gtk_widget_get_visible(tropas_dialog)) {
		gtk_widget_show_all(tropas_dialog);
	} else {
		gtk_widget_destroy(tropas_dialog);
	}
}

/* regroup */
static void reagrupe_response_cb(GtkDialog *dialog, gint id, gpointer data)
{
	if(id == GTK_RESPONSE_OK) {
		if(GTK_IS_RANGE(reagrupe_hscale_cant))
			reagrupe_cant = (gint) gtk_adjustment_get_value
			                (gtk_range_get_adjustment
			                 (GTK_RANGE(reagrupe_hscale_cant)));

		if(reagrupe_cant > 0) {
			reagrupe_out(reagrupe_src, reagrupe_dst,
			             reagrupe_cant);
		} else {
			reagrupe_reset();
		}
	}

	gtk_widget_destroy(GTK_WIDGET(reagrupe_dialog));
}

/* window that shows armies to move in a regroup */
void reagrupe_window(int src, int dst, int cant)
{
	GtkWidget *label;
	GtkWidget *table;
	GtkWidget *frame;
	GtkWidget *vbox_dia, *vbox;
	GtkWidget *hbox;
	GtkAdjustment *adj;
	char buf[200];

	reagrupe_src = src;
	reagrupe_dst = dst;

	reagrupe_cant = 0;

	reagrupe_dialog = teg_dialog_new(_("Regrouping armies"), _("Regroup your armies"));

	gtk_dialog_add_button(GTK_DIALOG(reagrupe_dialog), _("_OK"),
	                      GTK_RESPONSE_OK);

	gtk_window_set_transient_for(GTK_WINDOW(reagrupe_dialog),
	                             GTK_WINDOW(main_window));

	g_signal_connect(G_OBJECT(reagrupe_dialog), "destroy",
	                 G_CALLBACK(gtk_widget_destroyed), &reagrupe_dialog);

	g_signal_connect(reagrupe_dialog, "response",
	                 G_CALLBACK(reagrupe_response_cb), NULL);

	vbox_dia = gtk_dialog_get_content_area(GTK_DIALOG(reagrupe_dialog));

	table = gtk_grid_new();
	gtk_container_set_border_width(GTK_CONTAINER(table), GNOME_PAD);
	gtk_grid_set_row_spacing(GTK_GRID(table), GNOME_PAD);
	gtk_grid_set_column_spacing(GTK_GRID(table), GNOME_PAD);

	gtk_box_pack_start(GTK_BOX(vbox_dia), table, TRUE, TRUE, 0);

	frame = gtk_frame_new(_("Regrouping"));
	gtk_container_set_border_width(GTK_CONTAINER(frame), 0);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), GNOME_PAD);

	snprintf(buf, sizeof(buf) -1, _("Moving armies from '%s' to '%s'\n"), countries_get_name(src), countries_get_name(dst));
	buf[ sizeof(buf) -1 ] = 0;
	label = gtk_label_new(buf);
	gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_container_add(GTK_CONTAINER(vbox), hbox);
	label = gtk_label_new(_("Armies to move"));
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	adj = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, (gfloat)(cant + 1), 1.0, 1.0, 1.0);

	reagrupe_hscale_cant = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, adj);

	gtk_box_pack_start(GTK_BOX(hbox), reagrupe_hscale_cant, TRUE, TRUE, 0);
	gtk_scale_set_digits(GTK_SCALE(reagrupe_hscale_cant), 0);
	gtk_scale_set_draw_value(GTK_SCALE(reagrupe_hscale_cant), 1);

	snprintf(buf, sizeof(buf) -1, _("[Note: You can regroup as many times as you want,\nas long as you dont regroup an armie that were\nregrouped before in this turn.]"));
	buf[ sizeof(buf) -1 ] = 0;
	label = gtk_label_new(buf);
	gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(frame), vbox);

	gtk_grid_attach(GTK_GRID(table), frame, 0, 0, 1, 1);


	if(!gtk_widget_get_visible(reagrupe_dialog)) {
		gtk_widget_show_all(reagrupe_dialog);
	} else {
		gtk_widget_destroy(reagrupe_dialog);
	}
}

}
