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
  /* GList *items; */
} Image;


void procedure_free(GList *Procedure);
GList *procedure_add_item(GList *Procedure, gpointer item, gint itemtype, gboolean NewImage, gchar *imagename);

int print_ProcedureList(GList *ProcedureList,gchar *filename);

GList *procedure_open_file(char *filename);
