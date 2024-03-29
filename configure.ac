dnl Process this file with autoconf to produce a configure script.
AC_INIT([gmdb2],[0.9.1],[https://github.com/mdbtools/gmdb2/issues],[],[https://github.com/mdbtools/gmdb2])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR(src/uifiles/gmdb.ui)
AM_INIT_AUTOMAKE([foreign])

AM_MAINTAINER_MODE([enable])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PROG_CC(gcc)
dnl Checks for programs.
AC_PROG_MAKE_SET
m4_pattern_allow([AM_PROG_AR], [AM_PROG_AR])
LT_INIT([win32-dll])

dnl Define HAVE_ICONV so that libmdb 0.9.x is correctly linked
AM_ICONV

dnl Configure the help system
YELP_HELP_INIT

dnl ---------------------------------------------------------------------
dnl Compile time options
dnl ---------------------------------------------------------------------

CFLAGS="$CFLAGS -Wall -Werror"
AS_CASE([$host],
        [*mingw*|*cygwin*], [LDFLAGS="$LDFLAGS -no-undefined"],
        [*linux*|*bsd*], [LDFLAGS="$LDFLAGS -rdynamic"],
        [])

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
PKG_CHECK_MODULES([MDBTOOLS], [libmdb >= 0.9.0], HAVE_MDBTOOLS=true, HAVE_MDBTOOLS=false)
PKG_CHECK_MODULES([MDBSQL], [libmdbsql >= 0.9.0], HAVE_MDBSQL=true, HAVE_MDBSQL=false)

if test "x$HAVE_MDBSQL" = "xtrue"; then
    CFLAGS="$CFLAGS -DSQL"
fi
AC_SUBST(MDBSQL_CFLAGS)
AC_SUBST(MDBSQL_LIBS)

if test "x$HAVE_MDBTOOLS" != "xtrue"; then
    AC_MSG_ERROR([Could not find MDB Tools, which are required to build gmdb2.])
fi
AC_SUBST(MDBTOOLS_CFLAGS)
AC_SUBST(MDBTOOLS_LIBS)

dnl check for gtk3

PKG_CHECK_MODULES([GTK3], [gtk+-3.0 >= 3.22], HAVE_GTK3=true, HAVE_GTK3=false)
if test "x$HAVE_GTK3" != "xtrue"; then
    AC_MSG_ERROR([Could not find GTK3 libraries, which are required to build gmdb2.])
fi
AC_SUBST(GTK3_CFLAGS)
AC_SUBST(GTK3_LIBS)

AC_DEFINE([HAVE_GLIB], [1], [glib check])
AC_CHECK_LIB(glib-2.0, g_memdup2, [AC_DEFINE([HAVE_G_MEMDUP2], [1], [g_memdup2 check])])

GLIB_GSETTINGS

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

AC_CONFIG_FILES([
Makefile
doc/Makefile
src/Makefile
src/uifiles/Makefile
src/pixmaps/Makefile
src/help/Makefile])
AC_OUTPUT

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
if test x$HAVE_MDBSQL = xtrue; then summary=${bold_green}enabled; else summary=${bold_red}disabled; fi
AC_MSG_NOTICE([ SQL : ${summary}${reset}])
if test x$enable_man = xyes; then summary=${bold_green}enabled; else summary=${bold_red}disabled; fi
AC_MSG_NOTICE([ man : ${summary}${reset}])
AC_MSG_NOTICE([])
