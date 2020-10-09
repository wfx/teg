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

SUBDIRS= macros po common docs $(GSUBDIRS) $(SSUBDIRS) $(CSUBDIRS) $(RSUBDIRS)

EXTRA_DIST= teg.suse.spec teg.rh.spec README HACKING PEOPLE protocol.txt ReleaseNotes.txt

ACLOCAL_FLAGS = -I `pwd`/macros

