#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <gtkgl/gdkgl.h>
#include <gtkgl/gtkglarea.h>
#include "../src/api.h"
#include <math.h> /* for HUGE */

#include "support_cb.h"
#include "interface.h"
#include "support.h"
#include "graph_labels.h"

void label_position(int dc, int cnt, int *x, int *y)
{
  gint text_height, height, width;
  Window throwaway;

#ifdef USE_SYSTEM_FONTS
  vis5d_get_font_height(dc, &text_height, WINDOW_3D_FONT);
#else
  vis5d_get_font_height(dc, &text_height);
#endif

  vis5d_get_window(dc, &throwaway, &width, &height);


  *x = 5;
  *y = height - (text_height+1)*cnt;

}

graph_label *add_label(v5d_info *info, gchar *str, v5d_graph_type gtype)
{
  graph_label *label;
  gint cnt=1, x, y;


  if(info->graph_label_list){
	 cnt = g_list_length (info->graph_label_list )+1;
  }

  label_position(info->v5d_display_context, cnt, &x, &y);

  label = (graph_label *) g_malloc(sizeof(graph_label));

  label->gtype = gtype;

  label->data = NULL;

  label->labelid = vis5d_make_label(info->v5d_display_context, x,
												y, str);

  info->graph_label_list = g_list_append(info->graph_label_list, (gpointer) label);

  return label;
}
  
void update_graph_labels(v5d_info *info)
{
  GList *item;
  gint cnt=1, x, y;
  if(! info->graph_label_list)
	 return;
  
  item = g_list_first(info->graph_label_list);

  while(item!=NULL){
	 label_position(info->v5d_display_context, cnt, &x, &y);
	 
	 vis5d_move_label(info->v5d_display_context, ((graph_label *)item->data)->labelid, 
							x,y);
	 item = g_list_next(item);
	 cnt++;
  }


}


void delete_label(v5d_info *info, graph_label *label)
{
  
  vis5d_delete_label(info->v5d_display_context, label->labelid);
  
  info->graph_label_list = g_list_remove(info->graph_label_list, (gpointer) label);
  
  g_free(label);
  
}

void update_label(v5d_info *info, graph_label *label, gchar *str)
{
  gint cnt, x, y;



  vis5d_delete_label(info->v5d_display_context, label->labelid);
  
  cnt = g_list_index(info->graph_label_list, label) + 1;

  label_position(info->v5d_display_context, cnt, &x, &y);

  label->labelid = vis5d_make_label(info->v5d_display_context, x,
												y, str);

}

void graph_label_button_press(v5d_info *info, gint label_id, gint button)
{
  GList *item;
  graph_label *label=NULL;
  v5d_var_info *vinfo;
  GtkWidget *widget;

  item = g_list_first(info->graph_label_list);

  while(item!=NULL){
	 if(((graph_label *) item->data)->labelid==label_id){
		label = (graph_label *) item->data;
	 }
	 item = g_list_next(item);
  }

  if(label==NULL){
	 /* TODO: need an error handler*/
	 printf("couldnt find labelid %d in list\n",label_id);
	 return;
  }
  vinfo = (v5d_var_info *) label->data;
  switch(label->gtype){
  case HSLICE:
	 switch(button){
	 case 1:
	   gtk_widget_show(vinfo->VarGraphicsDialog);
		break;
	 case 2:
		/* toggles the graphics but leaves the label in the window */
		hslice_toggle(vinfo);
		break;
	 case 3:
		/* removes the graphic and label from the display */
		widget = lookup_widget(vinfo->VarGraphicsDialog,"Hslicebutton");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(widget), FALSE);
      break;
	 default:
	 }
	 break;
  case CHSLICE:
  case VSLICE:
  case CVSLICE:
  case ISOSURF:
  case VOLUME:
  default:
	 printf("Dont know what to do in graph_label_button_press\n");
  }



}
