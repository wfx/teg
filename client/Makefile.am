INCLUDES = -I$(top_srcdir)/common -I../intl	\
	@CLIENT_CFLAGS@					\
	@XML_CFLAGS@					\
	-DBINDIR=\"$(bindir)\"				\
	-DTHEMEDIR=\"$(datadir)/pixmaps/teg_pix/themes/\"

SUBDIRS = teg_pix themes gui-gnome

noinst_LIBRARIES = libclient.a
libclient_a_SOURCES = \
	client.h		\
	globals.h	\
	player.c player.h	\
	misc.c misc.h		\
	inputfd.c inputfd.h	\
	outputfd.c outputfd.h	\
	countries.c countries.h	\
	helper.c helper.h	\
	fichas.c fichas.h	\
	attack.c attack.h	\
	reagrupe.c reagrupe.h	\
	ejer2.c ejer2.h		\
	canje.c canje.h		\
	themes.c themes.h

bin_PROGRAMS = tegclient

tegclient_SOURCES = \
	main.c main.h

tegclient_DEPENDENCIES = gui-gnome/libguiclient.a libclient.a ../libtegcommon.a $(INTLDEPS)
tegclient_LDADD        = gui-gnome/libguiclient.a @TEG_LIBGNOME_LIBS@ libclient.a ../libtegcommon.a $(INTLLIBS) @CLIENT_LIBS@ @XML_LIBS@
