#ifndef __GRADIENT_CB__
#define __GRADIENT_CB__ 1
#include <gtk/gtk.h>
#include <support_cb.h>

void
on_clist1_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_Edit_clicked                        (GtkButton       *button,
                                        gpointer         user_data);

void
on_Close_clicked                       (GtkButton       *button,
                                        gpointer         user_data);


GtkWidget *new_GtkGradient(void);

void gradient_preview_update(preview_area *sample, gboolean resize);

void gradient_update_sample_from_name(preview_area *sample, gchar *name);

#endif
