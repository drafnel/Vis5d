#include <gtk/gtk.h>
#include "api.h"
#include "support_cb.h"

GtkWidget *new_VarGraphicsControls();
void
on_hsspin_changed                      (GtkEditable     *editable,
                                        gpointer         user_data);

gboolean
on_interval_hscale_button_changed      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_colorselect_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_level_vscale_button_release_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_colortable_select_clicked           (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_alpha_button_release_event          (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_hsclear_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_hsclose_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_colorselect_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_chsspin_changed                     (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_colortable_select_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_colorselect_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_Hslicebutton_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_CHslicebutton_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_hsspin_changed                      (GtkEditable     *editable,
                                        gpointer         user_data);

gboolean
on_drawingarea_button_press_event      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_VarGraphicsControls_destroy_event   (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_VarGraphicsControls_destroy         (GtkObject       *object,
                                        gpointer         user_data);

void
on_VarGraphicsControls_hide            (GtkWidget       *widget,
                                        gpointer         user_data);

gboolean
on_VarGraphicsControls_delete_event    (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_linestyle_toggled                   (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_linestyle_toggled                   (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_linewidth_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_linewidth_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_linewidth_changed                   (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_linewidth_changed                   (GtkEditable     *editable,
                                        gpointer         user_data);

gboolean
on_level_vscale_button_release_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_hsspin_changed                      (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_hsspin_changed                      (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_colortable_select_clicked           (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_hs_color_sample_motion_notify_event (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

void
on_hs_color_sample_show                (GtkWidget       *widget,
                                        gpointer         user_data);

gboolean
on_hs_color_sample_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_hs_color_sample_expose_event        (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

void
on_hs_color_sample_map                 (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_hs_color_sample_map                 (GtkWidget       *widget,
                                        gpointer         user_data);

gboolean
on_hs_color_sample_enterorleave        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_hs_color_sample_enterorleave        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

void
on_Vslicebutton_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data);
