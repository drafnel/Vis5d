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

GtkWidget *new_window3D(GtkWidget *oldwindow3D)
{
  GtkWidget *window3D, *delete_frame;
  GList *window3Dlist=NULL;

  window3D = create_window3D();

  if(oldwindow3D){
	 delete_frame = lookup_widget(oldwindow3D,"delete_frame1");
	 gtk_widget_set_sensitive(delete_frame,TRUE);
	 window3Dlist = (GList *) gtk_object_get_data(GTK_OBJECT(oldwindow3D),"window3Dlist");

	 delete_frame = lookup_widget(window3D,"delete_frame1");
	 gtk_widget_set_sensitive(delete_frame,TRUE);

  }
  window3Dlist = g_list_append(window3Dlist,(gpointer) window3D);
  gtk_object_set_data(GTK_OBJECT(window3D),"window3Dlist",(gpointer) window3Dlist);

  if(oldwindow3D)
	 gtk_object_set_data(GTK_OBJECT(oldwindow3D),"window3Dlist",(gpointer) window3Dlist);
	 

  gtk_widget_show (window3D);
  return window3D;
}

void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *window3D;
  if(FileSelectionDialog == NULL)
	 FileSelectionDialog = create_fileselection1();

  /* This is the only window that should accept input */
  gtk_grab_add(FileSelectionDialog);

  gtk_window_set_title(GTK_WINDOW(FileSelectionDialog),_("Open Data File"));

  if(user_data){
	 /* called from open in new frame */
	 window3D=GTK_WIDGET(user_data);
  }else{
	 window3D=lookup_widget(GTK_WIDGET (menuitem),"window3D");
  }
  
  gtk_object_set_data(GTK_OBJECT(FileSelectionDialog),"window3D" , window3D);

  gtk_object_set_data(GTK_OBJECT(FileSelectionDialog),"OpenWhat" , GINT_TO_POINTER(DATA_FILE));

  gtk_widget_show (FileSelectionDialog);

  gtk_window_set_transient_for(GTK_WINDOW(FileSelectionDialog),GTK_WINDOW( window3D));
}

void
on_exit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *window3D;
  GList *window3Dlist, *item;
  v5d_info *info;

  window3D = lookup_widget(GTK_WIDGET(menuitem),"window3D");
  window3Dlist = g_list_first((GList *) gtk_object_get_data(GTK_OBJECT(window3D),"window3Dlist"));
  item = window3Dlist;
  while(item!=NULL){
	 info = (v5d_info*)gtk_object_get_data(GTK_OBJECT(item->data),"v5d_info");
	 if(info->timeout_id){
		gtk_timeout_remove(info->timeout_id);
	 }
	 vis5d_destroy_display_context(info->v5d_display_context);
	 
	 free(info);
	 gtk_widget_destroy(GTK_WIDGET(item->data));
	 item = g_list_next(item);
  }

  g_list_free(window3Dlist);
  vis5d_terminate(1);
  gtk_main_quit();

}
void
on_delete_frame1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *window3D, *delete_frame;
  GList *window3Dlist, *item;

  v5d_info *info;
  window3D = lookup_widget(GTK_WIDGET(menuitem),"window3D");
  info = (v5d_info*)gtk_object_get_data(GTK_OBJECT(window3D),"v5d_info");

  window3Dlist = g_list_first((GList *) gtk_object_get_data(GTK_OBJECT(window3D),"window3Dlist"));
  window3Dlist = g_list_remove(window3Dlist,(gpointer) window3D);
  

  if(info->timeout_id){
	 gtk_timeout_remove(info->timeout_id);
  }
  free(info);

  gtk_widget_destroy(window3D);

  if(g_list_last(window3Dlist) == window3Dlist){
	 /* only one window3D left */
	 window3D = GTK_WIDGET(window3Dlist->data);
	 delete_frame = lookup_widget(window3D,"delete_frame1");
	 gtk_widget_set_sensitive(delete_frame,FALSE);
  }
  item = window3Dlist;
  /* Reset the window3Dlist pointer for each window */  
  while(item!=NULL){
	 gtk_object_set_data(GTK_OBJECT(item->data),"window3Dlist",window3Dlist);
	 item = item->next;
  }
}

void
on_open_in_new_frame1_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *window3D;
    
  window3D = new_window3D(lookup_widget(GTK_WIDGET(menuitem),"window3D"));
  on_open1_activate(menuitem,(gpointer) window3D);

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
	 info->timestep = info->numtimes+info->timestep;
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
  color->pixel = (gulong)(256.*(256.*(256.*red + green) + blue));

  /* However, the pixel value is only truly valid on 24-bit (TrueColor)
	* displays. Therefore, this call is required so that GDK and X can
	* give us the closest color available in the colormap
	*/
  gdk_color_alloc(gtk_widget_get_colormap(widget), color);

  return color;
}



void
on_save_options1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

void
on_openprocedure_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *window3D;

  if(FileSelectionDialog == NULL)
	 FileSelectionDialog = create_fileselection1();

  /* This is the only window that should accept input */
  gtk_grab_add(FileSelectionDialog);
  
  gtk_window_set_title(GTK_WINDOW(FileSelectionDialog),_("Open Procedure File"));

  window3D=lookup_widget(GTK_WIDGET (menuitem),"window3D");
   
  gtk_object_set_data(GTK_OBJECT(FileSelectionDialog),"window3D" , window3D);

  gtk_object_set_data(GTK_OBJECT(FileSelectionDialog),"OpenWhat" , GINT_TO_POINTER(PROCEDURE_FILE));

  gtk_widget_show (FileSelectionDialog);

  gtk_window_set_transient_for(GTK_WINDOW(FileSelectionDialog),GTK_WINDOW( window3D));
 

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
  gtk_object_set_data(GTK_OBJECT(FileSelectionDialog),"OpenWhat" ,GINT_TO_POINTER(TOPO_FILE));
  
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
  gtk_object_set_data(GTK_OBJECT(FileSelectionDialog),"OpenWhat" ,GINT_TO_POINTER(MAP_FILE));

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
on_newprocedure_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}
