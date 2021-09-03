[![Build Status](https://github.com/mdbtools/gmdb2/workflows/build/badge.svg)](https://github.com/mdbtools/gmdb2/actions)

gmdb2
==

This is the official GUI for [mdbtools](https://github.com/mdbtools/mdbtools). gmdb2 lets you open, inspect, and export Microsoft Access databases (.mdb and .accdb files) on Mac and Unix machines.

This standalone package (which was included with mdbtools prior to version 0.9.0) is considered beta-quality software. Originally written by Brian Bruns in the early 2000s, the software was recently ported to GTK+3 and has been tested on Mac, Linux, and FreeBSD.

![Screenshot](https://mdbtools.github.io/static/images/gmdb2screenshot.png)

Installing
--

gmdb2 is available as package on these platforms:

[MacPorts](https://www.macports.org):

```
sudo port install gmdb2
```

FreeBSD:

```
sudo pkg install databases/gmdb2
```

Building from a release:
--

Building gmdb2 from a [release](https://github.com/mdbtools/gmdb2/releases) requires:

* mdbtools 0.9.0 or later
* GTK+ 3.22 or later
* gawk and yelp (for documentation)

Download, extract and then run:

```bash
./configure
make
make install
```

mdbtools and gtk3 need to be in a place where pkg-config can find them.

Building from GitHub
--

To build the latest code from GitHub, you will need a few extra tools, namely

* autoconf
* automake
* gettext
* yelp-tools

With these packages installed, run

```bash
autoreconf -f -i
```

Then as before

```bash
./configure
make
make install
```
