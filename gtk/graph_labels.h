#ifndef __GRAPH_LABELS__
#define __GRAPH_LABELS__ 1
#include "support_cb.h"
void label_position(int display_context, int cnt, int *x, int *y);
graph_label *add_label(v5d_info *info, gchar *str, v5d_graph_type gtype);
void update_graph_labels(v5d_info *info);
void delete_label(v5d_info *info, graph_label *label);
void update_label(v5d_info *info, graph_label *label, gchar *str);
void graph_label_button_press(v5d_info *info, gint label_id, gint button);
#endif
