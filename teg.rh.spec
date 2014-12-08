# spec file for package teg
# based on the original file for Suse by David Haller

<<<<<<< HEAD
%define  ver     0.12
=======
%define  ver     0.11.3
>>>>>>> b8e1f4d5000e931e81e397369fa5ec789dffe26c
%define  RELEASE 1
%define  rel     %{?CUSTOM_RELEASE} %{!?CUSTOM_RELEASE:%RELEASE}
%define  prefix  /usr
%define  sysdocs /usr/doc/packages

Name:      		teg
Summary:   		Clone of a Risk clone
Version:		%ver
Release:		%rel
Distribution:		RedHat
License:		GPL
Group: 			Amusements/Games
URL:			http://teg.sourceforge.net/
Packager:     		Ricardo Quesada <riq@corest.com>
Requires:		glibc gnome-libs esound gtk+ imlib glib
Provides:		teg
#Autoreqprov:		On

BuildRequires: gnome-libs-devel
Docdir:    %{sysdocs}
BuildRoot: /var/tmp/rpm/teg-%{PACKAGE_VERSION}-root

Source:     http://prdownloads.sourceforge.net/teg/teg-%{PACKAGE_VERSION}.tar.bz2

%description
<<<<<<< HEAD
Tenes Emapandas Graciela (TEG) is a clone of 'Plan Tï¿½ctico y Estratï¿½gico
=======
Tenes Emapandas Graciela (TEG) is a clone of 'Plan Táctico y Estratégico
>>>>>>> b8e1f4d5000e931e81e397369fa5ec789dffe26c
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
rm -f $RPM_BUILD_ROOT%{prefix}/share/locale/locale.alias

%clean
#rm -rf $RPM_BUILD_DIR/teg-%{ver}
#rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%{prefix}/etc/gconf/schemas/teg.schemas
%{prefix}/bin/tegclient
%{prefix}/bin/tegserver
%{prefix}/bin/tegrobot
%{prefix}/share/gnome/help/teg/*
%{prefix}/share/locale
%{prefix}/share/pixmaps
%{prefix}/share/gnome/apps/Games/teg.desktop

%doc AUTHORS COPYING ChangeLog INSTALL NEWS README TODO README.GGZ PEOPLE HACKING

%changelog
* Wed Aug 28 2002 Ganesh Sittampalam <ganesh-rpms@earth.li>
- Updated to 0.10.1
- Added BuildRequires: gnome-libs-devel
- Removed /usr/share/locale/locale.alias from final package
