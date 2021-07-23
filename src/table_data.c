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

extern GtkWidget *app;
extern MdbHandle *mdb;

typedef struct GMdbDataWindow {
	GtkWidget *window;
	gchar table_name[MDB_MAX_OBJ_NAME+1];
} GMdbDataWindow;

static GList *window_list;

/* callbacks */
static gint
gmdb_table_data_close(GtkWidget *w, GdkEvent *event, GMdbDataWindow *dataw)
{
	window_list = g_list_remove(window_list, dataw);
	g_free(dataw);
	return FALSE;
}
/* functions */
GtkWidget *
gmdb_table_data_new(MdbCatalogEntry *entry)
{
MdbTableDef *table;
MdbColumn *col;
GType *types;
GtkListStore *list;
GtkWidget *tree_view;
GtkWidget *scroll;
GtkTreeIter iter = { 0 };
int i;
long row, maxrow;
gchar *bound_data[256];
GMdbDataWindow *dataw = NULL;
GtkTreeViewColumn *column;
GtkCellRenderer *renderer;

	/* do we have an active window for this object? if so raise it */
	for (i=0;i<g_list_length(window_list);i++) {
		dataw = g_list_nth_data(window_list, i);
		if (!strcmp(dataw->table_name, entry->object_name)) {
			gdk_window_raise (gtk_widget_get_window(dataw->window));
			return dataw->window;
		}
	}

	dataw = g_malloc(sizeof(GMdbDataWindow));
	snprintf(dataw->table_name, sizeof(dataw->table_name), "%s", entry->object_name);

	dataw->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);	
	gtk_window_set_title(GTK_WINDOW(dataw->window), entry->object_name);
	gtk_window_set_default_size(GTK_WINDOW(dataw->window), 600, 400);
	gtk_window_set_position(GTK_WINDOW(dataw->window), GTK_WIN_POS_NONE);
	gtk_widget_show(dataw->window);

    g_signal_connect (dataw->window, "delete_event",
        G_CALLBACK(gmdb_table_data_close), dataw);

	scroll = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_show(scroll);
	gtk_container_add(GTK_CONTAINER(dataw->window), scroll);

	/* read table */
	table = mdb_read_table(entry);
	mdb_read_columns(table);
	mdb_rewind_table(table);

	types = g_malloc0(table->num_cols * sizeof(GType));
	for (i=0;i<table->num_cols;i++) {
		types[i] = G_TYPE_STRING;
	}
	list = gtk_list_store_newv(table->num_cols, types);
	g_free(types);

	tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list));
	gtk_widget_show(tree_view);
	gtk_container_add(GTK_CONTAINER(scroll),tree_view);
	renderer = gtk_cell_renderer_text_new(); 

	for (i=0;i<table->num_cols;i++) {
		/* bind columns */
		bound_data[i] = g_malloc0(MDB_BIND_SIZE);
		mdb_bind_column(table, i+1, bound_data[i], NULL);

		/* display column titles */
		col=g_ptr_array_index(table->columns,i);
		column = gtk_tree_view_column_new_with_attributes(col->name, renderer, "text", i, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
	}
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree_view), TRUE);

	maxrow = gmdb_prefs_get_maxrows();

	/* fetch those rows! */
	row = 0;
	while(mdb_fetch_row(table) && 
			(!maxrow || (row < maxrow))) {
		row++;
		gtk_list_store_append(list, &iter);
		for (i=0;i<table->num_cols;i++) {
			gtk_list_store_set(list, &iter, i, bound_data[i], -1);
		}
	}

	/* free the memory used to bind */
	for (i=0;i<table->num_cols;i++) {
		g_free(bound_data[i]);
	}

	/* add this one to the window list */
	window_list = g_list_append(window_list, dataw);

	return dataw->window;
}
