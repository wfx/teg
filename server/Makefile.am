## Process this file with automake to produce Makefile.in

bin_PROGRAMS = tegserver
INCLUDES = -I$(top_srcdir)/common -I../intl $(GGZINC)	\
	@SERVER_CFLAGS@					\
	@XML_CFLAGS@					\
	@GGZDMOD_INCLUDES@				\
	-DBINDIR=\"$(bindir)\"


## Above, note -I../intl instead of -I$(top_srdir/intl) is deliberate.

tegserver_SOURCES = \
	main.c main.h		\
	globals.h		\
	server.h		\
	player.c player.h	\
	sernet.c sernet.h	\
	countries.c countries.h	\
	play.c play.h		\
	turno.c turno.h		\
	console.c console.h	\
	helper.c helper.h	\
	missions.c missions.h \
	ggz_server.c ggz_server.h	\
	options.c options.h	\
	xmlscores.c xmlscores.h	\
	metaserver.c metaserver.h	\
	fow.c fow.h


tegserver_DEPENDENCIES = ../common/libtegcommon.a $(INTLDEPS)
tegserver_LDADD        = ../common/libtegcommon.a @GGZDMOD_LDFLAGS@ @LIB_GGZDMOD@ $(INTLLIBS) $(SERVER_LIBS) @XML_LIBS@
