## Process this file with automake to produce Makefile.in

bin_PROGRAMS = tegrobot

INCLUDES = 	\
	@CLIENT_CFLAGS@	\
	@XML_CFLAGS@ \
	-I$(top_srcdir)/common -I$(top_srcdir)/client -I../intl 

tegrobot_SOURCES = 		\
	main.c main.h		\
	gui.c gui.h		\
	ai.c ai.h		\
	dijkstra.c dijkstra.h	\
	ai_msg.c ai_msg.h	\
	ai_misc.c ai_misc.h	\
	ai_fichas.c ai_fichas.h


tegrobot_DEPENDENCIES = ../client/libclient.a ../common/libtegcommon.a $(INTLDEPS)
tegrobot_LDADD        = ../client/libclient.a @CLIENT_LIBS@ ../common/libtegcommon.a $(INTLLIBS) @XML_LIBS@
