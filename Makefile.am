SUBDIRS=po . docs client robot

EXTRA_DIST= teg.suse.spec teg.rh.spec README HACKING PEOPLE protocol.txt ReleaseNotes.txt

ACLOCAL_FLAGS = -I `pwd`/macros

noinst_LIBRARIES = libtegcommon.a
bin_PROGRAMS = tegserver

AM_CPPFLAGS = @XML_CFLAGS@

libtegcommon_a_SOURCES = 	\
	common/all.h			\
	common/net.c common/net.h		\
	common/share.c	common/share.h		\
	common/fcintl.h		\
	common/protocol.h		\
	common/common.c common/common.h	\
	common/parser.c common/parser.h	\
	common/tegdebug.h		\
	common/version.h		\
	common/support.c common/support.h	\
	common/tarjeta.c common/tarjeta.h	\
	common/country.c common/country.h	\
	common/limitrof.h		\
	common/cont.c common/cont.h		\
	common/missions.c common/missions.h	\
	common/stats.c common/stats.h		\
	common/scores.c common/scores.h 	\
	common/my_inet_ntop.c common/my_inet_ntop.h	\
	common/xml_support.c common/xml_support.h

tegserver_CPPFLAGS = $(AM_CPPFLAGS) @SERVER_CFLAGS@ -I$(top_srcdir)/common -DBINDIR=\"$(bindir)\"
tegserver_SOURCES = \
	server/main.c server/main.h		\
	server/globals.h		\
	server/server.h		\
	server/player.c server/player.h	\
	server/sernet.c server/sernet.h	\
	server/countries.c countries.h	\
	server/play.c server/play.h		\
	server/turno.c server/turno.h		\
	server/console.c server/console.h	\
	server/helper.c server/helper.h	\
	server/missions.c server/missions.h \
	server/options.c server/options.h	\
	server/xmlscores.c server/xmlscores.h	\
	server/fow.c server/fow.h

tegserver_DEPENDENCIES = libtegcommon.a $(INTLDEPS)
tegserver_LDADD        = libtegcommon.a $(INTLLIBS) $(SERVER_LIBS) @XML_LIBS@
