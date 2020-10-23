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

#include <gtk/gtk.h>

#include "fcintl.h"
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

/** dialogs & toolbar */
static GtkWidget	*conf_cb_showattackturn=NULL;
static GtkWidget	*conf_cb_showplacearmies=NULL;
static GtkWidget	*conf_cb_showregrouparmies=NULL;
static GtkWidget	*conf_cb_showtoolbar=NULL;
static GtkWidget	*conf_cb_toolbarstyle=NULL;

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

	if ( !GTK_IS_FRAME (theme_frame_prev) )
	        return;

	filename = theme_load_fake_file("mini_shot.png",theme);
	theme_widget = gtk_image_new_from_file(filename);
	if( theme_widget ) {
		gtk_container_add( GTK_CONTAINER(theme_frame_prev), theme_widget );
		gtk_widget_show(theme_widget);
		g_signal_connect (G_OBJECT(theme_widget), "destroy",
	                          G_CALLBACK (free_pixmap), NULL);
	} else
		g_warning (_("Could not find the %s file"),"mini_shot.png");
}

static void theme_activated_callback (GtkComboBox *combo, gpointer data)
{
	gchar *sel;

	if ( !pref_dialog )
	        return;

	sel = gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT (combo));
	theme_activated = g_strdup (sel);
	load_preview(sel);
	g_free(sel);
}

static void
toolbar_style_cb (GtkComboBox *combo, gpointer data)
{
        gint sel;

        sel = gtk_combo_box_get_active (combo);
        g_settings_set_enum (settings, "toolbar-style", sel);
}

static void
show_toolbar_cb (GtkToggleButton *button, gpointer data)
{
        if (gtk_toggle_button_get_active (button)) {
                g_settings_set_boolean (settings, "toolbar-visibility", TRUE);
                gtk_widget_show (toolbar_main);
        } else {
                g_settings_set_boolean (settings, "toolbar-visibility", FALSE);
                gtk_widget_hide (toolbar_main);
        }
}

static void
free_str (GtkWidget *widget, void *data)
{
	free (data);
}



static void apply_cb (GtkDialog *widget, gint id, gpointer data)
{
	if (id != GTK_RESPONSE_ACCEPT)
	        return;
 
	/** dialogs **/


	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
	                                  (conf_cb_showattackturn)))
		gui_private.dialog_show |= (1 << DIALOG_ATTACK_TURN);
	else
		gui_private.dialog_show &= ~(1 << DIALOG_ATTACK_TURN);

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
	                                  (conf_cb_showplacearmies)))
		gui_private.dialog_show |= (1 << DIALOG_PLACE_ARMIES);
	else
		gui_private.dialog_show &= ~(1 << DIALOG_PLACE_ARMIES);
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
	                                  (conf_cb_showregrouparmies)))
		gui_private.dialog_show |= (1 << DIALOG_REGROUP_ARMIES);
	else
		gui_private.dialog_show &= ~(1 << DIALOG_REGROUP_ARMIES);
	

	/** messages **/
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON( conf_cb_showerr )))
		g_game.msg_show |= M_ERR;
	else
		g_game.msg_show &= ~M_ERR;


	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON( conf_cb_showimp )))
		g_game.msg_show |= M_IMP;
	else
		g_game.msg_show &= ~M_IMP;


	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON( conf_cb_showmsg )))
		g_game.msg_show |= M_MSG;
	else
		g_game.msg_show &= ~M_MSG;


	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
	                                  ( conf_cb_showmsgcolor )))
		gui_private.msg_show_colors = 1;
	else
		gui_private.msg_show_colors = 0;


	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON( conf_cb_showinf )))
		g_game.msg_show |= M_INF;
	else
		g_game.msg_show &= ~M_INF;

	/** status **/
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON( conf_cb_stscolor)))
		gui_private.status_show |= (1 << STATUS_COLUMN_COLOR);
	else
		gui_private.status_show &= ~(1 << STATUS_COLUMN_COLOR);

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
	                                  ( conf_cb_stsnumber)))
		gui_private.status_show |= (1 << STATUS_COLUMN_NUMBER);
	else
		gui_private.status_show &= ~(1 << STATUS_COLUMN_NUMBER);

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON( conf_cb_stsname)))
		gui_private.status_show |= (1 << STATUS_COLUMN_NAME);
	else
		gui_private.status_show &= ~(1 << STATUS_COLUMN_NAME);

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON( conf_cb_stsscore)))
		gui_private.status_show |= (1 << STATUS_COLUMN_SCORE);
	else
		gui_private.status_show &= ~(1 << STATUS_COLUMN_SCORE);

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
	                                  ( conf_cb_stsaddress )))
		gui_private.status_show |= (1 << STATUS_COLUMN_ADDR);
	else
		gui_private.status_show &= ~(1 << STATUS_COLUMN_ADDR);

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON( conf_cb_stshuman)))
		gui_private.status_show |= (1 << STATUS_COLUMN_HUMAN);
	else
		gui_private.status_show &= ~(1 << STATUS_COLUMN_HUMAN);

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
	                                  ( conf_cb_stscountries )))
		gui_private.status_show |= (1 << STATUS_COLUMN_COUNTRIES);
	else
		gui_private.status_show &= ~(1 << STATUS_COLUMN_COUNTRIES);

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
	                                  ( conf_cb_stsarmies )))
		gui_private.status_show |= (1 << STATUS_COLUMN_ARMIES);
	else
		gui_private.status_show &= ~(1 << STATUS_COLUMN_ARMIES);

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (conf_cb_stscards)))
		gui_private.status_show |= (1 << STATUS_COLUMN_CARDS);
	else
		gui_private.status_show &= ~(1 << STATUS_COLUMN_CARDS);

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
	                                  ( conf_cb_stsstatus )))
		gui_private.status_show |= (1 << STATUS_COLUMN_STATUS);
	else
		gui_private.status_show &= ~(1 << STATUS_COLUMN_STATUS);

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON( conf_cb_stswho)))
		gui_private.status_show |= (1 << STATUS_COLUMN_WHO);
	else
		gui_private.status_show &= ~(1 << STATUS_COLUMN_WHO);

	status_update_visibility_of_columns();


	/* robot */
	g_game.robot_in_server = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(conf_cb_robotserver) );

	
	/* save new configurations */
	g_settings_delay( settings );
	g_settings_set_int( settings, "msgshow", g_game.msg_show );
	g_settings_set_boolean( settings, "msgshow-with-color",
	                        gui_private.msg_show_colors );
	g_settings_set_int( settings, "status-show", gui_private.status_show );
	g_settings_set_int( settings, "dialog-show", gui_private.dialog_show );
	g_settings_set_boolean( settings, "robot-in-server",
	                        g_game.robot_in_server );

	if( theme_activated ){
		g_settings_set_string( settings, "theme", theme_activated);
		if( strcmp(theme_activated,g_game.theme) ) {
	          GtkWidget *dlg;

	          dlg = gtk_message_dialog_new (GTK_WINDOW (main_window),
	                                        GTK_DIALOG_DESTROY_WITH_PARENT,
	                                        GTK_MESSAGE_INFO,
	                                        GTK_BUTTONS_CLOSE,
	                                        _("You have to restart TEG to"
	                                          " use the new theme."));
	          gtk_dialog_run (GTK_DIALOG (dlg));
	          gtk_widget_destroy (dlg);
	        }
	}

	/* Apply all settings atomically.  */
	g_settings_apply (settings);
}



static void
fill_menu (GtkWidget *menu)
{
	TInfo Info;
	pTInfo pI;
	char *s;
	int i=0;

	if( theme_enum_themes(&Info) != TEG_STATUS_SUCCESS ) {
		textmsg(M_ERR,_("Error while loading information about themes!"));
		return;
	}

	for( pI=&Info; pI != NULL; pI = pI->next ) {
		s = strdup (pI->name);
	        gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (menu), s);
		g_signal_connect (G_OBJECT(menu), "changed",
			    G_CALLBACK (theme_activated_callback), NULL);

		g_signal_connect (G_OBJECT(menu), "destroy",
                                  G_CALLBACK (free_str), s);

		if( !strcmp(pI->name, g_game.theme ))
			gtk_combo_box_set_active(GTK_COMBO_BOX(menu), i);
		i++;
	}
}

#define GNOME_PAD 8

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
	GtkWidget *toolbar_frame, *toolbar_vbox;
	GtkComboBoxText *combo;
	GtkWidget *vbox, *hbox;
	GtkWidget *omenu, *nb;

	/* Theme options */
	theme_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	
	theme_frame_sel = gtk_frame_new (_("Select theme"));
	gtk_container_set_border_width (GTK_CONTAINER (theme_frame_sel),
	                                GNOME_PAD);
	omenu = gtk_combo_box_text_new ();
	fill_menu (omenu);
	gtk_widget_show (omenu);
	label = gtk_label_new (_("Select theme"));
	gtk_widget_show (label);

	hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox), GNOME_PAD);

	gtk_box_pack_start( GTK_BOX( hbox ), label, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX( hbox ), omenu, TRUE, TRUE, 0);
	gtk_container_add (GTK_CONTAINER (theme_frame_sel), hbox);
	gtk_widget_show(theme_frame_sel);

	theme_frame_prev = gtk_frame_new (_("Preview"));
	gtk_container_set_border_width (GTK_CONTAINER (theme_frame_prev),
	                                GNOME_PAD);
	gtk_widget_show(theme_frame_prev);

	load_preview(g_game.theme);

	gtk_box_pack_start( GTK_BOX( theme_vbox ), theme_frame_sel, FALSE, FALSE, 0);
	gtk_box_pack_start( GTK_BOX( theme_vbox ), theme_frame_prev, FALSE, FALSE, 0);

	/* Message Options */
	msg_frame = gtk_frame_new (_("Console Messages"));
	gtk_container_set_border_width (GTK_CONTAINER (msg_frame), GNOME_PAD);

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), GNOME_PAD);


	conf_cb_showerr = gtk_check_button_new_with_label(_("Show Error Messages"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(conf_cb_showerr),
	                              (g_game.msg_show & M_ERR) ?1:0);
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_showerr, FALSE, FALSE, 0);

	conf_cb_showimp = gtk_check_button_new_with_label(_("Show Important Messages"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(conf_cb_showimp),
	                              (g_game.msg_show & M_IMP) ?1:0);
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_showimp, FALSE, FALSE, 0);

	conf_cb_showmsg = gtk_check_button_new_with_label(_("Show Players Messages"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(conf_cb_showmsg),
	                              (g_game.msg_show & M_MSG) ?1:0);
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_showmsg, FALSE, FALSE, 0);

	conf_cb_showmsgcolor = gtk_check_button_new_with_label(_("Show Players Messages with colors"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(conf_cb_showmsgcolor),
	                              (gui_private.msg_show_colors & 1) ?1:0);
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_showmsgcolor, FALSE, FALSE, 0);

	conf_cb_showinf = gtk_check_button_new_with_label(_("Show Informative Messages"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(conf_cb_showinf),
	                              (g_game.msg_show & M_INF) ?1:0);
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_showinf, FALSE, FALSE, 0);

	gtk_container_add (GTK_CONTAINER (msg_frame), vbox);


	/** Status options **/

	sts_frame = gtk_frame_new (_("Status Of Players"));
	gtk_container_set_border_width (GTK_CONTAINER (sts_frame), GNOME_PAD);

	hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox), GNOME_PAD);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 0);


	conf_cb_stscolor = gtk_check_button_new_with_label(_("Show Color"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(conf_cb_stscolor),
	                              (gui_private.status_show & (1 <<STATUS_COLUMN_COLOR) ) ?1:0);
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_stscolor, FALSE, FALSE, 0);


	conf_cb_stsnumber = gtk_check_button_new_with_label(_("Show Player Number"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(conf_cb_stsnumber),
	                              (gui_private.status_show & (1 << STATUS_COLUMN_NUMBER) ) ?1:0);
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_stsnumber, FALSE, FALSE, 0);


	conf_cb_stsname = gtk_check_button_new_with_label(_("Show Name"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(conf_cb_stsname),
	                              (gui_private.status_show & (1 <<STATUS_COLUMN_NAME) ) ?1:0);
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_stsname, FALSE, FALSE, 0);


	conf_cb_stsscore= gtk_check_button_new_with_label(_("Show Score"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(conf_cb_stsscore),
	                              (gui_private.status_show & (1 << STATUS_COLUMN_SCORE) ) ?1:0);
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_stsscore, FALSE, FALSE, 0);

	conf_cb_stsaddress = gtk_check_button_new_with_label(_("Show IP Address"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(conf_cb_stsaddress),
	                              (gui_private.status_show & (1 << STATUS_COLUMN_ADDR) ) ?1:0);
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_stsaddress, FALSE, FALSE, 0);

	conf_cb_stshuman= gtk_check_button_new_with_label(_("Show Human/Robot"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(conf_cb_stshuman),
	                              (gui_private.status_show & (1 << STATUS_COLUMN_HUMAN) ) ?1:0);
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_stshuman, FALSE, FALSE, 0);

	// now start the left box
	gtk_box_pack_start ( GTK_BOX (hbox), vbox, FALSE, TRUE, 0);
	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 0);


	conf_cb_stscountries = gtk_check_button_new_with_label(_("Show Countries"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(conf_cb_stscountries),
	                              (gui_private.status_show & (1 << STATUS_COLUMN_COUNTRIES) ) ?1:0);
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_stscountries, FALSE, FALSE, 0);

	conf_cb_stsarmies = gtk_check_button_new_with_label(_("Show Armies"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(conf_cb_stsarmies),
	                              (gui_private.status_show & (1 << STATUS_COLUMN_ARMIES) ) ?1:0);
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_stsarmies, FALSE, FALSE, 0);

	conf_cb_stscards = gtk_check_button_new_with_label(_("Show Cards"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(conf_cb_stscards),
	                              (gui_private.status_show & (1 << STATUS_COLUMN_CARDS) ) ?1:0);
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_stscards, FALSE, FALSE, 0);

	conf_cb_stsstatus = gtk_check_button_new_with_label(_("Show Status"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(conf_cb_stsstatus),
	                              (gui_private.status_show & (1 << STATUS_COLUMN_STATUS) ) ?1:0);
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_stsstatus, FALSE, FALSE, 0);

	conf_cb_stswho = gtk_check_button_new_with_label(_("Show Who Started"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(conf_cb_stswho),
	                              ( gui_private.status_show & (1<<STATUS_COLUMN_WHO) ) ?1:0);
	gtk_box_pack_start( GTK_BOX( vbox ), conf_cb_stswho, FALSE, FALSE, 0);

	/* hbox contains two vboxes which contain the buttons/labels */
	gtk_box_pack_start ( GTK_BOX (hbox), vbox, FALSE, TRUE, 0);

	gtk_container_add (GTK_CONTAINER (sts_frame), hbox);


	/* Robot Options */

	robot_frame = gtk_frame_new (_("Launch Robot"));
	gtk_container_set_border_width (GTK_CONTAINER (robot_frame), GNOME_PAD);

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), GNOME_PAD);

	conf_cb_robotserver = gtk_radio_button_new_with_label ( NULL, 
			_("Starts the robots in the server (faster)"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (conf_cb_robotserver), g_game.robot_in_server );
	gtk_box_pack_start( GTK_BOX(vbox), conf_cb_robotserver, FALSE, FALSE, 0);

	conf_cb_robotclient = gtk_radio_button_new_with_label_from_widget( GTK_RADIO_BUTTON(conf_cb_robotserver),
			_("Starts the robots in the client (preserves localization)"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (conf_cb_robotclient), ! g_game.robot_in_server );
	gtk_box_pack_start( GTK_BOX(vbox), conf_cb_robotclient, FALSE, FALSE, 0);


	gtk_container_add (GTK_CONTAINER (robot_frame), vbox);


	/* Dialogs & Toolbar Options */
	toolbar_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, GNOME_PAD);
	dialog_frame = gtk_frame_new (_("Popup Dialogs"));
	gtk_container_set_border_width (GTK_CONTAINER (dialog_frame), GNOME_PAD);

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), GNOME_PAD);

	conf_cb_showattackturn = gtk_check_button_new_with_label (_("Show 'your turn for attack' dialog"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(conf_cb_showattackturn),
	                              ( gui_private.dialog_show & (1<<DIALOG_ATTACK_TURN) ) ?1:0);
	gtk_box_pack_start( GTK_BOX(vbox), conf_cb_showattackturn, FALSE, FALSE, 0);

	conf_cb_showregrouparmies = gtk_check_button_new_with_label (_("Show 'regroup your armies' dialog"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(conf_cb_showregrouparmies),
	                              ( gui_private.dialog_show & (1<<DIALOG_REGROUP_ARMIES) ) ?1:0);
	gtk_box_pack_start( GTK_BOX(vbox), conf_cb_showregrouparmies, FALSE, FALSE, 0);

	conf_cb_showplacearmies = gtk_check_button_new_with_label (_("Show 'place your armies' dialog"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(conf_cb_showplacearmies),
	                              ( gui_private.dialog_show & (1<<DIALOG_PLACE_ARMIES) ) ?1:0);
	gtk_box_pack_start( GTK_BOX(vbox), conf_cb_showplacearmies, FALSE, FALSE, 0);

	gtk_container_add (GTK_CONTAINER (dialog_frame), vbox);
	gtk_box_pack_start (GTK_BOX (toolbar_vbox), dialog_frame,
	                    TRUE, TRUE, 0);

	toolbar_frame = gtk_frame_new (_("Toolbar Visibility & Style"));
	gtk_container_set_border_width (GTK_CONTAINER (toolbar_frame),
                                        GNOME_PAD);
	hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_set_border_width (GTK_CONTAINER (hbox), GNOME_PAD);
	conf_cb_showtoolbar
	  = gtk_check_button_new_with_label (_("Show toolbar"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (conf_cb_showtoolbar),
	                              g_settings_get_boolean (settings,
	                                                      "toolbar-visibility"));
	g_signal_connect (conf_cb_showtoolbar, "toggled",
	                  G_CALLBACK (show_toolbar_cb), NULL);
	gtk_box_pack_start (GTK_BOX (hbox), conf_cb_showtoolbar,
	                    TRUE, TRUE, 0);

	label = gtk_label_new (_("Toolbar style:"));
	gtk_widget_set_halign (label, GTK_ALIGN_END);
	gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);

	conf_cb_toolbarstyle = gtk_combo_box_text_new ();
	combo = GTK_COMBO_BOX_TEXT (conf_cb_toolbarstyle);
	gtk_combo_box_text_append_text (combo, _("Text Below Icons"));
	gtk_combo_box_text_append_text (combo, _("Priority Text Beside Icons"));
	gtk_combo_box_text_append_text (combo, _("Icons Only"));
	gtk_combo_box_text_append_text (combo, _("Text Only"));
	gtk_combo_box_set_active (GTK_COMBO_BOX (combo),
	                          g_settings_get_enum (settings,
	                                               "toolbar-style"));
	g_signal_connect (combo, "changed",
	                  G_CALLBACK (toolbar_style_cb), NULL);
	gtk_box_pack_start (GTK_BOX (hbox), conf_cb_toolbarstyle,
	                    TRUE, TRUE, 0);

	gtk_container_add (GTK_CONTAINER (toolbar_frame), hbox);
	gtk_container_add (GTK_CONTAINER (toolbar_vbox), toolbar_frame);

	/** end **/

	pref_dialog = gtk_dialog_new_with_buttons (_("TEG Preferences"),
	                                           GTK_WINDOW (main_window),
	                                           GTK_DIALOG_DESTROY_WITH_PARENT,
	                                           _("_OK"),
	                                           GTK_RESPONSE_ACCEPT,
	                                           _("_Cancel"),
	                                           GTK_RESPONSE_REJECT,
	                                           NULL);

	nb = gtk_notebook_new ();
	gtk_container_add (GTK_CONTAINER (gtk_dialog_get_content_area
	                                  (GTK_DIALOG (pref_dialog))), nb);

	label = gtk_label_new (_("Themes"));
	gtk_notebook_append_page (GTK_NOTEBOOK (nb),
			theme_vbox, label);

	label = gtk_label_new (_("Console Messages"));
	gtk_notebook_append_page (GTK_NOTEBOOK (nb),
			msg_frame, label);

	label = gtk_label_new (_("Status Of Players"));
	gtk_notebook_append_page (GTK_NOTEBOOK (nb),
			sts_frame, label);

	label = gtk_label_new (_("Robot"));
	gtk_notebook_append_page (GTK_NOTEBOOK (nb),
			robot_frame, label);

	label = gtk_label_new (_("Dialogs & Toolbar"));
	gtk_notebook_append_page (GTK_NOTEBOOK (nb),
			toolbar_vbox, label);



	g_signal_connect (G_OBJECT (pref_dialog), "response", G_CALLBACK
			(apply_cb), NULL);

	gtk_widget_show_all (pref_dialog);
	gtk_dialog_run (GTK_DIALOG (pref_dialog));
	gtk_widget_destroy (pref_dialog);
}
