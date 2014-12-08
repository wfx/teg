## Process this file with automake to produce Makefile.in
Gamesdir = $(datadir)/gnome/apps/Games

noinst_LIBRARIES = libguiclient.a
INCLUDES = \
	$(TEG_LIBGNOME_CFLAGS)				\
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
	fonts.h			\
	robot.xpm



@INTLTOOL_DESKTOP_RULE@

schemadir   = $(sysconfdir)/gconf/schemas
schema_DATA = teg.schemas

EXTRA_DIST = teg.desktop $(schema_DATA)

Games_DATA = teg.desktop

install-data-local:
	GCONF_CONFIG_SOURCE=$(GCONF_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $(srcdir)/$(schema_DATA)
