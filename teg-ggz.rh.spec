# spec file for package teg
# based on the original file for Suse by David Haller

%define  ver     0.9.0
%define  RELEASE 1
%define  rel     %{?CUSTOM_RELEASE} %{!?CUSTOM_RELEASE:%RELEASE}
%define  prefix  /usr
%define  sysdocs /usr/doc/packages

Name:      		teg
Summary:   		Clone of a Risk clone
Version:		%ver
Release:		%rel
Distribution:		RedHat Linux 7.1 (i386)
Copyright: 	  	GPL
Group: 			Amusements/Games
URL:			http://teg.sourceforge.net/
Packager:     		Ricardo Quesada <riq@core-sdi.com>
Requires:		glibc gnome-libs esound gtk+ imlib glib libggzcore.so.0
Provides:		teg
#Autoreqprov:		On

Docdir:    %{sysdocs}
BuildRoot: /var/tmp/rpm/teg-%{PACKAGE_VERSION}-root

Source:     teg-%{PACKAGE_VERSION}.tar.gz

%description
Tenes Emapandas Graciela (TEG) is a clone of 'Plan Táctico y Estratégico
de la Guerra' (Tactical and Strategic plan of the War), which is a
pseudo-clone of Risk, a turn-based strategy game. Some rules are 
different.

This package works with GGZ, and requires the ggz-client-libs package to work.
See http://ggz.sourceforge.net/ for more.

Authors:
--------
	Ricardo Quesada (riq@core-sdi.com)

%prep
%setup
CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%{prefix} --with-included-gettext

%build
make

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/etc/
touch $RPM_BUILD_ROOT/etc/ggz.modules
make prefix=$RPM_BUILD_ROOT%{prefix} install
strip $RPM_BUILD_ROOT%{prefix}/bin/*
rm $RPM_BUILD_ROOT/etc/ggz.modules
rmdir $RPM_BUILD_ROOT/etc

# install the GGZ client file.  We have to change the "/usr/local" to 
# whatever the prefix is.
mkdir -p $RPM_BUILD_ROOT%{prefix}/share/ggz/ggz-config
sed sX/usr/localX%{prefix}X ggz/tegclient.dsc > $RPM_BUILD_ROOT%{prefix}/share/ggz/ggz-config/teg.dsc

# install the GGZ server files.  This will only work if ggz-server is
# installed at the same prefix (as from the RPM), but it won't really
# _hurt_ otherwise so...
mkdir -p $RPM_BUILD_ROOT/etc/ggzd/games/
mkdir -p $RPM_BUILD_ROOT/etc/ggzd/rooms/
cp ggz/teg.dsc $RPM_BUILD_ROOT/etc/ggzd/games/
cp ggz/teg.room $RPM_BUILD_ROOT/etc/ggzd/rooms/

%post
ggz-config --install --fromfile=/usr/share/ggz/ggz-config/teg.dsc

%preun
ggz-config --remove --fromfile=/usr/share/ggz/ggz-config/teg.dsc

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

# GGZ client file
%{prefix}/share/ggz/ggz-config/teg.dsc

# GGZ server files
/etc/ggzd/games/teg.dsc
/etc/ggzd/rooms/teg.room


%doc ABOUT-NLS AUTHORS COPYING ChangeLog INSTALL NEWS README TODO README.GGZ

%changelog
* Wed Jun 20 2001 Jason Short <jdorje@users.sourceforge.net>
- Added info for GGZ files.
