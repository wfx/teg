## Process this file with automake to produce Makefile.in

noinst_LIBRARIES = libguiclient.a
INCLUDES = -I$(srcdir)/..  -I$(top_srcdir)/common -I../../intl @CLIENT_CFLAGS@

## Above, note -I../../intl instead of -I$(top_srdir/intl) is deliberate.

libguiclient_a_SOURCES = \
	null.c
