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
extern GSettings *settings;

GtkBuilder *prefswin_xml;

unsigned long
gmdb_prefs_get_maxrows()
{
    return g_settings_get_int(settings, "maxrows");
}

/* callbacks */
void
gmdb_prefs_save_cb(GtkWidget *w, gpointer user_data)
{
	GtkWidget *entry;
	GtkWidget *win;
	gchar *str;

	entry = GTK_WIDGET(gtk_builder_get_object(prefswin_xml, "maxrows_entry"));
	str = (gchar *) gtk_entry_get_text(GTK_ENTRY(entry));
	printf("str = %s\n",str);
    g_settings_set_int(settings, "maxrows", atol(str));
	g_settings_sync();
	win = GTK_WIDGET(gtk_builder_get_object(prefswin_xml, "prefs_dialog"));
	if (win) gtk_widget_destroy(win);
}

void
gmdb_prefs_cancel_cb(GtkWidget *w, gpointer user_data)
{
	GtkWidget *win;

	win = GTK_WIDGET(gtk_builder_get_object(prefswin_xml, "prefs_dialog"));
	if (win) gtk_widget_destroy(win);
}

GtkWidget *
gmdb_prefs_new()
{
	GtkWidget *prefswin;
	GtkEntry *entry;
	gchar *str;
    GError *error = NULL;

	/* load the interface */
	prefswin_xml = gtk_builder_new();
    if (!gtk_builder_add_from_file(prefswin_xml, GMDB_UIDIR "gmdb-prefs.ui", NULL)) {
        g_warning("Error adding " GMDB_UIDIR "gmdb-prefs.ui: %s", error->message);
        g_error_free(error);
    }
	/* connect the signals in the interface */
	gtk_builder_connect_signals(prefswin_xml, NULL);

	entry = GTK_ENTRY(gtk_builder_get_object(prefswin_xml, "maxrows_entry"));

	str = g_strdup_printf("%d", g_settings_get_int(settings, "maxrows"));
	gtk_entry_set_text(entry, str);
    g_free(str);

	prefswin = GTK_WIDGET(gtk_builder_get_object(prefswin_xml, "prefs_dialog"));
	return prefswin;
}
