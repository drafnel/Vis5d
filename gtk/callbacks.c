/* This file is only a template for glade, it is not intended to be compiled
   for vis5d+ */

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

extern GtkWidget *FileSelectionDialog;
GtkWidget *graph_menu=NULL;
extern GtkWidget *FontSelectionDialog;




void
on_import1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}





void
on_help1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}





void
VarSelectionDialog_Append(GtkWidget *window,v5d_info *info, char *fname, int dc)
{
  GtkCTree *ctree;
  GtkCTreeNode *pnode, *node;
  gint numvars, i;
  gchar vname[10];
  gchar *nstr[1];
  v5d_var_info *vinfo;

  ctree = GTK_CTREE(lookup_widget(window,"VariableCTree"));

  
  nstr[0] = fname;

  pnode = gtk_ctree_insert_node(ctree,NULL,NULL,nstr,0,NULL,NULL,NULL,NULL,0,1);
  gtk_ctree_node_set_selectable(ctree,pnode,0);

  vis5d_get_ctx_numvars(dc,&numvars);

  for(i=0;i < numvars; i++){
	 vinfo = (v5d_var_info *) g_malloc(sizeof(v5d_var_info));
	 
	 vinfo->VarGraphicsDialog = NULL;
	 vinfo->varid=i;
	 vinfo->v5d_data_context=dc;
	 vinfo->info = info;
	 vis5d_get_ctx_var_name(dc,i,vname);
	 nstr[0] = vname;
	 node = gtk_ctree_insert_node(ctree,pnode,NULL,nstr,0,NULL,NULL,NULL,NULL,1,0);

	 gtk_ctree_node_set_row_data(ctree,node,(gpointer) vinfo);

  }

}





void
on_3d_window1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *window3D;

  window3D = create_window3D();
  gtk_widget_show(window3D);

}


void
on_Isosurface_checkbutton_toggled      (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}
void
on_Contour_checkbutton_toggled      (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  gint v5dtype, v5dmode;
  v5d_var_info * var_info;
  GtkWidget *VarGraphicsDialog;
  GtkToggleButton *ContourHorizontal, *ContourLatitude, 
	 *ContourLongitude;
  gboolean ColorShaded;
 
	 /* should call vis5d_enable_graphics */
	 /* Needs to know state of ContourOrientation toggle */
	 /* Needs to know state of ColorShaded toggle */

  VarGraphicsDialog = gtk_widget_get_toplevel(GTK_WIDGET (togglebutton));
  ContourHorizontal = GTK_TOGGLE_BUTTON(lookup_widget(VarGraphicsDialog,"ContourHorizontal"));
  ContourLongitude = GTK_TOGGLE_BUTTON(lookup_widget(VarGraphicsDialog,"ContourLongitude"));
  ContourLatitude = GTK_TOGGLE_BUTTON(lookup_widget(VarGraphicsDialog,"ContourLatitude"));
  ColorShaded = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(
							lookup_widget(VarGraphicsDialog,"ColorShadedToggle")));

  if(gtk_toggle_button_get_active(togglebutton)){
	 v5dmode = VIS5D_ON;
  }else{
	 v5dmode = VIS5D_OFF;
  }
  if(gtk_toggle_button_get_active(ContourHorizontal)){
	 v5dtype = ColorShaded ? VIS5D_CHSLICE : VIS5D_HSLICE;
  }
  else if(gtk_toggle_button_get_active(ContourLongitude)){
	 v5dtype = ColorShaded ? VIS5D_CVSLICE : VIS5D_VSLICE;
  }
  else if(gtk_toggle_button_get_active(ContourLatitude)){
	 v5dtype = ColorShaded ? VIS5D_CVSLICE : VIS5D_VSLICE;
  }else{
	 g_print("Somethings wrong - no radio is active\n");
	 return;
  }

  /* still need the variable data */

  var_info = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(VarGraphicsDialog), "v5d_var_info");
  
  vis5d_enable_graphics(var_info->v5d_data_context,v5dtype,var_info->varid,v5dmode);
  
  if(v5dmode == VIS5D_OFF) return;


}




void
ResetContourDefaults                   (GtkButton       *button,
                                        gpointer         user_data)
{

  v5d_var_info *var_info;

  GtkWidget *VarGraphicsDialog;

  GtkToggleButton *ContourHorizontal, *ContourLatitude, 
	 *ContourLongitude;
 
	 /* should call vis5d_enable_graphics */
	 /* Needs to know state of ContourOrientation toggle */
	 /* Needs to know state of ColorShaded toggle */

  VarGraphicsDialog = gtk_widget_get_toplevel(GTK_WIDGET (button));
  ContourHorizontal = GTK_TOGGLE_BUTTON(lookup_widget(VarGraphicsDialog,"ContourHorizontal"));
  ContourLongitude = GTK_TOGGLE_BUTTON(lookup_widget(VarGraphicsDialog,"ContourLongitude"));
  ContourLatitude = GTK_TOGGLE_BUTTON(lookup_widget(VarGraphicsDialog,"ContourLatitude"));

  if(gtk_toggle_button_get_active(ContourHorizontal))
  { 
    /* this selects an initial min, max and interval and level */
    /* need to know type of slice */
	 float v5dvals[3], level;
	 GtkSpinButton *contour[3];
	 GtkAdjustment *adj;
	 gint i;
	 var_info = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(VarGraphicsDialog), "v5d_var_info");

	 /* level should not be set here */
	 vis5d_set_hslice(var_info->v5d_data_context,var_info->varid,0,0,0,0);

	 
	 vis5d_get_hslice(var_info->v5d_data_context,var_info->varid,v5dvals+2,
							v5dvals,v5dvals+1,&level);

	 contour[0] = GTK_SPIN_BUTTON(lookup_widget(VarGraphicsDialog,"ContourMinimum"));
	 contour[1] = GTK_SPIN_BUTTON(lookup_widget(VarGraphicsDialog,"ContourMaximum"));
	 contour[2] = GTK_SPIN_BUTTON(lookup_widget(VarGraphicsDialog,"ContourInterval"));

    for(i=0;i<3;i++){
		adj = gtk_spin_button_get_adjustment(contour[i]);
		adj->value = v5dvals[i];
		if(i<2){
		  adj->lower = v5dvals[0]-20.*v5dvals[2];
		  adj->upper = v5dvals[1]+20.*v5dvals[2];
		  adj->step_increment = v5dvals[2];
		}else{
		  adj->lower = 1./HUGE;
		  adj->upper = 20.*v5dvals[2];
		  adj->step_increment = v5dvals[2]/10.;
		}
		gtk_spin_button_configure(contour[i],adj,adj->step_increment,4);
	 }										
  }

}





void
on_ContourValue_changed                (GtkEditable     *editable,
                                        gpointer         user_data)
{
  float v5dvals[3];
  GtkSpinButton *contour[3];
  GtkWidget *VarGraphicsDialog;
  v5d_var_info *var_info;
  gint i;

  VarGraphicsDialog = gtk_widget_get_toplevel(GTK_WIDGET (editable));
  
  contour[0] = GTK_SPIN_BUTTON(lookup_widget(VarGraphicsDialog,"ContourMinimum"));
  contour[1] = GTK_SPIN_BUTTON(lookup_widget(VarGraphicsDialog,"ContourMaximum"));
  contour[2] = GTK_SPIN_BUTTON(lookup_widget(VarGraphicsDialog,"ContourInterval"));

  for(i=0;i<2;i++){
	 v5dvals[i] = gtk_spin_button_get_value_as_float(contour[i]);
  }

  
  var_info = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(VarGraphicsDialog), "v5d_var_info");

  /* level should not be set here */
  vis5d_set_hslice(var_info->v5d_data_context,var_info->varid,v5dvals[2],
						 v5dvals[0],v5dvals[1],0);

}


gboolean
on_VarGraphicsDialog_expose_event      (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
  v5d_var_info *var_info;

  g_print("vgw expose event\n");
  var_info = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(widget), "v5d_var_info");
  if(var_info != NULL){
	 /*	 
			I want the glarea put on top here, but haven't figured out how yet
	 glarea_draw(var_info->info->glarea,NULL,NULL);
	 gdk_window_raise(var_info->info->glarea->window);*/
  }
  return FALSE;
}

void on_option_toggle(GtkMenuItem *menuitem,gpointer user_data, int v5dwhat)
{
  GtkWidget *window3D;
  v5d_info *info;
  

  window3D = lookup_widget(GTK_WIDGET(menuitem),"window3D");
  info = (v5d_info *)gtk_object_get_data(GTK_OBJECT(window3D),"v5d_info");
  
  if(info){
	 if( GTK_CHECK_MENU_ITEM(menuitem)->active ){
		vis5d_graphics_mode(info->v5d_display_context,v5dwhat,VIS5D_ON);
	 }else{
		vis5d_graphics_mode(info->v5d_display_context,v5dwhat,VIS5D_OFF);
	 }
  }else{
	 printf("ERROR: info undefined in option_toggle\n");
  }
}

void
on_map1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  on_option_toggle(menuitem,user_data,VIS5D_MAP);
}

void
on_topo1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  on_option_toggle(menuitem,user_data,VIS5D_TOPO);
  
}

void
on_box1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  on_option_toggle(menuitem,user_data,VIS5D_BOX);

}


void
on_clock1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  on_option_toggle(menuitem,user_data,VIS5D_CLOCK);

}




void
on_preferences1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *window3D, *prefs, *map_entry, *topo_entry;
  v5d_info *info;
  gchar v5dstr[V5D_MAXSTRLEN];

  if(user_data==NULL) return;

  window3D = GTK_WIDGET(user_data);

  info = (v5d_info *)gtk_object_get_data(GTK_OBJECT(window3D),"v5d_info");

  prefs = create_PreferenceDialog();
  
  gtk_grab_add(prefs);
  gtk_window_set_transient_for(GTK_WINDOW(prefs),GTK_WINDOW(window3D));

  if(info){
	 gtk_object_set_data (GTK_OBJECT(prefs), "v5d_info", info);
	 map_entry = lookup_widget(prefs, "Map_entry");

	 vis5d_get_map(info->v5d_display_context , (char *) v5dstr);

	 gtk_entry_set_text(GTK_ENTRY(map_entry) , v5dstr);

	 topo_entry = lookup_widget(prefs, "Topo_entry");

	 vis5d_get_topo(info->v5d_display_context , (char *) v5dstr);

	 gtk_entry_set_text(GTK_ENTRY(topo_entry) , v5dstr);
  }

  gtk_widget_show(prefs);

}



void
on_browse_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *Prefs;
  gchar title[40];
  if(FileSelectionDialog == NULL)
	 FileSelectionDialog = create_fileselection1();

  sprintf(title,_("Open %s File"),(gchar *) user_data);

  gtk_window_set_title(GTK_WINDOW(FileSelectionDialog),title);
  gtk_grab_add(FileSelectionDialog);

  /* TODO: Need to set the default directory? */
  Prefs = gtk_widget_get_toplevel (GTK_WIDGET (button));

  gtk_object_set_data(GTK_OBJECT(FileSelectionDialog),"OpenWhat" ,user_data);

  
  if(! strncmp("map",(gchar *) user_data,3)){
	 char v5dstr[V5D_MAXSTRLEN];
	 v5d_info *info = (v5d_info*) gtk_object_get_data(GTK_OBJECT(Prefs), "v5d_info");
	 vis5d_get_map(info->v5d_display_context , (char *) v5dstr);
	 if(v5dstr[0]=='/'){
		gtk_file_selection_set_filename(GTK_FILE_SELECTION(FileSelectionDialog),v5dstr);
	 }else{
		gtk_file_selection_set_filename(GTK_FILE_SELECTION(FileSelectionDialog),DATA_PREFIX );
	 }
  }else if(! strncmp("topo",(gchar *) user_data,4)){
	 char v5dstr[V5D_MAXSTRLEN];
	 v5d_info *info = (v5d_info*) gtk_object_get_data(GTK_OBJECT(Prefs), "v5d_info");
	 vis5d_get_topo(info->v5d_display_context , (char *) v5dstr);
	 if(v5dstr[0]=='/'){
		gtk_file_selection_set_filename(GTK_FILE_SELECTION(FileSelectionDialog),v5dstr);
	 }else{
		gtk_file_selection_set_filename(GTK_FILE_SELECTION(FileSelectionDialog),DATA_PREFIX );
	 }
  }
  gtk_object_set_data(GTK_OBJECT(FileSelectionDialog),"PrefsDialog" ,Prefs );

  gtk_widget_show (FileSelectionDialog);
  gtk_grab_add(FileSelectionDialog);
  gtk_window_set_transient_for(GTK_WINDOW(FileSelectionDialog),GTK_WINDOW(Prefs));

}


void
on_Prefs_OK_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *Prefs, *map_entry, *topo_entry;
  v5d_info *info;
  int hires;

  Prefs = gtk_widget_get_toplevel (GTK_WIDGET (button));

  info = (v5d_info*) gtk_object_get_data(GTK_OBJECT(Prefs), "v5d_info");

  map_entry = lookup_widget(Prefs,"Map_entry");
  topo_entry = lookup_widget(Prefs,"Topo_entry");

  vis5d_init_map(info->v5d_display_context,gtk_entry_get_text(GTK_ENTRY(map_entry)));

  hires = vis5d_graphics_mode(info->v5d_display_context,VIS5D_HIRESTOPO,VIS5D_GET);

  vis5d_init_topo(info->v5d_display_context,gtk_entry_get_text(GTK_ENTRY(topo_entry)),hires);

  vis5d_load_topo_and_map(info->v5d_display_context);

  gtk_grab_remove( Prefs );

  gtk_widget_destroy( Prefs );
}


void
on_Prefs_cancel_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *Prefs = gtk_widget_get_toplevel( GTK_WIDGET(button) );
  gtk_grab_remove(Prefs);
  gtk_widget_destroy( Prefs );

}

GdkColor *vis5d_color_to_gdk(GtkWidget *widget, float red, float green, float blue)
{
  GdkColor *color;
  
  /* the color we want to use */
  color = (GdkColor *)g_malloc(sizeof(GdkColor));
                          
  /* red, green, and blue are passed values, indicating the RGB triple
	* of the color we want to draw. Note that the values of the RGB components
	* within the GdkColor are taken from 0 to 65535, not 0 to 255.
	*/

  color->red = (gushort) (red * 65535);
  color->green = (gushort) (green * 65535);
  color->blue = (gushort) (blue * 65535);
  
  /* the pixel value indicates the index in the colormap of the color.
	* it is simply a combination of the RGB values we set earlier
	*/
  color->pixel = (gulong)(red*65536 + green*256 + blue);

  /* However, the pixel value is only truly valid on 24-bit (TrueColor)
	* displays. Therefore, this call is required so that GDK and X can
	* give us the closest color available in the colormap
	*/
  gdk_color_alloc(gtk_widget_get_colormap(widget), color);

  return color;
}



void
on_VariableCTree_tree_select_row       (GtkCTree        *ctree,
                                        GList           *node,
                                        gint             column,
                                        gpointer         user_data)
{
  v5d_var_info *vinfo;
  
  vinfo = (v5d_var_info *) gtk_ctree_node_get_row_data(ctree,GTK_CTREE_NODE(node));

  if(vinfo->VarGraphicsDialog == NULL){
	 vinfo->VarGraphicsDialog = create_VarGraphicsDialog();
  }

  gtk_widget_show(vinfo->VarGraphicsDialog);

  {/* to be moved */
	 int times, numtimes,curtime;
	 float interval, low, high, level, pressure;
	 gchar labelstring[80];
    gchar *listentry[1];
	 gchar varname[10];
	 GtkCList *clist;
	 GdkColor *gcolor;
	 float alpha, red, green, blue;

	 vis5d_get_ctx_numtimes( vinfo->v5d_data_context, &numtimes );
	 vis5d_get_ctx_timestep( vinfo->v5d_data_context,  &curtime);
	 for ( times = 0; times < numtimes; times++){
		vis5d_make_hslice( vinfo->v5d_data_context, times, vinfo->varid, times==curtime);
	 }
	 vis5d_set_hslice(vinfo->v5d_data_context,vinfo->varid,0,0,0,0);

	 vis5d_get_hslice(vinfo->v5d_data_context,vinfo->varid, &interval, &low, &high, &level);

	 vis5d_get_ctx_var_name(vinfo->v5d_data_context,vinfo->varid,varname);

	 vis5d_gridlevel_to_pressure(vinfo->v5d_data_context,vinfo->varid,level,&pressure);

#ifdef HAVE_SNPRINTF	 
	 snprintf(labelstring,80,_("Contours of %s from %4.4g to %4.4g by %4.4g at %4.4g MB"),
				 varname,low,high,interval,pressure);
#else
	 sprintf(labelstring,_("Contours of %s from %4.4g to %4.4g by %4.4g at %4.4g MB"),
				 varname,low,high,interval,pressure);
#endif

	 clist = GTK_CLIST(lookup_widget(vinfo->info->GtkGlArea,"Graphs_CList"));

	 listentry[0]=labelstring;

    vinfo->clistrow = gtk_clist_append(clist,listentry);

	 
	 vis5d_get_color( vinfo->v5d_data_context, VIS5D_HSLICE, vinfo->varid,
							&red,&green,&blue,&alpha);

	 gcolor = vis5d_color_to_gdk(GTK_WIDGET(clist), red,green,blue);

	 gtk_clist_set_foreground(clist,vinfo->clistrow,gcolor);
	 

	 vis5d_enable_graphics(vinfo->v5d_data_context, VIS5D_HSLICE,
								  vinfo->varid, VIS5D_ON);


	 
  }



}


void
on_VariableCTree_tree_unselect_row     (GtkCTree        *ctree,
                                        GList           *node,
                                        gint             column,
                                        gpointer         user_data)
{
  v5d_var_info *vinfo;

  vinfo = (v5d_var_info *) gtk_ctree_node_get_row_data(ctree,GTK_CTREE_NODE(node));

  printf("Row unselected\n");

}


void
on_VSDClose_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{

  gtk_widget_hide (GTK_WIDGET(user_data));

}







void
on_Arrow_clicked                       (GtkButton       *button,
                                        gpointer         user_data)
{
  v5d_info *info;

  info = (v5d_info*)gtk_object_get_data(GTK_OBJECT(
			  lookup_widget(GTK_WIDGET(button),"window3D")),"v5d_info");
  
  if(info==NULL)
	 return;

  vis5d_get_dtx_timestep(info->v5d_display_context  ,&info->timestep);
  /* returns Numtimes - lasttime is one less */
  vis5d_get_dtx_numtimes(info->v5d_display_context, &info->numtimes);

  if(strncmp("next",user_data,4)==0)
	 {
		info->timestep+=info->stepsize;
	 }
  else if(strncmp("previous",user_data,8)==0)
	 {
		info->timestep-=info->stepsize;
	 }
  else if(strncmp("first",user_data,5)==0)
	 {
		info->timestep=0;
	 }
  else if(strncmp("last",user_data,4)==0)
	 {
		info->timestep = info->numtimes-1;
	 }
  if(info->timestep<0){
	 info->timestep = info->numtimes-info->timestep;
  }else if(info->timestep>=info->numtimes){
	 info->timestep = info->timestep-info->numtimes;
  }

  vis5d_make_timestep_graphics(info->v5d_display_context, info->timestep);
#ifdef SINGLE_TASK
  vis5d_finish_work();
#endif
  vis5d_set_dtx_timestep(info->v5d_display_context  ,info->timestep);

  glarea_draw (info->GtkGlArea,NULL,NULL);

}


void
on_animate_toggled                     (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

  v5d_info *info = (v5d_info*)gtk_object_get_data(GTK_OBJECT(
			  lookup_widget(GTK_WIDGET(togglebutton),"window3D")),"v5d_info");

  if(info==NULL) return;

  if(gtk_toggle_button_get_active(togglebutton)&&info->numtimes>0){
	 if(user_data){
		info->animate=-1; /* animate backwards */
	 }else{
		info->animate=1;
	 }
	 printf("animate on\n");
  }else{
	 info->animate=0;
	 printf("animate off\n");
  }

}




void
on_window_3d1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *window3D;
  v5d_info *info;

  if(!FontSelectionDialog)
	 FontSelectionDialog = create_fontselectiondialog1();

  window3D = lookup_widget(GTK_WIDGET(menuitem),"window3D");
  if(!window3D) return;
  info = (v5d_info*) gtk_object_get_data(GTK_OBJECT(window3D), "v5d_info");
  if(!info) return;
  gtk_object_set_data(GTK_OBJECT(FontSelectionDialog),"v5d_info",info);

  gtk_object_set_data(GTK_OBJECT(FontSelectionDialog),"Font",GINT_TO_POINTER(WINDOW_3D_FONT));
	 
  gtk_widget_show(FontSelectionDialog);

}


void
on_contour_label1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *window3D;
  v5d_info *info;

  if(!FontSelectionDialog)
	 FontSelectionDialog = create_fontselectiondialog1();

  window3D = lookup_widget(GTK_WIDGET(menuitem),"window3D");
  if(!window3D) return;
  info = (v5d_info*) gtk_object_get_data(GTK_OBJECT(window3D), "v5d_info");
  if(!info) return;
  gtk_object_set_data(GTK_OBJECT(FontSelectionDialog),"v5d_info",info);

  gtk_object_set_data(GTK_OBJECT(FontSelectionDialog),"Font",GINT_TO_POINTER(CONTOUR_LABEL_FONT));
	 
  gtk_widget_show(FontSelectionDialog);

}


void
on_topography1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  char v5dstr[V5D_MAXSTRLEN];
  GtkWidget *window3D;
  v5d_info *info;

  window3D = lookup_widget(GTK_WIDGET(menuitem),"window3D");
  if(!window3D) return;
  info = (v5d_info*) gtk_object_get_data(GTK_OBJECT(window3D), "v5d_info");
  if(!info) return;

  if(FileSelectionDialog == NULL)
	 FileSelectionDialog = create_fileselection1();

  gtk_window_set_title(GTK_WINDOW(FileSelectionDialog),_("Select Topography File"));
  gtk_grab_add(FileSelectionDialog);
  gtk_object_set_data(GTK_OBJECT(FileSelectionDialog),"OpenWhat" ,"topo");
  
  vis5d_get_topo(info->v5d_display_context , (char *) v5dstr);
  if(v5dstr[0]=='/'){
	 gtk_file_selection_set_filename(GTK_FILE_SELECTION(FileSelectionDialog),v5dstr);
  }else{
	 gtk_file_selection_set_filename(GTK_FILE_SELECTION(FileSelectionDialog),DATA_PREFIX );
  }
  gtk_widget_show (FileSelectionDialog);
  gtk_grab_add(FileSelectionDialog);
  gtk_window_set_transient_for(GTK_WINDOW(FileSelectionDialog),GTK_WINDOW(window3D));
}


void
on_map2_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  char v5dstr[V5D_MAXSTRLEN];
  GtkWidget *window3D;
  v5d_info *info;

  window3D = lookup_widget(GTK_WIDGET(menuitem),"window3D");
  if(!window3D) return;
  info = (v5d_info*) gtk_object_get_data(GTK_OBJECT(window3D), "v5d_info");
  if(!info) return;

  if(FileSelectionDialog == NULL)
	 FileSelectionDialog = create_fileselection1();

  gtk_window_set_title(GTK_WINDOW(FileSelectionDialog),_("Select Map File"));
  gtk_grab_add(FileSelectionDialog);
  gtk_object_set_data(GTK_OBJECT(FileSelectionDialog),"OpenWhat" ,"map");

  vis5d_get_map(info->v5d_display_context , (char *) v5dstr);
  if(v5dstr[0]=='/'){
	 gtk_file_selection_set_filename(GTK_FILE_SELECTION(FileSelectionDialog),v5dstr);
  }else{
	 gtk_file_selection_set_filename(GTK_FILE_SELECTION(FileSelectionDialog),DATA_PREFIX );
  }  
 
  gtk_widget_show (FileSelectionDialog);
  gtk_grab_add(FileSelectionDialog);
  gtk_window_set_transient_for(GTK_WINDOW(FileSelectionDialog),GTK_WINDOW(window3D));
}


void
on_fontselectionbutton_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
  int whichbutton, whichfont;
  gchar *fontname;
  v5d_info *info;

  whichbutton = (int) user_data;

  printf("whichbutton = %d\n",whichbutton);

  if(whichbutton==0)/* OK */{
	 fontname = gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG (FontSelectionDialog));

	 whichfont = (int) gtk_object_get_data(GTK_OBJECT(FontSelectionDialog),"Font");
	 info = (v5d_info *) gtk_object_get_data(GTK_OBJECT(FontSelectionDialog),"v5d_info");
  
	 vis5d_set_font(info->v5d_display_context,fontname,0,whichfont);
  }

  gtk_widget_hide(FontSelectionDialog);

}








void
on_save_options1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

