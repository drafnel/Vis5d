#include <gtk/gtk.h>
#include "support_cb.h"

GtkWidget * new_ProcedureDialog(v5d_info *info, gchar *filename);

void
on_ProcedureCtree_tree_select_row      (GtkCTree        *ctree,
                                        GList           *node,
                                        gint             column,
                                        gpointer         user_data);

void
on_ProcedureCtree_tree_unselect_row    (GtkCTree        *ctree,
                                        GList           *node,
                                        gint             column,
                                        gpointer         user_data);

gboolean
on_ProcedureCtree_key_press_event      (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data);

void
on_ProcedureCtree_tree_expand          (GtkCTree        *ctree,
                                        GList           *node,
                                        gpointer         user_data);

void
on_ProcedureCtree_tree_collapse        (GtkCTree        *ctree,
                                        GList           *node,
                                        gpointer         user_data);

void
on_add_clicked                         (GtkButton       *button,
                                        gpointer         user_data);

void
on_delete_clicked                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_close_clicked                       (GtkButton       *button,
                                        gpointer         user_data);

void
on_close_clicked                       (GtkButton       *button,
                                        gpointer         user_data);

void
on_add_clicked                         (GtkButton       *button,
                                        gpointer         user_data);

void
on_delete_clicked                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_add_clicked                         (GtkButton       *button,
                                        gpointer         user_data);

void
on_okay_clicked                        (GtkButton       *button,
                                        gpointer         user_data);

void
on_cancel_clicked                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_capture1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_cut2_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_copy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_paste1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_delete1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_openProcedure_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_saveProcedure_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save_asProcedure_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
