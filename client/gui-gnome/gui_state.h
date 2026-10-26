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

#include <gio/gio.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <goocanvas.h>

#include "../../common/common.h"

#include "../state.h"

#include "themes.h"
#include "priv.h"
#include "colors.h"
#include "g_country.h"
#include "g_cont.h"
#include "armies.h"

namespace teg::client::callbacks
{

/// \brief A country entry of the army locate animation
struct LocateEntry {
	int country{-1};
	int is_hidden{FALSE};
	int number_times_refreshed{0};
};

/// \brief All mutable state of the GNOME GUI instance.
struct GuiState {
	/// \brief The client state this GUI belongs to
	ClientState* client{nullptr};

	/// \brief The loaded theme
	TTheme theme{};

	/// \brief GUI preferences and GSettings handle
	struct _gui_private priv {};
	GSettings* settings{nullptr};

	/// \brief The main window and its global widgets
	struct {
		GtkTextView* main_message_area{nullptr};
		GtkWidget* main_window{nullptr};
		GtkWidget* toolbar_main{nullptr};
		GtkWidget* statusbar{nullptr};
		GtkWidget* canvas_map{nullptr};
		GtkWidget* window_mainstatus{nullptr};
		double canvas_map_zoom{1.00};

		GtkWidget* tropas_dialog{nullptr};
		GtkWidget* tropas_hscale_cant{nullptr};
		int tropas_cant{0};
		int tropas_src{0};
		int tropas_dst{0};

		GtkWidget* reagrupe_dialog{nullptr};
		GtkWidget* reagrupe_hscale_cant{nullptr};
		int reagrupe_cant{-1};
		int reagrupe_src{-1};
		int reagrupe_dst{-1};

		struct {
			GtkWidget* connect_button{nullptr};
			GtkWidget* start_button{nullptr};
			GtkWidget* enviarfichas_button{nullptr};
			GtkWidget* reagrupe_button{nullptr};
			GtkWidget* getcard_button{nullptr};
			GtkWidget* endturn_button{nullptr};
			GtkWidget* status_button{nullptr};
		} sensi;

		GtkWidget* connect_menu{nullptr};
		GtkWidget* disconnect_menu{nullptr};
		GtkWidget* robot_menu{nullptr};
		GtkWidget* scores_menu{nullptr};
		GtkWidget* start_menu{nullptr};
		GtkWidget* send_menu{nullptr};
		GtkWidget* update_menu{nullptr};
		GtkWidget* reattack_menu{nullptr};
		GtkWidget* reset_menu{nullptr};
		GtkWidget* regroup_menu{nullptr};
		GtkWidget* get_menu{nullptr};
		GtkWidget* end_menu{nullptr};
		GtkWidget* surrender_menu{nullptr};
		GtkWidget* players_menu{nullptr};
		GtkWidget* mission_menu{nullptr};
		GtkWidget* cards_menu{nullptr};
	} ui;

	/// \brief The connection and color/game type dialogs
	struct {
		GtkWidget* connect_window{nullptr};
		GtkWidget* con_entry_name{nullptr};
		GtkWidget* con_spinner_port{nullptr};
		GtkWidget* con_entry_server{nullptr};
		GtkWidget* button_launch{nullptr};
		GtkWidget* button_observe{nullptr};
		GtkWidget* colortype_dialog{nullptr};
		GtkWidget* gametype_dialog{nullptr};
		GtkWidget* gametype_button_conqworld{nullptr};
		GtkWidget* gametype_button_secret{nullptr};
		GtkWidget* gametype_button_viewall{nullptr};
		GtkWidget* gametype_button_fow{nullptr};
		GtkWidget* gametype_button_commonmission{nullptr};
		GtkWidget* gametype_spinner_armies1{nullptr};
		GtkWidget* gametype_spinner_armies2{nullptr};
		GtkWidget* boton_color[maximum_player_count] {};
		GIOChannel* channel{nullptr};
		guint watch_id{0};
	} connect;

	/// \brief The card window
	struct {
		GtkWidget* dialog{nullptr};
		GtkGrid* table{nullptr};

		struct Sensi {
			GtkWidget* card{nullptr};
			GtkWidget* button_armies{nullptr};
			GtkWidget* button_select{nullptr};
			PCOUNTRY country{nullptr};
			bool selected{false};
		} sensi[maximum_country_cards] {};

		struct Kind {
			GdkPixbuf* tar{nullptr};
			TARJTIPO tipo{TARJ_CANION};
			char const* filename{nullptr};
		} kinds[4] {
			{nullptr, TARJ_CANION, "tar_canon.png"},
			{nullptr, TARJ_GALEON, "tar_galeon.png"},
			{nullptr, TARJ_GLOBO, "tar_globo.png"},
			{nullptr, TARJ_COMODIN, "tar_comodin.png"},
		};
	} cards;

	/// \brief Colors and color pixmaps
	struct {
		int allocated{0};
		int images_loaded{0};
		struct _G_colores table[7] {
			{"red", "white"},
			{"yellow", "black"},
			{"blue", "white"},
			{"grey27", "white"},
			{"PaleVioletRed1", "black"},
			{"green", "black"},
			{"grey", "black"},
		};
		GdkRGBA players_rgba[7] {};
		char const* common_names[2] {"white", "black"};
		GdkRGBA common_rgba[2] {};
		int foreground[7] {0, 1, 0, 0, 1, 1, 1};
		GdkPixbuf* player_pixbufs[maximum_player_count] {};
		GdkPixbuf* circle_pixbufs[maximum_player_count + 1] {};
		GdkPixbuf* circle_over_pixbuf{nullptr};
		char tag_buffer[256] {};
	} colors;

	/// \brief Canvas state of the map
	struct {
		bool country_initialized{false};
		bool cont_initialized{false};
		struct _G_conts conts[CONT_CANT] {};
		struct _G_countries countries[COUNTRIES_CANT] {};

		struct {
			double x{0.0};
			double y{0.0};
			double new_x{0.0};
			double new_y{0.0};
			double item_x{0.0};
			double item_y{0.0};
			bool dragging{false};
		} drag;
	} map;

	/// \brief The dices on the map
	struct {
		GdkPixbuf* pixbufs[sides_on_the_dice] {};
		GooCanvasItem* items[sides_on_the_dice] {};
		GooCanvasItem* texts[2] {};
		GooCanvasItem* group{nullptr};
		int initialized{0};

		struct Coord {
			double x{0.0};
			double y{0.0};
		};
		struct {
			Coord attacker[3] {};
			Coord attacker_text{};
			Coord defender[3] {};
			Coord defender_text{};
		} positions;
	} dices;

	/// \brief The mission window
	struct {
		GdkPixbuf* pixbuf{nullptr};
		int number{-1};
		GtkWidget* canvas{nullptr};
		GooCanvasItem* image_text{nullptr};
		GooCanvasItem* image_text2{nullptr};
	} mission;

	/// \brief The high score window
	struct {
		GtkWidget* dialog{nullptr};
		GtkListStore* store{nullptr};
		GtkWidget* treeview{nullptr};
		GtkWidget* mini_clist{nullptr};
	} scores;

	/// \brief The status windows
	struct {
		GtkWidget* dialog{nullptr};
		GtkWidget* treeview{nullptr};
		GtkWidget* ministatus{nullptr};
		GtkWidget* mainstatus_canvas{nullptr};
		GtkListStore* model{nullptr};
		int offset_left{-1};
		int offset_right{-1};
		GooCanvasItem* gamestatus_item{nullptr};
		GooCanvasItem* country_item{nullptr};
		GooCanvasItem* cont_item{nullptr};
		GooCanvasItem* players_color[maximum_player_count] {};
		GooCanvasItem* color_started_item{nullptr};
		GooCanvasItem* round_number_item{nullptr};
		GooCanvasItem* players_color_over{nullptr};
	} status;

	/// \brief The preferences dialog
	struct {
		GtkWidget* dialog{nullptr};

		GtkWidget* showerr{nullptr};
		GtkWidget* showimp{nullptr};
		GtkWidget* showmsg{nullptr};
		GtkWidget* showmsgcolor{nullptr};
		GtkWidget* showinf{nullptr};

		GtkWidget* showattackturn{nullptr};
		GtkWidget* showplacearmies{nullptr};
		GtkWidget* showregrouparmies{nullptr};
		GtkWidget* showtoolbar{nullptr};
		GtkWidget* toolbarstyle{nullptr};

		char* theme_activated{nullptr};
		GtkWidget* theme_widget{nullptr};
		GtkWidget* theme_frame_prev{nullptr};

		GtkWidget* stscolor{nullptr};
		GtkWidget* stsnumber{nullptr};
		GtkWidget* stsname{nullptr};
		GtkWidget* stsscore{nullptr};
		GtkWidget* stsaddress{nullptr};
		GtkWidget* stshuman{nullptr};
		GtkWidget* stscountries{nullptr};
		GtkWidget* stsarmies{nullptr};
		GtkWidget* stscards{nullptr};
		GtkWidget* stsstatus{nullptr};
		GtkWidget* stswho{nullptr};

		GtkWidget* robotclient{nullptr};
		GtkWidget* robotserver{nullptr};
	} prefs;

	/// \brief The draggable army counters on the map
	struct {
		ARMIES_POS prop[CONT_CANT + 1] {};
		bool initialized{false};
		GooCanvasItem* group{nullptr};

		struct {
			double x{0.0};
			double y{0.0};
			double new_x{0.0};
			double new_y{0.0};
			double item_x{0.0};
			double item_y{0.0};
			gboolean dragging{FALSE};
		} drag;
	} armies;

	/// \brief The army locate animation
	struct {
		guint timeout_id{static_cast<guint>(-1)};
		LocateEntry countries[16] {};
		LocateEntry armies[16] {};
	} locate;
};

/// \brief The GUI state of the running GNOME client
GuiState& gui_state();

/// \brief The client state the GUI works on
ClientState& client_state();

}
