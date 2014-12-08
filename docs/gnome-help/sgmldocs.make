# To use this template:
#     1) Define: figs, docname, lang, omffile, xml_ents although figs, 
#        omffile, and xml_ents may be empty in your Makefile.am which 
#        will "include" this one 
#     2) Figures must go under figures/ and be in PNG format
#     3) You should only have one document per directory 
#
#        Note that this makefile forces the directory name under
#        $prefix/share/gnome/help/ to be the same as the XML filename
#        of the document.  This is required by GNOME. eg:
#        $prefix/share/gnome/help/fish_applet/C/fish_applet.xml
#                                 ^^^^^^^^^^^   ^^^^^^^^^^^
# Definitions:
#   figs         A list of screenshots which will be included in EXTRA_DIST
#                Note that these should reside in figures/ and should be .png
#                files, or you will have to make modifications below.
#   docname      This is the name of the XML file: <docname>.xml
#   lang         This is the document locale
#   omffile      This is the name of the OMF file.  Convention is to name
#                it <docname>-<locale>.omf.
#   xml_ents    This is a list of XML entities which must be installed 
#                with the main XML file and included in EXTRA_DIST. 
# eg:
#   figs = \
#          figures/fig1.png            \
#          figures/fig2.png
#   docname = scrollkeeper-manual
#   lang = C
#   omffile=scrollkeeper-manual-C.omf
#   xml_ents = fdl.xml
#   include $(top_srcdir)/help/xmldocs.make
#   dist-hook: app-dist-hook
#

docdir = $(datadir)/gnome/help/$(docname)/$(lang)

doc_DATA = index.html

xml_files = $(xml_ents) $(docname).xml

omf_dir=$(top_srcdir)/omf-install

EXTRA_DIST = $(xml_files) $(doc_DATA) $(omffile) $(figs)

CLEANFILES = omf_timestamp

all: index.html omf

omf: omf_timestamp

omf_timestamp: $(omffile)
	-for file in $(omffile); do \
	  scrollkeeper-preinstall $(docdir)/$(docname).xml $$file $(omf_dir)/$$file; \
	done
	touch omf_timestamp

index.html: $(docname)/index.html
	-cp $(docname)/index.html .

$(docname).xml: $(xml_ents)
        ourdir=`pwd`;  \
        cd $(srcdir);   \
        cp $(xml_ents) $$ourdir


# The weird srcdir trick is because the db2html from the Cygnus RPMs
# cannot handle relative filenames
# Suse Linux 10.0 does not have "db2html", only "docbook2html" which is a script that calls "jw"
# Check if any of those is available.
$(docname)/index.html: $(srcdir)/$(docname).xml
	-srcdir=`cd $(srcdir) && pwd`; \
	test ! "x`xmlto`" = "x" && xmlto --skip-validation html-nochunks html $$srcdir/$(docname).xml

# remove $(docname) to make installation work
app-dist-hook: index.html
	-$(mkinstalldirs) $(distdir)/$(docname)/stylesheet-images
	-$(mkinstalldirs) $(distdir)/figures
	-cp $(srcdir)/*.html $(distdir)/$(docname)
	-cp $(srcdir)/*.css $(distdir)/$(docname)
	-cp $(srcdir)/stylesheet-images/*.gif \
		$(distdir)/$(docname)/stylesheet-images
	-cp $(srcdir)/figures/*.jpg \
		$(distdir)/figures
	-if [ -e topic.dat ]; then \
		cp $(srcdir)/topic.dat $(distdir); \
	 fi

# remove $(docname) to make installation work
install-data-am: index.html omf
	-$(mkinstalldirs) $(DESTDIR)$(docdir)/stylesheet-images
	-$(mkinstalldirs) $(DESTDIR)$(docdir)/figures
	-cp $(srcdir)/$(xml_files) $(DESTDIR)$(docdir)
	-for file in $(srcdir)/*.html $(srcdir)/$(docname)/*.css; do \
	  basefile=`echo $$file | sed -e 's,^.*/,,'`; \
	  $(INSTALL_DATA) $$file $(DESTDIR)$(docdir)/$$basefile; \
	done
	-for file in $(srcdir)/figures/*.jpg; do \
	  basefile=`echo $$file | sed -e 's,^.*/,,'`; \
	  $(INSTALL_DATA) $$file $(DESTDIR)$(docdir)/figures/$$basefile; \
	done
	-for file in $(srcdir)/stylesheet-images/*.gif; do \
	  basefile=`echo $$file | sed -e  's,^.*/,,'`; \
	  $(INSTALL_DATA) $$file $(DESTDIR)$(docdir)/stylesheet-images/$$basefile; \
	done
	-if [ -e $(srcdir)/topic.dat ]; then \
		$(INSTALL_DATA) $(srcdir)/topic.dat $(DESTDIR)$(docdir); \
	 fi

$(docname).ps: $(srcdir)/$(docname).xml
	-srcdir=`cd $(srcdir) && pwd`; \
	db2ps $$srcdir/$(docname).xml

$(docname).rtf: $(srcdir)/$(docname).xml
	-srcdir=`cd $(srcdir) && pwd`; \
	db2ps $$srcdir/$(docname).xml

uninstall-local:
	-for file in $(srcdir)/$(docname)/stylesheet-images/*.gif; do \
	  basefile=`echo $$file | sed -e  's,^.*/,,'`; \
	  rm -f $(docdir)/stylesheet-images/$$basefile; \
	done
	-for file in $(srcdir)/figures/*.jpg; do \
	  basefile=`echo $$file | sed -e  's,^.*/,,'`; \
	  rm -f $(docdir)/figures/$$basefile; \
	done
	-for file in $(srcdir)/$(docname)/*.html $(srcdir)/$(docname)/*.css; do \
	  basefile=`echo $$file | sed -e 's,^.*/,,'`; \
	  rm -f $(DESTDIR)$(docdir)/$$basefile; \
	done
	-for file in $(xml_files); do \
	  rm -f $(DESTDIR)$(docdir)/$$file; \
	done
	-rmdir $(DESTDIR)$(docdir)/stylesheet-images
	-rmdir $(DESTDIR)$(docdir)/figures
	-rmdir $(DESTDIR)$(docdir)
