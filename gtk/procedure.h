#include <gdk/gdk.h>
#include <glib.h>

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
