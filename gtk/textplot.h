#include <gtk/gtk.h>
#include "support_cb.h"

void TextPlotDialog_add_variable(GtkWidget *TextPlotDialog, v5d_var_info *vinfo);
GtkWidget *new_TextPlotDialog(GtkWidget *window);

void
on_size_sb_changed                     (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_spacing_sb_changed                  (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_fontx_sb_changed                    (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_fonty_sb_changed                    (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_Color_clicked                       (GtkButton       *button,
                                        gpointer         user_data);

void
on_textplot_variables_released         (GtkButton       *button,
                                        gpointer         user_data);

void
on_okay_clicked                        (GtkButton       *button,
                                        gpointer         user_data);

void
on_cancel_clicked                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_tpokay_clicked                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_tpcancel_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_textplot_variables_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_tp_sb_changed                       (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_tpclose_clicked                     (GtkButton       *button,
                                        gpointer         user_data);
