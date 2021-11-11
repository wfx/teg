ACLOCAL_AMFLAGS = -I macros
AUTOMAKE_OPTIONS = foreign

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
	common/net.cpp common/net.h		\
	common/share.cpp	common/share.h		\
	common/fcintl.h		\
	common/protocol.h		\
	common/common.cpp common/common.h	\
	common/parser.cpp common/parser.h	\
	common/tegdebug.h		\
	common/version.h		\
	common/tarjeta.cpp common/tarjeta.h	\
	common/country.cpp common/country.h	\
	common/limitrof.h		\
	common/cont.cpp common/cont.h		\
	common/missions.cpp common/missions.h	\
	common/stats.cpp common/stats.h		\
        common/scores.cpp common/scores.h 	\
	common/xml_support.cpp common/xml_support.h \
	common/execute.cpp common/execute.hpp

libtegserver_a_CPPFLAGS = @XML_CFLAGS@ @SERVER_CFLAGS@ -I$(top_srcdir)/common
libtegserver_a_SOURCES = \
        server/globals.h server/main.h		\
	server/server.h		\
	server/player.cpp server/player.h	\
	server/sernet.cpp server/sernet.h	\
	server/countries.cpp server/countries.h	\
	server/play.cpp server/play.h		\
	server/turno.cpp server/turno.h		\
	server/console.cpp server/console.h	\
	server/helper.cpp server/helper.h	\
	server/missions.cpp server/missions.h \
	server/options.cpp server/options.h	\
	server/xmlscores.cpp server/xmlscores.h	\
	server/fow.cpp server/fow.h

tegserver_CPPFLAGS = @XML_CFLAGS@ @SERVER_CFLAGS@ -I$(top_srcdir)/common
tegserver_SOURCES = server/main.cpp
tegserver_LDADD = libtegserver.a libtegcommon.a $(INTLLIBS) $(SERVER_LIBS) \
                  @XML_LIBS@ @CXXLIBS@

libclient_a_CPPFLAGS = @CLIENT_CFLAGS@ @XML_CFLAGS@ -I$(top_srcdir)/common \
	-DTHEMEDIR=\"$(datadir)/teg/themes/\"
libclient_a_SOURCES = \
	client/client.h		\
	client/globals.h	\
	client/player.cpp client/player.h	\
	client/misc.cpp client/misc.h		\
	client/inputfd.cpp client/inputfd.h	\
	client/outputfd.cpp client/outputfd.h	\
	client/helper.cpp client/helper.h	\
	client/fichas.cpp client/fichas.h	\
	client/attack.cpp client/attack.h	\
	client/reagrupe.cpp client/reagrupe.h	\
	client/ejer2.cpp client/ejer2.h		\
	client/canje.cpp client/canje.h		\
	client/themes.cpp client/themes.h


tegclient_CPPFLAGS = @XML_CFLAGS@ -I$(top_srcdir)/common
tegclient_SOURCES = client/main.cpp

tegclient_DEPENDENCIES = libguiclient.a libclient.a libtegcommon.a $(INTLDEPS)
tegclient_LDADD        = libguiclient.a @TEG_LIBGNOME_LIBS@ libclient.a \
                         libtegcommon.a $(INTLLIBS) @CLIENT_LIBS@ @XML_LIBS@ \
                         @CXXLIBS@

libguiclient_a_CPPFLAGS = \
	$(TEG_LIBGNOME_CFLAGS)				\
	@XML_CFLAGS@					\
	-I$(top_srcdir)/common				\
	-I$(top_srcdir)/client				\
	-DTEGDATADIR=\"$(datadir)/teg/\"		\
	-DTHEMEDIR=\"$(datadir)/teg/themes/\"		\
	-DG_LOG_DOMAIN=\"TEG\"				\
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\" 

libguiclient_a_SOURCES = \
	client/gui-gnome/gui.cpp client/gui-gnome/gui.h		\
	client/gui-gnome/callbacks.cpp client/gui-gnome/callbacks.h	\
	client/gui-gnome/interface.cpp client/gui-gnome/interface.h	\
	client/gui-gnome/chatline.cpp client/gui-gnome/chatline.h	\
	client/gui-gnome/priv.cpp client/gui-gnome/priv.h		\
	client/gui-gnome/support.cpp client/gui-gnome/support.h	\
	client/gui-gnome/g_cont.cpp client/gui-gnome/g_cont.h	\
	client/gui-gnome/g_country.cpp client/gui-gnome/g_country.h	\
	client/gui-gnome/stock.cpp client/gui-gnome/stock.h		\
	client/gui-gnome/cards.cpp client/gui-gnome/cards.h		\
	client/gui-gnome/dices.cpp client/gui-gnome/dices.h		\
	client/gui-gnome/mission.cpp client/gui-gnome/mission.h	\
	client/gui-gnome/status.cpp client/gui-gnome/status.h	\
	client/gui-gnome/connect.cpp client/gui-gnome/connect.h	\
	client/gui-gnome/armies.cpp client/gui-gnome/armies.h	\
	client/gui-gnome/colors.cpp client/gui-gnome/colors.h	\
	client/gui-gnome/preferences.cpp client/gui-gnome/preferences.h \
	client/gui-gnome/g_scores.cpp client/gui-gnome/g_scores.h \
	client/gui-gnome/locate_country.cpp client/gui-gnome/locate_country.h \
	client/gui-gnome/tegcanvasimage.cpp client/gui-gnome/tegcanvasimage.h \
	stock_gresources.c stock_gresources.h \
	client/gui-gnome/fonts.h			\
	client/gui-gnome/robot.xpm

CLEANFILES = stock_gresources.h stock_gresources.c
RESOURCE = $(srcdir)/client/gui-gnome/stock.gresource.xml

$(srcdir)/client/gui-gnome/stock.cpp: stock_gresources.h stock_gresources.c
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
        robot/main.cpp \
        robot/gui.cpp \
	robot/ai.cpp robot/ai.h		\
	robot/ai_msg.cpp robot/ai_msg.h	\
	robot/ai_misc.cpp robot/ai_misc.h	\
	robot/ai_fichas.cpp robot/ai_fichas.h


tegrobot_DEPENDENCIES = libclient.a libtegcommon.a $(INTLDEPS)
tegrobot_LDADD        = libclient.a @CLIENT_LIBS@ libtegcommon.a $(INTLLIBS) @XML_LIBS@ @CXXLIBS@

tegtest_CPPFLAGS = @XML_CFLAGS@
tegtest_SOURCES = common/xml_support_check.cpp common/tarjeta_check.cpp \
    common/stats_check.cpp common/share_check.cpp common/scores_check.cpp \
    common/parser_check.cpp common/common_check.cpp \
    client/helper_check.cpp client/gui-null/null.cpp
tegtest_LDADD = -lgmock_main -lgtest -lgmock libclient.a libtegcommon.a @CLIENT_LIBS@ @XML_LIBS@ @CXXLIBS@ -lpthread
tegtest_LDFLAGS = -Wl,--as-needed

tegservertest_CPPFLAGS = -DTEGSERVER_OMIT_MAIN=1
tegservertest_SOURCES = server/main.cpp server/helper_check.cpp server/player_check.cpp
tegservertest_LDADD = libtegserver.a libtegcommon.a \
    -lgmock_main -lgtest -lgmock @SERVER_LIBS@ @XML_LIBS@ @CXXLIBS@ -lpthread
