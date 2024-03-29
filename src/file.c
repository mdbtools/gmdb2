/* MDB Tools - A library for reading MS Access database file
 * Copyright (C) 2000 Brian Bruns
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "gmdb.h"
#include <libgen.h>

MdbHandle *mdb;
gchar *mdb_filename;
extern int main_show_debug;
extern GtkBuilder *mainwin_xml;
extern GSettings *settings;

#define MAX_ACTIONITEMS 9
#define MAX_ICONVIEWS 6
typedef struct {
	GtkWidget* actionitems[MAX_ACTIONITEMS];
	GtkWidget* iconviews[MAX_ICONVIEWS];
} GmdbWidgets;
GmdbWidgets* gmdbwidgets = NULL;

static void gmdb_file_open_recent(gchar *menuname) 
{ 
gchar *text, cfgname[100];

	snprintf(cfgname, sizeof(cfgname), "%s-filepath", menuname);
	text = g_settings_get_string(settings, cfgname);
	gmdb_file_open(text);
	g_free(text);
	gmdb_load_recent_files();
}
void gmdb_file_open_recent_1() { gmdb_file_open_recent("recent1"); }
void gmdb_file_open_recent_2() { gmdb_file_open_recent("recent2"); }
void gmdb_file_open_recent_3() { gmdb_file_open_recent("recent3"); }
void gmdb_file_open_recent_4() { gmdb_file_open_recent("recent4"); }

static void
gmdb_file_shuffle_recent(gchar *file_path)
{
gchar *text, cfgname[100];
int i, index=0;

	for (i=1; i<=4; i++) {
		snprintf(cfgname, sizeof(cfgname), "recent%d-filepath", i);
		text = g_settings_get_string(settings, cfgname);
		if (!strcmp(text,file_path)) {
			index = i;
			break;
		}
		g_free(text);
	}
	/* printf("found file %slocation at menu %d\n",file_path, index); */
	/* it is the most recently used file, we're done */
	if (index==1) return;

	/* if this file is not on the recent list bump item 4 */
	if (!index) index=4;

	for (i=1; i<index; i++) {
		snprintf(cfgname, sizeof(cfgname), "recent%d-filepath", i);
		text = g_settings_get_string(settings, cfgname);
		if (text[0]) {
			snprintf(cfgname, sizeof(cfgname),
				"recent%d-filepath",
				i+1);
			g_settings_set_string(settings, cfgname, text);
			g_free(text);

			snprintf(cfgname, sizeof(cfgname),
				"recent%d-basename",
				i);
			text = g_settings_get_string(settings, cfgname);
			snprintf(cfgname, sizeof(cfgname),
				"recent%d-basename",
				i+1);
			g_settings_set_string(settings, cfgname, text);
			g_free(text);
		}
	}
}
static void
gmdb_file_add_recent(gchar *file_path)
{
	g_settings_set_string(settings, "recent1-basename", basename(file_path));
	g_settings_set_string(settings, "recent1-filepath", file_path);
	g_settings_sync();
}

static void
gmdb_reset_widgets (GmdbWidgets* gw) {
	int i;
	GtkWidget *w;

	gmdb_table_set_sensitive (FALSE);

	for (i = 0; i < MAX_ACTIONITEMS; ++i) {
		w = gw->actionitems[i];
		gtk_widget_set_sensitive (w, FALSE);
	}
	for (i = 0; i < MAX_ICONVIEWS; ++i) {
		w = gw->iconviews[i];
		gtk_list_store_clear (GTK_LIST_STORE (gtk_icon_view_get_model (GTK_ICON_VIEW (w))));
	}

	w = GTK_WIDGET(gtk_builder_get_object(mainwin_xml, "gmdb"));
	gtk_window_set_title (GTK_WINDOW (w), "MDB File Viewer");
}

static void
gmdb_icon_list_fill (MdbHandle* mdb, GtkTreeModel* store, GdkPixbuf* pixbuf, int objtype) {
	int i;
	MdbCatalogEntry* entry;
	GtkTreeIter iter;

	for (i=0; i < mdb->num_catalog; i++) {
		entry = g_ptr_array_index (mdb->catalog, i);
		if (entry->object_type == objtype) {
			/* skip the MSys tables - FIXME: can other objects be hidden too? */
			if (objtype != MDB_TABLE || mdb_is_user_table (entry)) {
				gtk_list_store_prepend (GTK_LIST_STORE (store), &iter);
				gtk_list_store_set (GTK_LIST_STORE (store), &iter, 0, pixbuf, 1, entry->object_name, -1);
			}
		}
	}
}

static void
gmdb_file_init (void) {
	GtkWidget* w;
	GtkListStore* store;
	int i;
	gchar* ainames[] = { "sql_menu", "debug_menu", "schema_menu", "info_menu", "close_menu", "sql_button", "schema_button", "info_button", "close_button" };
	gchar* swnames[] = { "sw_form", "sw_macro", "sw_module", "sw_query", "sw_report", "sw_table" };

	if (gmdbwidgets) {
		return;
	}

	gmdbwidgets = g_new0 (GmdbWidgets, 1);

	for (i = 0; i < MAX_ACTIONITEMS; ++i) {
		gmdbwidgets->actionitems[i] = GTK_WIDGET(gtk_builder_get_object(mainwin_xml, ainames[i]));
	}
	for (i = 0; i < MAX_ICONVIEWS; ++i) {
		store = gtk_list_store_new (2, GDK_TYPE_PIXBUF, G_TYPE_STRING);

		gmdbwidgets->iconviews[i] = gtk_icon_view_new();
		gtk_icon_view_set_model (GTK_ICON_VIEW (gmdbwidgets->iconviews[i]), GTK_TREE_MODEL (store));
		gtk_icon_view_set_pixbuf_column (GTK_ICON_VIEW (gmdbwidgets->iconviews[i]), 0);
		gtk_icon_view_set_text_column (GTK_ICON_VIEW (gmdbwidgets->iconviews[i]), 1);

		w = GTK_WIDGET(gtk_builder_get_object(mainwin_xml, swnames[i]));
		gtk_container_add (GTK_CONTAINER (w), gmdbwidgets->iconviews[i]);
		gtk_widget_show_all (w);
	}

	g_signal_connect_after (gmdbwidgets->iconviews[5], "selection-changed", G_CALLBACK (gmdb_table_select_cb), NULL);
	gmdb_table_init_popup (gmdbwidgets->iconviews[5]);
}

void
gmdb_file_open(gchar *file_path)
{
	GtkWidget *win;
	GdkPixbuf *pixbuf;
	GtkTreeModel *store;
	gchar title[100];
	int i;
	gchar* pbnames[] = { GMDB_ICONDIR "form_big.xpm", GMDB_ICONDIR "macro_big.xpm",
	  GMDB_ICONDIR "module_big.xpm", GMDB_ICONDIR "query_big.xpm", GMDB_ICONDIR "report_big.xpm",
	  GMDB_ICONDIR "table_big.xpm" };
	int objtype[] = { MDB_FORM, MDB_MACRO, MDB_MODULE, MDB_QUERY, MDB_REPORT, MDB_TABLE };

	if (!gmdbwidgets) {
		gmdb_file_init();
	}

	gmdb_reset_widgets (gmdbwidgets);
	mdb = mdb_open(file_path, MDB_NOFLAGS);
	if (!mdb) {
		GtkWidget* dlg = gtk_message_dialog_new (NULL,
		    GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING, GTK_BUTTONS_CLOSE,
		    _("Unable to open file."));
		gtk_dialog_run (GTK_DIALOG (dlg));
		gtk_widget_destroy (dlg);
		return;
	}
	mdb_set_default_backend(mdb, "access");
	gmdb_file_shuffle_recent(file_path);
	gmdb_file_add_recent(file_path);

#ifdef SQL
	sql->mdb = mdb;
#endif
	mdb_read_catalog(mdb, MDB_ANY);

	for (i = 0; i < MAX_ICONVIEWS; ++i) {
		store = gtk_icon_view_get_model (GTK_ICON_VIEW (gmdbwidgets->iconviews[i]));
		pixbuf = gdk_pixbuf_new_from_file (pbnames[i], NULL);
		gmdb_icon_list_fill (mdb, store, pixbuf, objtype[i]);
		g_object_unref (pixbuf);
	}

	//if (main_show_debug) gmdb_debug_init(mdb);
	
	for (i=strlen(file_path);i>0 && file_path[i-1]!='/';i--);
	mdb_filename=g_strdup(&file_path[i]);

	win = GTK_WIDGET(gtk_builder_get_object(mainwin_xml, "gmdb"));
	g_snprintf(title, sizeof(title), "%s - MDB File Viewer", mdb_filename);
	gtk_window_set_title(GTK_WINDOW(win), title);

	for (i = 0; i < MAX_ACTIONITEMS; ++i) {
		win = gmdbwidgets->actionitems[i];
		gtk_widget_set_sensitive (win, TRUE);
	}
}

void
gmdb_file_select_cb(GtkWidget *button, gpointer data)
{
	GtkWindow *parent_window = GTK_WINDOW(gtk_builder_get_object(mainwin_xml, "gmdb"));

	GtkWidget *dialog = gtk_file_chooser_dialog_new ("Please select a database.",
					      parent_window,
					      GTK_FILE_CHOOSER_ACTION_OPEN,
					      "_Cancel", GTK_RESPONSE_CANCEL,
					      "_Open", GTK_RESPONSE_ACCEPT,
					      NULL);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	  {
	    char *filename;

	    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
	    gmdb_file_open(filename);
	    gmdb_load_recent_files();
	  }

	gtk_widget_destroy (dialog);
}

void
gmdb_file_close_cb(GtkWidget *button, gpointer data)
{
	gmdb_reset_widgets (gmdbwidgets);
	gmdb_debug_close_all();
#if SQL
	gmdb_sql_close_all();
#endif
}
