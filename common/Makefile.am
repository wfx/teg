## Process this file with automake to produce Makefile.in

INCLUDES = -I../intl				\
	@TEG_COMMONLIBS_CFLAGS@ \
	@XML_CFLAGS@

noinst_LIBRARIES = libtegcommon.a

## Above, note -I../intl instead of -I$(top_srdir/intl) is deliberate.

libtegcommon_a_SOURCES = 	\
	all.h			\
	net.c net.h		\
	share.c	share.h		\
	fcintl.h		\
	protocol.h		\
	common.c common.h	\
	parser.c parser.h	\
	tegdebug.h		\
	version.h		\
	support.c support.h	\
	tarjeta.c tarjeta.h	\
	country.c country.h	\
	limitrof.h		\
	cont.c cont.h		\
	missions.c missions.h	\
	stats.c stats.h		\
	scores.c scores.h 	\
	my_inet_ntop.c my_inet_ntop.h	\
	xml_support.c xml_support.h
