#include "support_cb.h"

GtkWidget *new_window3D(GtkWidget *oldwindow3D);

void 
on_open1_activate(GtkMenuItem     *menuitem,
							  gpointer         user_data);

void 
on_exit1_activate(GtkMenuItem     *menuitem,
							  gpointer         user_data);


void 
on_delete_frame1_activate(GtkMenuItem     *menuitem,
										 gpointer         user_data);


void
on_open_in_new_frame1_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
void 
on_option_toggle(GtkMenuItem *menuitem,gpointer user_data, int v5dwhat);

void
on_map1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
void
on_topo1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
void
on_box1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
void
on_clock1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_Arrow_clicked                       (GtkButton       *button,
                                        gpointer         user_data);
void
on_animate_toggled                     (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

GtkWidget* create_glarea (gchar *widget_name, gchar *string1, gchar *string2,
								  gint int1, gint int2 );

void
on_setview_activate              (GtkMenuItem     *menuitem,
											 gpointer         user_data);


void
on_contour_label1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_window_3d1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);


gboolean 
glarea_draw (GtkWidget* widget, GdkEventExpose* event, gpointer user_data);

gboolean glarea_button_press (GtkWidget* widget, GdkEventButton* event, 
										gpointer         user_data);


void
on_topography1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_map2_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
void
on_save_options1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_openprocedure_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
void
on_newprocedure_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
void
on_hslice_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
void
on_vslice_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);


void
on_isosurface_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_volume_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);


gboolean 
glarea_button_release (GtkWidget* widget, GdkEventButton* event
										  ,gpointer         user_data);


gboolean 
glarea_motion_notify(GtkWidget *widget, GdkEventMotion *event, 
										gpointer         user_data);

gboolean 
glarea_reshape (GtkWidget* widget, 
					 GdkEventConfigure* event, 
					 gpointer user_data) ;

void 
glarea_init (GtkWidget* widget, 
						gpointer user_data);


void
on_variable_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
void
variable_menu_add_variable(GtkWidget *window3D, v5d_var_info *vinfo);

void
on_animate_slower                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_animate_faster                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_change_animate_speed                (GtkButton       *button,
                                        gpointer         user_data);

void
on_append1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
