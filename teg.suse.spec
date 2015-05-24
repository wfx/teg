# spec file for package teg
# (c)2000 David Haller

%define  ver     0.11.2
%define  RELEASE 0
%define  rel     %{?CUSTOM_RELEASE} %{!?CUSTOM_RELEASE:%RELEASE}
%define  prefix  /opt/gnome
%define  sysdocs /usr/doc/packages

Name:      		teg
Summary:   		Clone of a Risk clone
Version:		%ver
Release:		%rel
Distribution:	SuSE Linux 10.0
Copyright: 	  	GPL
Group: 			Amusements/Games
URL:			http://teg.sourceforge.net/
Packager:		Stefan Nordhausen <nordi@users.sourceforge.net>
Requires:		glibc esound audiofil gtk2 glib libz libxml2 xorg-x11-server
Provides:		teg
Autoreqprov:	On

Docdir:			%{sysdocs}
BuildRoot:		/var/tmp/rpm/teg-%{PACKAGE_VERSION}-root
# Note: docbook-utils, glibc and tidy are only necessary to build the documentation
# 		perl-XML-Parser is something that configure wants to have, I don't think you really need it
#		work around missing dependencies in Suse Linux 9.3 (and prior?):
#			popt-devel, esound-devel: libgnome-devel needs them
#			gnome-keyring-devel, libjpeg-devel: libgnomeui needs them
#			libgnomeui: contains a file that is in libgnomeui-devel on Suse 10.0
BuildRequires:	gcc, make, pkgconfig, glib2-devel, libxml2-devel, libgnome-devel, libgnomeui-devel, docbook-utils, glibc, tidy, gnome-common, perl-XML-Parser, popt-devel, gnome-keyring-devel, libjpeg-devel, esound-devel, libgnomeui


Source:			teg-%{PACKAGE_VERSION}.tar.gz

%description
Tenes Emapandas Graciela (TEG) is a clone of 'Plan Tactico y Estrategico
de la Guerra' (Tactical and Strategic plan of the War), which is a
pseudo-clone of Risk, a turn-based strategy game. Some rules are different.

Authors:
--------
	Idea to do the game: Sebastian Cativa Tolosa (scativa@hotmail.com)
	Design and implementation by: Ricardo Quesada (riq@core-sdi.com)

%prep
%setup
CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%{prefix} --with-included-gettext

%build
make

%install
rm -rf $RPM_BUILD_ROOT
make prefix=$RPM_BUILD_ROOT%{prefix} install
strip $RPM_BUILD_ROOT%{prefix}/bin/*

%clean
#rm -rf $RPM_BUILD_DIR/teg-%{ver}
#rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%{prefix}/bin/tegclient
%{prefix}/bin/tegserver
%{prefix}/bin/tegrobot
%{prefix}/share/gnome/help/teg/*
%{prefix}/share/locale
%{prefix}/share/pixmaps
%{prefix}/share/gnome/apps/Games/*
%{prefix}/etc/gconf/*

%doc AUTHORS COPYING ChangeLog INSTALL NEWS README

%changelog
* Tue Mar 13 2007 Stefan Nordhausen <nordi@users.sourceforge.net>
- [bug 1634385] Updated french translation (by Cyril Brulebois)
- [bug 1628790] Updated spanish translation (by Carlos Galisteo)

* Sun Apr 2 2006 Stefan Nordhausen <nordi@users.sourceforge.net>
- Change Metaserver address to teg.game-server.cc

* Thu Mar 16 2006 Stefan Nordhausen <nordi@users.sourceforge.net>
- Make sure documentation get installed when doing "make install"
- Adapted the SUSE spec-file to work for SUSE Linux 10.0
    Updated SUSE version to 10.0 (from 6.2)
    Added BuildRequires section
    Required: removed:gnlibs,xshared,gtk,imlib added:gtk2,libxml2
    Added changelog section

* Wed Mar 15 2006 Stefan Nordhausen <nordi@users.sourceforge.net>
- Switched board from png to jpg to save space for themes
  draco, m2

* Tue Mar 14 2006 Stefan Nordhausen <nordi@users.sourceforge.net>
- [bug 991399] Premature victory after surrender fixed
- Increased dialog width (German text did not fit inside)
- Cleaned up sentimental theme
- Switched board from png to jpg to save space for theme
  sentimental
- Make the documentation build under Suse Linux 10.0
- Add missing </param> tags to the Polish documentation

* Mon Mar 13 2006 Stefan Nordhausen <nordi@users.sourceforge.net>
- Fix for crash in tegrobot when placing more than 50 armies

* Thu Mar 09 2006 Stefan Nordhausen <nordi@users.sourceforge.net>
- [bug 1307267] Fixed typo in Spanish translation
- [bug 1107661] Fixed missing detail about getting new cards
- [bug 1107636] Regroup dialog pops up only if selected
- Many typos in German translation fixed
- Several minor fixes in the English documentation
- Intelligent defaults for server port and player status report
- Better i18n: country names are always in local language

* Sun Jan 1 2006 Wolfgang Morawetz <wfx@users.sourceforge.net>
- Remake of "sentimental" theme

* Tue Dec 20 2005 Nacho Resa
- Galician translation added

* Sun Apr 24 2005 Josef Spillner <dr_maux@users.sourceforge.net>
- switch from:
	old autoconf (2.13) / automake (1.4) to
	new autoconf (2.5) / automake (1.8)
  
* Wed Aug 4 2004 Lenart Janos <ocsi@debian.org>
- Hungarian translation added

* Wed Aug 4 2004 Josef Spillner <dr_maux@users.sourceforge.net>
- Patches for ggz 0.0.9
