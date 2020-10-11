dnl Process this file with autoconf to produce a configure script.
AC_INIT([gmdb2],[0.9.0-beta],[https://github.com/mdbtools/gmdb2/issues],[],[https://github.com/mdbtools/gmdb2])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR(src/gladefiles/gmdb.glade)
AM_INIT_AUTOMAKE([foreign])

MDBTOOLS_VERSION_MAJOR=0
MDBTOOLS_VERSION_MINOR=9
MDBTOOLS_VERSION_MICRO=0

# Update this logic when the major version changes
VERSION_INFO=`expr $MDBTOOLS_VERSION_MINOR - 6`:$MDBTOOLS_VERSION_MICRO
AC_SUBST(VERSION_INFO)

AM_MAINTAINER_MODE([enable])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PROG_CC(gcc)
AC_PROG_CXX
dnl Checks for programs.
AC_PROG_MAKE_SET
m4_pattern_allow([AM_PROG_AR], [AM_PROG_AR])
LT_INIT([win32-dll])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h limits.h unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

dnl no optional stuff by default
OPTDIRS=""

dnl ---------------------------------------------------------------------
dnl Compile time options
dnl ---------------------------------------------------------------------

CFLAGS="$CFLAGS -Wall -Werror"
AS_CASE([$host],
        [*mingw*|*cygwin*], [LDFLAGS="$LDFLAGS -no-undefined"], [])
AS_CASE([$host],
        [*mingw*], [LDFLAGS="$LDFLAGS -lWs2_32"], [])

dnl Enable -Wl,--as-needed by default to prevent overlinking

AC_ARG_ENABLE([as-needed],
  AS_HELP_STRING([--disable-as-needed],[disable overlinking protection]),
  [enable_as_needed=$enableval], [enable_as_needed=yes])

if test "x$enable_as_needed" != "xno"; then
  AC_MSG_CHECKING([whether $LD accepts --as-needed])
  case `$LD --as-needed -v 2>&1 </dev/null` in
  *GNU* | *'with BFD'*)
    LDFLAGS="$LDFLAGS -Wl,--as-needed"
    AC_MSG_RESULT([yes])
    ;;
  *)
    AC_MSG_RESULT([no])
    ;;
  esac
fi

dnl Turn on -Wstrict-prototypes here because AC_CHECK_LIB is dumb
CFLAGS="$CFLAGS -Wstrict-prototypes"

dnl Conditionally build odbc wide version
AM_CONDITIONAL(ICONV, test "$am_cv_func_iconv" = "yes")

dnl Testing presence of pkg-config
AC_MSG_CHECKING([pkg-config m4 macros])
if test m4_ifdef([PKG_CHECK_MODULES], [yes], [no]) == yes; then
	AC_MSG_RESULT([yes]);
else
	AC_MSG_RESULT([no]);
	AC_MSG_ERROR([
pkg-config is required.
See pkg-config.freedesktop.org])
fi

dnl check for MDB Tools
PKG_CHECK_MODULES([MDBTOOLS], [libmdb >= 0.7.1], HAVE_MDBTOOLS=true, HAVE_MDBTOOLS=false)
PKG_CHECK_MODULES([MDBSQL], [libmdbsql >= 0.7.1], HAVE_MDBSQL=true, HAVE_MDBSQL=false)

if test "x$HAVE_MDBSQL" = "xtrue"; then
    CFLAGS="$CFLAGS -DSQL"
    AC_SUBST(MDBSQL_CFLAGS)
    AC_SUBST(MDBSQL_LIBS)
fi

if test "x$HAVE_MDBTOOLS" = "xtrue"; then
    AC_SUBST(MDBTOOLS_CFLAGS)
    AC_SUBST(MDBTOOLS_LIBS)
else
    AC_MSG_ERROR([Could not find MDB Tools, which are required to build gmdb2.])
fi

dnl check for gtk/gnome
PKG_CHECK_MODULES([GNOME], [gtk+-2.0 >= 2.14 libglade-2.0 libgnomeui-2.0], HAVE_GNOME=true, HAVE_GNOME=false)

GNOME_DOC_INIT

if test "x$HAVE_GNOME" = "xtrue"; then
    AC_SUBST(GNOME_CFLAGS)
    AC_SUBST(GNOME_LIBS)
    OPTDIRS="$OPTDIRS gmdb2"
else
    AC_MSG_ERROR([Could not find GNOME libraries, which are required to build gmdb2.])
fi

AC_SUBST([OPTDIRS])

##################################################
# Check for gtk-doc.
##################################################

AC_ARG_WITH(html-dir,
  AS_HELP_STRING([--with-html-dir=PATH], [path to installed docs]))

if test "x$with_html_dir" = "x" ; then
  HTML_DIR='${datadir}/gtk-doc/html'
else
  HTML_DIR=$with_html_dir
fi

AC_SUBST(HTML_DIR)

gtk_doc_min_version=1.0
AC_MSG_CHECKING([gtk-doc version >= $gtk_doc_min_version])
if pkg-config --atleast-version=$gtk_doc_min_version gtk-doc; then
  AC_MSG_RESULT(yes)
  GTKDOC=true
else
  AC_MSG_RESULT(no)
  GTKDOC=false
fi

dnl Let people disable the gtk-doc stuff.
AC_ARG_ENABLE(gtk-doc, AS_HELP_STRING([--enable-gtk-doc], [use gtk-doc to build documentation [default=auto]]), enable_gtk_doc="$enableval", enable_gtk_doc=auto)

if test x$enable_gtk_doc = xauto ; then
  if test x$GTKDOC = xtrue ; then
    enable_gtk_doc=yes
  else
    enable_gtk_doc=no 
  fi
fi

AM_CONDITIONAL(ENABLE_GTK_DOC, test x$enable_gtk_doc = xyes)

##################################################
# Check for txt2man
##################################################

AC_ARG_ENABLE(man,
  AS_HELP_STRING([--disable-man], [disable man generation]),
  enable_man="$enableval", [enable_man=yes])
if test "$enable_man" = yes; then
  if ! which gawk > /dev/null; then
    AC_MSG_ERROR([Could not find GNU awk. Install it or configure with --disable-man if you are not interested in manuals.])
  fi
fi
AM_CONDITIONAL(ENABLE_MAN, test "$enable_man" = yes)

##################################################
# Check for docbook
##################################################
AM_CONDITIONAL(ENABLE_DOCBOOK, test -n "$DOCBOOK_DSL")

localedir=${datadir}/locale
AC_SUBST(localedir)

AC_CONFIG_FILES([
Makefile
doc/Makefile
src/Makefile
src/gladefiles/Makefile
src/pixmaps/Makefile
src/help/Makefile])
AC_OUTPUT
dnl doc/reference/libmdb/Makefile])

##################################################
# Print summary
##################################################


bold_red=$(tput bold)$(tput setf 4)
bold_green=$(tput bold)$(tput setf 2)
bold=$(tput bold)
reset=$(tput sgr0)
AC_MSG_NOTICE([])
AC_MSG_NOTICE([${bold}gmdb2 $VERSION - Configuration summary${reset}])
AC_MSG_NOTICE([])
if test x$enable_gtk_doc = xyes; then summary=${bold_green}enabled; else summary=${bold_red}disabled; fi
AC_MSG_NOTICE([   Gtk doc      : ${summary}${reset}])
if test x$enable_man = xyes; then summary=${bold_green}enabled; else summary=${bold_red}disabled; fi
AC_MSG_NOTICE([   man          : ${summary}${reset}])
AC_MSG_NOTICE([])
