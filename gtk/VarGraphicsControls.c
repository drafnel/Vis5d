#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdio.h>

#include "api.h"
#include "support_cb.h"
#include "window3D.h"
#include "VarGraphicsControls.h"
#include "VGC_interface.h"
#include "support.h"
#include "graph_labels.h"
#include "gradient_cb.h"

extern GtkWidget *ColorSelectionDialog;
GtkWidget *current_sample_label=NULL;
static gint last_level;
void hs_label(v5d_var_info *vinfo);
void chs_label(v5d_var_info *vinfo);


gboolean
on_interval_hscale_button_changed      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{

  return FALSE;
}








void
on_colortable_select_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *Gradient, *VGC;
  v5d_var_info *vinfo;
  gint type=GPOINTER_TO_INT(user_data);

  
  VGC = lookup_widget(GTK_WIDGET(button), "VarGraphicsControls");
  vinfo = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(VGC),"v5d_var_info");
  
  Gradient = new_GtkGradient();
  if(type==CHSLICE)
	 gtk_object_set_data(GTK_OBJECT(Gradient), "Sample", (gpointer) vinfo->chs->sample);  

  gtk_object_set_data(GTK_OBJECT(Gradient), "GtkGlArea", (gpointer) vinfo->info->GtkGlArea);  
  
}

gboolean
on_alpha_button_release_event          (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
  GtkWidget *VGD;
  GtkAdjustment *adj;
  guchar *alpha;
  int i, time;
  v5d_var_info *vinfo;

  VGD = GTK_WIDGET(user_data);

  vinfo = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(VGD),"v5d_var_info");
  
  adj = gtk_range_get_adjustment(GTK_RANGE(widget)); 
  alpha = (guchar *) vinfo->chs->sample->colors;
  if(vinfo->chs->sample->orig_alpha==NULL) 
	 {
		int i;
		vinfo->chs->sample->orig_alpha = g_new(guchar, 255);
		for(i=0;i<255;i++)
		  vinfo->chs->sample->orig_alpha[i] = (guchar) UNPACK_ALPHA(vinfo->chs->sample->colors[i]);
	 }
  
#ifdef WORDS_BIGENDIAN
  for(i=0;i<255;i++)
	 alpha[i*4] = (guchar) vinfo->chs->sample->orig_alpha[i]*adj->value;
#else  
  for(i=0;i<255;i++)
	 alpha[i*4+3] = (guchar) vinfo->chs->sample->orig_alpha[i]*adj->value;
#endif
#ifdef USE_GLLISTS
  for(time=0;time<vinfo->info->numtimes;time++){
	 vis5d_invalidate_chslice(vinfo->v5d_data_context , vinfo->varid, time);
	 vis5d_make_chslice( vinfo->v5d_data_context, time, vinfo->varid, time==vinfo->info->timestep);
  }
#endif
  glarea_draw(vinfo->info->GtkGlArea,NULL,NULL);

  return FALSE;
}


void
on_hsclear_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_hsclose_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_hs_color_changed                    (GtkColorSelection *colorselection,
                                        gpointer         user_data)
{
  v5d_var_info *vinfo;

  vinfo = (v5d_var_info *) user_data;

  gtk_color_selection_get_color(GTK_COLOR_SELECTION(colorselection),vinfo->hs->color);

  vis5d_set_color( vinfo->info->v5d_display_context, VIS5D_HSLICE, vinfo->varid,
						 (float) vinfo->hs->color[0], 
						 (float) vinfo->hs->color[1], 
						 (float) vinfo->hs->color[2], 
						 (float) vinfo->hs->color[3] );

  hs_label(vinfo);

  glarea_draw(vinfo->info->GtkGlArea, NULL, NULL);

}


void
on_colorselect_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
  v5d_var_info *vinfo;
  GtkWidget *VarGraphicsControls;
  GtkWidget *colorselection;
  guint signal_handle;


  VarGraphicsControls = lookup_widget(GTK_WIDGET(button), "VarGraphicsControls");
  
  vinfo = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(VarGraphicsControls),"v5d_var_info");


  ColorSelectionDialog = new_ColorSelectionDialog();
  colorselection = GTK_COLOR_SELECTION_DIALOG(ColorSelectionDialog)->colorsel;
  signal_handle = GPOINTER_TO_UINT(gtk_object_get_data(GTK_OBJECT(colorselection), "signal_handle"));
  if(signal_handle)
	 gtk_signal_disconnect(GTK_OBJECT(colorselection), signal_handle);
  switch (GPOINTER_TO_INT(user_data)){
  case HSLICE:
	 gtk_color_selection_set_color(GTK_COLOR_SELECTION(colorselection),vinfo->hs->color);
	 signal_handle = gtk_signal_connect (GTK_OBJECT(colorselection), "color_changed",
													 GTK_SIGNAL_FUNC (on_hs_color_changed), (gpointer) vinfo);
	 break;
  case VSLICE:
  case ISOSURF:
	 printf("need to hook it up\n");
	 break;
  default:
	 break;
  }

  gtk_object_set_data(GTK_OBJECT(colorselection), "signal_handle", 
							 GUINT_TO_POINTER(signal_handle));

}


void
on_hsspin_changed                      (GtkEditable     *editable,
                                        gpointer         user_data)
{
  gint what;
  GtkWidget *VGD;
  gfloat value;
  gint time;
  v5d_var_info *vinfo;
  hslicecontrols *hsc;
  enum{HSMIN=0, HSMAX, HSINT, CHSMIN, CHSMAX} ;

  VGD = lookup_widget(GTK_WIDGET(editable),"VarGraphicsControls");

  what = GPOINTER_TO_INT(user_data);

  vinfo = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(VGD),"v5d_var_info");

  if(! vinfo)
	 return;

  value = gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(editable));

  if(what>=CHSMIN)
	 hsc = vinfo->chs;
  else
	 hsc = vinfo->hs;


  if(what==HSMIN || what==CHSMIN ){
	 hsc->min = value;
  }else if(what==HSMAX || what == CHSMAX){
	 hsc->max = value;
  }else{
	 hsc->interval = value;
  }
  if(what>=CHSMIN){
	 vis5d_set_chslice_limits(vinfo->v5d_data_context,vinfo->varid,
									  hsc->min,hsc->max,hsc->level);

	 for(time=0;time<vinfo->info->numtimes;time++){
		vis5d_invalidate_chslice(vinfo->v5d_data_context , vinfo->varid, time);
		vis5d_make_chslice( vinfo->v5d_data_context, time, vinfo->varid, time==vinfo->info->timestep);
	 }
	 chs_label(vinfo);
  }else{
	 vis5d_set_hslice(vinfo->v5d_data_context,vinfo->varid,hsc->interval,
							hsc->min,hsc->max,hsc->level);
	 for(time=0;time<vinfo->info->numtimes;time++){
		vis5d_make_hslice( vinfo->v5d_data_context, time, vinfo->varid, time==vinfo->info->timestep);
	 }
	 hs_label(vinfo);
  }
  glarea_draw(vinfo->info->GtkGlArea, NULL, NULL);
}

void
on_chsspin_changed                     (GtkEditable     *editable,
                                        gpointer         user_data)
{

}


void chs_label(v5d_var_info *vinfo)
{
  gchar text[300];


  if(vinfo->info->vcs==VERT_NONEQUAL_MB){
	 sprintf(text,"CHS: %s from %g to %g at level %g MB",
				vinfo->vname,vinfo->chs->min,vinfo->chs->max,
				vinfo->chs->pressure);
  }else  if(vinfo->info->vcs== VERT_EQUAL_KM || 
				vinfo->info->vcs==VERT_NONEQUAL_KM){  
	 float value;
	 vis5d_gridlevel_to_height(vinfo->v5d_data_context,0,
										vinfo->varid,vinfo->chs->level,&value);
	 sprintf(text,"CHS: %s from %g to %g at level %g Km",
				vinfo->vname,vinfo->chs->min,vinfo->chs->max,
				value);
		
  }else{
	 sprintf(text,"CHS: %s from %g to %g at level %g",
				vinfo->vname,vinfo->chs->min,vinfo->chs->max,
				vinfo->chs->level);
	 
  }
  
  if(vinfo->chs->label){
	 update_label(vinfo->info, vinfo->chs->label, text);
  }else{
	 vinfo->chs->label = add_label(vinfo->info, text, CHSLICE);
	 vinfo->chs->label->data = (gpointer) vinfo;
  }

  update_graph_labels(vinfo->info);
}

void hs_label(v5d_var_info *vinfo)
{
  gchar text[300];


  if(vinfo->info->vcs==VERT_NONEQUAL_MB){
	 sprintf(text,"HS: %s from %g to %g by %g at level %g MB",
				vinfo->vname,vinfo->hs->min,vinfo->hs->max, vinfo->hs->interval,
				vinfo->hs->pressure);
  }else  if(vinfo->info->vcs== VERT_EQUAL_KM || 
				vinfo->info->vcs==VERT_NONEQUAL_KM){  
	 float value;
	 vis5d_gridlevel_to_height(vinfo->v5d_data_context,0,
										vinfo->varid,vinfo->hs->level,&value);
	 sprintf(text,"HS: %s from %g to %g by %g at level %g Km",
				vinfo->vname,vinfo->hs->min,vinfo->hs->max, vinfo->hs->interval,
				value);
		
  }else{
	 sprintf(text,"HS: %s from %g to %g by %g at level %g",
				vinfo->vname,vinfo->hs->min,vinfo->hs->max, vinfo->hs->interval,
				vinfo->hs->level);
	 
  }
  
  if(vinfo->hs->label){
	 update_label(vinfo->info, vinfo->hs->label, text);
  }else{
	 vinfo->hs->label = add_label(vinfo->info, text, HSLICE);
	 vinfo->hs->label->data = (gpointer) vinfo;
  }
  vis5d_set_color( vinfo->info->v5d_display_context, VIS5D_LABEL, vinfo->hs->label->labelid, 
						 (float) vinfo->hs->color[0],(float) vinfo->hs->color[1],
						 (float) vinfo->hs->color[2],(float) vinfo->hs->color[3]);
  update_graph_labels(vinfo->info);
}

void on_level_vscale_value_changed(GtkAdjustment *adj, gpointer user_data)
{
  float value;
  gchar val[20];
  v5d_var_info *vinfo;
  hslicecontrols *hs;

  vinfo = (v5d_var_info *) user_data;

  hs = (hslicecontrols *) gtk_object_get_data(GTK_OBJECT(adj),"hslice_object");

  if(! (vinfo && hs)) return;

  hs->level = adj->upper + adj->lower - adj->value ;

  if(vinfo->info->vcs==VERT_NONEQUAL_MB){
	 vis5d_gridlevel_to_pressure(vinfo->v5d_data_context,
										  vinfo->varid,hs->level,
										  &(hs->pressure));
	 sprintf(val,"%6g MB",hs->pressure);

  }else if(vinfo->info->vcs== VERT_EQUAL_KM || 
			  vinfo->info->vcs==VERT_NONEQUAL_KM){  

	 vis5d_gridlevel_to_height(vinfo->v5d_data_context,0,
										vinfo->varid,hs->level,&value);
	 sprintf(val,"%6g Km",value);
  }else {
	 hs->level = adj->upper + adj->lower - adj->value;
	 sprintf(val,"%8g",hs->level);
  }
  gtk_label_set_text(GTK_LABEL(hs->level_value),val);

  last_level = hs->level;
}

void hs_draw_color_sample(v5d_var_info *vinfo, gboolean resize)
{
  static guint color;

  
  vinfo->chs->sample->preview = lookup_widget(vinfo->VarGraphicsDialog,"hs_color_sample");

  vis5d_get_color_table_address(vinfo->info->v5d_display_context, VIS5D_CHSLICE, vinfo->v5d_data_context,
										  vinfo->varid, &vinfo->chs->sample->colors);
  vinfo->chs->sample->ncolors = 255;
  gradient_preview_update(vinfo->chs->sample,resize);
}


void
update_hslice_controls(v5d_var_info *vinfo, gint type)
{
  GtkWidget *sbutton, *slider, *VGD;
  GtkAdjustment *adj;
  hslicecontrols *hs;
  gfloat min, diff; 
  gint maxlevel;
  gchar *minname[]={"hsmin","chsmin"};
  gchar *maxname[]={"hsmax","chsmax"};
  gchar *levelname[]={"hlevel_vscale","chlevel_vscale"};
  gchar *levellabel[]={"hlevel_value","chlevel_value"};

  VGD = vinfo->VarGraphicsDialog;

  if(type == CHSLICE)
	 hs = vinfo->chs;
  else
	 hs = vinfo->hs;

  if(! hs){
	 float color[4];
	 gint i;

	 hs = g_new(hslicecontrols,1);

	 hs->onscreen = TRUE;
	 hs->label = NULL;
	 hs->level = last_level;
	 hs->var = vinfo->vname;
	 if(type == CHSLICE){
		hs->sample= g_new(preview_area,1);
		hs->sample->buffer=NULL;	 
		hs->sample->orig_alpha=NULL;
		hs->sample->ncolors=255;
		vis5d_set_chslice_limits(vinfo->v5d_data_context,vinfo->varid,0,0,
							  hs->level);
		vis5d_get_chslice_limits(vinfo->v5d_data_context,vinfo->varid,
										 &(hs->min),
										 &(hs->max), &(hs->level));
	 }else{
		vis5d_set_hslice(vinfo->v5d_data_context,vinfo->varid,0,0,0,
							  hs->level);
		vis5d_get_hslice(vinfo->v5d_data_context,vinfo->varid,
							  &(hs->interval),&(hs->min),
							  &(hs->max), &(hs->level));

		vis5d_get_color( vinfo->v5d_data_context, VIS5D_HSLICE, vinfo->varid,
							  color,
							  color+1,
							  color+2,
							  color+3);

		for(i=0;i<4;i++){
		  hs->color[i] = (gdouble) color[i];
		}
	 }
	 

	 /* TODO: Need to deal with other vertical grids */  

	 vis5d_gridlevel_to_pressure(vinfo->v5d_data_context,
										  vinfo->varid,hs->level,
										  &(hs->pressure));


	 if(type == CHSLICE){
		vinfo->chs = hs ;
		hs_draw_color_sample(vinfo, TRUE);
	 }
	 else
		vinfo->hs = hs ;
  }
 
  sbutton = lookup_widget(VGD,minname[type]);
  adj = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(sbutton));
  if(adj->lower > hs->min)
	 adj->lower = hs->min ;
  if(adj->upper < hs->max)
	 adj->upper = hs->max ;
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbutton),hs->min);
  gtk_signal_emit_by_name (GTK_OBJECT (adj), "changed");


  sbutton = lookup_widget(VGD,maxname[type]);
  adj = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(sbutton));
  if(adj->lower > hs->min)
	 adj->lower = hs->min ;
  if(adj->upper < hs->max)
	 adj->upper = hs->max ;
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbutton),hs->max);
  gtk_signal_emit_by_name (GTK_OBJECT (adj), "changed");

  


  if(type == HSLICE){
	 sbutton = lookup_widget(VGD,"hsinterval");
	 adj = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(sbutton));
	 diff = hs->max - hs->min;
	 min = 0.01*diff;
	 adj->lower = min;
	 adj->upper = diff;
	 adj->step_increment=1.0;
	 adj->page_increment=1.0;
	 gtk_adjustment_set_value(adj,hs->interval);

	 gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbutton),hs->interval);
	 gtk_signal_emit_by_name (GTK_OBJECT (adj), "changed");
  }

  /* level slider */
  maxlevel = vis5d_get_levels( vinfo->v5d_data_context, vinfo->varid );
  if(maxlevel>1){
	 slider = lookup_widget(VGD,levelname[type]);
  
	 hs->level_value = lookup_widget(VGD,levellabel[type]);
	 adj = gtk_range_get_adjustment(GTK_RANGE(slider));

	 adj->upper = maxlevel;
	 adj->step_increment=1.0;
	 adj->page_increment=1.0;
	 gtk_adjustment_set_value(adj,maxlevel - hs->level );

	 gtk_range_set_adjustment(GTK_RANGE(slider),adj);

	 gtk_object_set_data(GTK_OBJECT(adj),"hslice_object",(gpointer) hs);

	 gtk_signal_connect (GTK_OBJECT(adj),"value_changed", 
								GTK_SIGNAL_FUNC (on_level_vscale_value_changed),(gpointer) vinfo);
	 
	 gtk_signal_emit_by_name (GTK_OBJECT (adj), "changed");
	 gtk_signal_emit_by_name (GTK_OBJECT (adj), "value_changed");
  }

  if(hs->label){
	 delete_label(vinfo->info, hs->label);
    hs->label=NULL;
  }
  if(type==CHSLICE)
	 chs_label(vinfo);
  else
	 hs_label(vinfo);

}

/* activated when button 2 is pressed over the label */
void hslice_toggle(v5d_var_info *vinfo)
{

  if(vinfo->hs->onscreen){
	 vinfo->hs->onscreen=FALSE;
	 vis5d_enable_graphics(vinfo->v5d_data_context, VIS5D_HSLICE,
								  vinfo->varid, VIS5D_OFF);
	 vis5d_set_color( vinfo->info->v5d_display_context, VIS5D_LABEL, vinfo->hs->label->labelid, 
							0.5,0.5,0.5,0.5);
  }else{
	 vis5d_enable_graphics(vinfo->v5d_data_context, VIS5D_HSLICE,
								  vinfo->varid, VIS5D_ON);
	 vinfo->hs->onscreen=TRUE;
	 hs_label(vinfo);
  }
  
  glarea_draw(vinfo->info->GtkGlArea, NULL, NULL);
}



void
on_Hslicebutton_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *VarGraphicsDialog = GTK_WIDGET(user_data);
  GtkWidget *notebook;
  v5d_var_info *vinfo;
  gint time;

  vinfo = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(VarGraphicsDialog),"v5d_var_info");

  if(gtk_toggle_button_get_active(togglebutton)){
	 notebook = lookup_widget(VarGraphicsDialog,"notebook3");

	 gtk_notebook_set_page(GTK_NOTEBOOK(notebook) ,HSLICE);
  
	 update_hslice_controls(vinfo, HSLICE);

	 for(time=0;time<vinfo->info->numtimes;time++){
		vis5d_make_hslice( vinfo->v5d_data_context, time, vinfo->varid, time==vinfo->info->timestep);
	 }
	 vis5d_enable_graphics(vinfo->v5d_data_context, VIS5D_HSLICE,
								  vinfo->varid, VIS5D_ON);
  }else{
	 delete_label(vinfo->info, vinfo->hs->label);
	 vis5d_enable_graphics(vinfo->v5d_data_context, VIS5D_HSLICE,
								  vinfo->varid, VIS5D_OFF); 
  }
  
}


gboolean
on_level_vscale_button_release_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
  int time;
  GtkWidget *VGD;
  v5d_var_info *vinfo;
  int type = GPOINTER_TO_INT(user_data);

  VGD = lookup_widget(widget,"VarGraphicsControls");
  
  vinfo = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(VGD),"v5d_var_info");

  if(!vinfo)
	 return FALSE;

  if(type==HSLICE){
	 vis5d_set_hslice(vinfo->v5d_data_context,vinfo->varid,0,
							0,0,vinfo->hs->level);
	 
	 vis5d_get_hslice(vinfo->v5d_data_context,vinfo->varid,
							&(vinfo->hs->interval),&(vinfo->hs->min),
							&(vinfo->hs->max), &(vinfo->hs->level));
	 
	 for(time=0;time<vinfo->info->numtimes;time++){
		vis5d_make_hslice( vinfo->v5d_data_context, time, vinfo->varid, time==vinfo->info->timestep);
	 }
  }else{
	 vis5d_set_chslice_limits(vinfo->v5d_data_context,vinfo->varid,0,0,
									  vinfo->chs->level);
	 vis5d_get_chslice_limits(vinfo->v5d_data_context,vinfo->varid,
									  &(vinfo->chs->min),
									  &(vinfo->chs->max), &(vinfo->chs->level));

	 for(time=0;time<vinfo->info->numtimes;time++){
		vis5d_make_chslice( vinfo->v5d_data_context, time, vinfo->varid, time==vinfo->info->timestep);
	 }


  }
  update_hslice_controls(vinfo, type);
	 
  glarea_draw(vinfo->info->GtkGlArea, NULL, NULL);

  return FALSE;
}


void
on_CHslicebutton_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *VarGraphicsDialog = GTK_WIDGET(user_data);
  GtkWidget *notebook;
  v5d_var_info *vinfo;
  gint time;

  vinfo = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(VarGraphicsDialog),"v5d_var_info");

  if(gtk_toggle_button_get_active(togglebutton)){
	 notebook = lookup_widget(VarGraphicsDialog,"notebook3");

	 gtk_notebook_set_page(GTK_NOTEBOOK(notebook) ,CHSLICE);
	 
	 update_hslice_controls(vinfo, CHSLICE);

	 for(time=0;time<vinfo->info->numtimes;time++){
		vis5d_make_chslice( vinfo->v5d_data_context, time, vinfo->varid, time==vinfo->info->timestep);
	 }
	 vis5d_enable_graphics(vinfo->v5d_data_context, VIS5D_CHSLICE,
								  vinfo->varid, VIS5D_ON);
  }else{
	 delete_label(vinfo->info, vinfo->chs->label);
	 vis5d_enable_graphics(vinfo->v5d_data_context, VIS5D_CHSLICE,
								  vinfo->varid, VIS5D_OFF); 
  }
  
	 



}


gboolean
on_drawingarea_button_press_event      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
  printf("works so far \n");
  return FALSE;
}



gboolean
on_VarGraphicsControls_delete_event    (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  /* just hide this widget rather than destroying it */
  gtk_widget_hide(widget);
  return TRUE;
}


void
on_linestyle_toggled                   (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *VGC;
  v5d_var_info *vinfo;

  VGC = lookup_widget(GTK_WIDGET(togglebutton),"VarGraphicsControls");
  vinfo = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(VGC),"v5d_var_info");

  if(gtk_toggle_button_get_active(togglebutton)){
	 vis5d_var_graphics_options(vinfo->v5d_data_context,VIS5D_HSLICE, 
										 vinfo->varid,VIS5D_LINE_STIPPLE, GPOINTER_TO_INT(user_data));
  }
  glarea_draw(vinfo->info->GtkGlArea,NULL,NULL);
}


void
on_linewidth_changed                   (GtkEditable     *editable,
                                        gpointer         user_data)
{
  GtkWidget *VGC = GTK_WIDGET(user_data);
  v5d_var_info *vinfo;
  gint value;

  vinfo = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(VGC),"v5d_var_info");

  value = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(editable));
  
  vis5d_var_graphics_options(vinfo->v5d_data_context,VIS5D_HSLICE, 
										 vinfo->varid,VIS5D_LINE_WIDTH, value);
  
  glarea_draw(vinfo->info->GtkGlArea,NULL,NULL);
}

gboolean
on_hs_color_sample_motion_notify_event (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data)
{

  printf("in here now\n");


  return FALSE;
}

void
on_colorsample_mouse_event (GtkWidget       *widget,
									 GdkEventButton  *event,
									 gpointer         user_data)
{
  float val;
  v5d_var_info *vinfo;
  gchar str[9];
  int x = event->x;
  int y = event->y;
  int width = widget->allocation.width;
  GtkWidget *label;

  /*
  gtk_widget_hide(GTK_WIDGET(SampleValuePopup));
  */

  vinfo = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(user_data),"v5d_var_info");
  
  if( !(vinfo && vinfo->chs))
	 return;

  val = vinfo->chs->min + ((float) x * (vinfo->chs->max - vinfo->chs->min))/ (float) width ;

  /* JPE: It might be nice if the widget moved with the pointer */
  if(! current_sample_label)
	 current_sample_label = lookup_widget(vinfo->VarGraphicsDialog,"sample_label");
  sprintf(str,"%8g",val);
  gtk_label_set_text(GTK_LABEL(current_sample_label),str);
  /*
  gtk_widget_show(GTK_WIDGET(SampleValuePopup));
  */
}


GtkWidget *new_VarGraphicsControls()
{
  GtkWidget *vgc, *preview;
  gint old_mask;

  /* things I couldnt do in glade */
  vgc = create_VarGraphicsControls();
  preview = lookup_widget(vgc,"hs_color_sample");

  old_mask =  gtk_widget_get_events(preview);

  gtk_widget_set_events (preview,
								 old_mask |
								 GDK_ENTER_NOTIFY_MASK |
                         GDK_LEAVE_NOTIFY_MASK |
								 GDK_POINTER_MOTION_MASK );  
 
  gtk_signal_connect(GTK_OBJECT(preview), "motion_notify_event",
							GTK_SIGNAL_FUNC(on_colorsample_mouse_event),
							(gpointer) vgc);


  preview = lookup_widget(vgc,"drawingarea1");
  old_mask =  gtk_widget_get_events(preview);

  gtk_widget_set_events (preview,
								 old_mask |
								 GDK_BUTTON_PRESS_MASK|
								 GDK_BUTTON_RELEASE_MASK);

 
  


  return vgc;

}



gboolean
on_hs_color_sample_enterorleave        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
  /* are we leaving because we are out of the box or because the popup is underneath */
  if(event->type == GDK_LEAVE_NOTIFY ){
	 gtk_label_set_text(GTK_LABEL(current_sample_label),"");
	 current_sample_label=NULL;
	 
  }

  /*
  if(event->type == GDK_LEAVE_NOTIFY && SampleValuePopup &&
	  ((event->x <=0 || event->x >= widget->allocation.width) ||
		(event->y <=0 || event->y >= widget->allocation.height))){
	 printf("%f %f %f %f %x\n",event->x,event->y,event->x_root, event->y_root, event->subwindow);
	 gtk_widget_destroy(SampleValuePopup);
	 SampleValuePopup=NULL;
  }

else if(!SampleValuePopup){
	 SampleValuePopup = create_popup();
	 gtk_widget_show(SampleValuePopup);
  }
  */

  return FALSE;
}


void
on_Vslicebutton_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *VGC = GTK_WIDGET(user_data);

  GtkWidget *notebook;
  v5d_var_info *vinfo;
  gint time;

  vinfo = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(VGC),"v5d_var_info");

  if(gtk_toggle_button_get_active(togglebutton)){
	 notebook = lookup_widget(VGC,"notebook3");

	 gtk_notebook_set_page(GTK_NOTEBOOK(notebook) ,VSLICE); 
	 for(time=0;time<vinfo->info->numtimes;time++){
		vis5d_make_vslice( vinfo->v5d_data_context, time, vinfo->varid, time==vinfo->info->timestep);
	 }
	 vis5d_enable_graphics(vinfo->v5d_data_context, VIS5D_VSLICE,
								  vinfo->varid, VIS5D_ON);
	 
  }else{
	 /*
	 delete_label(vinfo->info, vinfo->vs->label);
	 */
	 vis5d_enable_graphics(vinfo->v5d_data_context, VIS5D_VSLICE,
								  vinfo->varid, VIS5D_OFF); 
  }

}

void
update_vslice_controls(v5d_var_info *vinfo, gint type)
{
  GtkWidget *VGD;
  float interval, low, high, r0, c0, r1, c1;
  /*
  vslicecontrols *vs;

  if(type == CHSLICE)
	 vs = vinfo->cvs;
  else
	 vs = vinfo->vs;
  */
  VGD = vinfo->VarGraphicsDialog;

  vis5d_get_vslice( vinfo->v5d_data_context, vinfo->varid, &interval , &low, &high, 
						  &r0, &c0, &r1, &c1);

  

}
