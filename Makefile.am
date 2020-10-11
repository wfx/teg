SUBDIRS=po . docs server client robot

EXTRA_DIST= teg.suse.spec teg.rh.spec README HACKING PEOPLE protocol.txt ReleaseNotes.txt

ACLOCAL_FLAGS = -I `pwd`/macros

noinst_LIBRARIES = libtegcommon.a

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
