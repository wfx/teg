/* Eye of Gnome image viewer - stock icons
 *
 * Copyright (C) 1999 The Free Software Foundation
 *
 * Author: Federico Mena-Quintero <federico@gimp.org>
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

namespace teg::client::callbacks
{

#define STOCK_ZOOM_1	"TEG_stock_zoom_1"
#define STOCK_ZOOM_FIT	"TEG_stock_zoom_fit"
#define STOCK_ZOOM_IN	"TEG_stock_zoom_in"
#define STOCK_ZOOM_OUT	"TEG_stock_zoom_out"
#define STOCK_CONNECT   "stock-connect-symbolic"
#define STOCK_STARTGAME    "stock-startgame-symbolic"
#define STOCK_SENDARMIES    "stock-sendarmies-symbolic"
#define STOCK_REGROUPARMIES "stock-regrouparmies-symbolic"
#define STOCK_GETCARD   "stock-getcard-symbolic"
#define STOCK_ENDTURN	"stock-endturn-symbolic"
#define STOCK_VIEWPLAYERS	"stock-viewplayers-symbolic"
void stock_init(void);

}
