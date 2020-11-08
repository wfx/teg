SUBDIRS = . docs po client/themes client/teg_pix

EXTRA_DIST= README HACKING PEOPLE protocol.txt \
    $(gsettings_SCHEMAS) $(RESOURCE)

ACLOCAL_FLAGS = -I `pwd`/macros

noinst_LIBRARIES = libtegcommon.a libclient.a libguiclient.a libtegserver.a
bin_PROGRAMS = tegserver tegclient tegrobot

data_desktopdir = $(datadir)/applications
dist_data_desktop_DATA = client/gui-gnome/teg.desktop

data_convertdir = $(datadir)/GConf/gsettings
dist_data_convert_DATA = client/gui-gnome/teg.convert

check_PROGRAMS = tegtest tegservertest
TESTS = $(check_PROGRAMS)

libtegcommon_a_CPPFLAGS = @XML_CFLAGS@
libtegcommon_a_SOURCES = 	\
	common/net.c common/net.h		\
	common/share.c	common/share.h		\
	common/fcintl.h		\
	common/protocol.h		\
	common/common.c common/common.h	\
	common/parser.c common/parser.h	\
	common/tegdebug.h		\
	common/version.h		\
	common/tarjeta.c common/tarjeta.h	\
	common/country.c common/country.h	\
	common/limitrof.h		\
	common/cont.c common/cont.h		\
	common/missions.c common/missions.h	\
	common/stats.c common/stats.h		\
        common/scores.c common/scores.h 	\
	common/xml_support.c common/xml_support.h

libtegserver_a_CPPFLAGS = @XML_CFLAGS@ @SERVER_CFLAGS@ -I$(top_srcdir)/common -DBINDIR=\"$(bindir)\"
libtegserver_a_SOURCES = \
        server/globals.h server/main.h		\
	server/server.h		\
	server/player.c server/player.h	\
	server/sernet.c server/sernet.h	\
        server/countries.c server/countries.h	\
	server/play.c server/play.h		\
	server/turno.c server/turno.h		\
	server/console.c server/console.h	\
	server/helper.c server/helper.h	\
	server/missions.c server/missions.h \
	server/options.c server/options.h	\
	server/xmlscores.c server/xmlscores.h	\
	server/fow.c server/fow.h

tegserver_CPPFLAGS = @XML_CFLAGS@ @SERVER_CFLAGS@ -I$(top_srcdir)/common
tegserver_SOURCES = server/main.c
tegserver_LDADD = libtegserver.a libtegcommon.a $(INTLLIBS) $(SERVER_LIBS) @XML_LIBS@

libclient_a_CPPFLAGS = @CLIENT_CFLAGS@ @XML_CFLAGS@ -I$(top_srcdir)/common \
	-DBINDIR=\"$(bindir)\" -DTHEMEDIR=\"$(datadir)/pixmaps/teg_pix/themes/\"
libclient_a_SOURCES = \
	client/client.h		\
	client/globals.h	\
	client/player.c client/player.h	\
	client/misc.c client/misc.h		\
	client/inputfd.c client/inputfd.h	\
	client/outputfd.c client/outputfd.h	\
	client/helper.c client/helper.h	\
	client/fichas.c client/fichas.h	\
	client/attack.c client/attack.h	\
	client/reagrupe.c client/reagrupe.h	\
	client/ejer2.c client/ejer2.h		\
	client/canje.c client/canje.h		\
	client/themes.c client/themes.h


tegclient_CPPFLAGS = @XML_CFLAGS@ -I$(top_srcdir)/common
tegclient_SOURCES = client/main.c

tegclient_DEPENDENCIES = libguiclient.a libclient.a libtegcommon.a $(INTLDEPS)
tegclient_LDADD        = libguiclient.a @TEG_LIBGNOME_LIBS@ libclient.a libtegcommon.a $(INTLLIBS) @CLIENT_LIBS@ @XML_LIBS@

libguiclient_a_CPPFLAGS = \
	$(TEG_LIBGNOME_CFLAGS)				\
	@XML_CFLAGS@					\
	-I$(top_srcdir)/common				\
	-I$(top_srcdir)/client				\
	-DPIXMAPDIR=\"$(datadir)/pixmaps/\"		\
	-DTHEMEDIR=\"$(datadir)/pixmaps/teg_pix/themes/\"	\
	-DG_LOG_DOMAIN=\"TEG\"				\
	-DDATADIR=\""$(datadir)"\"		   	\
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\" 

libguiclient_a_SOURCES = \
	client/gui-gnome/gui.c client/gui-gnome/gui.h		\
	client/gui-gnome/callbacks.c client/gui-gnome/callbacks.h	\
	client/gui-gnome/interface.c client/gui-gnome/interface.h	\
	client/gui-gnome/chatline.c client/gui-gnome/chatline.h	\
	client/gui-gnome/priv.c client/gui-gnome/priv.h		\
	client/gui-gnome/support.c client/gui-gnome/support.h	\
	client/gui-gnome/g_cont.c client/gui-gnome/g_cont.h	\
	client/gui-gnome/g_country.c client/gui-gnome/g_country.h	\
	client/gui-gnome/stock.c client/gui-gnome/stock.h		\
	client/gui-gnome/cards.c client/gui-gnome/cards.h		\
	client/gui-gnome/dices.c client/gui-gnome/dices.h		\
	client/gui-gnome/mission.c client/gui-gnome/mission.h	\
	client/gui-gnome/status.c client/gui-gnome/status.h	\
	client/gui-gnome/connect.c client/gui-gnome/connect.h	\
	client/gui-gnome/armies.c client/gui-gnome/armies.h	\
	client/gui-gnome/colors.c client/gui-gnome/colors.h	\
	client/gui-gnome/preferences.c client/gui-gnome/preferences.h \
	client/gui-gnome/g_scores.c client/gui-gnome/g_scores.h \
	client/gui-gnome/locate_country.c client/gui-gnome/locate_country.h \
	client/gui-gnome/tegcanvasimage.c client/gui-gnome/tegcanvasimage.h \
	stock_gresources.c stock_gresources.h \
	client/gui-gnome/fonts.h			\
	client/gui-gnome/robot.xpm

CLEANFILES = stock_gresources.h stock_gresources.c
RESOURCE = $(srcdir)/client/gui-gnome/stock.gresource.xml

$(srcdir)/client/gui-gnome/stock.c: stock_gresources.h stock_gresources.c
stock_gresources.c: $(RESOURCE) stock_gresources.h $(shell \
	$(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir=$(srcdir)/client/gui-gnome $(RESOURCE))
	$(GLIB_COMPILE_RESOURCES) --sourcedir=$(srcdir)/client/gui-gnome --target=$@ \
	  --generate-source --manual-register --c-name=stock $<
stock_gresources.h:  $(RESOURCE) $(shell \
	$(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir=$(srcdir)/client/gui-gnome $(RESOURCE))
	$(GLIB_COMPILE_RESOURCES) --sourcedir=$(srcdir)/client/gui-gnome --target=$@ \
	  --generate-header --manual-register --c-name=stock $<

gsettings_SCHEMAS = client/gui-gnome/net.sf.teg.gschema.xml

@GSETTINGS_RULES@

tegrobot_CPPFLAGS = \
	@CLIENT_CFLAGS@	\
	@XML_CFLAGS@ \
	-I$(top_srcdir)/common -I$(top_srcdir)/client

tegrobot_SOURCES = 		\
        robot/main.c \
        robot/gui.c \
	robot/ai.c robot/ai.h		\
	robot/ai_msg.c robot/ai_msg.h	\
	robot/ai_misc.c robot/ai_misc.h	\
	robot/ai_fichas.c robot/ai_fichas.h


tegrobot_DEPENDENCIES = libclient.a libtegcommon.a $(INTLDEPS)
tegrobot_LDADD        = libclient.a @CLIENT_LIBS@ libtegcommon.a $(INTLLIBS) @XML_LIBS@

tegtest_CPPFLAGS = @XML_CFLAGS@
tegtest_SOURCES = common/xml_support_check.cpp common/tarjeta_check.cpp \
    common/stats_check.cpp common/share_check.cpp common/scores_check.cpp \
    common/parser_check.cpp common/common_check.cpp \
    client/helper_check.cpp client/gui-null/null.c
tegtest_LDADD = -lgmock_main -lgtest -lgmock libclient.a libtegcommon.a @CLIENT_LIBS@ @XML_LIBS@ -lpthread
tegtest_LDFLAGS = -Wl,--as-needed

tegservertest_CPPFLAGS = -DTEGSERVER_OMIT_MAIN=1
tegservertest_SOURCES = server/main.c server/helper_check.cpp server/player_check.cpp
tegservertest_LDADD = libtegserver.a libtegcommon.a \
    -lgmock_main -lgtest -lgmock @SERVER_LIBS@ @XML_LIBS@ -lpthread
