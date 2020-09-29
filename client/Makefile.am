## Process this file with automake to produce Makefile.in


if CLIENT
PIXSUBDIRS = teg_pix themes
GUI_SUBDIR = gui-gnome
else
PIXSUBDIRS =
GUI_SUBDIR =
endif


INCLUDES = -I$(top_srcdir)/common -I../intl	\
	@CLIENT_CFLAGS@					\
	@XML_CFLAGS@					\
	@GGZMOD_INCLUDES@				\
	-DBINDIR=\"$(bindir)\"				\
	-DTHEMEDIR=\"$(datadir)/pixmaps/teg_pix/themes/\"

SUBDIRS= $(PIXSUBDIRS) $(GUI_SUBDIR)


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
	themes.c themes.h	\
	metaserver.c metaserver.h	\
	ggz_client.c ggz_client.h

if CLIENT
bin_PROGRAMS = tegclient

tegclient_SOURCES = \
	main.c main.h

tegclient_DEPENDENCIES = @gui_sources@/libguiclient.a libclient.a  ../common/libtegcommon.a $(INTLDEPS)
tegclient_LDADD        = @gui_sources@/libguiclient.a @TEG_LIBGNOME_LIBS@ @GGZMOD_LDFLAGS@ @LIB_GGZMOD@ libclient.a  ../common/libtegcommon.a $(INTLLIBS) @CLIENT_LIBS@ @XML_LIBS@
endif
