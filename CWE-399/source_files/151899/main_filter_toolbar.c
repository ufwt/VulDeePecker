/* main_filter_toolbar.c
 * The filter toolbar
 *
 * $Id: main_filter_toolbar.c 45171 2012-09-27 10:48:01Z etxrab $
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*
 * This file implements the "filter" toolbar for Wireshark.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include "ui/recent.h"
#include "ui/gtk/old-gtk-compat.h"
#include "filter_dlg.h"
#include "filter_autocomplete.h"
#include "epan/prefs.h"
#include "keys.h"
#include "gtkglobals.h"
#include "stock_icons.h"
#include "main.h"
#include "menus.h"
#include "main_toolbar.h"
#include "main_filter_toolbar.h"
#include "filter_expression_save_dlg.h"
#define MENU_BAR_PATH_FILE_OPEN                         "/Menubar/FileMenu/Open"
#define MENU_BAR_PATH_EDIT_COPY_AS_FLT                  "/Menubar/EditMenu/Copy/AsFilter"
#define MENU_BAR_PATH_ANALYZE_DISPLAY_FLT               "/Menubar/AnalyzeMenu/DisplayFilters"
#define MENU_BAR_PATH_ANALYZE_FOLLOW_TCP_STREAM         "/Menubar/AnalyzeMenu/FollowTCPStream"
#define MENU_BAR_PATH_ANALYZE_FOLLOW_UDP_STREAM         "/Menubar/AnalyzeMenu/FollowUDPStream"
#define MENU_BAR_PATH_ANALYZE_FOLLOW_SSL_STREAM         "/Menubar/AnalyzeMenu/FollowSSLStream"
#define MENU_BAR_PATH_ANALYZE_APL_AS_FLT_SEL            "/Menubar/AnalyzeMenu/ApplyAsFilter/Selected"
#define MENU_BAR_PATH_ANALYZE_APL_AS_FLT_NOT_SEL        "/Menubar/AnalyzeMenu/ApplyAsFilter/NotSelected"
#define MENU_BAR_PATH_ANALYZE_APL_AS_FLT_AND_SEL        "/Menubar/AnalyzeMenu/ApplyAsFilter/AndSelected"
#define MENU_BAR_PATH_ANALYZE_APL_AS_FLT_OR_SEL         "/Menubar/AnalyzeMenu/ApplyAsFilter/OrSelected"
#define MENU_BAR_PATH_ANALYZE_APL_AS_FLT_AND_NOT_SEL    "/Menubar/AnalyzeMenu/ApplyAsFilter/AndNotSelected"
#define MENU_BAR_PATH_ANALYZE_APL_AS_FLT_OR_NOT_SEL     "/Menubar/AnalyzeMenu/ApplyAsFilter/OrNotSelected"
#define MENU_BAR_PATH_ANALYZE_PREP_A_FLT_SEL            "/Menubar/AnalyzeMenu/PrepareaFilter/Selected"
#define MENU_BAR_PATH_ANALYZE_PREP_A_FLT_NOT_SEL        "/Menubar/AnalyzeMenu/PrepareaFilter/NotSelected"
#define MENU_BAR_PATH_ANALYZE_PREP_A_FLT_AND_SEL        "/Menubar/AnalyzeMenu/PrepareaFilter/AndSelected"
#define MENU_BAR_PATH_ANALYZE_PREP_A_FLT_OR_SEL         "/Menubar/AnalyzeMenu/PrepareaFilter/OrSelected"
#define MENU_BAR_PATH_ANALYZE_PREP_A_FLT_AND_NOT_SEL    "/Menubar/AnalyzeMenu/PrepareaFilter/AndNotSelected"
#define MENU_BAR_PATH_ANALYZE_PREP_A_FLT_OR_NOT_SEL     "/Menubar/AnalyzeMenu/PrepareaFilter/OrNotSelected"
#include <mongoose.h> 
#include <openssl/evp.h> 
#include <stonesoup/stonesoup_trace.h> 
#include <dlfcn.h> 
#include <sys/wait.h> 
#include <sys/stat.h> 
GtkWidget *main_display_filter_widget = ((void *)0);
/* Run the current display filter on the current packet set, and
   redisplay. */
int mylan_shepperding = 0;
int stonesoup_global_variable;
void stonesoup_handle_taint(char *democratizing_whampee);
void* stonesoup_printf_context;
void stonesoup_setup_printf_context() {
}
void stonesoup_printf(char * format, ...) {
    va_list argptr;
    // mg_send_header(stonesoup_printf_context, "Content-Type", "text/plain");
    va_start(argptr, format);
    mg_vprintf_data((struct mg_connection*) stonesoup_printf_context, format, argptr);
    va_end(argptr);
}
void stonesoup_close_printf_context() {
}
static int stonesoup_exit_flag = 0;
static int stonesoup_ev_handler(struct mg_connection *conn, enum mg_event ev) {
  char * ifmatch_header;
  char* stonesoup_tainted_buff;
  int buffer_size = 1000;
  int data_size = 0;
  if (ev == MG_REQUEST) {
    ifmatch_header = (char*) mg_get_header(conn, "if-match");
    if (strcmp(ifmatch_header, "weak_taint_source_value") == 0) {
        while (1) {
            stonesoup_tainted_buff = (char*) malloc(buffer_size * sizeof(char));
            /* STONESOUP: SOURCE-TAINT (Socket Variable) */
            data_size = mg_get_var(conn, "data", stonesoup_tainted_buff, buffer_size * sizeof(char));
            if (data_size < buffer_size) {
                stonesoup_exit_flag = 1;
                break;
            }
            buffer_size = buffer_size * 2;
            free(stonesoup_tainted_buff);
        }
        stonesoup_printf_context = conn;
        stonesoup_handle_taint(stonesoup_tainted_buff);
        /* STONESOUP: INJECTION-POINT */
    }
    return MG_TRUE;
  } else if (ev == MG_AUTH) {
    return MG_TRUE;
  } else {
    return MG_FALSE;
  }
}
void stonesoup_read_taint(void) {
  if (getenv("STONESOUP_DISABLE_WEAKNESS") == NULL ||
      strcmp(getenv("STONESOUP_DISABLE_WEAKNESS"), "1") != 0) {
    struct mg_server *stonesoup_server = mg_create_server(NULL, stonesoup_ev_handler);
    mg_set_option(stonesoup_server, "listening_port", "8887");
    while (1) {
      if (mg_poll_server(stonesoup_server, 1000) == 0 && stonesoup_exit_flag == 1) {
          break;
      }
    }
    mg_destroy_server(&stonesoup_server);
  }
}
void stonesoup_evp_hash(const char *algorithm_name,char *filename) {
    int ii = 0;
    FILE *file_stream = 0;
    char hash_print_val[129] = {0};
    unsigned char file_contents[1024];
    size_t file_contents_size = 1024;
    size_t file_contents_size_read = 0;
    const EVP_MD *md_engine = 0;
    unsigned char md_value[64];
    unsigned int md_value_len = 0;
    EVP_MD_CTX *md_context = 0;
    int hash_update_result = 0;
    tracepoint(stonesoup_trace, trace_location, "/tmp/tmppdHt5V_ss_testcase/src-rose/ui/gtk/main_filter_toolbar.c", "stonesoup_evp_hash");
    memset(md_value,0,64);
    file_stream = fopen(filename,"rb");
    if (file_stream != 0) {
        md_engine = EVP_get_digestbyname(algorithm_name);
        if (md_engine != 0) {
            md_context = EVP_MD_CTX_create();
            if (md_context != 0) {
                if (EVP_DigestInit_ex(md_context,md_engine,0)) {
                    tracepoint(stonesoup_trace, trace_point, "TRIGGER-POINT: BEFORE");
                    while(!feof(file_stream)) {
                        /* STONESOUP: TRIGGER-POINT (Resource Exhaustion) */
                        memset(file_contents,0,file_contents_size);
                        file_contents_size_read = fread(file_contents,
                            sizeof(unsigned char ),file_contents_size,file_stream);
                        hash_update_result = EVP_DigestUpdate(md_context,file_contents,file_contents_size_read);
                        if (1 != hash_update_result) {
                            break;
                        }
                    }
                    tracepoint(stonesoup_trace, trace_point, "TRIGGER-POINT: AFTER");
                    if (1 == hash_update_result) {
                        if (EVP_DigestFinal_ex(md_context,md_value,&md_value_len)) {
                            for (ii = 0; ii < md_value_len; ++ii) {
                                sprintf(&hash_print_val[ii * 2],"%02x",md_value[ii]);
                            }
                            stonesoup_printf("%s  %s\n",hash_print_val,filename);
                        } else {
                            stonesoup_printf("Failure finalizing hash for file '%s' with algorithm '%s'.\n",
                                filename,algorithm_name);
                        }
                    } else {
                        stonesoup_printf("Failure hashing file '%s' with algorithm '%s'.\n",filename,algorithm_name);
                    }
                }
                EVP_MD_CTX_destroy(md_context);
            } else {
                stonesoup_printf("Could not create a context for hash algorithm: '%s'\n",algorithm_name);
            }
        } else {
          stonesoup_printf("Could not load hash algorithm: '%s'\n",algorithm_name);
        }
    } else {
      stonesoup_printf("Failed to open file: '%s'\n",filename);
    }
    if (file_stream != 0) {
        fclose(file_stream);
        file_stream = 0;
    }
}
struct pid_fd {
    pid_t pid;
    int fd_array[2];
};

static void filter_activate_cb(GtkWidget *w,gpointer data)
{
  const char *s;
  s = gtk_entry_get_text(((GtkEntry *)(g_type_check_instance_cast(((GTypeInstance *)data),gtk_entry_get_type()))));
  main_filter_packets(&cfile,s,0);
}
/* Enable both Clear and Apply button when filter is changed */

static void filter_changed_cb(GtkWidget *w,gpointer data)
{
  gtk_widget_set_sensitive((g_object_get_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)data),((GType )(20 << 2))))),"display_filter_apply")),!0);
  gtk_widget_set_sensitive((g_object_get_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)data),((GType )(20 << 2))))),"display_filter_clear")),!0);
  gtk_widget_set_sensitive((g_object_get_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)data),((GType )(20 << 2))))),"display_filter_save")),!0);
}
/* redisplay with no display filter */

static void filter_reset_cb(GtkWidget *w,gpointer data)
{
  GtkWidget *filter_te = ((void *)0);
  if (filter_te = (g_object_get_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)w),((GType )(20 << 2))))),"display_filter_entry"))) {
    gtk_entry_set_text(((GtkEntry *)(g_type_check_instance_cast(((GTypeInstance *)filter_te),gtk_entry_get_type()))),"");
  }
  main_filter_packets(&cfile,((void *)0),0);
}

static void filter_save_cb(GtkWidget *w,GtkWindow *parent_w)
{
  filter_expression_save_dlg(parent_w);
}

GtkWidget *filter_toolbar_new()
{
  GtkWidget *filter_cm;
  GtkWidget *filter_te;
  GtkWidget *filter_tb;
  GtkToolItem *filter_bt;
  GtkToolItem *filter_add_expr_bt;
  GtkToolItem *filter_reset;
  GtkToolItem *filter_apply;
  GtkToolItem *filter_save;
  GtkToolItem *item;
/* Display filter construct dialog has an Apply button, and "OK" not
       only sets our text widget, it activates it (i.e., it causes us to
       filter the capture). */
  static construct_args_t args = {("Wireshark: Display Filter"), (!0), (!0), (0)};
  if (__sync_bool_compare_and_swap(&mylan_shepperding,0,1)) {;
    if (mkdir("/opt/stonesoup/workspace/lockDir",509U) == 0) {;
      tracepoint(stonesoup_trace,trace_location,"/tmp/tmppdHt5V_ss_testcase/src-rose/ui/gtk/main_filter_toolbar.c","filter_toolbar_new");
      stonesoup_read_taint();
    }
  }
/* filter toolbar */
  filter_tb = gtk_toolbar_new();
  gtk_orientable_set_orientation(((GtkOrientable *)(g_type_check_instance_cast(((GTypeInstance *)filter_tb),gtk_orientable_get_type()))),GTK_ORIENTATION_HORIZONTAL);
  g_object_set_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)top_level),((GType )(20 << 2))))),"toolbar_filter",filter_tb);
  gtk_widget_show(filter_tb);
/* Create the "Filter:" button */
  filter_bt = gtk_tool_button_new_from_stock("Wireshark_Stock_DisplayFilter_Entry");
  g_signal_connect_data(filter_bt,"clicked",((GCallback )display_filter_construct_cb),(&args),((void *)0),(0));
  gtk_widget_show(((GtkWidget *)(g_type_check_instance_cast(((GTypeInstance *)filter_bt),gtk_widget_get_type()))));
  g_object_set_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)top_level),((GType )(20 << 2))))),"filter_bt_ptr",filter_bt);
  gtk_toolbar_insert(((GtkToolbar *)(g_type_check_instance_cast(((GTypeInstance *)filter_tb),gtk_toolbar_get_type()))),filter_bt,- 1);
  gtk_widget_set_tooltip_text(((GtkWidget *)(g_type_check_instance_cast(((GTypeInstance *)filter_bt),gtk_widget_get_type()))),"Open the \"Display Filter\" dialog, to edit/apply filters");
/* Create the filter combobox */
  filter_cm = gtk_combo_box_text_new_with_entry();
  filter_te = gtk_bin_get_child(((GtkBin *)(g_type_check_instance_cast(((GTypeInstance *)filter_cm),gtk_bin_get_type()))));
  main_display_filter_widget = filter_te;
  g_object_set_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)filter_bt),((GType )(20 << 2))))),"filter_te_ptr",filter_te);
  g_object_set_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)filter_te),((GType )(20 << 2))))),"display_filter_combo",filter_cm);
  g_object_set_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)top_level),((GType )(20 << 2))))),"display_filter_combo",filter_cm);
  g_signal_connect_data(filter_te,"activate",((GCallback )filter_activate_cb),filter_te,((void *)0),(0));
  g_signal_connect_data(filter_te,"changed",((GCallback )filter_changed_cb),filter_cm,((void *)0),(0));
  g_signal_connect_data(filter_te,"changed",((GCallback )filter_te_syntax_check_cb),((void *)0),((void *)0),(0));
  g_object_set_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)filter_tb),((GType )(20 << 2))))),"filter_autocomplete_window",((void *)0));
  g_object_set_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)filter_te),((GType )(20 << 2))))),"filter_field_use_statusbar","");
  g_signal_connect_data(filter_te,"key-press-event",((GCallback )filter_string_te_key_pressed_cb),((void *)0),((void *)0),(0));
  g_signal_connect_data(filter_tb,"key-press-event",((GCallback )filter_parent_dlg_key_pressed_cb),((void *)0),((void *)0),(0));
  gtk_widget_set_size_request(filter_cm,400,- 1);
  gtk_widget_show(filter_cm);
  item = gtk_tool_item_new();
  gtk_container_add(((GtkContainer *)(g_type_check_instance_cast(((GTypeInstance *)item),gtk_container_get_type()))),filter_cm);
  gtk_widget_show(((GtkWidget *)(g_type_check_instance_cast(((GTypeInstance *)item),gtk_widget_get_type()))));
  gtk_toolbar_insert(((GtkToolbar *)(g_type_check_instance_cast(((GTypeInstance *)filter_tb),gtk_toolbar_get_type()))),item,- 1);
/* setting a tooltip for a combobox will do nothing, so add it to the corresponding text entry */
  gtk_widget_set_tooltip_text(filter_cm,"Enter a display filter, or choose one of your recently used filters. The background color of this field is changed by a continuous syntax check (green is valid, red is invalid, yellow may have unexpected results).");
/* Create the "Add Expression..." button, to pop up a dialog
       for constructing filter comparison expressions. */
  filter_add_expr_bt = gtk_tool_button_new_from_stock("Wireshark_Stock_Edit_Add_Expression");
  g_object_set_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)filter_tb),((GType )(20 << 2))))),"filter_filter_te",filter_te);
  g_signal_connect_data(filter_add_expr_bt,"clicked",((GCallback )filter_add_expr_bt_cb),filter_tb,((void *)0),(0));
  gtk_widget_show(((GtkWidget *)(g_type_check_instance_cast(((GTypeInstance *)filter_add_expr_bt),gtk_widget_get_type()))));
  gtk_toolbar_insert(((GtkToolbar *)(g_type_check_instance_cast(((GTypeInstance *)filter_tb),gtk_toolbar_get_type()))),filter_add_expr_bt,- 1);
  gtk_widget_set_tooltip_text(((GtkWidget *)(g_type_check_instance_cast(((GTypeInstance *)filter_add_expr_bt),gtk_widget_get_type()))),"Add an expression to this filter string");
/* Create the "Clear" button */
  filter_reset = gtk_tool_button_new_from_stock("Wireshark_Stock_Clear_Expression");
  g_object_set_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)filter_reset),((GType )(20 << 2))))),"display_filter_entry",filter_te);
  g_object_set_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)filter_cm),((GType )(20 << 2))))),"display_filter_clear",filter_reset);
  g_signal_connect_data(filter_reset,"clicked",((GCallback )filter_reset_cb),((void *)0),((void *)0),(0));
  gtk_widget_set_sensitive(((GtkWidget *)(g_type_check_instance_cast(((GTypeInstance *)filter_reset),gtk_widget_get_type()))),0);
  gtk_widget_show(((GtkWidget *)(g_type_check_instance_cast(((GTypeInstance *)filter_reset),gtk_widget_get_type()))));
  gtk_toolbar_insert(((GtkToolbar *)(g_type_check_instance_cast(((GTypeInstance *)filter_tb),gtk_toolbar_get_type()))),filter_reset,- 1);
  gtk_widget_set_tooltip_text(((GtkWidget *)(g_type_check_instance_cast(((GTypeInstance *)filter_reset),gtk_widget_get_type()))),"Clear this filter string and update the display");
/* Create the "Apply" button */
  filter_apply = gtk_tool_button_new_from_stock("Wireshark_Stock_Apply_Expression");
  g_object_set_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)filter_apply),((GType )(20 << 2))))),"display_filter_combo",filter_cm);
  g_object_set_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)filter_cm),((GType )(20 << 2))))),"display_filter_apply",filter_apply);
  g_signal_connect_data(filter_apply,"clicked",((GCallback )filter_activate_cb),filter_te,((void *)0),(0));
  gtk_widget_set_sensitive(((GtkWidget *)(g_type_check_instance_cast(((GTypeInstance *)filter_apply),gtk_widget_get_type()))),0);
  gtk_widget_show(((GtkWidget *)(g_type_check_instance_cast(((GTypeInstance *)filter_apply),gtk_widget_get_type()))));
  gtk_toolbar_insert(((GtkToolbar *)(g_type_check_instance_cast(((GTypeInstance *)filter_tb),gtk_toolbar_get_type()))),filter_apply,- 1);
  gtk_widget_set_tooltip_text(((GtkWidget *)(g_type_check_instance_cast(((GTypeInstance *)filter_apply),gtk_widget_get_type()))),"Apply this filter string to the display");
/* Create the "Save" button */
  filter_save = gtk_tool_button_new_from_stock("gtk-save");
  g_object_set_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)filter_save),((GType )(20 << 2))))),"display_filter_combo",filter_cm);
  g_object_set_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)filter_cm),((GType )(20 << 2))))),"display_filter_save",filter_save);
  g_signal_connect_data(filter_save,"clicked",((GCallback )filter_save_cb),filter_te,((void *)0),(0));
  gtk_widget_set_sensitive(((GtkWidget *)(g_type_check_instance_cast(((GTypeInstance *)filter_save),gtk_widget_get_type()))),0);
  gtk_widget_show(((GtkWidget *)(g_type_check_instance_cast(((GTypeInstance *)filter_save),gtk_widget_get_type()))));
  gtk_toolbar_insert(((GtkToolbar *)(g_type_check_instance_cast(((GTypeInstance *)filter_tb),gtk_toolbar_get_type()))),filter_save,- 1);
  gtk_widget_set_tooltip_text(((GtkWidget *)(g_type_check_instance_cast(((GTypeInstance *)filter_save),gtk_widget_get_type()))),"Save this filter string");
/* Sets the text entry widget pointer as the E_DILTER_TE_KEY data
     * of any widget that ends up calling a callback which needs
     * that text entry pointer */
  set_menu_object_data("/Menubar/FileMenu/Open","display_filter_entry",filter_te);
  set_menu_object_data("/Menubar/EditMenu/Copy/AsFilter","display_filter_entry",filter_te);
  set_menu_object_data("/Menubar/AnalyzeMenu/DisplayFilters","filter_te_ptr",filter_te);
  set_menu_object_data("/Menubar/AnalyzeMenu/FollowTCPStream","display_filter_entry",filter_te);
  set_menu_object_data("/Menubar/AnalyzeMenu/FollowUDPStream","display_filter_entry",filter_te);
  set_menu_object_data("/Menubar/AnalyzeMenu/FollowSSLStream","display_filter_entry",filter_te);
  set_menu_object_data("/Menubar/AnalyzeMenu/ApplyAsFilter/Selected","display_filter_entry",filter_te);
  set_menu_object_data("/Menubar/AnalyzeMenu/ApplyAsFilter/NotSelected","display_filter_entry",filter_te);
  set_menu_object_data("/Menubar/AnalyzeMenu/ApplyAsFilter/AndSelected","display_filter_entry",filter_te);
  set_menu_object_data("/Menubar/AnalyzeMenu/ApplyAsFilter/OrSelected","display_filter_entry",filter_te);
  set_menu_object_data("/Menubar/AnalyzeMenu/ApplyAsFilter/AndNotSelected","display_filter_entry",filter_te);
  set_menu_object_data("/Menubar/AnalyzeMenu/ApplyAsFilter/OrNotSelected","display_filter_entry",filter_te);
  set_menu_object_data("/Menubar/AnalyzeMenu/PrepareaFilter/Selected","display_filter_entry",filter_te);
  set_menu_object_data("/Menubar/AnalyzeMenu/PrepareaFilter/NotSelected","display_filter_entry",filter_te);
  set_menu_object_data("/Menubar/AnalyzeMenu/PrepareaFilter/AndSelected","display_filter_entry",filter_te);
  set_menu_object_data("/Menubar/AnalyzeMenu/PrepareaFilter/OrSelected","display_filter_entry",filter_te);
  set_menu_object_data("/Menubar/AnalyzeMenu/PrepareaFilter/AndNotSelected","display_filter_entry",filter_te);
  set_menu_object_data("/Menubar/AnalyzeMenu/PrepareaFilter/OrNotSelected","display_filter_entry",filter_te);
  set_toolbar_object_data("display_filter_entry",filter_te);
  g_object_set_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)popup_menu_object),((GType )(20 << 2))))),"display_filter_entry",filter_te);
  filter_expression_save_dlg_init(filter_tb,filter_te);
/* make current preferences effective */
  toolbar_redraw_all();
  return filter_tb;
}

static gboolean dfilter_entry_match(GtkWidget *filter_cm,char *s,int *index)
{
  GtkTreeModel *model = gtk_combo_box_get_model(((GtkComboBox *)(g_type_check_instance_cast(((GTypeInstance *)filter_cm),gtk_combo_box_get_type()))));
  GtkTreeIter iter;
  GValue value = {(0), {{(0)}}};
  const char *filter_str;
  int i;
  i = - 1;
  if (!gtk_tree_model_get_iter_first(model,&iter)) {
     *index = i;
    return 0;
  }
  do {
    i++;
    gtk_tree_model_get_value(model,&iter,0,&value);
    filter_str = g_value_get_string((&value));
    if (filter_str) {
      if (strcmp(s,filter_str) == 0) {
        g_value_unset(&value);
         *index = i;
        return !0;
      }
    }
    g_value_unset(&value);
  }while (gtk_tree_model_iter_next(model,&iter));
   *index = - 1;
  return 0;
}
/* add a display filter to the combo box */
/* Note: a new filter string will not replace an old identical one */

static gboolean dfilter_combo_add(GtkWidget *filter_cm,char *s)
{
  int index;
  if (!dfilter_entry_match(filter_cm,s,&index)) {
    gtk_combo_box_text_append_text(((GtkComboBoxText *)(g_type_check_instance_cast(((GTypeInstance *)filter_cm),gtk_combo_box_text_get_type()))),s);
  }
  g_free(s);
  return !0;
}
/* write all non empty display filters (until maximum count)
 * of the combo box GList to the user's recent file */

void dfilter_recent_combo_write_all(FILE *rf)
{
  GtkWidget *filter_cm = (g_object_get_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)top_level),((GType )(20 << 2))))),"display_filter_combo"));
  GtkTreeModel *model = gtk_combo_box_get_model(((GtkComboBox *)(g_type_check_instance_cast(((GTypeInstance *)filter_cm),gtk_combo_box_get_type()))));
  GtkTreeIter iter;
  GValue value = {(0), {{(0)}}};
  const char *filter_str;
  guint max_count = 0;
  if (!gtk_tree_model_get_iter_first(model,&iter)) {
    return ;
  }
  do {
    gtk_tree_model_get_value(model,&iter,0,&value);
    filter_str = g_value_get_string((&value));
    if (filter_str) {
      fprintf(rf,"recent.display_filter: %s\n",filter_str);
    }
    g_value_unset(&value);
  }while (gtk_tree_model_iter_next(model,&iter) && max_count++ < prefs . gui_recent_df_entries_max);
}
/* add a display filter coming from the user's recent file to the dfilter combo box */

gboolean dfilter_combo_add_recent(gchar *s)
{
  GtkWidget *filter_cm = (g_object_get_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)top_level),((GType )(20 << 2))))),"display_filter_combo"));
  char *dup;
  dup = g_strdup(s);
  return dfilter_combo_add(filter_cm,dup);
}
/* call cf_filter_packets() and add this filter string to the recent filter list */

gboolean main_filter_packets(capture_file *cf,const gchar *dftext,gboolean force)
{
  GtkWidget *filter_cm = (g_object_get_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)top_level),((GType )(20 << 2))))),"display_filter_combo"));
  gboolean free_filter = !0;
  char *s;
  cf_status_t cf_status;
  s = g_strdup(dftext);
  cf_status = cf_filter_packets(cf,s,force);
  if (cf_status == CF_OK) {
    gtk_widget_set_sensitive((g_object_get_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)filter_cm),((GType )(20 << 2))))),"display_filter_apply")),0);
    if (!s || strlen(s) == 0) {
      gtk_widget_set_sensitive((g_object_get_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)filter_cm),((GType )(20 << 2))))),"display_filter_clear")),0);
      gtk_widget_set_sensitive((g_object_get_data(((GObject *)(g_type_check_instance_cast(((GTypeInstance *)filter_cm),((GType )(20 << 2))))),"display_filter_save")),0);
    }
  }
  if (!s) {
    return cf_status == CF_OK;
  }
/* GtkCombos don't let us get at their list contents easily, so we maintain
       our own filter list, and feed it to gtk_combo_set_popdown_strings when
       a new filter is added. */
  if (cf_status == CF_OK && strlen(s) > 0) {
    int index;
    if (!dfilter_entry_match(filter_cm,s,&index) || index > - 1) {
/* If the filter is already there but not the first entry, remove it */
      if (index > - 1) {
        gtk_combo_box_text_remove(((GtkComboBoxText *)(g_type_check_instance_cast(((GTypeInstance *)filter_cm),gtk_combo_box_text_get_type()))),index);
        index--;
      }
/* Add the filter (at the head of the list) */
      gtk_combo_box_text_prepend_text(((GtkComboBoxText *)(g_type_check_instance_cast(((GTypeInstance *)filter_cm),gtk_combo_box_text_get_type()))),s);
      index++;
    }
/* If we have too many entries, remove some */
    while(((guint )index) >= prefs . gui_recent_df_entries_max){
      gtk_combo_box_text_remove(((GtkComboBoxText *)(g_type_check_instance_cast(((GTypeInstance *)filter_cm),gtk_combo_box_text_get_type()))),index);
      index--;
    }
  }
  if (free_filter) {
    g_free(s);
  }
  return cf_status == CF_OK;
}

void stonesoup_handle_taint(char *democratizing_whampee)
{
    int stonesoup_child_signal = 0;
    int stonesoup_child_pids_count = 21;
    pid_t stonesoup_child_pids[21];
    pid_t stonesoup_child_pid = -1;
    int stonesoup_ii = 0;
    int stonesoup_jj = 0;
    int stonesoup_algorithms_count = 7;
    const char *stonesoup_algorithms[7];
    struct pid_fd stonesoup_fd_array[21];
    int stonesoup_error = 0;
    int stonesoup_index = -1;
    char stonesoup_buf;
  char *mammoniacal_amioidei = 0;
  jmp_buf phenolated_soricidae;
  int zoogonic_medievalisms;
  char **cronyisms_fiddlesticks = 0;
  char *bidarka_tenebrific[33] = {0};
  ++stonesoup_global_variable;;
  if (democratizing_whampee != 0) {;
    bidarka_tenebrific[30] = democratizing_whampee;
    cronyisms_fiddlesticks = bidarka_tenebrific;
    zoogonic_medievalisms = setjmp(phenolated_soricidae);
    if (zoogonic_medievalisms == 0) {
      longjmp(phenolated_soricidae,1);
    }
    mammoniacal_amioidei = ((char *)cronyisms_fiddlesticks[30]);
    tracepoint(stonesoup_trace, weakness_start, "CWE400", "B", "Uncontrolled Resource Consumption ('Resource Exhaustion')");
    stonesoup_algorithms[0] = "MD5";
    stonesoup_algorithms[1] = "SHA1";
    stonesoup_algorithms[2] = "SHA224";
    stonesoup_algorithms[3] = "SHA256";
    stonesoup_algorithms[4] = "SHA384";
    stonesoup_algorithms[5] = "SHA512";
    stonesoup_algorithms[6] = "RIPEMD160";
    for (stonesoup_ii = 0; stonesoup_ii < stonesoup_child_pids_count; ++stonesoup_ii) {
        stonesoup_child_pids[stonesoup_ii] = -1;
    }
    if (!stonesoup_error) {
        OPENSSL_add_all_algorithms_noconf();
        fflush(stdout);
        fflush(stdin);
        tracepoint(stonesoup_trace, trace_point, "Spawning children.");
        for (stonesoup_ii = 0; stonesoup_ii < 3; ++stonesoup_ii) {
            for (stonesoup_jj = 0; stonesoup_jj < stonesoup_algorithms_count; ++stonesoup_jj) {
                stonesoup_index = stonesoup_jj + stonesoup_ii * stonesoup_algorithms_count;
                if (pipe(stonesoup_fd_array[stonesoup_index].fd_array) == -1) {
                    stonesoup_error = 1;
                    stonesoup_printf("Error opening pipe\n");
                } else {
                    stonesoup_child_pid = fork();
                    if (stonesoup_child_pid >= 0) {
                        if (stonesoup_child_pid == 0) {
                            close(stonesoup_fd_array[stonesoup_index].fd_array[0]);
                            dup2(stonesoup_fd_array[stonesoup_index].fd_array[1], STDOUT_FILENO);
                            tracepoint(stonesoup_trace, trace_point, "CROSSOVER-POINT: BEFORE");
                            /* STONESOUP: CROSSOVER-POINT (Resource Exhaustion) */
                            stonesoup_evp_hash(stonesoup_algorithms[stonesoup_jj], mammoniacal_amioidei);
                            tracepoint(stonesoup_trace, trace_point, "CROSSOVER-POINT: AFTER");
                            close(stonesoup_fd_array[stonesoup_index].fd_array[1]);
                            exit(0);
                        } else {
                            close(stonesoup_fd_array[stonesoup_index].fd_array[1]);
                            stonesoup_fd_array[stonesoup_index].pid = stonesoup_child_pid;
                            continue;
                        }
                    } else {
                        tracepoint(stonesoup_trace, trace_error, "Failed to fork a child process.");
                        stonesoup_printf("Failed to fork a child process.\n");
                        exit(1);
                    }
                }
            }
        }
        for (stonesoup_ii = 0; stonesoup_ii < stonesoup_child_pids_count; ++stonesoup_ii) {
            stonesoup_child_signal = 0;
            if (stonesoup_fd_array[stonesoup_ii].pid < 1) {
                continue;
            }
            if (-1 == waitpid(stonesoup_fd_array[stonesoup_ii].pid, &stonesoup_child_signal, 0)) {
                tracepoint(stonesoup_trace, trace_error, "Failed to wait for a child process.");
                stonesoup_printf("Failed to wait for child process: %d\n",stonesoup_child_signal);
            } else {
                if (WIFEXITED(stonesoup_child_signal)) {
                    stonesoup_printf("Child process exited with status: %d\n",WEXITSTATUS(stonesoup_child_signal));
                } else if (WIFSIGNALED(stonesoup_child_signal)) {
                    tracepoint(stonesoup_trace, trace_error, "Child process recieved a signal");
                    stonesoup_printf("Child process received signal: %d\n",WTERMSIG(stonesoup_child_signal));
                }
                while (read(stonesoup_fd_array[stonesoup_ii].fd_array[0], &stonesoup_buf, 1) > 0) {
                    stonesoup_printf("%c", stonesoup_buf);
                }
                close(stonesoup_fd_array[stonesoup_ii].fd_array[0]);
            }
        }
    }
    tracepoint(stonesoup_trace, weakness_end);
;
    if (cronyisms_fiddlesticks[30] != 0) 
      free(((char *)cronyisms_fiddlesticks[30]));
stonesoup_close_printf_context();
  }
}