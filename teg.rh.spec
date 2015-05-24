# spec file for package teg
# based on the original file for Suse by David Haller

%define  ver     0.10.1
%define  RELEASE 1
%define  rel     %{?CUSTOM_RELEASE} %{!?CUSTOM_RELEASE:%RELEASE}
%define  prefix  /usr
%define  sysdocs /usr/doc/packages

Name:      		teg
Summary:   		Clone of a Risk clone
Version:		%ver
Release:		%rel
Distribution:		RedHat
Copyright: 	  	GPL
Group: 			Amusements/Games
URL:			http://teg.sourceforge.net/
Packager:     		Ricardo Quesada <riq@corest.com>
Requires:		glibc gnome-libs esound gtk+ imlib glib
Provides:		teg
#Autoreqprov:		On

BuildRequires: gnome-libs-devel
Docdir:    %{sysdocs}
BuildRoot: /var/tmp/rpm/teg-%{PACKAGE_VERSION}-root

Source:     http://prdownloads.sourceforge.net/teg/teg-%{PACKAGE_VERSION}.tar.gz

%description
Tenes Emapandas Graciela (TEG) is a clone of 'Plan Táctico y Estratégico
de la Guerra' (Tactical and Strategic plan of the War), which is a
pseudo-clone of Risk, a turn-based strategy game. Some rules are different.

Authors:
--------
	Ricardo Quesada (riq@corest.com)

%prep
%setup
CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%{prefix} --with-included-gettext

%build
make

%install
rm -rf $RPM_BUILD_ROOT
make prefix=$RPM_BUILD_ROOT%{prefix} install
strip $RPM_BUILD_ROOT%{prefix}/bin/*
rm $RPM_BUILD_ROOT%{prefix}/share/locale/locale.alias

%clean
#rm -rf $RPM_BUILD_DIR/teg-%{ver}
#rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%{prefix}/bin/tegclient
%{prefix}/bin/tegserver
%{prefix}/bin/tegrobot
%{prefix}/share/gnome/help/teg/C/*
%{prefix}/share/locale
%{prefix}/share/pixmaps
%{prefix}/share/gnome/apps/Games/teg.desktop

%doc ABOUT-NLS AUTHORS COPYING ChangeLog INSTALL NEWS README TODO README.GGZ PEOPLE HACKING

%changelog
* Wed Aug 28 2002 Ganesh Sittampalam <ganesh-rpms@earth.li>
- Updated to 0.10.1
- Added BuildRequires: gnome-libs-devel
- Removed /usr/share/locale/locale.alias from final package
