noinst_LIBRARIES = libguiclient.a
INCLUDES = \
	$(TEG_LIBGNOME_CFLAGS)				\
	$(XML_CFLAGS)					\
	-I$(srcdir)/..					\
	-I$(top_srcdir)/common				\
	-I../../intl					\
	-DPIXMAPDIR=\"$(datadir)/pixmaps/\"		\
	-DTHEMEDIR=\"$(datadir)/pixmaps/teg_pix/themes/\"	\
	-DG_LOG_DOMAIN=\"TEG\"				\
	-DDATADIR=\""$(datadir)"\"		   	\
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\" 


SUBDIRS= stock

libguiclient_a_SOURCES = \
	gui.c gui.h		\
	callbacks.c callbacks.h	\
	interface.c interface.h	\
	chatline.c chatline.h	\
	priv.c priv.h		\
	support.c support.h	\
	g_cont.c g_cont.h	\
	g_country.c g_country.h	\
	stock.c stock.h		\
	cards.c cards.h		\
	dices.c dices.h		\
	mission.c mission.h	\
	status.c status.h	\
	connect.c connect.h	\
	armies.c armies.h	\
	colors.c colors.h	\
	preferences.c preferences.h \
	g_scores.c g_scores.h \
	locate_country.c locate_country.h \
	tegcanvasimage.c tegcanvasimage.h \
	stock_gresources.c stock_gresources.h \
	fonts.h			\
	robot.xpm

CLEANFILES = stock_gresources.h stock_gresources.c
RESOURCE = stock.gresource.xml

$(srcdir)/stock.c: stock_gresources.h stock_gresources.c
stock_gresources.c: $(RESOURCE) stock_gresources.h $(shell \
	$(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir=$(srcdir) $(RESOURCE))
	$(GLIB_COMPILE_RESOURCES) --sourcedir=$(srcdir) --target=$@ \
	  --generate-source --manual-register --c-name=stock $<
stock_gresources.h:  $(RESOURCE) $(shell \
	$(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir=$(srcdir) $(RESOURCE))
	$(GLIB_COMPILE_RESOURCES) --sourcedir=$(srcdir) --target=$@ \
	  --generate-header --manual-register --c-name=stock $<

EXTRA_DIST = $(gsettings_SCHEMAS) $(RESOURCE)

gsettings_SCHEMAS = net.sf.teg.gschema.xml

@GSETTINGS_RULES@

data_desktopdir = $(datadir)/applications
dist_data_desktop_DATA = teg.desktop

data_convertdir = $(datadir)/GConf/gsettings
dist_data_convert_DATA = teg.convert
