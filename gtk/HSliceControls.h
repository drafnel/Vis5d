#include <gtk/gtk.h>
#include "support_cb.h"

void
on_hslicectree_tree_select_row         (GtkCTree        *ctree,
                                        GList           *node,
                                        gint             column,
                                        gpointer         user_data);

void
on_hslicectree_tree_unselect_row       (GtkCTree        *ctree,
                                        GList           *node,
                                        gint             column,
                                        gpointer         user_data);

void
on_hsspin_changed                      (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_colorselect_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_interval_hscale_button_changed      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_level_vscale_button_release_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_hsclear_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_hsclose_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_notebook1_switch_page               (GtkNotebook     *notebook,
                                        GtkNotebookPage *page,
                                        gint             page_num,
                                        gpointer         user_data);

void
on_notebook1_switch_page               (GtkNotebook     *notebook,
                                        GtkNotebookPage *page,
                                        gint             page_num,
                                        gpointer         user_data);

void
on_colortable_select_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_hsspin_changed                      (GtkEditable     *editable,
                                        gpointer         user_data);

void hslicecontrol(v5d_var_info *vinfo);

void
on_colorcnt_changed                    (GtkEditable     *editable,
                                        gpointer         user_data);

gboolean
on_alpha_button_release_event          (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);
