SUBDIRS = uifiles pixmaps
bin_PROGRAMS = gmdb2
include_HEADERS = gmdb.h

gmdb2_SOURCES = main2.c file.c table.c info.c table_def.c table_data.c table_export.c debug.c sql.c schema.c prefs.c
LIBS = $(GTK3_LIBS) $(MDBSQL_LIBS) $(MDBTOOLS_LIBS) @LIBS@
AM_CFLAGS = -I$(top_srcdir)/include $(GTK3_CFLAGS) $(MDBSQL_CFLAGS) $(MDBTOOLS_CFLAGS) \
        -DGMDB_ICONDIR="\"$(datadir)/gmdb/ui/\"" \
        -DGMDB_UIDIR="\"$(datadir)/gmdb/ui/\""

EXTRA_DIST = gmdb.desktop gmdb.bash-completion
