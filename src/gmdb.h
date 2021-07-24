#ifndef _gmdb_h_
#define _gmdb_h_

#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <gtk/gtk.h>
#ifndef GETTEXT_PACKAGE
#define GETTEXT_PACKAGE "gtk20"
#endif
#include <glib/gi18n-lib.h>
#include <mdbtools.h>
#include <mdbsql.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* info.c */
GtkWidget *gmdb_info_new(void);

/* main2.c */
void gmdb_info_cb(GtkWidget *w, gpointer data);
void gmdb_prefs_cb(GtkWidget *w, gpointer data);
void gmdb_help_cb(GtkWidget *w, gpointer data);
void gmdb_about_cb(GtkWidget *w, gpointer data);
void gmdb_load_recent_files();

GtkWidget *gmdb_table_data_new(MdbCatalogEntry *entry);
GtkWidget *gmdb_table_export_new(MdbCatalogEntry *entry);
void gmdb_table_export(MdbCatalogEntry *entry) ;
void gmdb_table_export_populate_dialog(GtkBuilder *xml);

/* table_def.c */
void gmdb_table_def_new(MdbCatalogEntry *entry);

/* file.c */
void gmdb_file_select_cb(GtkWidget *w, gpointer data);
void gmdb_file_close_cb(GtkWidget *w, gpointer data);
void gmdb_file_open_recent_1(void);
void gmdb_file_open_recent_2(void);
void gmdb_file_open_recent_3(void);
void gmdb_file_open_recent_4(void);
void gmdb_file_open(gchar *file_path);

void gmdb_sql_new_window_cb(GtkWidget *w, gpointer data);

void gmdb_table_add_tab(GtkWidget *notebook);
void gmdb_debug_tab_new(GtkWidget *notebook);

/* debug.c */
void gmdb_debug_jump_cb(GtkWidget *w, gpointer data);
void gmdb_debug_jump_msb_cb(GtkWidget *w, gpointer data);
void gmdb_debug_display_cb(GtkWidget *w, gpointer data);
void gmdb_debug_close_cb(GtkWidget *w, gpointer data);
void gmdb_debug_forward_cb(GtkWidget *w, gpointer data);
void gmdb_debug_back_cb(GtkWidget *w, gpointer data);
void gmdb_debug_new_cb(GtkWidget *w, gpointer data);
void gmdb_debug_set_dissect_cb(GtkWidget *w, gpointer data);
void gmdb_debug_close_all(void);

/* sql.c */
void gmdb_sql_new_cb(GtkWidget *w, gpointer data);
void gmdb_sql_close_all(void);

unsigned long gmdb_prefs_get_maxrows(void);

extern GtkWidget *gmdb_prefs_new(void);

/* schema.c */
void gmdb_schema_new_cb(GtkWidget *w, gpointer data);
void gmdb_schema_export_cb(GtkWidget *w, gpointer data);

/* table.c */
void gmdb_table_debug_cb(GtkContainer *list, GtkWidget *w, gpointer data);
void gmdb_table_export_cb(GtkContainer *list, GtkWidget *w, gpointer data);
void gmdb_table_def_cb(GtkContainer *list, GtkWidget *w, gpointer data);
void gmdb_table_unselect_cb (GtkIconView*, gpointer);
void gmdb_table_select_cb (GtkIconView*, gpointer);
void gmdb_table_data_cb(GtkContainer *list, GtkWidget *w, gpointer data);
void gmdb_table_init_popup (GtkWidget*);
void gmdb_table_set_sensitive(gboolean b);

/* table_export.c */
void gmdb_export_get_delimiter(GtkBuilder *xml, gchar *delimiter, int max_buf);
void gmdb_export_get_lineterm(GtkBuilder *xml, gchar *lineterm, int max_buf);
void gmdb_export_get_quotechar(GtkBuilder *xml, gchar *quotechar, int max_buf);
void gmdb_export_get_escapechar(GtkBuilder *xml, gchar *escapechar, int max_buf);
int gmdb_export_get_binmode(GtkBuilder *xml);
int gmdb_export_get_headers(GtkBuilder *xml);
gchar *gmdb_export_get_filepath(GtkBuilder *xml);
void gmdb_table_export_button_cb(GtkWidget *w, gpointer data);

extern MdbSQL *sql;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
