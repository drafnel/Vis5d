#include <gtk/gtk.h>

#ifndef __V5D_CALLBACKS__
#define __V5D_CALLBACKS__ 1

#if defined(HAVE_OPENGL)
#  ifndef WORDS_BIGENDIAN
#    define PACK_COLOR(R,G,B,A)   ( (A)<<24 | (B)<<16 | (G)<<8 | (R) )
#    define UNPACK_RED(X)         ( (X) & 0xff )
#    define UNPACK_GREEN(X)       ( ( (X) >> 8 ) & 0xff )
#    define UNPACK_BLUE(X)        ( ( (X) >> 16 ) & 0xff )
#    define UNPACK_ALPHA(X)       ( ( (X) >> 24 ) & 0xff )
#  else
#    define PACK_COLOR(R,G,B,A)   ( (R)<<24 | (G)<<16 | (B)<<8 | (A) )
#    define UNPACK_RED(X)         ( ( (X) >> 24 ) & 0xff )
#    define UNPACK_GREEN(X)       ( ( (X) >> 16 ) & 0xff )
#    define UNPACK_BLUE(X)        ( ( (X) >> 8 ) & 0xff )
#    define UNPACK_ALPHA(X)       ( (X) & 0xff )
#  endif
#endif

typedef enum {
  HSLICE=0,
  CHSLICE,
  VSLICE,
  CVSLICE,
  ISOSURF,
  VOLUME
} v5d_graph_type;

typedef struct {
  GtkWidget *preview;
  /* the colors array is always of length one or 256, ncolors refers to 
	  the number of colors requested by the user not to the size of the array */
  gint ncolors;
  guint *colors;
  guchar *orig_alpha;
  guchar *buffer;
} preview_area;


typedef struct {
  int labelid;
  v5d_graph_type gtype;
  gpointer data; 
} graph_label;


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
  int label_count;
  GtkWidget *HSliceControls;

  GList *graph_label_list;

} v5d_info;

typedef struct {

  gfloat min, max, interval, level, pressure;
  gdouble color[4];
  gint linewidth;
  gint    stipplefactor;
  gushort stipple;
  gboolean onscreen;

  graph_label *label;

  GtkWidget *level_value;  /* the vertical level label */
  preview_area *sample;

} hslicecontrols;

typedef struct {
  gint v5d_data_context;
  gint varid;
  v5d_info *info;
  gint maxlevel;
  gchar vname[10];
  hslicecontrols *hc;
} v5d_var_info;



typedef enum {
  DATA_FILE,
  PROCEDURE_FILE,
  TOPO_FILE,
  MAP_FILE
} openwhatfile;


void
on_fileselect_ok                       (GtkButton       *button,
                                        gpointer         user_data);

void
on_fileselect_cancel                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_fontselectionbutton_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_ColorSelectionOk_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_ColorSelectionCancel_clicked        (GtkButton       *button,
                                        gpointer         user_data);

GtkWidget *
new_ColorSelectionDialog(void);
#endif
