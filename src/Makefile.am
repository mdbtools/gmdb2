SUBDIRS = help gladefiles pixmaps
bin_PROGRAMS	= gmdb2 
include_HEADERS = gmdb.h

gmdb2_SOURCES = main2.c file.c table.c info.c table_def.c table_data.c table_export.c debug.c sql.c schema.c prefs.c
LIBS =	$(GNOME_LIBS) $(MDBSQL_LIBS) $(MDBTOOLS_LIBS) @LIBS@
AM_CFLAGS	=	-I$(top_srcdir)/include $(GNOME_CFLAGS) $(MDBSQL_CFLAGS) $(MDBTOOLS_CFLAGS) \
        -DDATADIR="\"$(datadir)\""                      \
        -DGMDB_ICONDIR="\"$(datadir)/gmdb/glade/\""        \
        -DGMDB_GLADEDIR="\"$(datadir)/gmdb/glade/\"" 

EXTRA_DIST = gmdb.desktop gmdb.bash-completion
