#ifndef __V5D_CALLBACKS__
#define __V5D_CALLBACKS__ 1

#include <gtk/gtk.h>

/* from ../src/graphics.h */
#ifndef WORDS_BIGENDIAN
#    define PACK_COLOR(R,G,B,A)   ( (A)<<24 | (B)<<16 | (G)<<8 | (R) )
#    define UNPACK_RED(X)         ( (X) & 0xff )
#    define UNPACK_GREEN(X)       ( ( (X) >> 8 ) & 0xff )
#    define UNPACK_BLUE(X)        ( ( (X) >> 16 ) & 0xff )
#    define UNPACK_ALPHA(X)       ( ( (X) >> 24 ) & 0xff )
#else
#    define PACK_COLOR(R,G,B,A)   ( (R)<<24 | (G)<<16 | (B)<<8 | (A) )
#    define UNPACK_RED(X)         ( ( (X) >> 24 ) & 0xff )
#    define UNPACK_GREEN(X)       ( ( (X) >> 16 ) & 0xff )
#    define UNPACK_BLUE(X)        ( ( (X) >> 8 ) & 0xff )
#    define UNPACK_ALPHA(X)       ( (X) & 0xff )
#endif

typedef enum {
  HSLICE=0,
  CHSLICE,
  VSLICE,
  CVSLICE,
  ISOSURF,
  VOLUME,
  TEXTPLOT
} v5d_graph_type;



typedef struct {
  GtkWidget *preview;
  /* the colors array is always of length one or 256, ncolors refers to 
	  the number of colors requested by the user not to the size of the array */
  gchar *name;
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
  gchar *var;
  gfloat spacing, fontx, fonty, fontspace;
  gfloat color[4];
  graph_label *label;
} textplotcontrols;

typedef struct {
  gint v5d_display_context;

  gint beginx,beginy;  /* position of mouse */

  float zoom;           /* field of view in degrees */

  int numtimes;

  GtkWidget *GtkGlArea;
  int animate;
  guint32 animate_speed;
  gint timeout_id;
  gint stepsize;
  int timestep;
  int vcs;
  int label_count;
  GPtrArray *vinfo_array;
  GList *graph_label_list;
  GtkWidget *ProcedureDialog;
} v5d_info;

typedef struct {
  gfloat min, max, interval, level, height, pressure;
  gdouble color[4];
  gint linewidth;
  gint    stipplefactor;
  gushort stipple;
  gboolean onscreen;
  graph_label *label;
  preview_area *sample;
  guint alpha_adjust;
  GtkWidget *level_value;  /* the vertical level label */
  gchar *var;  /* should be a reference used for procedure saving */
} hslicecontrols;

typedef struct {
  gfloat min, max, interval;
  gfloat r0, r1, c0, c1;
  gdouble color[4];
  gint linewidth;
  gint    stipplefactor;
  gushort stipple;
  gboolean onscreen;
  gchar *colortable;
  graph_label *label;
  preview_area *sample;
  guint alpha_adjust;
  gchar *var;  /* should be a reference used for procedure saving */
} vslicecontrols;

typedef struct {
  v5d_graph_type type;
  gpointer data;
  
} controls;




typedef struct {
  gint v5d_data_context;
  gint varid;
  v5d_info *info;
  gint maxlevel;
  gchar vname[10];
  
  GList *ControlList;

  hslicecontrols *hs, *chs;
  vslicecontrols *vs, *cvs;
  textplotcontrols *textplot;
  int numtimes;

  GtkWidget *VarGraphicsDialog;
} v5d_var_info;



typedef enum {
  DATA_FILE,
  PROCEDURE_FILE,
  TOPO_FILE,
  MAP_FILE,
  SAVE_PROCEDURE_FILE

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

void 
Invoke_VerifyDialog(gchar *label, GtkSignalFunc on_okay, gpointer okay_data,
						  GtkSignalFunc on_cancel, gpointer cancel_data);
#endif
