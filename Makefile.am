## Process this file with automake to produce Makefile.in

if CLIENT
CSUBDIRS = client
else
CSUBDIRS = 
endif

if SERVER
SSUBDIRS = server
else
SSUBDIRS =
endif

if ROBOT
RSUBDIRS = robot
CSUBDIRS = client
else
RSUBDIRS =
endif

if GGZSUPPORT
GSUBDIRS = ggz
else
GSUBDIRS =
endif

SUBDIRS= macros po common docs metaserver $(GSUBDIRS) $(SSUBDIRS) $(CSUBDIRS) $(RSUBDIRS)

EXTRA_DIST= teg-ggz.rh.spec teg.suse.spec teg.rh.spec README README.GGZ HACKING PEOPLE protocol.txt ReleaseNotes.txt

ACLOCAL_FLAGS = -I macros

