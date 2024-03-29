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
GtkBuilder *exportwin_xml;
MdbCatalogEntry *cat_entry;

#define COMMA	"Comma (,)"
#define TAB		"Tab"
#define SPACE	"Space"
#define COLON	"Colon (:)"
#define SEMICOLON	"Semicolon (;)"
#define PIPE	"Pipe (|)"

#define LF "Unix (linefeed only)"
#define CR "Mac (carriage return only)"
#define CRLF "Windows (CR + LF)"

#define ALWAYS "Always"
#define NEVER "Never"
#define AUTOMAT "Automatic (where necessary)"

#define NOQUOTE "Don't quote"

#define BIN_STRIP "Strip"
#define BIN_RAW "Raw"
#define BIN_OCTAL "Octal"
#define BIN_HEXADECIMAL "Hexademical"

void
gmdb_export_get_delimiter(GtkBuilder *xml, gchar *delimiter, int max_buf)
{
	GtkComboBoxText *combobox;
	const gchar *str;

	combobox = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(xml, "sep_combo"));
	str = gtk_combo_box_text_get_active_text(combobox);
	if (!strcmp(str,COMMA))
		strncpy(delimiter, ",", max_buf);
	else if (!strcmp(str,TAB))
		strncpy(delimiter, "\t", max_buf);
	else if (!strcmp(str,SPACE))
		strncpy(delimiter, " ", max_buf);
	else if (!strcmp(str,COLON))
		strncpy(delimiter, ":", max_buf);
	else if (!strcmp(str,SEMICOLON))
		strncpy(delimiter, ";", max_buf);
	else if (!strcmp(str,PIPE))
		strncpy(delimiter, "|", max_buf);
	else
		strncpy(delimiter, str, max_buf);
	if (max_buf)
		delimiter[max_buf-1] = '\0';
}

void
gmdb_export_get_lineterm(GtkBuilder *xml, gchar *lineterm, int max_buf)
{
	GtkComboBoxText *combobox;
	gchar *str;

	combobox = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(xml, "term_combo"));
	str = gtk_combo_box_text_get_active_text(combobox);
	if (!strcmp(str,LF))
		strncpy(lineterm, "\n", max_buf);
	else if (!strcmp(str,CR))
		strncpy(lineterm, "\r", max_buf);
	else if (!strcmp(str,CRLF))
		strncpy(lineterm, "\r\n", max_buf);
	if (max_buf)
		lineterm[max_buf-1] = '\0';
}

void
gmdb_export_get_quotechar(GtkBuilder *xml, gchar *quotechar, int max_buf)
{
	GtkComboBoxText *combobox;
	gchar *str;

	combobox = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(xml, "qchar_combo"));
	str = gtk_combo_box_text_get_active_text (combobox);
	if (!strcmp(str, NOQUOTE))
		quotechar[0] = '\0'; /* Quoting disabled */
	else
		strncpy(quotechar, str, max_buf);
	if (max_buf)
		quotechar[max_buf-1] = '\0';
}

void
gmdb_export_get_escapechar(GtkBuilder *xml, gchar *escapechar, int max_buf)
{
	GtkComboBoxText *combobox;
	gchar *str;

	combobox = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(xml, "escchar_combo"));
	str = gtk_combo_box_text_get_active_text (combobox);
	strncpy(escapechar, str, max_buf);
	if (max_buf)
		escapechar[max_buf-1] = '\0';
}

int
gmdb_export_get_binmode(GtkBuilder *xml)
{
	GtkComboBoxText *combobox;
	gchar *str;

	combobox = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(xml, "bin_combo"));
	str = gtk_combo_box_text_get_active_text (combobox);

	if (!strcmp(str,BIN_STRIP))
		return MDB_BINEXPORT_STRIP;
	else if (!strcmp(str,BIN_OCTAL))
		return MDB_BINEXPORT_OCTAL;
	else if (!strcmp(str,BIN_HEXADECIMAL))
		return MDB_BINEXPORT_HEXADECIMAL;
	else
		return MDB_BINEXPORT_RAW;
}


int
gmdb_export_get_headers(GtkBuilder *xml)
{
	GtkWidget *checkbox;

	checkbox = GTK_WIDGET(gtk_builder_get_object(xml, "headers_checkbox"));
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox)))
		return 1;
	else
		return 0;
}
gchar *
gmdb_export_get_filepath(GtkBuilder *xml)
{
	GtkWidget *fchoose;

	fchoose = GTK_WIDGET(gtk_builder_get_object(xml, "filename_entry"));
	return (gchar *) gtk_entry_get_text (GTK_ENTRY (fchoose));
}

void
gmdb_table_export_button_cb(GtkWidget *w, gpointer data)
{
gchar *file_path;
FILE *outfile;
char **bound_values;
int *bound_lens; 
MdbTableDef *table;
MdbColumn *col;
int i;
int need_headers = 0;
gchar delimiter[11];
gchar quotechar[5];
gchar escape_char[5];
gchar lineterm[5];
int bin_mode = MDB_BINEXPORT_RAW;
int rows=0;
char *value;
size_t length;

	GtkWidget *exportwin, *dlg;

	gmdb_export_get_delimiter(exportwin_xml, delimiter, sizeof(delimiter));
	gmdb_export_get_lineterm(exportwin_xml, lineterm, sizeof(lineterm));
	gmdb_export_get_quotechar(exportwin_xml, quotechar, sizeof(quotechar));
	gmdb_export_get_escapechar(exportwin_xml, escape_char, sizeof(escape_char));
	need_headers = gmdb_export_get_headers(exportwin_xml);
	bin_mode = gmdb_export_get_binmode(exportwin_xml);
	file_path = gmdb_export_get_filepath(exportwin_xml);

	// printf("file path %s\n",file_path);
	if ((outfile=fopen(file_path, "w"))==NULL) {
		GtkWidget* dlg = gtk_message_dialog_new (GTK_WINDOW (gtk_widget_get_toplevel (w)),
		    GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING, GTK_BUTTONS_CLOSE,
		    _("Unable to open file."));
		gtk_dialog_run (GTK_DIALOG (dlg));
		gtk_widget_destroy (dlg);
		return;
	}

	/* read table */
	table = mdb_read_table(cat_entry);
	mdb_read_columns(table);
	mdb_rewind_table(table);

	bound_values = (char **) g_malloc(table->num_cols * sizeof(char *));
	bound_lens = (int *) g_malloc(table->num_cols * sizeof(int));
	for (i=0;i<table->num_cols;i++) {
		/* bind columns */
		bound_values[i] = (char *) g_malloc0(MDB_BIND_SIZE);
		mdb_bind_column(table, i+1, bound_values[i], &bound_lens[i]);

		/* display column titles */
		if (need_headers) {
			if (i>0)
				fputs(delimiter, outfile);
			col=g_ptr_array_index(table->columns,i);
			mdb_print_col(outfile, col->name, quotechar[0]!='\0', MDB_TEXT, 0, quotechar, escape_char, bin_mode);
		}
	}
	if (need_headers) fputs(lineterm, outfile);

	/* fetch those rows! */
	while(mdb_fetch_row(table)) {
		for (i=0;i<table->num_cols;i++) {
			if (i>0)
				fputs(delimiter, outfile);
			col=g_ptr_array_index(table->columns,i);
			/* Don't quote NULLs */
			if (bound_lens[i]) {
				if (col->col_type == MDB_OLE) {
					value = mdb_ole_read_full(mdb, col, &length);
				} else {
					value = bound_values[i];
					length = bound_lens[i];
				}
				mdb_print_col(outfile, value, quotechar[0]!='\0', col->col_type, length, quotechar, escape_char, bin_mode);
				if (col->col_type == MDB_OLE)
					free(value);
			}
		}
		fputs(lineterm, outfile);
		rows++;
	}

	/* free the memory used to bind */
	for (i=0;i<table->num_cols;i++) {
		g_free(bound_values[i]);
	}
	g_free(bound_values);
	g_free(bound_lens);

	fclose(outfile);
	exportwin = GTK_WIDGET(gtk_builder_get_object(exportwin_xml, "export_dialog"));
	gtk_widget_destroy(exportwin);
	dlg = gtk_message_dialog_new (GTK_WINDOW (gtk_widget_get_toplevel (w)),
	    GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE,
	    _("%d rows successfully exported."), rows);
	gtk_dialog_run (GTK_DIALOG (dlg));
	gtk_widget_destroy (dlg);
}

void gmdb_table_export(MdbCatalogEntry *entry) 
{
    GError *error = NULL;
	cat_entry = entry;

	/* load the interface */
	exportwin_xml = gtk_builder_new();
    if (!gtk_builder_add_from_file(exportwin_xml, GMDB_UIDIR "gmdb-export.ui", &error)) {
        g_warning("Error loading " GMDB_UIDIR "gmdb-export.ui: %s", error->message);
        g_error_free(error);
    }
	/* connect the signals in the interface */
	gtk_builder_connect_signals(exportwin_xml, NULL);
}
