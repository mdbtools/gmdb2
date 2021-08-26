/* MDB Tools - A library for reading MS Access database file
 * Copyright (C) 2000-2004 Brian Bruns
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

GSettings *settings;
GtkBuilder *mainwin_xml;
GtkWindow *gmdb;
MdbSQL *sql;

void
gmdb_about_cb(GtkWidget *button, gpointer data)
{
const gchar *authors[] = {
	"Brian Bruns",
	"Jeff Smith",
	"Filip Van Raemdonck",
	"Bernhard Reiter",
	"Nirgal Vourgère",
    "Evan Miller",
	NULL
};
const gchar *documenters[] = {
	"Brian Bruns",
	NULL
};
GtkWidget *parent;
GdkPixbuf *pixbuf=NULL;
FILE *flicense;
guint32 licenselen;
char *license = NULL;

	parent = gtk_widget_get_toplevel (button);
	if (!gtk_widget_is_toplevel(parent))
		parent = NULL;

	if (!pixbuf)
		pixbuf = gdk_pixbuf_new_from_file (GMDB_ICONDIR "logo.xpm", NULL);

	flicense = fopen(GMDB_ICONDIR "COPYING", "r");
	if (flicense)
	{
		fseek(flicense, 0, SEEK_END);
		licenselen = ftell(flicense);
		fseek(flicense, 0, SEEK_SET);
		license = g_malloc(licenselen+1);
		if (fread(license, 1, licenselen, flicense) == licenselen) {
            license[licenselen] = 0;
        } else {
            fprintf(stderr, "Can't read " GMDB_ICONDIR "COPYING\n");
            g_free(license);
            license = NULL;
        }
		fclose(flicense);
	} else {
		fprintf(stderr, "Can't open " GMDB_ICONDIR "COPYING\n");
	}

  	gtk_show_about_dialog ((GtkWindow*)parent,
   		"authors", authors,
		"comments", _(PACKAGE_NAME " is a grapical interface to "
			"MDB Tools. It lets you view and export data and schema "
			"from MDB files produced by MS Access 97/2000/XP/2003/2007/2010."),
		"copyright", _("Copyright 2002-2021 Brian Bruns and others"),
		"documenters", documenters,
		"logo", pixbuf,
		"program-name", PACKAGE_NAME,
		"version", PACKAGE_VERSION,
		"website", PACKAGE_URL,
		"license", license,
		NULL);
	g_free(license);
}

void
gmdb_prefs_cb(GtkWidget *button, gpointer data)
{
	gmdb_prefs_new();
}
void
gmdb_info_cb(GtkWidget *button, gpointer data)
{
	gmdb_info_new();
}

void
gmdb_help_cb(GtkWidget *button, gpointer data)
{
    gtk_show_uri_on_window(gmdb, "help:gmdb2", GDK_CURRENT_TIME, NULL);
}

void gmdb_load_recent_files()
{
GtkMenuItem *menuitem;
char menuname[100];
char cfgname[100];
int i;
gchar *text, *text2;

	for (i=4;i>=1;i--) {	
		snprintf(menuname, sizeof(menuname), "menu_recent%d",i);
		snprintf(cfgname, sizeof(cfgname), "recent%d-basename",i);
		menuitem = GTK_MENU_ITEM(gtk_builder_get_object(mainwin_xml, menuname));
		text = g_settings_get_string(settings, cfgname);
		if (!text[0]) {
			gtk_widget_hide(GTK_WIDGET(menuitem));
		} else {
			text2 = g_malloc(strlen(text)+4);
			sprintf(text2,"%d. %s",i,text);
			gtk_menu_item_set_label(menuitem,text2);
            gtk_widget_show(GTK_WIDGET(menuitem));
			g_free(text2);
		}
		g_free(text);
	}
}


static void gmdb_app_startup(GApplication *app, gpointer user_data) {
    GError *error = NULL;
#ifdef SQL
	/* initialize the SQL engine */
	sql = mdb_sql_init();
#endif

    settings = g_settings_new("mdbtools.gmdb2");
	/* load the interface */
	mainwin_xml = gtk_builder_new();
    if (!gtk_builder_add_from_file(mainwin_xml, GMDB_UIDIR "gmdb.ui", &error)) {
        g_warning("Error loading " GMDB_UIDIR "gmdb.ui: %s", error->message);
        g_error_free(error);
    }
	/* connect the signals in the interface */
	gtk_builder_connect_signals(mainwin_xml, NULL);

	gmdb = GTK_WINDOW(gtk_builder_get_object(mainwin_xml, "gmdb"));
    gtk_application_add_window(GTK_APPLICATION(app), gmdb);

	gmdb_load_recent_files();
}

static void gmdb_app_shutdown(GApplication *app, gpointer user_data) {
#ifdef SQL
	/* free MDB Tools library */
	mdb_sql_exit(sql);
#endif
}

static void gmdb_app_open(GApplication *app, GFile **files, gint n_files, const gchar *hint) {
    gmdb_file_open(g_file_get_path(files[0]));
    gtk_widget_show(GTK_WIDGET(gmdb));
}

static void gmdb_app_activate(GApplication *app, gpointer user_data) {
    gtk_widget_show(GTK_WIDGET(gmdb));
}

int main(int argc, char *argv[])
{
GtkApplication *app = NULL;
int status;

    if (argc == 2 && (!strcmp(argv[1], "-v") || !strcmp(argv[1], "--version"))) {
        printf(PACKAGE_NAME " " PACKAGE_VERSION "\n");
        return 0;
    }
    app = gtk_application_new("mdbtools.gmdb2", G_APPLICATION_HANDLES_OPEN);
    g_signal_connect(app, "startup", G_CALLBACK(gmdb_app_startup), NULL);
    g_signal_connect(app, "shutdown", G_CALLBACK(gmdb_app_shutdown), NULL);
    g_signal_connect(app, "activate", G_CALLBACK(gmdb_app_activate), NULL);
    g_signal_connect(app, "open", G_CALLBACK(gmdb_app_open), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

	return status;
}
