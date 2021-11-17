/*
 * GooCanvas. Copyright (C) 2005 Damon Chaplin.
 * Released under the GNU LGPL license. See COPYING for details.
 */

#include "tegcanvasimage.h"

#include <glib/gi18n.h>
#include <glib-object.h>

namespace teg::client::callbacks
{

typedef struct _TegCanvasImagePrivate TegCanvasImagePrivate;
struct _TegCanvasImagePrivate {
	gboolean scale_to_fit;
	gdouble alpha;
	gdouble scale_to_units;
};


enum {
	PROP_0,

	PROP_PATTERN,
	PROP_X,
	PROP_Y,
	PROP_WIDTH,
	PROP_HEIGHT,
	PROP_SCALE_TO_FIT,
	PROP_ALPHA,

	PROP_PIXBUF
};

static void teg_canvas_image_dispose(GObject *object);
static void teg_canvas_image_finalize(GObject *object);
static void canvas_item_interface_init(GooCanvasItemIface *iface);
static void teg_canvas_image_get_property(GObject    *object,
        guint       param_id,
        GValue     *value,
        GParamSpec *pspec);
static void teg_canvas_image_set_property(GObject      *object,
        guint         param_id,
        const GValue *value,
        GParamSpec   *pspec);

static void item_model_interface_init(GooCanvasItemModelIface *iface);
static void teg_canvas_image_model_dispose(GObject *object);
static void teg_canvas_image_model_get_property(GObject    *object,
        guint       param_id,
        GValue     *value,
        GParamSpec *pspec);
static void teg_canvas_image_model_set_property(GObject       *object,
        guint         param_id,
        const GValue *value,
        GParamSpec   *pspec);


G_DEFINE_TYPE_WITH_CODE(TegCanvasImage, teg_canvas_image,
                        GOO_TYPE_CANVAS_ITEM_SIMPLE,
                        G_IMPLEMENT_INTERFACE(GOO_TYPE_CANVAS_ITEM,
                                canvas_item_interface_init)
                        G_ADD_PRIVATE(TegCanvasImage))

G_DEFINE_TYPE_WITH_CODE(TegCanvasImageModel, teg_canvas_image_model,
                        GOO_TYPE_CANVAS_ITEM_MODEL_SIMPLE,
                        G_IMPLEMENT_INTERFACE(GOO_TYPE_CANVAS_ITEM_MODEL,
                                item_model_interface_init)
                        G_ADD_PRIVATE(TegCanvasImage))

TegCanvasImagePrivate* TEG_CANVAS_IMAGE_GET_PRIVATE(TegCanvasImage* image)
{
	return static_cast<TegCanvasImagePrivate*>(teg_canvas_image_get_instance_private(image));
}

TegCanvasImagePrivate* TEG_CANVAS_IMAGE_MODEL_GET_PRIVATE(TegCanvasImageModel* model)
{
	return static_cast<TegCanvasImagePrivate*>(teg_canvas_image_model_get_instance_private(model));
}

static void teg_canvas_image_install_common_properties(GObjectClass *gobject_class)
{
	g_object_class_install_property(gobject_class, PROP_PATTERN,
	                                g_param_spec_boxed("pattern",
	                                        _("Pattern"),
	                                        _("The cairo pattern "
	                                                "to paint"),
	                                        GOO_TYPE_CAIRO_PATTERN,
	                                        G_PARAM_READWRITE));

	g_object_class_install_property(gobject_class, PROP_X,
	                                g_param_spec_double("x",
	                                        "X",
	                                        _("The x coordinate "
	                                                "of the image"),
	                                        -G_MAXDOUBLE,
	                                        G_MAXDOUBLE, 0.0,
	                                        G_PARAM_READWRITE));

	g_object_class_install_property(gobject_class, PROP_Y,
	                                g_param_spec_double("y",
	                                        "Y",
	                                        _("The y coordinate "
	                                                "of the image"),
	                                        -G_MAXDOUBLE,
	                                        G_MAXDOUBLE, 0.0,
	                                        G_PARAM_READWRITE));

	g_object_class_install_property(gobject_class, PROP_WIDTH,
	                                g_param_spec_double("width",
	                                        _("Width"),
	                                        _("The width of the "
	                                                "image"),
	                                        0.0, G_MAXDOUBLE, 0.0,
	                                        G_PARAM_READWRITE));

	g_object_class_install_property(gobject_class, PROP_HEIGHT,
	                                g_param_spec_double("height",
	                                        _("Height"),
	                                        _("The height of the "
	                                                "image"),
	                                        0.0, G_MAXDOUBLE, 0.0,
	                                        G_PARAM_READWRITE));

	g_object_class_install_property(gobject_class, PROP_SCALE_TO_FIT,
	                                g_param_spec_boolean("scale-to-fit",
	                                        _("Scale To Fit"),
	                                        _("If the image is "
	                                                "scaled to fit the "
	                                                "width and height "
	                                                "settings"),
	                                        FALSE,
	                                        G_PARAM_READWRITE));

	g_object_class_install_property(gobject_class, PROP_ALPHA,
	                                g_param_spec_double("alpha",
	                                        _("Alpha"),
	                                        _("The opacity of the "
	                                                "image, 0.0 is fully"
	                                                " transparent, and "
	                                                "1.0 is opaque."),
	                                        0.0, 1.0, 1.0,
	                                        G_PARAM_READWRITE));

	g_object_class_install_property(gobject_class, PROP_PIXBUF,
	                                g_param_spec_object("pixbuf",
	                                        _("Pixbuf"),
	                                        _("The GdkPixbuf to "
	                                                "display"),
	                                        GDK_TYPE_PIXBUF,
	                                        G_PARAM_WRITABLE));
}

/* Gets the private data to use, from the model or from the item itself. */
static TegCanvasImagePrivate* teg_canvas_image_get_private(gpointer object)
{
	GooCanvasItemSimple *simple;

	if(TEG_IS_CANVAS_IMAGE(object)) {
		simple = (GooCanvasItemSimple*) object;
		if(simple->model) {
			return TEG_CANVAS_IMAGE_MODEL_GET_PRIVATE(
			           reinterpret_cast<TegCanvasImageModel*>(simple->model));
		} else {
			return TEG_CANVAS_IMAGE_GET_PRIVATE(
			           reinterpret_cast<TegCanvasImage*>(object));
		}
	} else {
		return TEG_CANVAS_IMAGE_MODEL_GET_PRIVATE(
		           reinterpret_cast<TegCanvasImageModel*>(object));
	}
}


static void teg_canvas_image_init(TegCanvasImage *image)
{
	TegCanvasImagePrivate *priv = TEG_CANVAS_IMAGE_GET_PRIVATE(image);

	image->image_data = g_slice_new0(TegCanvasImageData);

	priv->alpha = 1.0;
}

/*
 * Convert the width and height to the canvas's units, from the
 * pixbuf's size in pixels.
 */
static void teg_canvas_image_convert_pixbuf_sizes(GooCanvasItem *item,
        TegCanvasImageData *image_data)
{
	TegCanvasImagePrivate *priv = teg_canvas_image_get_private(item);

	const double original_width = image_data->width;

	GooCanvas *canvas = goo_canvas_item_get_canvas(item);
	if(canvas) {
		goo_canvas_convert_units_from_pixels(canvas,
		                                     &(image_data->width),
		                                     &(image_data->height));
	}

	if(image_data->width) { /* Avoid division by zero. */
		priv->scale_to_units = original_width / image_data->width;
	} else {
		priv->scale_to_units = 1.0f;
	}
}

GooCanvasItem* teg_canvas_image_new(GooCanvasItem *parent,
                                    GdkPixbuf     *pixbuf,
                                    gdouble        x,
                                    gdouble        y,
                                    ...)
{
	GooCanvasItem *item;
	TegCanvasImage *image;
	TegCanvasImageData *image_data;
	const char *first_property;
	va_list var_args;

	item = static_cast<GooCanvasItem*>(g_object_new(TEG_TYPE_CANVAS_IMAGE, NULL));

	/* Put it in the parent first because other property setters might need
	 * access to the parent.
	 */
	if(parent) {
		goo_canvas_item_add_child(parent, item, -1);
		g_object_unref(item);
	}

	image = (TegCanvasImage*) item;

	image_data = image->image_data;
	image_data->x = x;
	image_data->y = y;

	if(pixbuf) {
		image_data->pattern = goo_canvas_cairo_pattern_from_pixbuf(pixbuf);
		image_data->width = gdk_pixbuf_get_width(pixbuf);
		image_data->height = gdk_pixbuf_get_height(pixbuf);

		teg_canvas_image_convert_pixbuf_sizes(item, image_data);
	}

	va_start(var_args, y);
	first_property = va_arg(var_args, char*);
	if(first_property) {
		g_object_set_valist((GObject*) item, first_property, var_args);
	}
	va_end(var_args);

	return item;
}

static void teg_canvas_image_dispose(GObject *object)
{
	GooCanvasItemSimple *simple = (GooCanvasItemSimple*) object;
	TegCanvasImage *image = (TegCanvasImage*) object;

	if(!simple->model) {
		cairo_pattern_destroy(image->image_data->pattern);
		image->image_data->pattern = NULL;
	}

	G_OBJECT_CLASS(teg_canvas_image_parent_class)->dispose(object);
}

static void teg_canvas_image_finalize(GObject *object)
{
	GooCanvasItemSimple *simple = (GooCanvasItemSimple*) object;
	TegCanvasImage *image = (TegCanvasImage*) object;

	/* Free our data if we didn't have a model. (If we had a model it would
	   have been reset in dispose() and simple_data will be NULL.) */
	if(simple->simple_data) {
		g_slice_free(TegCanvasImageData, image->image_data);
	}
	image->image_data = NULL;

	G_OBJECT_CLASS(teg_canvas_image_parent_class)->finalize(object);
}

static void teg_canvas_image_get_common_property(GObject            *object,
        TegCanvasImageData *image_data,
        guint               prop_id,
        GValue             *value,
        GParamSpec         *pspec)
{
	TegCanvasImagePrivate *priv = teg_canvas_image_get_private(object);

	switch(prop_id) {
	case PROP_PATTERN:
		g_value_set_boxed(value, image_data->pattern);
		break;
	case PROP_X:
		g_value_set_double(value, image_data->x);
		break;
	case PROP_Y:
		g_value_set_double(value, image_data->y);
		break;
	case PROP_WIDTH:
		g_value_set_double(value, image_data->width);
		break;
	case PROP_HEIGHT:
		g_value_set_double(value, image_data->height);
		break;
	case PROP_SCALE_TO_FIT:
		g_value_set_boolean(value, priv->scale_to_fit);
		break;
	case PROP_ALPHA:
		g_value_set_double(value, priv->alpha);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void teg_canvas_image_get_property(GObject    *object,
        guint       prop_id,
        GValue     *value,
        GParamSpec *pspec)
{
	TegCanvasImage *image = (TegCanvasImage*) object;

	teg_canvas_image_get_common_property(object, image->image_data, prop_id,
	                                     value, pspec);
}

static gboolean teg_canvas_image_set_common_property(GObject            *object,
        TegCanvasImageData *image_data,
        guint               prop_id,
        const GValue       *value,
        GParamSpec         *pspec)
{
	TegCanvasImagePrivate *priv = teg_canvas_image_get_private(object);
	GdkPixbuf *pixbuf;
	gboolean recompute_bounds = TRUE;

	switch(prop_id) {
	case PROP_PATTERN:
		cairo_pattern_destroy(image_data->pattern);
		image_data->pattern = static_cast<cairo_pattern_t*>(g_value_get_boxed(value));
		cairo_pattern_reference(image_data->pattern);
		break;
	case PROP_X:
		image_data->x = g_value_get_double(value);
		break;
	case PROP_Y:
		image_data->y = g_value_get_double(value);
		break;
	case PROP_WIDTH:
		image_data->width = g_value_get_double(value);
		break;
	case PROP_HEIGHT:
		image_data->height = g_value_get_double(value);
		break;
	case PROP_SCALE_TO_FIT:
		priv->scale_to_fit = g_value_get_boolean(value);
		break;
	case PROP_PIXBUF:
		cairo_pattern_destroy(image_data->pattern);
		pixbuf = static_cast<GdkPixbuf*>(g_value_get_object(value));
		image_data->pattern = pixbuf
		                      ? goo_canvas_cairo_pattern_from_pixbuf(pixbuf)
		                      : static_cast<cairo_pattern_t*>(nullptr);
		image_data->width = pixbuf ? gdk_pixbuf_get_width(pixbuf) : 0;
		image_data->height = pixbuf ? gdk_pixbuf_get_height(pixbuf) : 0;

		if(GOO_IS_CANVAS_ITEM(object))
			teg_canvas_image_convert_pixbuf_sizes(GOO_CANVAS_ITEM(object),
			                                      image_data);

		break;
	case PROP_ALPHA:
		priv->alpha = g_value_get_double(value);
		recompute_bounds = FALSE;
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}

	return recompute_bounds;
}

static void teg_canvas_image_set_property(GObject      *object,
        guint         prop_id,
        const GValue *value,
        GParamSpec   *pspec)
{
	GooCanvasItemSimple *simple = (GooCanvasItemSimple*) object;
	TegCanvasImage *image = (TegCanvasImage*) object;
	gboolean recompute_bounds;

	if(simple->model) {
		g_warning("Can't set property of a canvas item with a model - "
		          "set the model property instead");
		return;
	}

	recompute_bounds = teg_canvas_image_set_common_property(object,
	                   image->image_data,
	                   prop_id,
	                   value, pspec);
	goo_canvas_item_simple_changed(simple, recompute_bounds);
}

static gboolean teg_canvas_image_is_item_at(GooCanvasItemSimple *simple,
        gdouble              x,
        gdouble              y,
        cairo_t             *cr,
        gboolean             is_pointer_event)
{
	TegCanvasImage *image = (TegCanvasImage*) simple;
	TegCanvasImageData *image_data = image->image_data;
	cairo_surface_t *surface;
	unsigned char *rawdata;
	guint32 *pixel;
	int alpha, stride;

	if(x < image_data->x || (x > image_data->x + image_data->width)
	        || y < image_data->y || (y > image_data->y + image_data->height)) {
		return FALSE;
	}

	cairo_pattern_get_surface(image_data->pattern, &surface);

	if(cairo_surface_get_type(surface) != CAIRO_SURFACE_TYPE_IMAGE
	        || cairo_image_surface_get_format(surface) != CAIRO_FORMAT_ARGB32) {
		return TRUE;
	}

	/* Check whether the pixel where the pointer is is translucent.  */
	cairo_surface_flush(surface);
	rawdata = cairo_image_surface_get_data(surface);
	stride = cairo_image_surface_get_stride(surface);
	pixel = (guint32*)(rawdata + ((int)(y - image_data->y)) * stride);
	alpha = (pixel[(int)(x - image_data->x)] & 0xff000000) >> 24;

	if(alpha == 0) {
		return FALSE;
	}

	return TRUE;
}


static void teg_canvas_image_update(GooCanvasItemSimple *simple,
                                    cairo_t             *cr)
{
	TegCanvasImage *image = (TegCanvasImage*) simple;
	TegCanvasImageData *image_data = image->image_data;

	/* Compute the new bounds. */
	simple->bounds.x1 = image_data->x;
	simple->bounds.y1 = image_data->y;
	simple->bounds.x2 = image_data->x + image_data->width;
	simple->bounds.y2 = image_data->y + image_data->height;
}

static void teg_canvas_image_paint(GooCanvasItemSimple   *simple,
                                   cairo_t               *cr,
                                   const GooCanvasBounds *bounds)
{
	TegCanvasImagePrivate *priv = teg_canvas_image_get_private(simple);
	TegCanvasImage *image = (TegCanvasImage*) simple;
	TegCanvasImageData *image_data = image->image_data;
	cairo_matrix_t matrix = { 1, 0, 0, 1, 0, 0 };
	cairo_surface_t *surface;
	gdouble width, height;

	if(!image_data->pattern) {
		return;
	}

	/* scale-to-fit means a simple scale, not keeping the aspect ratio.
	   This does not need to consider the units used. */
	if(priv->scale_to_fit) {
		if(cairo_pattern_get_surface(image_data->pattern, &surface)
		        == CAIRO_STATUS_SUCCESS
		        && cairo_surface_get_type(surface) == CAIRO_SURFACE_TYPE_IMAGE) {
			width = cairo_image_surface_get_width(surface);
			height = cairo_image_surface_get_height(surface);
			cairo_matrix_scale(&matrix, width / image_data->width,
			                   height / image_data->height);
		}
	} else if(priv->scale_to_units && (priv->scale_to_units != 1.0f)) {
		/* Scale the image to fit the size in units.
		   We have already scaled the width and height numbers. */
		if(cairo_pattern_get_surface(image_data->pattern, &surface)
		        == CAIRO_STATUS_SUCCESS
		        && cairo_surface_get_type(surface) == CAIRO_SURFACE_TYPE_IMAGE) {
			cairo_matrix_scale(&matrix, priv->scale_to_units,
			                   priv->scale_to_units);
		}
	}

	cairo_matrix_translate(&matrix, -image_data->x, -image_data->y);

	cairo_pattern_set_matrix(image_data->pattern, &matrix);
	goo_canvas_style_set_fill_options(simple->simple_data->style, cr);
	cairo_set_source(cr, image_data->pattern);
	cairo_rectangle(cr, image_data->x, image_data->y,
	                image_data->width, image_data->height);
	/* To have better performance, we don't use cairo_paint_with_alpha if
	 * the image is not transparent at all. */
	if(priv->alpha != 1.0) {
		cairo_clip(cr);
		cairo_paint_with_alpha(cr, priv->alpha);
	} else {
		cairo_fill(cr);
	}
}

static void teg_canvas_image_set_model(GooCanvasItem      *item,
                                       GooCanvasItemModel *model)
{
	GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
	TegCanvasImage *image = (TegCanvasImage*) item;
	TegCanvasImageModel *imodel = (TegCanvasImageModel*) model;

	/* If our data was allocated, free it. */
	if(!simple->model) {
		cairo_pattern_destroy(image->image_data->pattern);
		g_slice_free(TegCanvasImageData, image->image_data);
	}

	/* Now use the new model's data instead. */
	image->image_data = &imodel->image_data;

	/* Let the parent GooCanvasItemSimple code do the rest. */
	goo_canvas_item_simple_set_model(simple, model);
}

static void canvas_item_interface_init(GooCanvasItemIface *iface)
{
	iface->set_model = teg_canvas_image_set_model;
}

static void teg_canvas_image_class_init(TegCanvasImageClass *klass)
{
	GObjectClass *gobject_class = (GObjectClass*) klass;
	GooCanvasItemSimpleClass *simple_class = (GooCanvasItemSimpleClass*) klass;

	gobject_class->dispose  = teg_canvas_image_dispose;
	gobject_class->finalize = teg_canvas_image_finalize;

	gobject_class->get_property = teg_canvas_image_get_property;
	gobject_class->set_property = teg_canvas_image_set_property;

	simple_class->simple_update      = teg_canvas_image_update;
	simple_class->simple_paint       = teg_canvas_image_paint;
	simple_class->simple_is_item_at  = teg_canvas_image_is_item_at;

	teg_canvas_image_install_common_properties(gobject_class);
}

//------------------------------------------------------------------------------

static void teg_canvas_image_model_class_init(TegCanvasImageModelClass *klass)
{
	GObjectClass *gobject_class = (GObjectClass*) klass;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	g_type_class_add_private(gobject_class, sizeof(TegCanvasImagePrivate));
#pragma GCC diagnostic pop

	gobject_class->dispose      = teg_canvas_image_model_dispose;

	gobject_class->get_property = teg_canvas_image_model_get_property;
	gobject_class->set_property = teg_canvas_image_model_set_property;

	teg_canvas_image_install_common_properties(gobject_class);
}

static void teg_canvas_image_model_init(TegCanvasImageModel *emodel)
{
	TegCanvasImagePrivate *priv = TEG_CANVAS_IMAGE_MODEL_GET_PRIVATE(emodel);

	priv->alpha = 1.0;
}

GooCanvasItemModel* teg_canvas_image_model_new(GooCanvasItemModel *parent,
        GdkPixbuf          *pixbuf,
        gdouble             x,
        gdouble             y,
        ...)
{
	GooCanvasItemModel *model;
	TegCanvasImageModel *imodel;
	TegCanvasImageData *image_data;
	const char *first_property;
	va_list var_args;

	model = static_cast<GooCanvasItemModel*>(
	            g_object_new(TEG_TYPE_CANVAS_IMAGE_MODEL, NULL));
	imodel = (TegCanvasImageModel*) model;

	image_data = &imodel->image_data;
	image_data->x = x;
	image_data->y = y;

	if(pixbuf) {
		image_data->pattern = goo_canvas_cairo_pattern_from_pixbuf(pixbuf);
		image_data->width = gdk_pixbuf_get_width(pixbuf);
		image_data->height = gdk_pixbuf_get_height(pixbuf);
	}

	va_start(var_args, y);
	first_property = va_arg(var_args, char*);
	if(first_property) {
		g_object_set_valist((GObject*) model, first_property, var_args);
	}
	va_end(var_args);

	if(parent) {
		goo_canvas_item_model_add_child(parent, model, -1);
		g_object_unref(model);
	}

	return model;
}

static void teg_canvas_image_model_dispose(GObject *object)
{
	TegCanvasImageModel *imodel = (TegCanvasImageModel*) object;

	cairo_pattern_destroy(imodel->image_data.pattern);
	imodel->image_data.pattern = NULL;

	G_OBJECT_CLASS(teg_canvas_image_model_parent_class)->dispose(object);
}

static void teg_canvas_image_model_get_property(GObject    *object,
        guint       prop_id,
        GValue     *value,
        GParamSpec *pspec)
{
	TegCanvasImageModel *imodel = (TegCanvasImageModel*) object;

	teg_canvas_image_get_common_property(object, &imodel->image_data, prop_id,
	                                     value, pspec);
}


static void teg_canvas_image_model_set_property(GObject      *object,
        guint         prop_id,
        const GValue *value,
        GParamSpec   *pspec)
{
	TegCanvasImageModel *imodel = (TegCanvasImageModel*) object;
	gboolean recompute_bounds;

	recompute_bounds = teg_canvas_image_set_common_property(object,
	                   &imodel->image_data,
	                   prop_id,
	                   value, pspec);
	g_signal_emit_by_name(imodel, "changed", recompute_bounds);
}

static GooCanvasItem* teg_canvas_image_model_create_item(GooCanvasItemModel *model,
        GooCanvas          *canvas)
{
	GooCanvasItem *item;

	item = static_cast<GooCanvasItem*>(g_object_new(TEG_TYPE_CANVAS_IMAGE, NULL));
	goo_canvas_item_set_model(item, model);

	return item;
}

static void item_model_interface_init(GooCanvasItemModelIface *iface)
{
	iface->create_item    = teg_canvas_image_model_create_item;
}

}
