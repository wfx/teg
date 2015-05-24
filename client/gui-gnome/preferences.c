/*	$Id: preferences.c,v 1.24 2002/10/08 04:35:57 riq Exp $	*/
/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2001 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <riq@core-sdi.com>
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
 * @file preferences.c
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include "client.h"
#include "gui.h"
#include "priv.h"
#include "interface.h"
#include "support.h"
#include "status.h"

extern TTheme gui_theme;

static GtkWidget	*pref_dialog=NULL;

/** messages */
static GtkWidget	*conf_cb_showerr=NULL;
static GtkWidget	*conf_cb_showimp=NULL;
static GtkWidget	*conf_cb_showmsg=NULL;
static GtkWidget	*conf_cb_showmsgcolor=NULL;
static GtkWidget	*conf_cb_showinf=NULL;

/** dialogs */
static GtkWidget	*conf_cb_showattackturn=NULL;
static GtkWidget	*conf_cb_showplacearmies=NULL;
static GtkWidget	*conf_cb_showregrouparmies=NULL;

/** themes */
static char		*theme_activated=NULL;
static GtkWidget	*theme_widget=NULL;
static GtkWidget	*theme_frame_prev=NULL;

/** status */
static GtkWidget	*conf_cb_stscolor=NULL;
static GtkWidget	*conf_cb_stsnumber=NULL;
static GtkWidget	*conf_cb_stsname=NULL;
static GtkWidget	*conf_cb_stsscore=NULL;
static GtkWidget	*conf_cb_stsaddress=NULL;
static GtkWidget	*conf_cb_stshuman=NULL;
static GtkWidget	*conf_cb_stscountries=NULL;
static GtkWidget	*conf_cb_stsarmies=NULL;
static GtkWidget	*conf_cb_stscards=NULL;
static GtkWidget	*conf_cb_stsstatus=NULL;
static GtkWidget	*conf_cb_stswho=NULL;

/** robot **/
static GtkWidget	*conf_cb_robotclient=NULL;
static GtkWidget	*conf_cb_robotserver=NULL;


static void free_pixmap( GtkWidget *widget, void *data )
{
	if( theme_widget )
		gtk_widget_destroy( theme_widget );
	theme_widget = NULL;
}

static void
load_preview( char *theme )
{
	char *filename;

	if( theme_widget ) {
		gtk_widget_destroy( theme_widget );
		theme_widget = NULL;
	}

	filename = theme_load_fake_file("mini_shot.png",theme);
	theme_widget = gtk_image_new_from_file(filename);
	if( theme_widget ) {
		gtk_container_add( GTK_CONTAINER(theme_frame_prev), theme_widget );
		gtk_widget_show(theme_widget);
		gtk_signal_connect (GTK_OBJECT(theme_widget), "destroy",
			GTK_SIGNAL_FUNC (free_pixmap), NULL);
	} else
		g_warning (_("Could not find the %s file"),"mini_shot.png");
}

static void prop_box_changed_callback (GtkWidget *widget, gpointer data)
{
	if(pref_dialog==NULL)
		return;
	gnome_property_box_changed (GNOME_PROPERTY_BOX (pref_dialog));
}

static void theme_activated_callback (GtkWidget *widget, gpointer data)
{
	theme_activated = data;
	load_preview(data);
	prop_box_changed_callback (widget,data);
}

static void
free_str (GtkWidget *widget, void *data)
{
	free (data);
}



static void apply_cb (GtkWidget *widget, gint pagenum, gpointer data)
{
	if (pagenum != -1)
		return;
 
	/** dialogs **/


	if (GTK_TOGGLE_BUTTON(conf_cb_showattackturn)->active)
		gui_private.dialog_show |= (1 << DIALOG_ATTACK_TURN);
	else
		gui_private.dialog_show &= ~(1 << DIALOG_ATTACK_TURN);

	if (GTK_TOGGLE_BUTTON(conf_cb_showplacearmies)->active)
		gui_private.dialog_show |= (1 << DIALOG_PLACE_ARMIES);
	else
		gui_private.dialog_show &= ~(1 << DIALOG_PLACE_ARMIES);
	
	if (GTK_TOGGLE_BUTTON(conf_cb_showregrouparmies)->active)
		gui_private.dialog_show |= (1 << DIALOG_REGROUP_ARMIES);
	else
		gui_private.dialog_show &= ~(1 << DIALOG_REGROUP_ARMIES);
	

	/** messages **/
	if (GTK_TOGGLE_BUTTON( conf_cb_showerr )->active)
		g_game.msg_show |= M_ERR;
	else
		g_game.msg_show &= ~M_ERR;


	if (GTK_TOGGLE_BUTTON( conf_cb_showimp )->active)
		g_game.msg_show |= M_IMP;
	else
		g_game.msg_show &= ~M_IMP;


	if (GTK_TOGGLE_BUTTON( conf_cb_showmsg )->active)
		g_game.msg_show |= M_MSG;
	else
		g_game.msg_show &= ~M_MSG;


	if (GTK_TOGGLE_BUTTON( conf_cb_showmsgcolor )->active)
		gui_private.msg_show_colors = 1;
	else
		gui_private.msg_show_colors = 0;


	if (GTK_TOGGLE_BUTTON( conf_cb_showinf )->active)
		g_game.msg_show |= M_INF;
	else
		g_game.msg_show &= ~M_INF;

	/** status **/
	if (GTK_TOGGLE_BUTTON( conf_cb_stscolor)->active)
		gui_private.status_show |= (1 << STATUS_COLUMN_COLOR);
	else
		gui_private.status_show &= ~(1 << STATUS_COLUMN_COLOR);

	if (GTK_TOGGLE_BUTTON( conf_cb_stsnumber)->active)
		gui_private.status_show |= (1 << STATUS_COLUMN_NUMBER);
	else
		gui_private.status_show &= ~(1 << STATUS_COLUMN_NUMBER);

	if (GTK_TOGGLE_BUTTON( conf_cb_stsname)->active)
		gui_private.status_show |= (1 << STATUS_COLUMN_NAME);
	else
		gui_private.status_show &= ~(1 << STATUS_COLUMN_NAME);

	if (GTK_TOGGLE_BUTTON( conf_cb_stsscore)->active)
		gui_private.status_show |= (1 << STATUS_COLUMN_SCORE);
	else
		gui_private.status_show &= ~(1 << STATUS_COLUMN_SCORE);

	if (GTK_TOGGLE_BUTTON( conf_cb_stsaddress )->active)
		gui_private.status_show |= (1 << STATUS_COLUMN_ADDR);
	else
		gui_private.status_show &= ~(1 << STATUS_COLUMN_ADDR);

	if (GTK_TOGGLE_BUTTON( conf_cb_stshuman)->active)
		gui_private.status_show |= (1 << STATUS_COLUMN_HUMAN);
	else
		gui_private.status_show &= ~(1 << STATUS_COLUMN_HUMAN);

	if (GTK_TOGGLE_BUTTON( conf_cb_stscountries)->active)
		gui_private.status_show |= (1 << STATUS_COLUMN_COUNTRIES);
	else
		gui_private.status_show &= ~(1 << STATUS_COLUMN_COUNTRIES);

	if (GTK_TOGGLE_BUTTON( conf_cb_stsarmies)->active)
		gui_private.status_show |= (1 << STATUS_COLUMN_ARMIES);
	else
		gui_private.status_show &= ~(1 << STATUS_COLUMN_ARMIES);

	if (GTK_TOGGLE_BUTTON( conf_cb_stscards)->active)
		gui_private.status_show |= (1 << STATUS_COLUMN_CARDS);
	else
		gui_private.status_show &= ~(1 << STATUS_COLUMN_CARDS);

	if (GTK_TOGGLE_BUTTON( conf_cb_stsstatus)->active)
		gui_private.status_show |= (1 << STATUS_COLUMN_STATUS);
	else
		gui_private.status_show &= ~(1 << STATUS_COLUMN_STATUS);

	if (GTK_TOGGLE_BUTTON( conf_cb_stswho)->active)
		gui_private.status_show |= (1 << STATUS_COLUMN_WHO);
	else
		gui_private.status_show &= ~(1 << STATUS_COLUMN_WHO);

	status_update_visibility_of_columns();


	/* robot */
	g_game.robot_in_server = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(conf_cb_robotserver) );

	
	/* save new configurations */
	gconf_client_set_int( g_conf_client, "/apps/teg/msgshow",g_game.msg_show, NULL);
	gconf_client_set_bool( g_conf_client, "/apps/teg/msgshow_with_color",gui_private.msg_show_colors, NULL);
	gconf_client_set_int( g_conf_client, "/apps/teg/status_show",gui_private.status_show, NULL);
	gconf_client_set_int( g_conf_client, "/apps/teg/dialog_show",gui_private.dialog_show, NULL);
	gconf_client_set_bool( g_conf_client, "/apps/teg/robot_in_server",g_game.robot_in_server, NULL);

	if( theme_activated ){
		gconf_client_set_string( g_conf_client, "/apps/teg/theme",theme_activated, NULL);
		if( strcmp(theme_activated,g_game.theme) )
			gnome_ok_dialog_parented(_("You have to restart TEG to use the new theme."),GTK_WINDOW(main_window));
	}
}



static void
fill_menu (GtkWidget *menu)
{
	TInfo Info;
	pTInfo pI;
	char *s;
	GtkWidget *item;
	int i=0;

	if( theme_enum_themes(&Info) != TEG_STATUS_SUCCESS ) {
		textmsg(M_ERR,_("Error while loading information about themes!"));
		return;
	}

	for( pI=&Info; pI != NULL; pI = pI->next ) {
		s = strdup (pI->name);
		item = gtk_menu_item_new_with_label (s);
		gtk_widget_show (item);
		gtk_menu_append (GTK_MENU(menu), item);
		gtk_signal_connect (GTK_OBJECT(item), "activate",
			    GTK_SIGNAL_FUNC (theme_activated_callback),s);

		gtk_signal_connect (GTK_OBJECT(item), "destroy",
			    GTK_SIGNAL_FUNC (free_str), s);

		if( !strcmp(pI->name, g_game.theme ))
			gtk_menu_set_active(GTK_MENU(menu), i);
		i++;
	}
}



/**
 * @fn void preferences_activate(void)
 */
void preferences_activate(void)
{
	GtkWidget *label;
	GtkWidget *msg_frame;
	GtkWidget *sts_frame;
	GtkWidget *dialog_frame;
	GtkWidget *robot_frame;
	GtkWidget *theme_frame_sel, *theme_vbox;
	GtkWidget *vbox, *hbox;
	GtkWidget *menu, *omenu;

	if( pref_dialog != NULL ) {
		gdk_window_show( pref_dialog->window);
		gdk_window_raise( pref_dialog->window);
		return ;
	}


	/* Theme options */
	theme_vbox = gtk_vbox_new (FALSE, 0);
	
	theme_frame_sel = gtk_frame_new (_("Select theme"));
	gtk_container_border_width (GTK_CONTAINER (theme_frame_sel), GNOME_PAD);
	omenu = gtk_option_menu_new ();
	menu = gtk_menu_new ();
	fill_menu (menu);
	gtk_widget_show (omenu);
	gtk_option_menu_set_menu (GTK_OPTION_MENU(omenu), menu);
	label = gtk_label_new (_("Select theme"));
	gtk_widget_show (label);

	hbox = gtk_hbox_new (TRUE, 0);
	gtk_container_border_width (GTK_CONTAINER (hbox), GNOME_PAD);

	gtk_box_pack_start( GTK_BOX( hbox ), label, FALSE, FALSE, 0);
	gtk_box_pack_start( GTK_BOX( hbox ), omenu, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (theme_frame_sel), hbox);
	gtk_widget_show(theme_frame_sel);

	theme_frame_prev = gtk_frame_new (_("Preview"));
	gtk_container_border_width (GTK_CONTAINER (theme_frame_prev), GNOME_PAD);
	gtk_widget_show(theme_frame_prev);

	load_preview(g_game.theme);

	gtk_box_pack_start( GTK_BOX( theme_vbox ), theme_frame_sel, FALSE, FALSE, 0);
	gtk_box_pack_start( GTK_BOX( theme_vbox ), theme_frame_prev, FALSE, FALSE, 0);

	/* Message Options */
	msg_frame = gtk_frame_new (_("Console Messages"));
	gtk_container_border_width (GTK_CONTAINER (msg_frame), GNOME_PAD);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_border_width (GTK_CONTAINER (vbox), GNOME_PAD);


	conf_cb_showerr = gtk_check_button_new_with_label(_("Show Error Messages"));
	GTK_TOGGLE_BUTTON(conf_cb_showerr)->active = (g_game.msg_show & M_ERR) ?1:0;
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_showerr, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (conf_cb_showerr), "clicked", GTK_SIGNAL_FUNC
			(prop_box_changed_callback), NULL);

	conf_cb_showimp = gtk_check_button_new_with_label(_("Show Important Messages"));
	GTK_TOGGLE_BUTTON(conf_cb_showimp)->active = (g_game.msg_show & M_IMP) ?1:0;
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_showimp, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (conf_cb_showimp), "clicked", GTK_SIGNAL_FUNC
			(prop_box_changed_callback), NULL);

	conf_cb_showmsg = gtk_check_button_new_with_label(_("Show Players Messages"));
	GTK_TOGGLE_BUTTON(conf_cb_showmsg)->active = (g_game.msg_show & M_MSG) ?1:0;
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_showmsg, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (conf_cb_showmsg), "clicked", GTK_SIGNAL_FUNC
			(prop_box_changed_callback), NULL);

	conf_cb_showmsgcolor = gtk_check_button_new_with_label(_("Show Players Messages with colors"));
	GTK_TOGGLE_BUTTON(conf_cb_showmsgcolor)->active = (gui_private.msg_show_colors & 1) ?1:0;
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_showmsgcolor, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (conf_cb_showmsgcolor), "clicked", GTK_SIGNAL_FUNC
			(prop_box_changed_callback), NULL);

	conf_cb_showinf = gtk_check_button_new_with_label(_("Show Informative Messages"));
	GTK_TOGGLE_BUTTON(conf_cb_showinf)->active = (g_game.msg_show & M_INF) ?1:0;
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_showinf, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (conf_cb_showinf), "clicked", GTK_SIGNAL_FUNC
			(prop_box_changed_callback), NULL);

	gtk_container_add (GTK_CONTAINER (msg_frame), vbox);


	/** Status options **/

	sts_frame = gtk_frame_new (_("Status Of Players"));
	gtk_container_border_width (GTK_CONTAINER (sts_frame), GNOME_PAD);

	hbox = gtk_hbox_new (TRUE, 0);
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_border_width (GTK_CONTAINER (hbox), GNOME_PAD);
	gtk_container_border_width (GTK_CONTAINER (vbox), 0);


	conf_cb_stscolor = gtk_check_button_new_with_label(_("Show Color"));
	GTK_TOGGLE_BUTTON(conf_cb_stscolor)->active = (gui_private.status_show & (1 <<STATUS_COLUMN_COLOR) ) ?1:0;
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_stscolor, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (conf_cb_stscolor), "clicked", GTK_SIGNAL_FUNC
			(prop_box_changed_callback), NULL);


	conf_cb_stsnumber = gtk_check_button_new_with_label(_("Show Player Number"));
	GTK_TOGGLE_BUTTON(conf_cb_stsnumber)->active = (gui_private.status_show & (1 << STATUS_COLUMN_NUMBER) ) ?1:0;
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_stsnumber, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (conf_cb_stsnumber), "clicked", GTK_SIGNAL_FUNC
			(prop_box_changed_callback), NULL);


	conf_cb_stsname = gtk_check_button_new_with_label(_("Show Name"));
	GTK_TOGGLE_BUTTON(conf_cb_stsname)->active = (gui_private.status_show & (1 <<STATUS_COLUMN_NAME) ) ?1:0;
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_stsname, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (conf_cb_stsname), "clicked", GTK_SIGNAL_FUNC
			(prop_box_changed_callback), NULL);


	conf_cb_stsscore= gtk_check_button_new_with_label(_("Show Score"));
	GTK_TOGGLE_BUTTON(conf_cb_stsscore)->active = (gui_private.status_show & (1 << STATUS_COLUMN_SCORE) ) ?1:0;
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_stsscore, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (conf_cb_stsscore), "clicked", GTK_SIGNAL_FUNC
			(prop_box_changed_callback), NULL);

	conf_cb_stsaddress = gtk_check_button_new_with_label(_("Show IP Address"));
	GTK_TOGGLE_BUTTON(conf_cb_stsaddress)->active = (gui_private.status_show & (1 << STATUS_COLUMN_ADDR) ) ?1:0;
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_stsaddress, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (conf_cb_stsaddress), "clicked", GTK_SIGNAL_FUNC
			(prop_box_changed_callback), NULL);

	conf_cb_stshuman= gtk_check_button_new_with_label(_("Show Human/Robot"));
	GTK_TOGGLE_BUTTON(conf_cb_stshuman)->active = (gui_private.status_show & (1 << STATUS_COLUMN_HUMAN) ) ?1:0;
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_stshuman, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (conf_cb_stshuman), "clicked", GTK_SIGNAL_FUNC
			(prop_box_changed_callback), NULL);

	// now start the left box
	gtk_box_pack_start ( GTK_BOX (hbox), vbox, FALSE, TRUE, 0);
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_border_width (GTK_CONTAINER (vbox), 0);


	conf_cb_stscountries = gtk_check_button_new_with_label(_("Show Countries"));
	GTK_TOGGLE_BUTTON(conf_cb_stscountries)->active = (gui_private.status_show & (1 << STATUS_COLUMN_COUNTRIES) ) ?1:0;
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_stscountries, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (conf_cb_stscountries), "clicked", GTK_SIGNAL_FUNC
			(prop_box_changed_callback), NULL);

	conf_cb_stsarmies = gtk_check_button_new_with_label(_("Show Armies"));
	GTK_TOGGLE_BUTTON(conf_cb_stsarmies)->active = (gui_private.status_show & (1 << STATUS_COLUMN_ARMIES) ) ?1:0;
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_stsarmies, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (conf_cb_stsarmies), "clicked", GTK_SIGNAL_FUNC
			(prop_box_changed_callback), NULL);

	conf_cb_stscards = gtk_check_button_new_with_label(_("Show Cards"));
	GTK_TOGGLE_BUTTON(conf_cb_stscards)->active = (gui_private.status_show & (1 << STATUS_COLUMN_CARDS) ) ?1:0;
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_stscards, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (conf_cb_stscards), "clicked", GTK_SIGNAL_FUNC
			(prop_box_changed_callback), NULL);

	conf_cb_stsstatus = gtk_check_button_new_with_label(_("Show Status"));
	GTK_TOGGLE_BUTTON(conf_cb_stsstatus)->active = (gui_private.status_show & (1 << STATUS_COLUMN_STATUS) ) ?1:0;
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_stsstatus, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (conf_cb_stsstatus), "clicked", GTK_SIGNAL_FUNC
			(prop_box_changed_callback), NULL);

	conf_cb_stswho = gtk_check_button_new_with_label(_("Show Who Started"));
	GTK_TOGGLE_BUTTON(conf_cb_stswho)->active = ( gui_private.status_show & (1<<STATUS_COLUMN_WHO) ) ?1:0;
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_stswho, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (conf_cb_stswho), "clicked", GTK_SIGNAL_FUNC
			(prop_box_changed_callback), NULL);

	/* hbox contains two vboxes which contain the buttons/labels */
	gtk_box_pack_start ( GTK_BOX (hbox), vbox, FALSE, TRUE, 0);

	gtk_container_add (GTK_CONTAINER (sts_frame), hbox);


	/* Robot Options */

	robot_frame = gtk_frame_new (_("Launch Robot"));
	gtk_container_border_width (GTK_CONTAINER (robot_frame), GNOME_PAD);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_border_width (GTK_CONTAINER (vbox), GNOME_PAD);

	conf_cb_robotserver = gtk_radio_button_new_with_label ( NULL, 
			_("Starts the robots in the server (faster)"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (conf_cb_robotserver), g_game.robot_in_server );
	gtk_box_pack_start( GTK_BOX(vbox), conf_cb_robotserver, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (conf_cb_robotserver), "clicked",
			GTK_SIGNAL_FUNC(prop_box_changed_callback), NULL);

	conf_cb_robotclient = gtk_radio_button_new_with_label_from_widget( GTK_RADIO_BUTTON(conf_cb_robotserver),
			_("Starts the robots in the client (preserves localization)"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (conf_cb_robotclient), ! g_game.robot_in_server );
	gtk_box_pack_start( GTK_BOX(vbox), conf_cb_robotclient, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (conf_cb_robotclient), "clicked",
			GTK_SIGNAL_FUNC(prop_box_changed_callback), NULL);


	gtk_container_add (GTK_CONTAINER (robot_frame), vbox);


	/* Dialogs Options */
	dialog_frame = gtk_frame_new (_("Popup Dialogs"));
	gtk_container_border_width (GTK_CONTAINER (dialog_frame), GNOME_PAD);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_border_width (GTK_CONTAINER (vbox), GNOME_PAD);

	conf_cb_showattackturn = gtk_check_button_new_with_label (_("Show 'your turn for attack' dialog"));
	GTK_TOGGLE_BUTTON(conf_cb_showattackturn)->active =
		( gui_private.dialog_show & (1<<DIALOG_ATTACK_TURN) ) ?1:0;
	gtk_box_pack_start( GTK_BOX(vbox), conf_cb_showattackturn, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (conf_cb_showattackturn), "clicked",
			GTK_SIGNAL_FUNC(prop_box_changed_callback), NULL);

	conf_cb_showregrouparmies = gtk_check_button_new_with_label (_("Show 'regroup your armies' dialog"));
	GTK_TOGGLE_BUTTON(conf_cb_showregrouparmies)->active =
		( gui_private.dialog_show & (1<<DIALOG_REGROUP_ARMIES) ) ?1:0;
	gtk_box_pack_start( GTK_BOX(vbox), conf_cb_showregrouparmies, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (conf_cb_showregrouparmies), "clicked",
			GTK_SIGNAL_FUNC(prop_box_changed_callback), NULL);

	conf_cb_showplacearmies = gtk_check_button_new_with_label (_("Show 'place your armies' dialog"));
	GTK_TOGGLE_BUTTON(conf_cb_showplacearmies)->active =
		( gui_private.dialog_show & (1<<DIALOG_PLACE_ARMIES) ) ?1:0;
	gtk_box_pack_start( GTK_BOX(vbox), conf_cb_showplacearmies, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (conf_cb_showplacearmies), "clicked",
			GTK_SIGNAL_FUNC(prop_box_changed_callback), NULL);

	gtk_container_add (GTK_CONTAINER (dialog_frame), vbox);


	/** end **/

	pref_dialog = gnome_property_box_new ();
	gnome_dialog_set_parent (GNOME_DIALOG (pref_dialog),
			GTK_WINDOW (main_window));
	gtk_window_set_title (GTK_WINDOW (pref_dialog),
			_("TEG Preferences"));
	gtk_signal_connect (GTK_OBJECT (pref_dialog), "destroy",
			GTK_SIGNAL_FUNC (gtk_widget_destroyed), &pref_dialog);


	label = gtk_label_new (_("Themes"));
	gnome_property_box_append_page (GNOME_PROPERTY_BOX (pref_dialog),
			theme_vbox, label);

	label = gtk_label_new (_("Console Messages"));
	gnome_property_box_append_page (GNOME_PROPERTY_BOX (pref_dialog),
			msg_frame, label);

	label = gtk_label_new (_("Status Of Players"));
	gnome_property_box_append_page (GNOME_PROPERTY_BOX (pref_dialog),
			sts_frame, label);

	label = gtk_label_new (_("Robot"));
	gnome_property_box_append_page (GNOME_PROPERTY_BOX (pref_dialog),
			robot_frame, label);

	label = gtk_label_new (_("Popup Dialogs"));
	gnome_property_box_append_page (GNOME_PROPERTY_BOX (pref_dialog),
			dialog_frame, label);



	gtk_signal_connect (GTK_OBJECT (pref_dialog), "apply", GTK_SIGNAL_FUNC
			(apply_cb), NULL);

	if (!GTK_WIDGET_VISIBLE (pref_dialog))
		gtk_widget_show_all (pref_dialog);
	else
		gtk_widget_destroy (pref_dialog);
}
