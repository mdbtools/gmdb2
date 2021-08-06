[![Build Status](https://github.com/mdbtools/gmdb2/workflows/build/badge.svg)](https://github.com/mdbtools/gmdb2/actions)

gmdb2
--

This is the official GUI for [mdbtools](https://github.com/mdbtools/mdbtools). This standalone package (which was included with mdbtools prior to version 0.9.0) is considered beta-quality software. It requires:

* mdbtools 0.9.0 or later
* GTK+ 3.22 or later
* gawk and yelp (for documentation)

The easiest way to build gmdb2 is to download an official [release](https://github.com/mdbtools/gmdb2/releases). Extract and then run:

```bash
./configure
make
make install
```

mdbtools and gtk3 need to be in a place where pkg-config can find them.

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

![Screenshot](https://mdbtools.github.io/static/images/gmdb2screenshot.png)
