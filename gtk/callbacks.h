#include <gtk/gtk.h>

typedef struct {
  gint v5d_display_context;

  float beginx,beginy;  /* position of mouse */

  float zoom;           /* field of view in degrees */

  GtkWidget *VarSelectionDialog;
  GtkWidget *GtkGlArea;
  int animate;
  guint32 animate_speed;
  gint timeout_id;
  gint stepsize;
  int timestep;
  int numtimes;
  int vcs;

  GtkWidget *HSliceControls;

} v5d_info;

typedef struct {
  gfloat min, max, interval, level, pressure;
  gdouble color[4];
  gint linewidth;
  gint    stipplefactor;
  gushort stipple;
  GtkWidget *level_value;  /* the vertical level label */
} hslicecontrols;

typedef struct {
  gint v5d_data_context;
  gint varid;
  v5d_info *info;
  gint maxlevel;
  hslicecontrols *hc;
} v5d_var_info;


typedef enum {
  DATA_FILE,
  PROCEDURE_FILE,
  TOPO_FILE,
  MAP_FILE
} openwhatfile;

GtkWidget *new_window3D(GtkWidget *oldwindow3D);


void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
void
on_open_server1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_import1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_exit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_help1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);


GtkWidget*
create_glarea (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2);

void
on_fileselect_ok                       (GtkButton       *button,
                                        gpointer         user_data);

void
on_fileselect_cancel                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_3d_window1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_sounding_window1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_fileselect_ok                       (GtkButton       *button,
                                        gpointer         user_data);

void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_fileselect_ok                       (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_V5DDataTree_button_press_event      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_Data_Selection_realize              (GtkWidget       *widget,
                                        gpointer         user_data);

gboolean
on_V5DDataTree_button_press_event      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_V5DDataTree_button_release_event    (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_V5DDataTree_tree_select_row         (GtkCTree        *ctree,
                                        GList           *node,
                                        gint             column,
                                        gpointer         user_data);

void
on_V5DDataTree_tree_unselect_row       (GtkCTree        *ctree,
                                        GList           *node,
                                        gint             column,
                                        gpointer         user_data);

void
CloseVarGraphicsWindow                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_VarGraphicsWindow_realize           (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_Contour_checkbutton_toggled         (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_Isosurface_checkbutton_toggled      (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

gboolean
glarea_button_release                  (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
glarea_buttone_press                   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
glarea_motion_notify                   (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

gboolean
glarea_expose_event                    (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

gboolean
glarea_reshape                         (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

void
glarea_init                            (GtkWidget       *widget,
                                        gpointer         user_data);


gboolean
glarea_button_press                    (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
glarea_draw                            (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

void
on_ContourValueDefaults_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
ResetContourDefaults                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_ContourValue_changed                (GtkEditable     *editable,
                                        gpointer         user_data);

gboolean
on_VarGraphicsWindow_expose_event      (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_map1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_topo1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_preferences1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_map1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_topo1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_map1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_box1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_clock1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_topo1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_browse_clicked                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_Prefs_OK_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_Prefs_Cancel_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_preferences1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_new_file1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_open2_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_exit2_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_cut1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_copy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_paste1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_clear1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_properties1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_preferences2_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_Prefs_OK_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_Prefs_cancel_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_VariableCTree_tree_select_row       (GtkCTree        *ctree,
                                        GList           *node,
                                        gint             column,
                                        gpointer         user_data);

void
on_VariableCTree_tree_unselect_row     (GtkCTree        *ctree,
                                        GList           *node,
                                        gint             column,
                                        gpointer         user_data);

void
on_VSDClose_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_ColorSelectionOk_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_Cancel_Clicked                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_Arrow_clicked                       (GtkButton       *button,
                                        gpointer         user_data);

void
on_Arrow_clicked                       (GtkButton       *button,
                                        gpointer         user_data);

void
on_animate_toggled                     (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_Arrow_clicked                       (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_animate_pressed                     (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_animate_toggled                     (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_animate_toggled                     (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_open_in_new_frame1_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_open_profile_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_new_profile_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_delete_frame1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_window_3d1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_contour_label1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_topography1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_map2_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_fontselectionbutton_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_window_3d1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_contour_label1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_topography1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_map2_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_fontselectionbutton_clicked         (GtkButton       *button,
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

GtkWidget *new_window3D(GtkWidget *oldwindow3D);
GdkColor *vis5d_color_to_gdk(GtkWidget *widget, float red, float green, float blue);
void update_hslice_controls(v5d_var_info *vinfo);

void
on_hslicecontrolsapply_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_hslicecontrolsclose_clicked         (GtkButton       *button,
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
on_hsmin_changed                       (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_hsmax_changed                       (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_hsspin_changed                      (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_hslice_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_chslice_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_vslice_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_cvslice_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_isosurface_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_volume_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_set_view_top1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_set_view_south1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_hslicecontrolsclose_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_hslicectree_select_row              (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

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
on_hsclearall_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_hsclose_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_hs_color_changed                    (GtkColorSelection *colorselection,
                                        gpointer         user_data);
