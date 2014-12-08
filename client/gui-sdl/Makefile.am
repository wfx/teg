## Process this file with automake to produce Makefile.in

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
	gui.c


@INTLTOOL_DESKTOP_RULE@
