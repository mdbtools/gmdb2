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
extern gchar *mdb_filename;

typedef struct GMdbInfoWindow {
	GtkWidget *window;
} GMdbInfoWindow;

GMdbInfoWindow *infow;

/* callbacks */
GtkWidget *
gmdb_info_new()
{
GtkWidget *propswin, *label;
GtkBuilder *propswin_xml;
GError *error = NULL;
gchar title[100];
gchar tmpstr[20];
struct stat st;
char* version;
MdbCatalogEntry *entry = mdb_get_catalogentry_by_name(mdb, "SummaryInfo");

	/* load the interface */
	propswin_xml = gtk_builder_new();
    if (!gtk_builder_add_from_file(propswin_xml, GMDB_UIDIR "gmdb-props.ui", &error)) {
        g_warning("Error loading " GMDB_UIDIR "gmdb-props.ui: %s", error->message);
        g_error_free(error);
    }
	/* connect the signals in the interface */
	gtk_builder_connect_signals(propswin_xml, NULL);
	
	propswin = GTK_WIDGET(gtk_builder_get_object(propswin_xml, "props_dialog"));
	g_snprintf(title, sizeof(title), "%s Properties", mdb_filename);
	gtk_window_set_title(GTK_WINDOW(propswin), title);
		
	label = GTK_WIDGET(gtk_builder_get_object(propswin_xml, "props_filename"));
	gtk_label_set_text(GTK_LABEL(label), mdb_filename);
		
	label = GTK_WIDGET(gtk_builder_get_object(propswin_xml, "props_jetver"));
	switch(mdb->f->jet_version) {
	case MDB_VER_JET3:
		version = "3 (Access 97)";
		break;
	case MDB_VER_JET4:
		version = "4 (Access 2000/XP/2003)";
		break;
	case MDB_VER_ACCDB_2007:
		version = "ACE 12 (Access 2007)";
		break;
	case MDB_VER_ACCDB_2010:
		version = "ACE 14 (Access 2010)";
		break;
	case MDB_VER_ACCDB_2013:
		version = "ACE 15 (Access 2013)";
		break;
	case MDB_VER_ACCDB_2016:
		version = "ACE 16 (Access 2016)";
		break;
	default:
		version = "Unknown";
	}
	gtk_label_set_text(GTK_LABEL(label), version);

	label = GTK_WIDGET(gtk_builder_get_object(propswin_xml, "props_encrypted"));
	gtk_label_set_text(GTK_LABEL(label), mdb->f->db_key ? "Yes" : "No");	

	assert( fstat(fileno(mdb->f->stream), &st)!=-1 );
	snprintf(tmpstr, sizeof(tmpstr), "%zd K", (size_t)(st.st_size/1024));
	label = GTK_WIDGET(gtk_builder_get_object(propswin_xml, "props_filesize"));
	gtk_label_set_text(GTK_LABEL(label), tmpstr);	
		
	snprintf(tmpstr, sizeof(tmpstr), "%zd", (size_t)(st.st_size / mdb->fmt->pg_size));
	label = GTK_WIDGET(gtk_builder_get_object(propswin_xml, "props_numpages"));
	gtk_label_set_text(GTK_LABEL(label), tmpstr);	

	snprintf(tmpstr, sizeof(tmpstr), "%d", mdb->num_catalog);
	label = GTK_WIDGET(gtk_builder_get_object(propswin_xml, "props_numobjs"));
	gtk_label_set_text(GTK_LABEL(label), tmpstr);	

	if (entry && entry->props && entry->props->len)
	{
		// There is only one MdbProps for that kind of entry
		MdbProperties *props = g_ptr_array_index(entry->props, 0);
		const char *propval;
		
		propval = g_hash_table_lookup(props->hash, "Title");
		if (propval) {
			label = GTK_WIDGET(gtk_builder_get_object(propswin_xml, "props_title"));
			gtk_label_set_text(GTK_LABEL(label), propval);
		}
		propval = g_hash_table_lookup(props->hash, "Company");
		if (propval) {
			label = GTK_WIDGET(gtk_builder_get_object(propswin_xml, "props_company"));
			gtk_label_set_text(GTK_LABEL(label), propval);
		}
		propval = g_hash_table_lookup(props->hash, "Author");
		if (propval) {
			label = GTK_WIDGET(gtk_builder_get_object(propswin_xml, "props_author"));
			gtk_label_set_text(GTK_LABEL(label), propval);
		}
	}

	return propswin;
}
