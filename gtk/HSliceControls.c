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
#include "graph_labels.h"

extern GtkWidget *ColorSelectionDialog;
void hs_draw_color_sample(v5d_var_info *vinfo, gboolean resize);

#define COLOR_FILL(obj) GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(obj),"color_fill"))


void on_level_vscale_value_changed(GtkAdjustment *adj, gpointer user_data)
{
  float value;
  gchar val[20];
  v5d_var_info *vinfo;

  vinfo = (v5d_var_info *) user_data;
  vinfo->hc->level = adj->upper + adj->lower - adj->value ;

  if(vinfo->info->vcs==VERT_NONEQUAL_MB){
	 vis5d_gridlevel_to_pressure(vinfo->v5d_data_context,
										  vinfo->varid,vinfo->hc->level,
										  &(vinfo->hc->pressure));
	 sprintf(val,"%6g MB",vinfo->hc->pressure);

  }else if(vinfo->info->vcs== VERT_EQUAL_KM || 
			  vinfo->info->vcs==VERT_NONEQUAL_KM){  

	 vis5d_gridlevel_to_height(vinfo->v5d_data_context,0,
										vinfo->varid,vinfo->hc->level,&value);
	 sprintf(val,"%6g Km",value);
  }else {
	 vinfo->hc->level = adj->upper + adj->lower - adj->value;
	 sprintf(val,"%8g",vinfo->hc->level);
  }
  gtk_label_set_text(GTK_LABEL(vinfo->hc->level_value),val);
  
}

void hs_label(v5d_var_info *vinfo)
{
  gchar text[300];


  if(vinfo->info->vcs==VERT_NONEQUAL_MB){
	 sprintf(text,"HS: %s from %g to %g by %g at level %g MB",
				vinfo->vname,vinfo->hc->min,vinfo->hc->max, vinfo->hc->interval,
				vinfo->hc->pressure);
  }else  if(vinfo->info->vcs== VERT_EQUAL_KM || 
			  vinfo->info->vcs==VERT_NONEQUAL_KM){  
	 float value;
	 vis5d_gridlevel_to_height(vinfo->v5d_data_context,0,
										vinfo->varid,vinfo->hc->level,&value);
	 sprintf(text,"HS: %s from %g to %g by %g at level %g Km",
				vinfo->vname,vinfo->hc->min,vinfo->hc->max, vinfo->hc->interval,
				value);

  }else{
	 sprintf(text,"HS: %s from %g to %g by %g at level %g",
				vinfo->vname,vinfo->hc->min,vinfo->hc->max, vinfo->hc->interval,
				vinfo->hc->level);

  }

  if(vinfo->hc->label){
	 update_label(vinfo->info, vinfo->hc->label, text);
  }else{
	 vinfo->hc->label = add_label(vinfo->info, text, HSLICE);
	 vinfo->hc->label->data = (gpointer) vinfo;
  }
  vis5d_set_color( vinfo->info->v5d_display_context, VIS5D_LABEL, vinfo->hc->label->labelid, 
						 (float) vinfo->hc->color[0],(float) vinfo->hc->color[1],
						 (float) vinfo->hc->color[2],(float) vinfo->hc->color[3]);

  update_graph_labels(vinfo->info);


}


void
update_hslice_controls(v5d_var_info *vinfo)
{
  GtkWidget *sbutton, *colorselection, *slider, *HSliceControls;
  GtkAdjustment *adj;
  gfloat min, diff; 
  gint maxlevel;
  GList *hc_vars;

  if(! vinfo->info->HSliceControls){
	 vinfo->info->HSliceControls = create_HSliceControls();
	 gtk_object_set_data(GTK_OBJECT(HSliceControls),"color_fill",GINT_TO_POINTER(FALSE));
  }

  HSliceControls = vinfo->info->HSliceControls;

  hc_vars = (GList *) gtk_object_get_data(GTK_OBJECT(HSliceControls),"hc_vars");

  if(!hc_vars || !g_list_find(hc_vars, (gpointer) vinfo)){
	 hc_vars = g_list_append(hc_vars, (gpointer) vinfo);
  }

  gtk_object_set_data(GTK_OBJECT(HSliceControls),"hc_vars",(gpointer) hc_vars);

  if(! vinfo->hc){
	 float color[4];
	 gint i;
	 GtkWidget *hs_color_sample;

	 vinfo->hc = (hslicecontrols *) g_malloc(sizeof(hslicecontrols));
	 vinfo->hc->sample_buf=NULL;
	 hs_color_sample = lookup_widget(HSliceControls,"hs_color_sample");
	 vinfo->hc->sample_gc = gdk_gc_new(hs_color_sample->window);

	 vinfo->hc->onscreen = TRUE;
	 vinfo->hc->label = NULL;

	 vis5d_set_hslice(vinfo->v5d_data_context,vinfo->varid,0,0,0,
							vinfo->hc->level);

	 vis5d_get_hslice(vinfo->v5d_data_context,vinfo->varid,
							&(vinfo->hc->interval),&(vinfo->hc->min),
							&(vinfo->hc->max), &(vinfo->hc->level));

	 

	 /* TODO: Need to deal with other vertical grids */  

	 vis5d_gridlevel_to_pressure(vinfo->v5d_data_context,
										  vinfo->varid,vinfo->hc->level,
										  &(vinfo->hc->pressure));

	 vis5d_get_color( vinfo->v5d_data_context, VIS5D_HSLICE, vinfo->varid,
							color,
							color+1,
					      color+2,
							color+3);

	 for(i=0;i<4;i++){
		vinfo->hc->color[i] = (gdouble) color[i];
	 }

	 hs_draw_color_sample(vinfo, TRUE);
  }


  if(! GTK_WIDGET_VISIBLE(HSliceControls))
	 gtk_widget_show(HSliceControls);
  if(! GTK_WIDGET_SENSITIVE(HSliceControls))
	 gtk_widget_set_sensitive(HSliceControls,TRUE);
 
  sbutton = lookup_widget(HSliceControls,"hsmin");
  adj = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(sbutton));
  if(adj->lower > vinfo->hc->min)
	 adj->lower = vinfo->hc->min ;
  if(adj->upper < vinfo->hc->max)
	 adj->upper = vinfo->hc->max ;
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbutton),vinfo->hc->min);
  gtk_signal_emit_by_name (GTK_OBJECT (adj), "changed");


  sbutton = lookup_widget(HSliceControls,"hsmax");
  adj = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(sbutton));
  if(adj->lower > vinfo->hc->min)
	 adj->lower = vinfo->hc->min ;
  if(adj->upper < vinfo->hc->max)
	 adj->upper = vinfo->hc->max ;
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbutton),vinfo->hc->max);
  gtk_signal_emit_by_name (GTK_OBJECT (adj), "changed");

  /* interval slider */
  slider = lookup_widget(HSliceControls,"interval_hscale");

  if(COLOR_FILL(HSliceControls)){
	 gtk_widget_set_sensitive(slider,FALSE);
  }else{
	 gtk_widget_set_sensitive(slider,TRUE);

	 diff = vinfo->hc->max - vinfo->hc->min;
	 min = 0.01*diff;

	 adj = gtk_range_get_adjustment(GTK_RANGE(slider));

	 adj->lower = min;
	 adj->upper = diff;
	 adj->step_increment=1.0;
	 adj->page_increment=1.0;
	 gtk_adjustment_set_value(adj,vinfo->hc->interval);

	 gtk_range_set_adjustment(GTK_RANGE(slider),adj);

	 gtk_signal_emit_by_name (GTK_OBJECT (adj), "changed");
	 gtk_signal_emit_by_name (GTK_OBJECT (adj), "value_changed");
  }

  /* level slider */
  slider = lookup_widget(HSliceControls,"level_vscale");
  
  maxlevel = vis5d_get_levels( vinfo->v5d_data_context, vinfo->varid );
  vinfo->hc->level_value = lookup_widget(HSliceControls,"level_value");
  adj = gtk_range_get_adjustment(GTK_RANGE(slider));

  adj->upper = maxlevel;
  adj->step_increment=1.0;
  adj->page_increment=1.0;
  gtk_adjustment_set_value(adj,maxlevel - vinfo->hc->level );

  gtk_range_set_adjustment(GTK_RANGE(slider),adj);

  if(maxlevel<=1){
	 gtk_widget_set_sensitive(GTK_WIDGET(slider),FALSE);
	 
  }else{
	 gtk_widget_set_sensitive(GTK_WIDGET(slider),TRUE);
  
	 gtk_signal_connect (GTK_OBJECT(adj),"value_changed", 
							 GTK_SIGNAL_FUNC (on_level_vscale_value_changed),(gpointer) vinfo);

	 
  }
  gtk_signal_emit_by_name (GTK_OBJECT (adj), "changed");
  gtk_signal_emit_by_name (GTK_OBJECT (adj), "value_changed");

  /*
  colorselection = lookup_widget(HSliceControls,"colorselection1");
  if(colorselection){
	 gtk_color_selection_set_opacity(GTK_COLOR_SELECTION(colorselection),TRUE);
	 gtk_color_selection_set_color(GTK_COLOR_SELECTION(colorselection),vinfo->hc->color);
  }
  */

  if(vinfo->hc->label){
	 vinfo->info->label_count--;  
	 vis5d_delete_label(vinfo->info->v5d_display_context, vinfo->hc->label->labelid);
  }

  hs_label(vinfo);

}




void
on_hslicecontrolsclose_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *HSliceControl; 
  HSliceControl = GTK_WIDGET(user_data);
  gtk_widget_hide(HSliceControl);
}

gboolean
on_level_vscale_button_release_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
  GList *hc_vars;
  int time;
  GtkWidget *HSliceControls;
  v5d_var_info *vinfo;
  HSliceControls = lookup_widget(widget,"HSliceControls");
  
  hc_vars = (GList *)  gtk_object_get_data(GTK_OBJECT(HSliceControls),"hc_vars");

  while(hc_vars!=NULL){
	 vinfo = (v5d_var_info *) hc_vars->data;

	 vis5d_set_hslice(vinfo->v5d_data_context,vinfo->varid,0,
							0,0,vinfo->hc->level);
	 
	 vis5d_get_hslice(vinfo->v5d_data_context,vinfo->varid,
							&(vinfo->hc->interval),&(vinfo->hc->min),
							&(vinfo->hc->max), &(vinfo->hc->level));

	 update_hslice_controls(vinfo);
	 if(COLOR_FILL(HSliceControls)){
		vis5d_set_chslice(vinfo->v5d_data_context,vinfo->varid,vinfo->hc->level);
		for(time=0;time<vinfo->info->numtimes;time++){
		  vis5d_make_chslice( vinfo->v5d_data_context, time, vinfo->varid, time==vinfo->info->timestep);
		}
	 }else{
		for(time=0;time<vinfo->info->numtimes;time++){
		  vis5d_make_hslice( vinfo->v5d_data_context, time, vinfo->varid, time==vinfo->info->timestep);
		}
	 }
	 
	 glarea_draw(vinfo->info->GtkGlArea, NULL, NULL);

	 hc_vars = g_list_next(hc_vars);
  }	 

  return FALSE;
}


gboolean
on_interval_hscale_button_changed      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
  GtkAdjustment *interval;
  GtkWidget *HSliceControl;
  GList *hc_vars;
  v5d_var_info *vinfo;
  int time;

  interval = gtk_range_get_adjustment (GTK_RANGE(widget));

  HSliceControl = lookup_widget(widget,"HSliceControls");

  hc_vars = (GList *) gtk_object_get_data(GTK_OBJECT(HSliceControl),"hc_vars");

  while(hc_vars!=NULL){
	 vinfo = (v5d_var_info *) hc_vars->data;
	 vinfo->hc->interval = interval->value;

	 vis5d_set_hslice(vinfo->v5d_data_context,vinfo->varid,vinfo->hc->interval,
							vinfo->hc->min,vinfo->hc->max,vinfo->hc->level);

	 update_hslice_controls(vinfo);

	 for(time=0;time<vinfo->info->numtimes;time++){
		vis5d_make_hslice( vinfo->v5d_data_context, time, vinfo->varid, time==vinfo->info->timestep);
	 }

	 glarea_draw(vinfo->info->GtkGlArea, NULL, NULL);

	 hc_vars = g_list_next(hc_vars);

  }


  return FALSE;
}


void
on_hsspin_changed                      (GtkEditable     *editable,
                                        gpointer         user_data)
{
  gint what;
  GtkWidget *HSliceControl;
  gfloat value;
  GList *hc_vars;
  HSliceControl = lookup_widget(GTK_WIDGET(editable),"HSliceControls");

  what = GPOINTER_TO_INT(user_data);

  hc_vars = (GList *) gtk_object_get_data(GTK_OBJECT(HSliceControl),"hc_vars");
  value = gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(editable));
  while(hc_vars!=NULL){
	 gint time;
	 v5d_var_info *vinfo = (v5d_var_info *) hc_vars->data;
	 if(what==0){
		vinfo->hc->min = value;
	 }else{
		vinfo->hc->max = value;
	 }
	 vis5d_set_hslice(vinfo->v5d_data_context,vinfo->varid,vinfo->hc->interval,
							vinfo->hc->min,vinfo->hc->max,vinfo->hc->level);

	 if(COLOR_FILL(HSliceControl)){
		vis5d_set_chslice(vinfo->v5d_data_context,vinfo->varid,vinfo->hc->level);
		for(time=0;time<vinfo->info->numtimes;time++){
		  vis5d_make_chslice( vinfo->v5d_data_context, time, vinfo->varid, time==vinfo->info->timestep);
		}
	 }else{
		for(time=0;time<vinfo->info->numtimes;time++){
		  vis5d_make_hslice( vinfo->v5d_data_context, time, vinfo->varid, time==vinfo->info->timestep);
		}
	 }

	 hs_label(vinfo);
	 glarea_draw(vinfo->info->GtkGlArea, NULL, NULL);

	 hc_vars = g_list_next(hc_vars);
  }

}

void
on_hslicectree_tree_select_row         (GtkCTree        *ctree,
                                        GList           *node,
                                        gint             column,
                                        gpointer         user_data)
{

  v5d_var_info *vinfo;
  
  gint time;

  vinfo = (v5d_var_info *) gtk_ctree_node_get_row_data(ctree,GTK_CTREE_NODE(node));
  
  update_hslice_controls(vinfo);
  
  if(COLOR_FILL(vinfo->info->HSliceControls)){
	 for(time=0;time<vinfo->info->numtimes;time++){
		vis5d_make_chslice( vinfo->v5d_data_context, time, vinfo->varid, time==vinfo->info->timestep);
	 }
	 vis5d_enable_graphics(vinfo->v5d_data_context, VIS5D_CHSLICE,
								  vinfo->varid, VIS5D_ON);
  }else{
	 for(time=0;time<vinfo->info->numtimes;time++){
		vis5d_make_hslice( vinfo->v5d_data_context, time, vinfo->varid, time==vinfo->info->timestep);
	 }
	 vis5d_enable_graphics(vinfo->v5d_data_context, VIS5D_HSLICE,
								  vinfo->varid, VIS5D_ON);
  }


  glarea_draw(vinfo->info->GtkGlArea, NULL, NULL);
  
}

/* This does not delete the contour plot, rather it just takes it out of the
	controller */

void
on_hslicectree_tree_unselect_row       (GtkCTree        *ctree,
                                        GList           *node,
                                        gint             column,
                                        gpointer         user_data)
{
  
  v5d_var_info *vinfo;
  GList *hc_vars;

  vinfo = (v5d_var_info *) gtk_ctree_node_get_row_data(ctree,GTK_CTREE_NODE(node));
  hc_vars = (GList *) gtk_object_get_data(GTK_OBJECT(vinfo->info->HSliceControls),"hc_vars");
  hc_vars = g_list_remove(hc_vars,vinfo);

  gtk_object_set_data(GTK_OBJECT(vinfo->info->HSliceControls),"hc_vars", (gpointer) hc_vars);

}

void
on_hsclearall_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
  int i;
  GtkWidget *HSliceControls = GTK_WIDGET(user_data);
  
  printf("i dont do a thing\n");
}


void
on_hsclose_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{

  gtk_widget_hide(GTK_WIDGET(user_data));

}


void
on_hs_color_changed                    (GtkColorSelection *colorselection,
                                        gpointer         user_data)
{
  v5d_var_info *vinfo;
  /* this should really only do one variable - so do the first one*/

  vinfo = (v5d_var_info *) user_data;

  gtk_color_selection_get_color(GTK_COLOR_SELECTION(colorselection),vinfo->hc->color);

  vis5d_set_color( vinfo->info->v5d_display_context, VIS5D_HSLICE, vinfo->varid,
						 (float) vinfo->hc->color[0], 
						 (float) vinfo->hc->color[1], 
						 (float) vinfo->hc->color[2], 
						 (float) vinfo->hc->color[3] );

  hs_label(vinfo);

  glarea_draw(vinfo->info->GtkGlArea, NULL, NULL);

}

/* sets the HSLiceControls for the var specified in vinfo */
/* activated when button 1 is pressed over the label */
void hslicecontrol(v5d_var_info *vinfo)
{
  GtkCTree *ctree;
  GtkCTreeNode *node;
  GList *hc_vars;

  ctree = GTK_CTREE( lookup_widget(vinfo->info->HSliceControls,"hslicectree"));

  gtk_clist_unselect_all(GTK_CLIST(ctree));

  hc_vars = (GList *) gtk_object_get_data(GTK_OBJECT(vinfo->info->HSliceControls),"hc_vars");
  g_list_free(hc_vars);
  gtk_object_set_data(GTK_OBJECT(vinfo->info->HSliceControls),"hc_vars",NULL);

  node = gtk_ctree_find_by_row_data(ctree, gtk_ctree_node_nth(ctree,0), vinfo);

  gtk_ctree_select(ctree,node);
  
}

/* activated when button 2 is pressed over the label */
void hslice_toggle(v5d_var_info *vinfo)
{
  gint i, cnt=0;

  if(vinfo->hc->onscreen){
	 vinfo->hc->onscreen=FALSE;
	 if(COLOR_FILL(vinfo->info->HSliceControls)){
		vis5d_enable_graphics(vinfo->v5d_data_context, VIS5D_CHSLICE,
									 vinfo->varid, VIS5D_OFF);
	 }else{
		vis5d_enable_graphics(vinfo->v5d_data_context, VIS5D_HSLICE,
									 vinfo->varid, VIS5D_OFF);

	 }
	 vis5d_set_color( vinfo->info->v5d_display_context, VIS5D_LABEL, vinfo->hc->label->labelid, 
							0.5,0.5,0.5,0.5);
	 
	 
  }else{
	 if(COLOR_FILL(vinfo->info->HSliceControls)){
		vis5d_enable_graphics(vinfo->v5d_data_context, VIS5D_CHSLICE,
									 vinfo->varid, VIS5D_ON);
	 }else{
		vis5d_enable_graphics(vinfo->v5d_data_context, VIS5D_HSLICE,
								  vinfo->varid, VIS5D_ON);
	 }

	 vinfo->hc->onscreen=TRUE;
	 hs_label(vinfo);
  }
  
  glarea_draw(vinfo->info->GtkGlArea, NULL, NULL);
}

void
on_colorselect_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
  v5d_var_info *vinfo;
  GtkWidget *HSliceControls;
  GtkWidget *colorselection;
  GList *hc_vars;
  guint signal_handle;

  HSliceControls = lookup_widget(GTK_WIDGET(button), "HSliceControls");
  
  hc_vars = (GList *) gtk_object_get_data(GTK_OBJECT(HSliceControls),"hc_vars");

  if(! hc_vars)
	 return;

  vinfo = (v5d_var_info *) hc_vars->data;

  ColorSelectionDialog = new_ColorSelectionDialog();
  colorselection = GTK_COLOR_SELECTION_DIALOG(ColorSelectionDialog)->colorsel;
  gtk_color_selection_set_color(GTK_COLOR_SELECTION(colorselection),vinfo->hc->color);

  signal_handle = GPOINTER_TO_UINT(gtk_object_get_data(GTK_OBJECT(colorselection), "signal_handle"));
  if(signal_handle)
	 gtk_signal_disconnect(GTK_OBJECT(colorselection), signal_handle);

  signal_handle = gtk_signal_connect (GTK_OBJECT(colorselection), "color_changed",
							 GTK_SIGNAL_FUNC (on_hs_color_changed), (gpointer) vinfo);

  gtk_object_set_data(GTK_OBJECT(colorselection), "signal_handle", 
							 GUINT_TO_POINTER(signal_handle));

}


void
on_hsstyle_toggled                     (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  v5d_graph_type which;
  GtkWidget *HSliceControls;
  GList *hc_vars;
  v5d_var_info *vinfo;
  GtkWidget *hs_colortable, *hs_color;

  /* just look at the one activated */
  if(! gtk_toggle_button_get_active(togglebutton))
	 return;

  which = GPOINTER_TO_INT(user_data);
  
  HSliceControls = lookup_widget(GTK_WIDGET(togglebutton), "HSliceControls");

  hs_colortable = lookup_widget(HSliceControls,"hs_colortable_select");
  hs_color = lookup_widget(HSliceControls,"hs_colorselect");
  
  if(which==CHSLICE){
	 gtk_object_set_data(GTK_OBJECT(HSliceControls),"color_fill",GINT_TO_POINTER(TRUE));
	 gtk_widget_set_sensitive(hs_colortable,TRUE);
	 gtk_widget_set_sensitive(hs_color,FALSE);
  }else{
	 gtk_object_set_data(GTK_OBJECT(HSliceControls),"color_fill",GINT_TO_POINTER(FALSE));
	 gtk_widget_set_sensitive(hs_colortable,FALSE);
	 gtk_widget_set_sensitive(hs_color,TRUE);
  }
	 
}

void
on_colortable_select_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{

}


void hs_draw_color_sample(v5d_var_info *vinfo, gboolean resize)
{
  gint x, y, i, wid, heig, f, half, n;
  guchar c[3 * 2], cc[3 * 4], *cp = c;
  gdouble o, oldo;
  GtkWidget *sample_area;

  sample_area = lookup_widget(vinfo->info->HSliceControls,"hs_color_sample");
  if (! GTK_WIDGET_DRAWABLE (GTK_WIDGET (sample_area)))
	 return;

  wid = sample_area->allocation.width;
  heig = sample_area->allocation.height;
  half = wid >> 1;

  if (resize)
    {
      if (vinfo->hc->sample_buf != NULL)
		  g_free (vinfo->hc->sample_buf);
		
      vinfo->hc->sample_buf = g_new(guchar, 3 * wid);
    }

  i = 0;
  for (n = 0; n < 3; n++)
    {
      c[n] = (guchar) (255.0 * vinfo->hc->color[i]);
      c[n + 3] = (guchar) (255.0 * vinfo->hc->color[i++]);
    }
  printf("color %d %d %d %d %d\n",c[3],c[4],c[5],wid,heig);

  /* TODO - handle opacity
  if (colorsel->use_opacity)
    {
      o = colorsel->values[OPACITY];
      oldo = colorsel->old_values[OPACITY];
		
      for (n = 0; n < 3; n++)
		  {
			 cc[n] = (guchar) ((1.0 - oldo) * 192 + (oldo * (gdouble) c[n]));
			 cc[n + 3] = (guchar) ((1.0 - oldo) * 128 + (oldo * (gdouble) c[n]));
			 cc[n + 6] = (guchar) ((1.0 - o) * 192 + (o * (gdouble) c[n + 3]));
			 cc[n + 9] = (guchar) ((1.0 - o) * 128 + (o * (gdouble) c[n + 3]));
		  }
      cp = cc;
    }
  */

  for (y = 0; y < heig; y++)
    {
      i = 0;
      for (x = 0; x < wid; x++)
		  {
			 /*
			 if (colorsel->use_opacity)
				{
				  f = 3 * (((x % 32) < 16) ^ ((y % 32) < 16));
				  f += (x > half) * 6;
				}
			 else
			 */
				f = (x > half) * 3;
			 
			 for (n = 0; n < 3; n++)
				vinfo->hc->sample_buf[i++] = cp[n + f];
		  }
		
      gtk_preview_draw_row (GTK_PREVIEW (sample_area), vinfo->hc->sample_buf, 0, y, wid);
    }
  gtk_widget_queue_draw (sample_area);
}
