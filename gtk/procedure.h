#include <gdk/gdk.h>
#include <glib.h>
#include "support_cb.h"

typedef struct {
  gchar *var;
  float min;
  float max;
  float interval;
  float level;
  GdkColor color;
} Hslice;

typedef struct {
  gchar *name;
  GPtrArray *items;
  GArray *item_type;
  GPtrArray *vinfo_array;
  /* GList *items; */
} Image;


void procedure_free_image(Image *image);
void procedure_free(GList *Procedure);
GList *procedure_add_item(GList *Procedure, gpointer item, gint itemtype, 
								  gboolean NewImage, gchar *imagename, gint position);

void
print_ProcedureList(GList *ProcedureList,gchar *filename);

GList *procedure_open_file(char *filename);
