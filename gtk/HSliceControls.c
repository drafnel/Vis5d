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

#include "callbacks.h"
#include "interface.h"
#include "support.h"

gchar *sbnames[]={"hsmin","hsmax","hsint","hslevel"};

void
update_hslice_controls(GtkWidget *HSliceControls, float spinvals[5], gdouble color[4])
{
  GtkWidget *sbutton, *colorselection;
  gint i;

  if(! GTK_WIDGET_VISIBLE(HSliceControls))
	 gtk_widget_show(HSliceControls);
  if(! GTK_WIDGET_SENSITIVE(HSliceControls))
	 gtk_widget_set_sensitive(HSliceControls,TRUE);

  for(i=0;i<4;i++){
	 sbutton = lookup_widget(HSliceControls,sbnames[i]);
	 /*
	 adj = GTK_ADJUSTMENT(gtk_adjustment_new(spinvals[i],
														  -HUGE,HUGE,1.,
														  1.,1.));
	 gtk_spin_button_set_adjustment(sbutton,adj);
	 gtk_spin_button_update(sbutton);
	 */
	 gtk_spin_button_set_value(sbutton,spinvals[i]);
  }
  
  colorselection = lookup_widget(HSliceControls,"colorselection1");
  if(colorselection){
	 printf("setting color 0x%x\n",colorselection);
	 gtk_color_selection_set_color(GTK_COLOR_SELECTION(colorselection),color);
  }

}



void
on_hslicecontrolsapply_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *sbutton, *colorselection;
  gint i;
  gdouble color[4];
  gfloat spinvals[4];
  GtkWidget *HSliceControls;
  v5d_var_info *vinfo;
  int time, numtimes, curtime;

  HSliceControls = lookup_widget(GTK_WIDGET(button),"HSliceControls");
  if(! HSliceControls) return;

  vinfo = (v5d_var_info *) gtk_object_get_data(HSliceControls,"v5d_var_info");
  if(! vinfo) return;
  
  colorselection = lookup_widget(HSliceControls,"colorselection1");

  printf("getting color 0x%x\n",colorselection);
  gtk_color_selection_get_color(GTK_COLOR_SELECTION(colorselection),color);

  for(i=0;i<4;i++){
	 sbutton = lookup_widget(HSliceControls,sbnames[i]);
	 spinvals[i] = gtk_spin_button_get_value_as_float(sbutton);
  }
  
  vis5d_set_hslice(vinfo->v5d_data_context,vinfo->varid,spinvals[2],
						 spinvals[0],spinvals[1],spinvals[3]);
  
  vis5d_set_color(vinfo->info->v5d_display_context ,  VIS5D_HSLICE, 
						vinfo->varid, color[0], color[1], color[2], color[3]);

  vis5d_get_ctx_numtimes( vinfo->v5d_data_context, &numtimes );
  vis5d_get_ctx_timestep( vinfo->v5d_data_context,  &curtime);
  for(time=0;time<numtimes;time++){
	 vis5d_invalidate_hslice(vinfo->v5d_data_context,vinfo->varid,time);
	 vis5d_make_hslice( vinfo->v5d_data_context, time, vinfo->varid, time==curtime);
  }
  vis5d_signal_redraw(vinfo->info->v5d_display_context,1);
}


void
on_hslicecontrolsclose_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{

}

