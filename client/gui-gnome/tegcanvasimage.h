/*
 * GooCanvas. Copyright (C) 2005 Damon Chaplin.
 * Released under the GNU LGPL license. See COPYING for details.
 */

#pragma once

#include <goocanvas.h>
namespace teg::client::callbacks
{

GooCanvasItem* teg_canvas_image_new(GooCanvasItem      *parent,
                                    GdkPixbuf          *pixbuf,
                                    gdouble             x,
                                    gdouble             y,
                                    ...);

}
