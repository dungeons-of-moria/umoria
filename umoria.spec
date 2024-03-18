%global debug_package %{nil}

%if 0%{?suse_version} || 0%{?rhel} == 8
%define __builddir %{_vpath_builddir}
%endif

Name:           umoria
Version:        5.7.15
Release:        3%{?dist}
Summary:    	Umoria %{version}

License:        GPL-3.0
URL:            https://umoria.org 
Source0:        umoria-%{version}.tar.gz

BuildRequires:  ncurses-devel gcc-c++ cmake
Requires:       ncurses-libs


%description
The Dungeons of Moria is a single player dungeon simulation originally written by Robert Alan Koeneke, with its first public release in 1983.
The game was originally developed using VMS Pascal before being ported to the C language by James E. Wilson in 1988, and released a Umoria.


%prep
%autosetup


%build

%if 0%{?rhel} == 8
mkdir %{_vpath_builddir} && (cd %{_vpath_builddir}; %cmake ..; %cmake_build )
%else
%cmake
%cmake_build
%endif


%install
mkdir -p $RPM_BUILD_ROOT/%{_bindir}
cp %{_vpath_builddir}/umoria/umoria $RPM_BUILD_ROOT/%{_bindir}/umoria.bin
mkdir -p $RPM_BUILD_ROOT/%{_datadir}/games/umoria
cp -R %{_vpath_builddir}/umoria/data $RPM_BUILD_ROOT/%{_datadir}/games/umoria
mkdir -p $RPM_BUILD_ROOT/%{_docdir}/umoria/historical
cp historical/* $RPM_BUILD_ROOT/%{_docdir}/umoria/historical

cat << EOF > $RPM_BUILD_ROOT/%{_bindir}/umoria
#!/bin/sh
CONFDIR=~/.config/umoria
DATADIR=%{_datadir}/games/umoria/data
BIN=%{_bindir}/umoria.bin
[ ! -d \$CONFDIR ] && mkdir -p \$CONFDIR && ln -s \$DATADIR \$CONFDIR/data
[ ! -f \$CONFDIR/scores.dat ] && touch \$CONFDIR/scores.dat
(cd \$CONFDIR; \$BIN \$@)
EOF

chmod +x $RPM_BUILD_ROOT/%{_bindir}/umoria

%postun
echo "Please remove each user's ~/.config/umoria manually, if you need."

%files
%{_bindir}/umoria*
%{_datadir}/games/umoria/data/*
%{_docdir}/umoria/historical/*
%license LICENSE
%doc *.md AUTHORS



%changelog
* Sat Dec 30 2023 Shiro Hara <white@vx-xv.com> - 5.7.15-3
- Enable roguelike keys via the CLI

* Sat Jul 1 2023 Shiro Hara <white@vx-xv.com> - 5.7.15-2
- Add files in "historical" to doc (Thanks, Justin Koh)

* Sat Feb 18 2023 Shiro Hara <white@vx-xv.com> -5.7.15-1
- Add .spec file
