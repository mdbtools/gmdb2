[![Build Status](https://github.com/mdbtools/gmdb2/workflows/build/badge.svg)](https://github.com/mdbtools/gmdb2/actions)

gmdb2
--

This is the official GUI for [mdbtools](https://github.com/mdbtools/mdbtools). It requires:

* GTK+ 2.14 or later
* libglade-2.0-dev
* libgnomeui-2.0-dev
* gnome-doc-utils

This standalone package is considered beta-quality software. For a stable release,
download MDB Tools version 0.7.1 or earlier and compile with the above
libraries installed.

Building from source requires autotools:

* autoreconf -f -i -Wno-portability -Wno-obsolete
* ./configure
* make
* make install

If you don't have scrollkeeper installed on your system (a likely scenario),
you will want to pass `--disable-scrollkeeper` to the configure script.

![Screenshot](https://mdbtools.github.io/static/images/gmdb2screenshot.png)
