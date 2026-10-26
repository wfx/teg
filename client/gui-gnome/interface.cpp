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
#include "gui_state.h"
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

namespace teg::client::callbacks
{

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

static void canvas_map_zoom_1(GtkWidget *widget, gpointer data)
{
	gui_state().ui.canvas_map_zoom = 1.0;
	goo_canvas_set_scale((GooCanvas*)gui_state().ui.canvas_map, gui_state().ui.canvas_map_zoom);
}

static void canvas_map_zoom_in(GtkWidget *widget, gpointer data)
{
	gui_state().ui.canvas_map_zoom *= 1.05;
	goo_canvas_set_scale((GooCanvas*)gui_state().ui.canvas_map, gui_state().ui.canvas_map_zoom);
}

static void canvas_map_zoom_out(GtkWidget *widget, gpointer data)
{
	gui_state().ui.canvas_map_zoom *= 0.95;
	goo_canvas_set_scale((GooCanvas*)gui_state().ui.canvas_map, gui_state().ui.canvas_map_zoom);
}

static TEG_STATUS canvas_create_countries(GooCanvasItem *group)
{
	int i=0;

	for(i=0; i<CONT_CANT; i++) {
		G_cont_create(i, group);
	}

	for(i=0; i<static_cast<int>(COUNTRIES_CANT); i++) {
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

	gui_state().ui.canvas_map = goo_canvas_new();
	g_object_set(gui_state().ui.canvas_map, "anchor", GOO_CANVAS_ANCHOR_CENTER, NULL);
	root = goo_canvas_get_root_item(GOO_CANVAS(gui_state().ui.canvas_map));

	auto fpath = theme_load_file(client_state(), gui_state().theme.board);
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
		goo_canvas_set_bounds(GOO_CANVAS(gui_state().ui.canvas_map), 0, 0,
		                      gdk_pixbuf_get_width(im),
		                      gdk_pixbuf_get_height(im));

		g_object_unref(im);

	} else {
		g_warning(_("Could not find the %s file"), gui_state().theme.board);
		return TEG_STATUS_ERROR;
	}

	return TEG_STATUS_SUCCESS;
}

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
	gtk_window_set_default_size(GTK_WINDOW(app1), 860, 800);

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

	gui_state().ui.connect_menu = gtk_menu_item_new_with_mnemonic(_("_Connect"));
	gtk_widget_set_tooltip_text(gui_state().ui.connect_menu, _("Connect to server"));
	gtk_menu_shell_append(GTK_MENU_SHELL(gamemenu), gui_state().ui.connect_menu);
	g_signal_connect(gui_state().ui.connect_menu, "activate",
	                 G_CALLBACK(on_connect_activate), NULL);

	gui_state().ui.disconnect_menu = gtk_menu_item_new_with_mnemonic(_("_Disconnect"));
	gtk_widget_set_tooltip_text(gui_state().ui.disconnect_menu,
	                            _("Disconnect from server"));
	gtk_menu_shell_append(GTK_MENU_SHELL(gamemenu), gui_state().ui.disconnect_menu);
	g_signal_connect(gui_state().ui.disconnect_menu, "activate",
	                 G_CALLBACK(on_disconnect_activate), NULL);

	/* Emulate GtkImageMenuItem.  This was the author's intention
	   but it doesn't work in the original code.  */
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
	robot = gdk_pixbuf_new_from_file(load_pixmap_file("robot.png").c_str(), NULL);
	icon = robot ? gtk_image_new_from_pixbuf(robot) : gtk_image_new();
	if(robot) {
		g_object_unref(robot);
	}
	label = gtk_label_new_with_mnemonic(_("_Launch robot"));
	gtk_widget_set_tooltip_text(label, _("Launch a robot"));
	gui_state().ui.robot_menu = gtk_menu_item_new();
	gtk_container_add(GTK_CONTAINER(hbox), icon);
	gtk_container_add(GTK_CONTAINER(hbox), label);
	gtk_container_add(GTK_CONTAINER(gui_state().ui.robot_menu), hbox);
	gtk_menu_shell_append(GTK_MENU_SHELL(gamemenu), gui_state().ui.robot_menu);
	g_signal_connect(gui_state().ui.robot_menu, "activate",
	                 G_CALLBACK(on_launchrobot_activate), NULL);

	separator = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(gamemenu), separator);

	gui_state().ui.scores_menu = gtk_menu_item_new_with_mnemonic(_("_Scores..."));
	gtk_widget_set_tooltip_text(gui_state().ui.scores_menu, _("View the scores"));
	gtk_menu_shell_append(GTK_MENU_SHELL(gamemenu), gui_state().ui.scores_menu);
	g_signal_connect(gui_state().ui.scores_menu, "activate",
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

	gui_state().ui.start_menu = gtk_menu_item_new_with_label(_("Start"));
	gtk_widget_set_tooltip_text(gui_state().ui.start_menu, _("Start the game"));
	gtk_menu_shell_append(GTK_MENU_SHELL(actionmenu), gui_state().ui.start_menu);
	g_signal_connect(gui_state().ui.start_menu, "activate",
	                 G_CALLBACK(on_start_activate), NULL);

	gui_state().ui.send_menu = gtk_menu_item_new_with_label(_("Send armies"));
	gtk_widget_set_tooltip_text(gui_state().ui.send_menu,
	                            _("Send the armies recently placed"));
	gtk_menu_shell_append(GTK_MENU_SHELL(actionmenu), gui_state().ui.send_menu);
	g_signal_connect(gui_state().ui.send_menu, "activate",
	                 G_CALLBACK(on_enviarfichas_activate), NULL);

	gui_state().ui.update_menu = gtk_menu_item_new_with_mnemonic(_("_Update Map"));
	gtk_widget_set_tooltip_text(gui_state().ui.update_menu,
	                            _("Show the armies that other players "
	                              "had placed"));
	gtk_menu_shell_append(GTK_MENU_SHELL(actionmenu), gui_state().ui.update_menu);
	g_signal_connect(gui_state().ui.update_menu, "activate",
	                 G_CALLBACK(on_updatemap_activate), NULL);

	gui_state().ui.reattack_menu = gtk_menu_item_new_with_mnemonic(_("_Reattack"));
	gtk_widget_set_tooltip_text(gui_state().ui.reattack_menu,
	                            _("Attack your enemy, again using same "
	                              "source and destination"));
	gtk_menu_shell_append(GTK_MENU_SHELL(actionmenu), gui_state().ui.reattack_menu);
	gtk_widget_add_accelerator(gui_state().ui.reattack_menu, "activate", accel_group,
	                           GDK_KEY_r, GDK_CONTROL_MASK,
	                           GTK_ACCEL_VISIBLE);
	g_signal_connect(gui_state().ui.reattack_menu, "activate",
	                 G_CALLBACK(on_enviarreattack_activate), NULL);

	gui_state().ui.reset_menu = gtk_menu_item_new_with_label(_("Reset the attack"));
	gtk_widget_set_tooltip_text(gui_state().ui.reset_menu,
	                            _("Start again selecting the source and "
	                              "destination country for the attack"));
	gtk_menu_shell_append(GTK_MENU_SHELL(actionmenu), gui_state().ui.reset_menu);
	g_signal_connect(gui_state().ui.reset_menu, "activate",
	                 G_CALLBACK(on_enviarattackreset_activate), NULL);

	gui_state().ui.regroup_menu = gtk_menu_item_new_with_mnemonic(_("Re_group armies"));
	gtk_widget_set_tooltip_text(gui_state().ui.regroup_menu,
	                            _("Regroup your armies, moving from "
	                              "one country to another"));
	gtk_menu_shell_append(GTK_MENU_SHELL(actionmenu), gui_state().ui.regroup_menu);
	g_signal_connect(gui_state().ui.regroup_menu, "activate",
	                 G_CALLBACK(on_reagrupe_activate), NULL);

	gui_state().ui.get_menu = gtk_menu_item_new_with_label(_("Get card"));
	gtk_widget_set_tooltip_text(gui_state().ui.get_menu, _("Get a country card"));
	gtk_menu_shell_append(GTK_MENU_SHELL(actionmenu), gui_state().ui.get_menu);
	g_signal_connect(gui_state().ui.get_menu, "activate",
	                 G_CALLBACK(on_pedirtarjeta_activate), NULL);

	gui_state().ui.end_menu = gtk_menu_item_new_with_mnemonic(_("_End turn"));
	gtk_widget_set_tooltip_text(gui_state().ui.end_menu,
	                            _("Pass the turn to another player"));
	gtk_menu_shell_append(GTK_MENU_SHELL(actionmenu), gui_state().ui.end_menu);
	g_signal_connect(gui_state().ui.end_menu, "activate",
	                 G_CALLBACK(on_endturn_activate), NULL);

	separator = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(actionmenu), separator);

	gui_state().ui.surrender_menu = gtk_menu_item_new_with_label(_("Surrender"));
	gtk_widget_set_tooltip_text(gui_state().ui.surrender_menu, _("I'm a coward"));
	gtk_menu_shell_append(GTK_MENU_SHELL(actionmenu), gui_state().ui.surrender_menu);
	g_signal_connect(gui_state().ui.surrender_menu, "activate",
	                 G_CALLBACK(on_surrender_activate), NULL);

	viewmenu = gtk_menu_new();
	gtk_menu_set_accel_group(GTK_MENU(viewmenu), accel_group);
	view_menu = gtk_menu_item_new_with_mnemonic(_("_View"));
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_menu), viewmenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), view_menu);

	gui_state().ui.players_menu = gtk_menu_item_new_with_mnemonic(_("View _players"));
	gtk_widget_set_tooltip_text(gui_state().ui.players_menu,
	                            _("View the status of the players"));
	gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), gui_state().ui.players_menu);
	g_signal_connect(gui_state().ui.players_menu, "activate",
	                 G_CALLBACK(on_status_activate), NULL);

	gui_state().ui.mission_menu = gtk_menu_item_new_with_mnemonic(_("View _mission"));
	gtk_widget_set_tooltip_text(gui_state().ui.mission_menu,
	                            _("View your mission of this game"));
	gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), gui_state().ui.mission_menu);
	g_signal_connect(gui_state().ui.mission_menu, "activate",
	                 G_CALLBACK(on_viewmission_activate), NULL);

	gui_state().ui.cards_menu = gtk_menu_item_new_with_mnemonic(_("View _cards"));
	gtk_widget_set_tooltip_text(gui_state().ui.cards_menu,
	                            _("View all the cards that you won"));
	gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), gui_state().ui.cards_menu);
	g_signal_connect(gui_state().ui.cards_menu, "activate",
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

	gui_state().ui.toolbar_main = gtk_toolbar_new();
	g_settings_bind(gui_state().settings, "toolbar-style", gui_state().ui.toolbar_main,
	                "toolbar-style", G_SETTINGS_BIND_DEFAULT);
	gtk_widget_show(gui_state().ui.toolbar_main);
	gtk_container_add(GTK_CONTAINER(dock1), gui_state().ui.toolbar_main);

	image = gtk_image_new_from_icon_name(STOCK_CONNECT,
	                                     GTK_ICON_SIZE_LARGE_TOOLBAR);
	button1 = gtk_tool_button_new(image, _("Connect"));
	gtk_tool_item_set_tooltip_text(button1, _("Connect to server"));
	gtk_toolbar_insert(GTK_TOOLBAR(gui_state().ui.toolbar_main), button1, -1);
	g_signal_connect(button1, "clicked",
	                 G_CALLBACK(on_connect_activate), NULL);

	image = gtk_image_new_from_icon_name(STOCK_STARTGAME,
	                                     GTK_ICON_SIZE_LARGE_TOOLBAR);
	button2 = gtk_tool_button_new(image, _("Start"));
	gtk_tool_item_set_tooltip_text(button2, _("Start the game"));
	gtk_toolbar_insert(GTK_TOOLBAR(gui_state().ui.toolbar_main), button2, -1);
	g_signal_connect(button2, "clicked",
	                 G_CALLBACK(on_start_activate), NULL);

	sep = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(gui_state().ui.toolbar_main), sep, -1);

	image = gtk_image_new_from_icon_name(STOCK_SENDARMIES,
	                                     GTK_ICON_SIZE_LARGE_TOOLBAR);
	button3 = gtk_tool_button_new(image, _("Send armies"));
	gtk_tool_item_set_tooltip_text(button3, _("Send your armies"));
	gtk_toolbar_insert(GTK_TOOLBAR(gui_state().ui.toolbar_main), button3, -1);
	g_signal_connect(button3, "clicked",
	                 G_CALLBACK(on_enviarfichas_activate), NULL);

	image = gtk_image_new_from_icon_name(STOCK_REGROUPARMIES,
	                                     GTK_ICON_SIZE_LARGE_TOOLBAR);
	button4 = gtk_tool_button_new(image, _("1. Regroup"));
	gtk_tool_item_set_tooltip_text(button4,
	                               _("Regroup your armies, moving from "
	                                 "one country to another"));
	gtk_toolbar_insert(GTK_TOOLBAR(gui_state().ui.toolbar_main), button4, -1);
	g_signal_connect(button4, "clicked",
	                 G_CALLBACK(on_reagrupe_activate), NULL);

	image = gtk_image_new_from_icon_name(STOCK_GETCARD,
	                                     GTK_ICON_SIZE_LARGE_TOOLBAR);
	button5 = gtk_tool_button_new(image, _("2. Get card"));
	gtk_tool_item_set_tooltip_text(button5, _("Get a country card"));
	gtk_toolbar_insert(GTK_TOOLBAR(gui_state().ui.toolbar_main), button5, -1);
	g_signal_connect(button5, "clicked",
	                 G_CALLBACK(on_pedirtarjeta_activate), NULL);

	image = gtk_image_new_from_icon_name(STOCK_ENDTURN,
	                                     GTK_ICON_SIZE_LARGE_TOOLBAR);
	button6 = gtk_tool_button_new(image, _("3. End turn"));
	gtk_tool_item_set_tooltip_text(button6,
	                               _("Pass the turn to another player"));
	gtk_toolbar_insert(GTK_TOOLBAR(gui_state().ui.toolbar_main), button6, -1);
	g_signal_connect(button6, "clicked",
	                 G_CALLBACK(on_endturn_activate), NULL);

	sep = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(gui_state().ui.toolbar_main), sep, -1);

	image = gtk_image_new_from_icon_name(STOCK_VIEWPLAYERS,
	                                     GTK_ICON_SIZE_LARGE_TOOLBAR);
	button7 = gtk_tool_button_new(image, _("View players"));
	gtk_tool_item_set_tooltip_text(button7, _("View players' status"));
	gtk_toolbar_insert(GTK_TOOLBAR(gui_state().ui.toolbar_main), button7, -1);
	g_signal_connect(button7, "clicked",
	                 G_CALLBACK(on_status_activate), NULL);

	gtk_widget_show_all(gui_state().ui.toolbar_main);

	gtk_widget_realize(app1);

	g_signal_connect(G_OBJECT(app1), "destroy",
	                 G_CALLBACK(on_exit_activate), NULL);

	g_signal_connect(G_OBJECT(app1), "delete_event",
	                 G_CALLBACK(on_exit_activate), NULL);


	gui_state().ui.sensi.connect_button = GTK_WIDGET(button1);
	gui_state().ui.sensi.start_button = GTK_WIDGET(button2);
	/* 2 is a separator */
	gui_state().ui.sensi.enviarfichas_button = GTK_WIDGET(button3);
	gui_state().ui.sensi.reagrupe_button = GTK_WIDGET(button4);
	gui_state().ui.sensi.getcard_button = GTK_WIDGET(button5);
	gui_state().ui.sensi.endturn_button = GTK_WIDGET(button6);
	/* 7 is a separator */
	gui_state().ui.sensi.status_button = GTK_WIDGET(button7);

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
	if(mainstatus_create(&gui_state().ui.window_mainstatus) != TEG_STATUS_SUCCESS) {
		goto error;
	}
	gtk_box_pack_start(GTK_BOX(vbox_main), gui_state().ui.window_mainstatus, FALSE, FALSE, 0);

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
	        (scrolledwindow), 860);
	gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW
	        (scrolledwindow), 600);

	if(canvas_create_map() != TEG_STATUS_SUCCESS) {
		goto error;
	}

	gtk_container_add(GTK_CONTAINER(scrolledwindow), gui_state().ui.canvas_map);
	gtk_box_pack_start(GTK_BOX(vbox_paned), scrolledwindow, TRUE, TRUE, 0);
	gtk_widget_show(gui_state().ui.canvas_map);
	gtk_widget_show(scrolledwindow);


	/* text */
	ahbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	gtk_paned_pack2(GTK_PANED(paned), ahbox, TRUE, TRUE);

	sw = gtk_scrolled_window_new(NULL, NULL);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_AUTOMATIC,
	                               GTK_POLICY_ALWAYS);
	gtk_box_pack_start(GTK_BOX(ahbox), sw, TRUE, TRUE, 0);

	gtk_widget_set_size_request(sw, 860, 60);

	text = gtk_text_view_new();
	gtk_text_view_set_editable(GTK_TEXT_VIEW(text), FALSE);
	gtk_container_add(GTK_CONTAINER(sw), text);

	gtk_widget_set_name(text, "chatline");

	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text), GTK_WRAP_WORD);
	gtk_widget_realize(text);
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(text), 5);

	gui_state().ui.main_message_area = GTK_TEXT_VIEW(text);

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
	gui_state().ui.statusbar = gtk_statusbar_new();
	g_object_ref(gui_state().ui.statusbar);
	g_object_set_data_full(G_OBJECT(app1), "statusbar", gui_state().ui.statusbar,
	                       (GDestroyNotify) g_object_unref);
	gtk_widget_show(gui_state().ui.statusbar);
	gtk_container_add(GTK_CONTAINER(dock1), gui_state().ui.statusbar);

	gtk_box_pack_start(GTK_BOX(gui_state().ui.statusbar),
	                   ministatus_build(), FALSE, TRUE, 0);


	gtk_statusbar_push(GTK_STATUSBAR(gui_state().ui.statusbar), 0,
	                   _("Welcome to TEG!"));

	/* icono */
	{
		GdkPixbuf *icono
		    = gdk_pixbuf_new_from_file(load_pixmap_file("teg_icono.png").c_str(), NULL);
		gtk_widget_realize(app1);
		if(icono) {
			gtk_window_set_icon(GTK_WINDOW(app1), icono);
			g_object_unref(icono);
		}
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
	PLAYER_STATUS e = client_status(client_state());

	/* CONNECT & START */
	if(e == PLAYER_STATUS_DESCONECTADO) {
		gtk_widget_set_sensitive(gui_state().ui.connect_menu, TRUE);
		gtk_widget_set_sensitive(gui_state().ui.disconnect_menu, FALSE);
		gtk_widget_set_sensitive(gui_state().ui.start_menu, FALSE);
		gtk_widget_set_sensitive(gui_state().ui.sensi.connect_button, TRUE);
		gtk_widget_set_sensitive(gui_state().ui.sensi.start_button, FALSE);

	} else if(e == PLAYER_STATUS_HABILITADO) {
		gtk_widget_set_sensitive(gui_state().ui.connect_menu, FALSE);
		gtk_widget_set_sensitive(gui_state().ui.disconnect_menu, TRUE);
		gtk_widget_set_sensitive(gui_state().ui.start_menu, TRUE);
		gtk_widget_set_sensitive(gui_state().ui.sensi.connect_button, FALSE);
		gtk_widget_set_sensitive(gui_state().ui.sensi.start_button, TRUE);

	} else {
		gtk_widget_set_sensitive(gui_state().ui.connect_menu, FALSE);
		gtk_widget_set_sensitive(gui_state().ui.disconnect_menu, TRUE);
		gtk_widget_set_sensitive(gui_state().ui.start_menu, FALSE);
		gtk_widget_set_sensitive(gui_state().ui.sensi.connect_button, FALSE);
		gtk_widget_set_sensitive(gui_state().ui.sensi.start_button, FALSE);
	}

	if(client_state().game.observer) {
		gtk_widget_set_sensitive(gui_state().ui.start_menu, FALSE);
		gtk_widget_set_sensitive(gui_state().ui.sensi.start_button, FALSE);
	}

	/* STATUS */
	if(e == PLAYER_STATUS_DESCONECTADO) {
		gtk_widget_set_sensitive(gui_state().ui.players_menu, FALSE);
		gtk_widget_set_sensitive(gui_state().ui.sensi.status_button, FALSE);
	} else {
		gtk_widget_set_sensitive(gui_state().ui.players_menu, TRUE);
		gtk_widget_set_sensitive(gui_state().ui.sensi.status_button, TRUE);
	}

	/* ATAQUE */
	if(e == PLAYER_STATUS_ATAQUE) {
		gtk_widget_set_sensitive(gui_state().ui.reattack_menu, TRUE);
		gtk_widget_set_sensitive(gui_state().ui.reset_menu, TRUE);
	} else {
		gtk_widget_set_sensitive(gui_state().ui.reattack_menu, FALSE);
		gtk_widget_set_sensitive(gui_state().ui.reset_menu, FALSE);
	}

	/* ENVIAR FICHAS */
	if(e==PLAYER_STATUS_FICHAS || e==PLAYER_STATUS_FICHAS2 || e==PLAYER_STATUS_FICHASC) {
		gtk_widget_set_sensitive(gui_state().ui.send_menu, TRUE);
		gtk_widget_set_sensitive(gui_state().ui.sensi.enviarfichas_button, TRUE);
	} else {
		gtk_widget_set_sensitive(gui_state().ui.send_menu, FALSE);
		gtk_widget_set_sensitive(gui_state().ui.sensi.enviarfichas_button, FALSE);
	}

	/* REAGRUPE */
	if(e==PLAYER_STATUS_ATAQUE || e==PLAYER_STATUS_TROPAS) {
		gtk_widget_set_sensitive(gui_state().ui.regroup_menu, TRUE);
		gtk_widget_set_sensitive(gui_state().ui.sensi.reagrupe_button, TRUE);
	} else {
		gtk_widget_set_sensitive(gui_state().ui.regroup_menu, FALSE);
		gtk_widget_set_sensitive(gui_state().ui.sensi.reagrupe_button, FALSE);
	}

	/* GET CARD */
	if(e==PLAYER_STATUS_ATAQUE || e==PLAYER_STATUS_TROPAS || e==PLAYER_STATUS_REAGRUPE) {
		gtk_widget_set_sensitive(gui_state().ui.get_menu, TRUE);
		gtk_widget_set_sensitive(gui_state().ui.sensi.getcard_button, TRUE);
	} else {
		gtk_widget_set_sensitive(gui_state().ui.get_menu, FALSE);
		gtk_widget_set_sensitive(gui_state().ui.sensi.getcard_button, FALSE);
	}

	/* END TURN */
	if(e==PLAYER_STATUS_ATAQUE || e==PLAYER_STATUS_TROPAS || e==PLAYER_STATUS_REAGRUPE || e==PLAYER_STATUS_TARJETA) {
		gtk_widget_set_sensitive(gui_state().ui.end_menu, TRUE);
		gtk_widget_set_sensitive(gui_state().ui.sensi.endturn_button, TRUE);
	} else {
		gtk_widget_set_sensitive(gui_state().ui.end_menu, FALSE);
		gtk_widget_set_sensitive(gui_state().ui.sensi.endturn_button, FALSE);
	}

	/* UPDATE MAP */
	if(e >= PLAYER_STATUS_START || (client_state().game.observer && e >= PLAYER_STATUS_CONNECTED)) {
		gtk_widget_set_sensitive(gui_state().ui.update_menu, TRUE);
	} else {
		gtk_widget_set_sensitive(gui_state().ui.update_menu, FALSE);
	}

	/* VIEW CARDS, MISSION */
	if(e >= PLAYER_STATUS_START) {
		gtk_widget_set_sensitive(gui_state().ui.mission_menu, TRUE);
		gtk_widget_set_sensitive(gui_state().ui.cards_menu, TRUE);
	} else {
		gtk_widget_set_sensitive(gui_state().ui.mission_menu, FALSE);
		gtk_widget_set_sensitive(gui_state().ui.cards_menu, FALSE);
	}

	/* LAUNCH ROBOT */
	if(e >= PLAYER_STATUS_CONNECTED && e < PLAYER_STATUS_START) {
		gtk_widget_set_sensitive(gui_state().ui.robot_menu, TRUE);
	} else {
		gtk_widget_set_sensitive(gui_state().ui.robot_menu, FALSE);
	}

	/* SURRENDER */
	if(e>=PLAYER_STATUS_START) {
		gtk_widget_set_sensitive(gui_state().ui.surrender_menu, TRUE);
	} else {
		gtk_widget_set_sensitive(gui_state().ui.surrender_menu, FALSE);
	}

	/* SCORES */
	if(e >= PLAYER_STATUS_CONNECTED) {
		gtk_widget_set_sensitive(gui_state().ui.scores_menu, TRUE);
	} else {
		gtk_widget_set_sensitive(gui_state().ui.scores_menu, FALSE);
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
		if(GTK_IS_RANGE(gui_state().ui.tropas_hscale_cant))
			gui_state().ui.tropas_cant = (gint) gtk_adjustment_get_value
			                             (gtk_range_get_adjustment
			                              (GTK_RANGE(gui_state().ui.tropas_hscale_cant)));

		if(gui_state().ui.tropas_cant > 0) {
			out_tropas(client_state(), gui_state().ui.tropas_src, gui_state().ui.tropas_dst, gui_state().ui.tropas_cant);
		}
	}

	gtk_widget_destroy(GTK_WIDGET(dialog));
	gui_state().ui.tropas_hscale_cant = nullptr;
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

	gui_state().ui.tropas_src = src;
	gui_state().ui.tropas_dst = dst;

	gui_state().ui.tropas_cant = 0;

	gui_state().ui.tropas_dialog = teg_dialog_new(_("A country was conquered"), _("A country was conquered"));
	gtk_dialog_add_button(GTK_DIALOG(gui_state().ui.tropas_dialog), _("_OK"),
	                      GTK_RESPONSE_OK);

	gtk_window_set_transient_for(GTK_WINDOW(gui_state().ui.tropas_dialog),
	                             GTK_WINDOW(gui_state().ui.main_window));

	g_signal_connect(G_OBJECT(gui_state().ui.tropas_dialog), "destroy",
	                 G_CALLBACK(gtk_widget_destroyed), &gui_state().ui.tropas_dialog);

	g_signal_connect(gui_state().ui.tropas_dialog, "response",
	                 G_CALLBACK(tropas_response_cb), NULL);

	vbox_dia = gtk_dialog_get_content_area(GTK_DIALOG(gui_state().ui.tropas_dialog));

	table = gtk_grid_new();
	gtk_container_set_border_width(GTK_CONTAINER(table), GNOME_PAD);
	gtk_grid_set_row_spacing(GTK_GRID(table), GNOME_PAD);
	gtk_grid_set_column_spacing(GTK_GRID(table), GNOME_PAD);

	gtk_box_pack_start(GTK_BOX(vbox_dia), table, TRUE, TRUE, 0);

	frame = gtk_frame_new(_("A new country"));
	gtk_container_set_border_width(GTK_CONTAINER(frame), 0);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), GNOME_PAD);

	snprintf(buf, sizeof(buf)-1, _("You conquered '%s' in your attack from '%s'!!\n"), countries_get_name(client_state().game.common, dst), countries_get_name(client_state().game.common, src));
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
		gui_state().ui.tropas_hscale_cant = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL,
		                                    adj);
		gtk_box_pack_start(GTK_BOX(hbox), gui_state().ui.tropas_hscale_cant, TRUE, TRUE, 0);
		gtk_scale_set_digits(GTK_SCALE(gui_state().ui.tropas_hscale_cant), 0);
		gtk_scale_set_draw_value(GTK_SCALE(gui_state().ui.tropas_hscale_cant), 1);
		/*		gtk_range_draw_trough(GTK_RANGE(gui_state().ui.tropas_hscale_cant));  */

		snprintf(buf, sizeof(buf) -1, _("[Note: The server moved 1 army to '%s' for you.\nChoose how many more armies you want to move]"), countries_get_name(client_state().game.common, dst));
		buf[ sizeof(buf) -1 ] = 0;
		label = gtk_label_new(buf);
		gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);
	}

	gtk_container_add(GTK_CONTAINER(frame), vbox);

	gtk_grid_attach(GTK_GRID(table), frame, 0, 0, 1, 1);


	if(!gtk_widget_get_visible(gui_state().ui.tropas_dialog)) {
		gtk_widget_show_all(gui_state().ui.tropas_dialog);
	} else {
		gtk_widget_destroy(gui_state().ui.tropas_dialog);
	}
}

/* regroup */
static void reagrupe_response_cb(GtkDialog *dialog, gint id, gpointer data)
{
	if(id == GTK_RESPONSE_OK) {
		if(GTK_IS_RANGE(gui_state().ui.reagrupe_hscale_cant))
			gui_state().ui.reagrupe_cant = (gint) gtk_adjustment_get_value
			                               (gtk_range_get_adjustment
			                                (GTK_RANGE(gui_state().ui.reagrupe_hscale_cant)));

		if(gui_state().ui.reagrupe_cant > 0) {
			reagrupe_out(client_state(), gui_state().ui.reagrupe_src, gui_state().ui.reagrupe_dst,
			             gui_state().ui.reagrupe_cant);
		} else {
			reagrupe_reset(client_state());
		}
	}

	gtk_widget_destroy(GTK_WIDGET(gui_state().ui.reagrupe_dialog));
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

	gui_state().ui.reagrupe_src = src;
	gui_state().ui.reagrupe_dst = dst;

	gui_state().ui.reagrupe_cant = 0;

	gui_state().ui.reagrupe_dialog = teg_dialog_new(_("Regrouping armies"), _("Regroup your armies"));

	gtk_dialog_add_button(GTK_DIALOG(gui_state().ui.reagrupe_dialog), _("_OK"),
	                      GTK_RESPONSE_OK);

	gtk_window_set_transient_for(GTK_WINDOW(gui_state().ui.reagrupe_dialog),
	                             GTK_WINDOW(gui_state().ui.main_window));

	g_signal_connect(G_OBJECT(gui_state().ui.reagrupe_dialog), "destroy",
	                 G_CALLBACK(gtk_widget_destroyed), &gui_state().ui.reagrupe_dialog);

	g_signal_connect(gui_state().ui.reagrupe_dialog, "response",
	                 G_CALLBACK(reagrupe_response_cb), NULL);

	vbox_dia = gtk_dialog_get_content_area(GTK_DIALOG(gui_state().ui.reagrupe_dialog));

	table = gtk_grid_new();
	gtk_container_set_border_width(GTK_CONTAINER(table), GNOME_PAD);
	gtk_grid_set_row_spacing(GTK_GRID(table), GNOME_PAD);
	gtk_grid_set_column_spacing(GTK_GRID(table), GNOME_PAD);

	gtk_box_pack_start(GTK_BOX(vbox_dia), table, TRUE, TRUE, 0);

	frame = gtk_frame_new(_("Regrouping"));
	gtk_container_set_border_width(GTK_CONTAINER(frame), 0);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), GNOME_PAD);

	snprintf(buf, sizeof(buf) -1, _("Moving armies from '%s' to '%s'\n"), countries_get_name(client_state().game.common, src), countries_get_name(client_state().game.common, dst));
	buf[ sizeof(buf) -1 ] = 0;
	label = gtk_label_new(buf);
	gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_container_add(GTK_CONTAINER(vbox), hbox);
	label = gtk_label_new(_("Armies to move"));
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	adj = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, (gfloat)(cant + 1), 1.0, 1.0, 1.0);

	gui_state().ui.reagrupe_hscale_cant = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, adj);

	gtk_box_pack_start(GTK_BOX(hbox), gui_state().ui.reagrupe_hscale_cant, TRUE, TRUE, 0);
	gtk_scale_set_digits(GTK_SCALE(gui_state().ui.reagrupe_hscale_cant), 0);
	gtk_scale_set_draw_value(GTK_SCALE(gui_state().ui.reagrupe_hscale_cant), 1);

	snprintf(buf, sizeof(buf) -1, _("[Note: You can regroup as many times as you want,\nas long as you dont regroup an armie that were\nregrouped before in this turn.]"));
	buf[ sizeof(buf) -1 ] = 0;
	label = gtk_label_new(buf);
	gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(frame), vbox);

	gtk_grid_attach(GTK_GRID(table), frame, 0, 0, 1, 1);


	if(!gtk_widget_get_visible(gui_state().ui.reagrupe_dialog)) {
		gtk_widget_show_all(gui_state().ui.reagrupe_dialog);
	} else {
		gtk_widget_destroy(gui_state().ui.reagrupe_dialog);
	}
}

}
