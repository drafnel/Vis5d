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

#include "support_cb.h"
#include "window3D.h"
#include "HSC_interface.h"
#include "support.h"
#include "graph_labels.h"
#include "gradient_cb.h"
#include "interface.h"

extern GtkWidget *ColorSelectionDialog;
void hs_draw_color_sample(v5d_var_info *vinfo, gboolean resize);
void delete_hslice(v5d_var_info *vinfo);

#define COLOR_FILL(obj) GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(obj),"color_fill"))
static gint last_level;


void on_level_vscale_value_changed(GtkAdjustment *adj, gpointer user_data)
{
  float value;
  gchar val[20];
  v5d_var_info *vinfo;

  vinfo = (v5d_var_info *) user_data;
  if(! (vinfo && vinfo->hc )) return;

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
  last_level = vinfo->hc->level;
}

void hs_label(v5d_var_info *vinfo)
{
  gchar text[300];


  if(COLOR_FILL(vinfo->info->HSliceControls)){
	 if(vinfo->info->vcs==VERT_NONEQUAL_MB){
		sprintf(text,"CHS: %s from %g to %g at level %g MB",
				  vinfo->vname,vinfo->hc->min,vinfo->hc->max, 
				  vinfo->hc->pressure);
	 }else  if(vinfo->info->vcs== VERT_EQUAL_KM || 
				  vinfo->info->vcs==VERT_NONEQUAL_KM){  
		float value;
		vis5d_gridlevel_to_height(vinfo->v5d_data_context,0,
										  vinfo->varid,vinfo->hc->level,&value);
		sprintf(text,"CHS: %s from %g to %g at level %g Km",
				  vinfo->vname,vinfo->hc->min,vinfo->hc->max,
				  value);
		
	 }else{
		sprintf(text,"CHS: %s from %g to %g at level %g",
				  vinfo->vname,vinfo->hc->min,vinfo->hc->max, 
				  vinfo->hc->level);
		  
	 }
  }else{
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
  }

  if(vinfo->hc->label){
	 update_label(vinfo->info, vinfo->hc->label, text);
  }else{
	 vinfo->hc->label = add_label(vinfo->info, text, HSLICE);
	 vinfo->hc->label->data = (gpointer) vinfo;
  }
  if(COLOR_FILL(vinfo->info->HSliceControls)){
	 vis5d_set_color( vinfo->info->v5d_display_context, VIS5D_LABEL, vinfo->hc->label->labelid, 
							0.8,0.8,0.8,1.0);
  }else{  
	 vis5d_set_color( vinfo->info->v5d_display_context, VIS5D_LABEL, vinfo->hc->label->labelid, 
							(float) vinfo->hc->color[0],(float) vinfo->hc->color[1],
							(float) vinfo->hc->color[2],(float) vinfo->hc->color[3]);
  }
  update_graph_labels(vinfo->info);


}


void
update_hslice_controls(v5d_var_info *vinfo)
{
  GtkWidget *sbutton, *slider, *HSliceControls;
  GtkAdjustment *adj;
  gfloat min, diff; 
  gint maxlevel;

  if(! vinfo->info->HSliceControls){
	 vinfo->info->HSliceControls = create_HSliceControls();

	 gtk_object_set_data(GTK_OBJECT(vinfo->info->HSliceControls),"color_fill",GINT_TO_POINTER(FALSE));
  }

  HSliceControls = vinfo->info->HSliceControls;
  gtk_object_set_data(GTK_OBJECT(HSliceControls),"v5d_var_info",(gpointer) vinfo);

  if(! vinfo->hc){
	 float color[4];
	 gint i;

	 vinfo->hc = g_new(hslicecontrols,1);
	 vinfo->hc->sample= g_new(preview_area,1);
	 vinfo->hc->sample->buffer=NULL;	 
	 vinfo->hc->sample->orig_alpha=NULL;
	 vinfo->hc->sample->ncolors=255;
	 vinfo->hc->onscreen = TRUE;
	 vinfo->hc->label = NULL;
	 vinfo->hc->level = last_level;
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

  if(vinfo->hc->label){
	 delete_label(vinfo->info, vinfo->hc->label);
    vinfo->hc->label=NULL;
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
  int time;
  GtkWidget *HSliceControls;
  v5d_var_info *vinfo;
  HSliceControls = lookup_widget(widget,"HSliceControls");
  
  vinfo = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(HSliceControls),"v5d_var_info");

  if(!vinfo)
	 return FALSE;

  if(COLOR_FILL(HSliceControls)){
	 vis5d_set_chslice_limits(vinfo->v5d_data_context,vinfo->varid, 0, 0, 
									  vinfo->hc->level);
	 vis5d_get_chslice_limits(vinfo->v5d_data_context,vinfo->varid,
							&(vinfo->hc->min),&(vinfo->hc->max), &(vinfo->hc->level));

	 for(time=0;time<vinfo->info->numtimes;time++){
		vis5d_make_chslice( vinfo->v5d_data_context, time, vinfo->varid, time==vinfo->info->timestep);
	 }
  }else{

	 vis5d_set_hslice(vinfo->v5d_data_context,vinfo->varid,0,
							0,0,vinfo->hc->level);
	 
	 vis5d_get_hslice(vinfo->v5d_data_context,vinfo->varid,
							&(vinfo->hc->interval),&(vinfo->hc->min),
							&(vinfo->hc->max), &(vinfo->hc->level));

	 for(time=0;time<vinfo->info->numtimes;time++){
		vis5d_make_hslice( vinfo->v5d_data_context, time, vinfo->varid, time==vinfo->info->timestep);
	 }
  }
  update_hslice_controls(vinfo);
	 
  glarea_draw(vinfo->info->GtkGlArea, NULL, NULL);


  return FALSE;
}


gboolean
on_interval_hscale_button_changed      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
  GtkAdjustment *interval;
  GtkWidget *HSliceControls;
  v5d_var_info *vinfo;
  int time;

  interval = gtk_range_get_adjustment (GTK_RANGE(widget));

  HSliceControls = lookup_widget(widget,"HSliceControls");


  vinfo = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(HSliceControls),"v5d_var_info");

  vinfo->hc->interval = interval->value;

  vis5d_set_hslice(vinfo->v5d_data_context,vinfo->varid,vinfo->hc->interval,
						 vinfo->hc->min,vinfo->hc->max,vinfo->hc->level);

  update_hslice_controls(vinfo);

  for(time=0;time<vinfo->info->numtimes;time++){
	 vis5d_make_hslice( vinfo->v5d_data_context, time, vinfo->varid, time==vinfo->info->timestep);
  }

  glarea_draw(vinfo->info->GtkGlArea, NULL, NULL);

  return FALSE;
}


void
on_hsspin_changed                      (GtkEditable     *editable,
                                        gpointer         user_data)
{
  gint what;
  GtkWidget *HSliceControls;
  gfloat value;
  gint time;
  v5d_var_info *vinfo;

  HSliceControls = lookup_widget(GTK_WIDGET(editable),"HSliceControls");

  what = GPOINTER_TO_INT(user_data);

  vinfo = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(HSliceControls),"v5d_var_info");

  if(! vinfo)
	 return;

  value = gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(editable));

  if(what==0){
	 vinfo->hc->min = value;
  }else{
	 vinfo->hc->max = value;
  }

  if(COLOR_FILL(HSliceControls)){
	 vis5d_set_chslice_limits(vinfo->v5d_data_context,vinfo->varid, 
									  vinfo->hc->min,vinfo->hc->max,vinfo->hc->level);
	 for(time=0;time<vinfo->info->numtimes;time++){
		vis5d_invalidate_chslice(vinfo->v5d_data_context, vinfo->varid, time);
		vis5d_make_chslice( vinfo->v5d_data_context, time, vinfo->varid, time==vinfo->info->timestep);
	 }
  }else{
	 vis5d_set_hslice(vinfo->v5d_data_context,vinfo->varid,vinfo->hc->interval,
						 vinfo->hc->min,vinfo->hc->max,vinfo->hc->level);
	 for(time=0;time<vinfo->info->numtimes;time++){
		vis5d_make_hslice( vinfo->v5d_data_context, time, vinfo->varid, time==vinfo->info->timestep);
	 }
  }

  hs_label(vinfo);
  glarea_draw(vinfo->info->GtkGlArea, NULL, NULL);

}

void
on_hslicectree_tree_select_row         (GtkCTree        *ctree,
                                        GList           *node,
                                        gint             column,
                                        gpointer         user_data)
{

  v5d_var_info *vinfo;
  
  gint time;

  if(user_data)
	 vinfo = (v5d_var_info *) user_data;
  else
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
  

}

void
on_hsclear_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{

  GtkWidget *HSliceControls = GTK_WIDGET(user_data);
  v5d_var_info *vinfo;

  vinfo = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(HSliceControls),"v5d_var_info");
  
  delete_hslice(vinfo)  ;
  
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

  ctree = GTK_CTREE( lookup_widget(vinfo->info->HSliceControls,"hslicectree"));

  gtk_clist_unselect_all(GTK_CLIST(ctree));

  node = gtk_ctree_find_by_row_data(ctree, gtk_ctree_node_nth(ctree,0), vinfo);

  gtk_ctree_select(ctree,node);
  
}

void delete_hslice(v5d_var_info *vinfo)
{
  
  if(COLOR_FILL(vinfo->info->HSliceControls)){
	 vis5d_enable_graphics(vinfo->v5d_data_context, VIS5D_CHSLICE,
								  vinfo->varid, VIS5D_OFF);
  }else{
	 vis5d_enable_graphics(vinfo->v5d_data_context, VIS5D_HSLICE,
								  vinfo->varid, VIS5D_OFF);
	 
  }  
  delete_label(vinfo->info,vinfo->hc->label);
  
  g_free (vinfo->hc->sample->buffer);
  g_free (vinfo->hc->sample);
  g_free (vinfo->hc);
  vinfo->hc=NULL;
  
}


/* activated when button 2 is pressed over the label */
void hslice_toggle(v5d_var_info *vinfo)
{

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
  guint signal_handle;

  HSliceControls = lookup_widget(GTK_WIDGET(button), "HSliceControls");
  
  vinfo = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(HSliceControls),"v5d_var_info");


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
on_notebook1_switch_page               (GtkNotebook     *notebook,
                                        GtkNotebookPage *page,
                                        gint             page_num,
                                        gpointer         user_data)
{
  GtkWidget *HSliceControls;

  v5d_var_info *vinfo;

  HSliceControls = GTK_WIDGET(user_data); 

  vinfo = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(HSliceControls),"v5d_var_info");
  /* TODO: How to get both hs and chs? */
  if(vinfo)
	 delete_hslice(vinfo);

  if(page_num==CHSLICE){    
	 gtk_object_set_data(GTK_OBJECT(HSliceControls),"color_fill",GINT_TO_POINTER(TRUE));
  }else{
	 gtk_object_set_data(GTK_OBJECT(HSliceControls),"color_fill",GINT_TO_POINTER(FALSE));
  }
  if(vinfo)
	 on_hslicectree_tree_select_row         (NULL,NULL,0,(gpointer) vinfo);

}

void
on_colortable_select_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *Gradient, *HSliceControls;
  v5d_var_info *vinfo;

  HSliceControls = GTK_WIDGET(user_data);
  vinfo = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(HSliceControls),"v5d_var_info");

  Gradient = new_GtkGradient();

  gtk_object_set_data(GTK_OBJECT(Gradient), "Sample", (gpointer) vinfo->hc->sample);  
  gtk_object_set_data(GTK_OBJECT(Gradient), "GtkGlArea", (gpointer) vinfo->info->GtkGlArea);  
  
}


void hs_draw_color_sample(v5d_var_info *vinfo, gboolean resize)
{
  static guint color;

  vinfo->hc->sample->preview = lookup_widget(vinfo->info->HSliceControls,"hs_color_sample");


  if( COLOR_FILL(vinfo->info->HSliceControls)){
	 vis5d_get_color_table_address(vinfo->info->v5d_display_context, VIS5D_CHSLICE, vinfo->v5d_data_context,
											 vinfo->varid, &vinfo->hc->sample->colors);
	 vinfo->hc->sample->ncolors = 255;
  }else{
	 vinfo->hc->sample->ncolors = 1;
	 color = PACK_COLOR((guint) (255*vinfo->hc->color[0]), (guint) (255*vinfo->hc->color[1]), 
							  (guint) (255*vinfo->hc->color[2]),(guint)  (255*vinfo->hc->color[3])); 
	 /* this may be a bug since color will go away*/
	 vinfo->hc->sample->colors = &color;
	 vinfo->hc->sample->ncolors = 1;
  }
  gradient_preview_update(vinfo->hc->sample,resize);
}


gboolean
on_alpha_button_release_event          (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
  GtkWidget *HSliceControls;
  GtkAdjustment *adj;
  guchar *alpha;
  int i;
  v5d_var_info *vinfo;

  HSliceControls = GTK_WIDGET(user_data);

  vinfo = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(HSliceControls),"v5d_var_info");
  
  adj = gtk_range_get_adjustment(GTK_RANGE(widget)); 
  alpha = (guchar *) vinfo->hc->sample->colors;
  if(vinfo->hc->sample->orig_alpha==NULL) 
	 {
		int i;
		vinfo->hc->sample->orig_alpha = g_new(guchar, 255);
		for(i=0;i<255;i++)
		  vinfo->hc->sample->orig_alpha[i] = (guchar) UNPACK_ALPHA(vinfo->hc->sample->colors[i]);
	 }
  
#ifdef WORDS_BIGENDIAN
  for(i=0;i<255;i++)
	 alpha[i*4] = (guchar) vinfo->hc->sample->orig_alpha[i]*adj->value;
#else  
  for(i=0;i<255;i++)
	 alpha[i*4+3] = (guchar) vinfo->hc->sample->orig_alpha[i]*adj->value;
#endif
  glarea_draw(vinfo->info->GtkGlArea,NULL,NULL);

  return FALSE;
}


