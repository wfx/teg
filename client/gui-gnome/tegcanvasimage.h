/*
 * GooCanvas. Copyright (C) 2005 Damon Chaplin.
 * Released under the GNU LGPL license. See COPYING for details.
 */

#pragma once

#include <goocanvas.h>

/* This is a private GooCanvas function we can't do without.  */
extern "C" cairo_pattern_t* goo_canvas_cairo_pattern_from_pixbuf(GdkPixbuf *pixbuf);

namespace teg::client::callbacks
{

typedef struct _TegCanvasImageData   TegCanvasImageData;

struct _TegCanvasImageData {
	cairo_pattern_t *pattern;

	gdouble x, y, width, height;
};

#define TEG_TYPE_CANVAS_IMAGE (teg_canvas_image_get_type ())
#define TEG_CANVAS_IMAGE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), TEG_TYPE_CANVAS_IMAGE, TegCanvasImage))
#define TEG_CANVAS_IMAGE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), TEG_TYPE_CANVAS_IMAGE, \
                            TegCanvasImageClass))
#define TEG_IS_CANVAS_IMAGE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TEG_TYPE_CANVAS_IMAGE))
#define TEG_IS_CANVAS_IMAGE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), TEG_TYPE_CANVAS_IMAGE))
#define TEG_CANVAS_IMAGE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), TEG_TYPE_CANVAS_IMAGE, \
                              TegCanvasImageClass))

typedef struct _TegCanvasImage       TegCanvasImage;
typedef struct _TegCanvasImageClass  TegCanvasImageClass;

struct _TegCanvasImage {
	GooCanvasItemSimple parent_object;
	TegCanvasImageData *image_data;
};

struct _TegCanvasImageClass {
	GooCanvasItemSimpleClass parent_class;
};

GType teg_canvas_image_get_type(void) G_GNUC_CONST;

GooCanvasItem* teg_canvas_image_new(GooCanvasItem      *parent,
                                    GdkPixbuf          *pixbuf,
                                    gdouble             x,
                                    gdouble             y,
                                    ...);

#define TEG_TYPE_CANVAS_IMAGE_MODEL (teg_canvas_image_model_get_type ())
#define TEG_CANVAS_IMAGE_MODEL(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), TEG_TYPE_CANVAS_IMAGE_MODEL, \
                               TegCanvasImageModel))
#define TEG_CANVAS_IMAGE_MODEL_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), TEG_TYPE_CANVAS_IMAGE_MODEL, \
                            TegCanvasImageModelClass))
#define TEG_IS_CANVAS_IMAGE_MODEL(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TEG_TYPE_CANVAS_IMAGE_MODEL))
#define TEG_IS_CANVAS_IMAGE_MODEL_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), TEG_TYPE_CANVAS_IMAGE_MODEL))
#define TEG_CANVAS_IMAGE_MODEL_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), TEG_TYPE_CANVAS_IMAGE_MODEL, \
                              TegCanvasImageModelClass))

typedef struct _TegCanvasImageModel       TegCanvasImageModel;
typedef struct _TegCanvasImageModelClass  TegCanvasImageModelClass;

struct _TegCanvasImageModel {
	GooCanvasItemModelSimple parent_object;
	TegCanvasImageData image_data;
};

struct _TegCanvasImageModelClass {
	GooCanvasItemModelSimpleClass parent_class;
};

GType teg_canvas_image_model_get_type(void) G_GNUC_CONST;

GooCanvasItemModel* teg_canvas_image_model_new(GooCanvasItemModel *parent,
        GdkPixbuf          *pixbuf,
        gdouble             x,
        gdouble             y,
        ...);

}
