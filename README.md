[![Build Status](https://github.com/mdbtools/gmdb2/workflows/build/badge.svg)](https://github.com/mdbtools/gmdb2/actions)

gmdb2
--

This is the official GUI for [mdbtools](https://github.com/mdbtools/mdbtools). It requires:

* GTK+ 2.14 or later
* libglade-2.0
* libgnomeui-2.0

Building requires autotools:

* autoreconf -f -i -Wno-portability
* ./configure
* make
* make install

This standalone package is considered beta software. For a stable release,
download MDB Tools version 0.7.1 or earlier and compile with the above
libraries installed.
