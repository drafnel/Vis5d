/*
 * Vis5d+/Gtk user interface 
 * Copyright (C) 2001 James P Edwards
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
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

#include "window3D.h"
#include "w3D_interface.h"
#include "interface.h"
#include "support.h"
#include "support_cb.h"
#include "graph_labels.h"
#include "VarGraphicsControls.h"
#include "ProcedureDialog.h"
#include "textplot.h"

extern GtkWidget *FileSelectionDialog;
extern GtkWidget *FontSelectionDialog;
gboolean vis5d_initialized=FALSE;

GtkWidget *new_window3D(GtkWidget *oldwindow3D)
{
  GtkWidget *window3D, *delete_frame;
  GList *window3Dlist=NULL;

#ifdef ENABLE_NLS
  bindtextdomain (PACKAGE, VIS5D_LOCALE_DIR);
  textdomain (PACKAGE);
#endif
  add_pixmap_directory (DATA_PREFIX "/pixmaps");
  add_pixmap_directory (VIS5D_SOURCE_DIR "/pixmaps");

  window3D = create_window3D();

  if(oldwindow3D){
	 delete_frame = lookup_widget(oldwindow3D,"delete_frame1");
	 gtk_widget_set_sensitive(delete_frame,TRUE);
	 window3Dlist = (GList *) gtk_object_get_data(GTK_OBJECT(oldwindow3D),"window3Dlist");

	 delete_frame = lookup_widget(window3D,"delete_frame1");
	 gtk_widget_set_sensitive(delete_frame,TRUE);

  }
  printf("Start the window\n");

  window3Dlist = g_list_append(window3Dlist,(gpointer) window3D);
  gtk_object_set_data(GTK_OBJECT(window3D),"window3Dlist",(gpointer) window3Dlist);

  if(oldwindow3D)
	 gtk_object_set_data(GTK_OBJECT(oldwindow3D),"window3Dlist",(gpointer) window3Dlist);
	 
  printf("Show the window\n");

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
	 /* called from open in new frame or main*/
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
  GtkToolbar *toolbar;
  GList *item;
  GtkWidget *widget;

  v5d_info *info = (v5d_info*)gtk_object_get_data(GTK_OBJECT(
			  lookup_widget(GTK_WIDGET(togglebutton),"window3D")),"v5d_info");

  if(info==NULL) return;

  toolbar = GTK_TOOLBAR(lookup_widget(GTK_WIDGET(togglebutton),"toolbar1"));

  if(gtk_toggle_button_get_active(togglebutton)&&info->numtimes>0){
	 if(user_data){
		info->animate=-1; /* animate backwards */
	 }else{
		info->animate=1;
	 }
	 /* set all other toolbar items to inactive */
	 item = g_list_first(toolbar->children);
	 while(item!=NULL){
		if((widget=((GtkToolbarChild *) item->data)->widget) != GTK_WIDGET(togglebutton)
			&& !GTK_IS_TOOLBAR(widget) ){
		  gtk_widget_set_sensitive(widget,FALSE);
		}
		item=g_list_next(item);
	 }
	 /* animation speed items should be active */
	 gtk_widget_set_sensitive(lookup_widget(GTK_WIDGET(togglebutton),"faster"),TRUE);
	 gtk_widget_set_sensitive(lookup_widget(GTK_WIDGET(togglebutton),"slower"),TRUE);


  }else{
	 info->animate=0;

	 /* set all other toolbar items to active */
	 item = g_list_first(toolbar->children);
	 while(item!=NULL){
		if((widget=((GtkToolbarChild *) item->data)->widget) != GTK_WIDGET(togglebutton)){
		  gtk_widget_set_sensitive(widget,TRUE);
		}
		item=g_list_next(item);
	 }
	 /* animation speed items should be inactive */
	 gtk_widget_set_sensitive(lookup_widget(GTK_WIDGET(togglebutton),"faster"),FALSE);
	 gtk_widget_set_sensitive(lookup_widget(GTK_WIDGET(togglebutton),"slower"),FALSE);
  }

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

  if(user_data)
	 window3D = GTK_WIDGET(user_data);
  else
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
  v5d_info *info;
  GtkWidget *window3D = lookup_widget(GTK_WIDGET(menuitem),"window3D");
  
  info = (v5d_info *)gtk_object_get_data(GTK_OBJECT(window3D),"v5d_info");

  if(info->ProcedureDialog)
	 gtk_widget_destroy(info->ProcedureDialog);
  info->ProcedureDialog = new_ProcedureDialog(info, NULL);
  gtk_window_set_transient_for(GTK_WINDOW(info->ProcedureDialog),GTK_WINDOW(window3D));
}

void
on_setview_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkLabel *label;
  gchar *label_str;

  label = GTK_LABEL(GTK_BIN(menuitem)->child);
  gtk_label_get(label, &label_str);

  printf("here %s\n",label_str);

}

/*****************************************************************************/
/*                                                                           */
/* Function: create_glarea (void)                                            */
/*                                                                           */
/* This function performs the necessary operations to construct a GtkGlarea  */
/* widget. These operations include creating the widget, setting the size    */
/* of the widget, and registering callbacks for the widget.                  */
/*                                                                           */
/* This is a good place to add function calls for any GtkGlarea              */
/* initialization that you need to do.                                       */
/*                                                                           */
/*****************************************************************************/

GtkWidget* 
create_glarea (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2 ) 
{

  GtkWidget* glarea;
  /* Choose the attributes that we would like for our visual. */
  /* These attributes are passed to glXChooseVisual by the    */
  /* gdk (see gdk_gl_choose_visual in gdkgl.c from the        */
  /* GtkGlarea distro).                                       */
  /*                                                          */
  /*                                                          */
  /* From the glXChooseVisual manpage:                        */
  /*                                                          */
  /* glXChooseVisual returns a pointer to an XVisualInfo      */
  /* structure describing the visual that best meets a        */
  /* minimum specification.                                   */
  /*                                                          */
  /* Check out the manpage for a complete list of attributes  */
  /* and their descriptions.                                  */

  int attrlist[] = {
    GDK_GL_RGBA,
    GDK_GL_DOUBLEBUFFER,
    GDK_GL_RED_SIZE, 1,
    GDK_GL_BLUE_SIZE, 1,
    GDK_GL_GREEN_SIZE, 1,
    GDK_GL_DEPTH_SIZE, 1,
    GDK_GL_NONE
  };

  /* First things first! Make sure that OpenGL is supported   */
  /* before trying to do OpenGL stuff!                        */

  if(gdk_gl_query() == FALSE) {
    g_print("OpenGL not supported!\n");
    return NULL;
  }

  /* Now, create the GtkGLArea using the attribute list that  */
  /* we defined above.                                        */

  if ((glarea = gtk_gl_area_new(attrlist)) == NULL) {
    g_print("Error creating GtkGLArea!\n");
    return NULL;
  }

  /* Indicate which events we are interested in receiving in  */
  /* in the window allocated to our glarea widget.            */
  /*                                                          */
  /* Check out gdk/gdktypes.h in your include directory for a */
  /* complete list of event masks that you can use.           */

  gtk_widget_set_events(GTK_WIDGET(glarea),GDK_ALL_EVENTS_MASK);
  /*  TODO: Refine this list 
                        GDK_EXPOSURE_MASK|
                        GDK_BUTTON_PRESS_MASK|
			GDK_BUTTON_RELEASE_MASK|
			GDK_POINTER_MOTION_MASK|
                        GDK_POINTER_MOTION_HINT_MASK);
  */

  return (glarea);

}


/*****************************************************************************/
/*                                                                           */
/* Function: glarea_button_release (GtkWidget*, GdkEventButton*)             */
/*                                                                           */
/* This function handles button-release events for the GtkGLArea into which  */
/* we are drawing.                                                           */
/*                                                                           */
/*****************************************************************************/

gboolean glarea_button_release (GtkWidget* widget, GdkEventButton* event
										  ,gpointer         user_data) {
  /*
  int x = event->x;
  int y = event->y;
  */
  if (event->button == 1) {

    /* Mouse button 1 was released */
    return TRUE;

  }

  if (event->button == 2) {

    /* Mouse button 2 was released */
    return TRUE;

  }

  return FALSE;

}

/*****************************************************************************/
/*                                                                           */
/* Function: glarea_button_press (GtkWidget*, GdkEventButton*)               */
/*                                                                           */
/* This function handles button-press events for the GtkGLArea into which we */
/* are drawing.                                                              */
/*                                                                           */
/*****************************************************************************/

gboolean glarea_button_press (GtkWidget* widget, GdkEventButton* event, 
										gpointer         user_data) {
  v5d_info *info;
  int label_id;
  int x = event->x;
  int y = event->y;

  info = (v5d_info*)gtk_object_get_data(GTK_OBJECT(lookup_widget(widget,"window3D")),"v5d_info");

  if(vis5d_find_label(info->v5d_display_context, &x, &y, &label_id)==0){
	 graph_label_button_press(info, label_id, event->button);
	 return TRUE;
  }

  if (event->button == 1) {
    /* Mouse button 1 was engaged */
    return TRUE;
  }

  if (event->button == 2) {

    /* Mouse button 2 was engaged */
    return TRUE;

  }

  if (event->button == 3) {

    /* Mouse button 3 was engaged */
    return TRUE;

  }

  g_print("Button %d press?\n",event->button);

  return FALSE;

}

/*****************************************************************************/
/*                                                                           */
/* Function: glarea_motion_notify (GtkWidget*, GdkEventMotion*)              */
/*                                                                           */
/* This function handles motion events for the GtkGLArea into which we are   */
/* drawing                                                                   */
/*                                                                           */
/*****************************************************************************/
gboolean glarea_motion_notify(GtkWidget *widget, GdkEventMotion *event, 
										gpointer         user_data)
{
  int x, y;
  GdkRectangle area;
  GdkModifierType state;


  v5d_info *info = (v5d_info*)gtk_object_get_data(GTK_OBJECT(lookup_widget(widget,"window3D")),"v5d_info");


  if(info==NULL)
	 return FALSE;

  if (event->is_hint) {
    /* fix this! */
#if !defined(WIN32)
    gdk_window_get_pointer(event->window, &x, &y, &state);
#endif
  } else {
    x = event->x;
    y = event->y;
    state = event->state;
  }
  
  area.x = 0;
  area.y = 0;
  area.width  = widget->allocation.width;
  area.height = widget->allocation.height;

  if (state & GDK_BUTTON1_MASK) {
    /* drag in progress, simulate trackball */
	 /* in ../src/matrix.c - not part of the api */
	 void make_matrix( float rotx, float roty, float rotz,
							 float scale, float transx, float transy, float transz,
							 float mat[4][4] );
	 float xangle,yangle,view[7], matrix[4][4];

	 yangle = (float) (x - info->beginx) * 200.0/ (float) area.width;
	 xangle = (float) (y - info->beginy) * 200.0/ (float) area.height;

	 make_matrix(xangle, yangle, 0.0, 1.0, 0., 0., 0., matrix);
	 vis5d_matrix_mult(info->v5d_display_context, matrix);

	 glarea_draw(widget,NULL,NULL);
  }

  if (state & GDK_BUTTON2_MASK) {
    /* translating drag */
	 float view[7];
	 vis5d_get_view(info->v5d_display_context,view,view+1,
						 view+2,view+3,view+4,view+5,view+6);

	 vis5d_set_view(info->v5d_display_context,
						 view[0],view[1],view[2],view[3],
						 view[4]+(info->beginx-x)*(-2.0/area.width),
						 view[5]+(info->beginy-y)*(2.0/area.height),view[6]);

	 glarea_draw(widget,NULL,NULL);
	 
  }
  if (state & GDK_BUTTON3_MASK) {
    /* zooming drag */
    info->zoom += ((float) (y - info->beginy) / (float) area.height) ;
	
	 vis5d_set_camera(info->v5d_display_context,0.0,0.0,info->zoom);
	 glarea_draw(widget,NULL,NULL);
  }
  info->beginx = x;
  info->beginy = y;

  return TRUE;
}

gint vis5d_do_work_gtk( gpointer data )
{

  vis5d_do_work();
  return 1;

}
	 
 

gint _glarea_draw(gpointer infoptr)
{
  int redraw;
  v5d_info *info=(v5d_info *) infoptr;
  
  /* Clear the drawing color buffer and depth buffers */
  /* before drawing.                                  */
  if(info->timeout_id==0){
    printf("glarea_draw \n");
	 return TRUE;
  }
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  
  if(info->animate){
	 info->timestep += (info->animate*info->stepsize);
	 if(info->timestep<0){
		info->timestep = info->numtimes+info->timestep;
	 }else if(info->timestep>=info->numtimes){
		info->timestep = info->timestep-info->numtimes;
	 }
	 /* 
	 vis5d_make_timestep_graphics(info->v5d_display_context, info->timestep);
	 */
	 vis5d_set_dtx_timestep(info->v5d_display_context  ,info->timestep);
	 
	 redraw=1;
  }else{
	 vis5d_check_redraw( info->v5d_display_context, &redraw );
  }


  if(redraw){
	 vis5d_draw_frame(info->v5d_display_context,info->animate);
	 /* is definitely recommended! Take a look at the red    */
	 /* book if you don't already have an understanding of   */
	 /* single vs. double buffered windows.                  */
	 gtk_gl_area_swapbuffers (GTK_GL_AREA(info->GtkGlArea));
  }

#ifdef SINGLE_TASK		
	 vis5d_do_work_gtk(NULL);
#endif
  return TRUE;
}


/*****************************************************************************/
/*                                                                           */
/* Function: glarea_draw (GtkWidget*, GdkEventExpose*)                       */
/*                                                                           */
/* This is the function that should render your scene to the GtkGLArea. It   */
/* can be used as a callback to the 'Expose' event.                          */
/*                                                                           */
/*****************************************************************************/

gboolean glarea_draw (GtkWidget* widget, GdkEventExpose* event, gpointer user_data) {

  /* Draw only on the last expose event. */
  /* If event is null called from other than a callback */
  v5d_info *info;
  if (event && event->count > 0) {
    return(TRUE);
  }


  info = (v5d_info *)gtk_object_get_data(GTK_OBJECT(gtk_widget_get_toplevel(widget)),"v5d_info");

  if(info)  
	 vis5d_signal_redraw(info->v5d_display_context,1);

  /* gtk_gl_area_make_current MUST be called before rendering */
  /* into the GtkGLArea.                                      */

  if (gtk_gl_area_make_current(GTK_GL_AREA(widget))) {
	 _glarea_draw(info);
  }

  return (TRUE);

}

/*****************************************************************************/
/*                                                                           */
/* Function: glarea_reshape (GtkWidget*, GdkEventConfigure*)                 */
/*                                                                           */
/* This function performs the operations needed to maintain the viewing area */
/* of the GtkGLArea. This should be called whenever the size of the area     */
/* is changed.                                                               */
/*                                                                           */
/*****************************************************************************/

gboolean glarea_reshape (GtkWidget* widget, GdkEventConfigure* event, gpointer user_data) 
{
  int w, h;

  w = widget->allocation.width;
  h = widget->allocation.height;

  /* gtk_gl_area_make_current MUST be called before rendering */
  /* into the GtkGLArea.                                      */

  if (gtk_gl_area_make_current (GTK_GL_AREA(widget))) {
	 v5d_info *info = (v5d_info *)gtk_object_get_data(GTK_OBJECT(gtk_widget_get_toplevel(widget)),"v5d_info");
	 if(info) {
		vis5d_resize_3d_window(info->v5d_display_context,w,h);
		vis5d_signal_redraw(info->v5d_display_context,1);
	 }
  }
  return (TRUE);

}

/*****************************************************************************/
/*                                                                           */
/* Function: glarea_init (GtkWidget*)                                        */
/*                                                                           */
/* This function is a callback for the realization of the GtkGLArea widtget. */
/* You should do any OpenGL initialization here.                             */
/*                                                                           */
/*****************************************************************************/


void glarea_init (GtkWidget* widget, gpointer user_data) {

  /* gtk_gl_area_make_current MUST be called before rendering */
  /* into the GtkGLArea.                                      */
  GLXContext glcontext;
  Display *Xdisplay;
  Window  Xwindow;

  if (gtk_gl_area_make_current (GTK_GL_AREA(widget))) {
	 GtkWidget *window3D;

	 /* set up v5d_info */
	 v5d_info *info ;

	 window3D = gtk_widget_get_toplevel(widget);
	 
	 info = g_new0(v5d_info,1);

	 info->stepsize=1;
	 info->v5d_display_context=-1;
	 info->GtkGlArea=widget;
	 info->vinfo_array = g_ptr_array_new();

	 /* set pointers to the info structure from the glarea */
	 gtk_object_set_data(GTK_OBJECT(window3D), "v5d_info", info);

	 /* initialize vis5d */
	 {
		/* from gdkgl.h */
		typedef struct _GdkGLContextPrivate GdkGLContextPrivate;
		struct _GdkGLContextPrivate {
		  Display    *xdisplay;
		  GLXContext glxcontext;
		  guint ref_count;
		};

		GdkGLContext *gdkglcontext;

		gdkglcontext = GTK_GL_AREA(widget)->glcontext;
      
		glcontext = ((GdkGLContextPrivate *) gdkglcontext)->glxcontext;
		Xdisplay = ((GdkGLContextPrivate *) gdkglcontext)->xdisplay;
		Xwindow = GDK_WINDOW_XWINDOW(widget->window);

		info->zoom = 1;
		info->animate_speed=500;
		if( vis5d_initialized==FALSE){
		  vis5d_initialize(0);
		  /*
			 vis5d_set_verbose_level(VERBOSE_DISPLAY);
		  */
		  vis5d_noexit(1);
		  vis5d_initialized=TRUE;
		}
		info->v5d_display_context = vis5d_alloc_display_context();

		vis5d_set_BigWindow(Xdisplay, Xwindow, glcontext);
		
		vis5d_init_opengl_window(info->v5d_display_context,Xdisplay, Xwindow, glcontext);

		vis5d_init_path(DATA_PREFIX);

		vis5d_graphics_mode(info->v5d_display_context,VIS5D_BOX,VIS5D_ON);
		vis5d_graphics_mode(info->v5d_display_context,VIS5D_CLOCK,VIS5D_ON);
		vis5d_graphics_mode(info->v5d_display_context,VIS5D_MAP,VIS5D_ON);

		vis5d_alpha_mode(info->v5d_display_context,VIS5D_ON );
		vis5d_set_logo_size(info->v5d_display_context, 0.0);

		vis5d_signal_redraw(info->v5d_display_context,3);


		info->timeout_id = gtk_timeout_add(info->animate_speed, (GtkFunction) (_glarea_draw),(gpointer) info);

	 }

  }

  return ;

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
on_irreg_variable_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gint i;
  GtkWidget *window3D;
  v5d_var_info *vinfo = (v5d_var_info *) user_data;
  
  window3D = lookup_widget(GTK_WIDGET(menuitem),"window3D");

  if(GTK_CHECK_MENU_ITEM(menuitem)->active ){
	 vis5d_enable_irregular_graphics(vinfo->v5d_data_context,VIS5D_TEXTPLOT ,VIS5D_ON);
	 vis5d_set_text_plot( vinfo->v5d_data_context, vinfo->varid, 1.,10.,10.,1.);
	 for(i=0;i<vinfo->numtimes;i++)
		vis5d_make_text_plot( vinfo->v5d_data_context,i, i==vinfo->info->timestep);

	 gtk_object_set_data(GTK_OBJECT(window3D),"itx_context",
								GINT_TO_POINTER(vinfo->v5d_data_context));

  }else{
	 vis5d_enable_irregular_graphics(vinfo->v5d_data_context,VIS5D_TEXTPLOT ,VIS5D_OFF);

	 gtk_object_remove_data(GTK_OBJECT(window3D),"itx_context");

  }


}
void
on_variable_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  /* JPE This function is a callback for the variable menuitem
     it may also be called from ProcedureDialog in which case menuitem==NULL 
  */

  v5d_var_info *vinfo = (v5d_var_info *) user_data;

  if(! vinfo->VarGraphicsDialog){
	 gchar title[80];
	 vinfo->VarGraphicsDialog = new_VarGraphicsControls();	 
	 
	 g_snprintf(title,80,_("%s Variable Graphics Controls"),vinfo->vname);

	 gtk_window_set_title(GTK_WINDOW(vinfo->VarGraphicsDialog),title);
								 

	 gtk_object_set_data(GTK_OBJECT(vinfo->VarGraphicsDialog),"v5d_var_info",(gpointer) vinfo);

	 if(vinfo->maxlevel==1){
		GtkWidget *notebook;
		int i;
		notebook = lookup_widget(vinfo->VarGraphicsDialog,"notebook3");
		/* get rid of all but the horizontal contour pages */
		for(i=5;i>1;i--){
		  gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), i);

		}
		/* perhaps we want to use these for 2d variables? (lui does allow movement) */
		gtk_widget_destroy(lookup_widget(vinfo->VarGraphicsDialog,"hsvbox"));
		gtk_widget_destroy(lookup_widget(vinfo->VarGraphicsDialog,"chvbox"));
	 }else{
		gtk_widget_set_sensitive(lookup_widget(vinfo->VarGraphicsDialog,"Vslicebutton"),TRUE);
		gtk_widget_set_sensitive(lookup_widget(vinfo->VarGraphicsDialog,"CVslicebutton"),TRUE);
		/* 
		gtk_widget_set_sensitive(lookup_widget(vinfo->VarGraphicsDialog,"Isosurfbutton"),TRUE);
		gtk_widget_set_sensitive(lookup_widget(vinfo->VarGraphicsDialog,"Volumebutton"),TRUE);
		*/
	 }
  }
  if(menuitem)
	 gtk_widget_show(vinfo->VarGraphicsDialog);

}


GtkWidget *create_variables_menu(GtkWidget *window3D, GtkWidget *parent, const gchar *name)
{
  GtkWidget *variables_menu;

  variables_menu = gtk_menu_new ();

  gtk_widget_set_name (variables_menu, name);
  gtk_widget_ref (variables_menu);
  gtk_object_set_data_full (GTK_OBJECT (window3D), name, variables_menu,
									 (GtkDestroyNotify) gtk_widget_unref);

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (parent), variables_menu);

  gtk_widget_show(GTK_WIDGET(variables_menu));
  return variables_menu;
}


void
variable_menu_add_variable(GtkWidget *window3D, v5d_var_info *vinfo)
{
  GtkWidget *variables_menu, *variable, *tearoff, *parent_item,
	 *parent_menu, *menu;
  gpointer tmp;
  int ctxcnt;
  const char *menus[]={ "vars2D_menu", "vars3D_menu","irregular_menu"};
  const char *vars[] ={ "vars2D", "vars3D","irregular"};

  char parent_menu_name[20];  
  char tearoffname[18];
  typedef enum {V2D, V3D, VIRREG} vtype;
  vtype myvtype;

  switch (vinfo->maxlevel){
  case 0:
    myvtype = VIRREG;
    break;
  case 1:
    myvtype = V2D;
    break;
  default:
    myvtype = V3D;
    break;
  }
  /* Do I need this 
  vis5d_get_num_of_data_sets_in_display(vinfo->info->v5d_display_context, ctxcnt);
  */
  variables_menu = gtk_object_get_data(GTK_OBJECT(window3D),menus[myvtype]);
  
  if(!variables_menu){
	 menu = lookup_widget(window3D,vars[myvtype]);
	 if(!menu){
		printf("ERROR: this widget should be here %s\n",vars[myvtype]);
		exit(-1);
	 }
	 gtk_widget_set_sensitive(menu,TRUE);

	 gtk_widget_show(menu);

	 if(myvtype == VIRREG){
		GtkWidget *new_TextPlotDialog(GtkWidget *window);
		GtkWidget *TextPlotDialog = new_TextPlotDialog(window3D);

		gtk_object_set_data_full(GTK_OBJECT(TextPlotDialog),
										 "window3D",window3D,
										 (GtkDestroyNotify) gtk_widget_unref );
		TextPlotDialog_add_variable(TextPlotDialog,vinfo);
		/* since only textplots are allowed for irregular data
			we can return here */
		return;
	 }

	 variables_menu = create_variables_menu(window3D,menu,menus[myvtype]);
	 gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu), variables_menu);

    tearoff =  gtk_tearoff_menu_item_new();
	 gtk_widget_ref(tearoff);
	 gtk_widget_show(tearoff);
	 gtk_container_add (GTK_CONTAINER (variables_menu), tearoff);
  }else{
	 variables_menu = GTK_WIDGET(variables_menu);
  }

  
  if(myvtype == VIRREG){
	 GtkWidget *TextPlotDialog = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(window3D),"TextPlotDialog"));
	 TextPlotDialog_add_variable(TextPlotDialog,vinfo);

	 return;
	 /*	 
	 variable = gtk_check_menu_item_new_with_label (vinfo->vname);
	 gtk_check_menu_item_set_show_toggle (GTK_CHECK_MENU_ITEM (variable), TRUE);
	 gtk_signal_connect (GTK_OBJECT (variable), "activate",
								GTK_SIGNAL_FUNC (on_irreg_variable_activate),
								(gpointer) vinfo);
	 */
  }else{
	 variable = gtk_menu_item_new_with_label (vinfo->vname);
	 gtk_signal_connect (GTK_OBJECT (variable), "activate",
								GTK_SIGNAL_FUNC (on_variable_activate),
								(gpointer) vinfo);
  }

  gtk_widget_ref (variable);
  gtk_widget_show (variable);
  gtk_container_add (GTK_CONTAINER (variables_menu), variable);

}

void
on_change_animate_speed                (GtkButton       *button,
                                        gpointer         user_data)
{
  v5d_info *info;
  GtkWidget *window3D;
  /* user_data == 0 -> slower ; 1 -> faster */
  gint faster = GPOINTER_TO_INT(user_data);

  window3D = lookup_widget(GTK_WIDGET(button),"window3D");
  info = gtk_object_get_data(GTK_OBJECT(window3D),"v5d_info");

  if(!(info && info->timeout_id ))
	 return;

  if(faster){
	 /* set a reasonable limit on speed (may not be achievable on all platforms) */
	 /* 16 is about 60 frames per second                                         */
	 if(info->animate_speed > 16) 
		info->animate_speed*=0.5; 	
  }else{
	 info->animate_speed*=2.0; 	 
  }

  gtk_timeout_remove(info->timeout_id);
  info->timeout_id = gtk_timeout_add(info->animate_speed, (GtkFunction) (_glarea_draw),(gpointer) info);
}


void
on_vars2D_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_irregular_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  /* irregular data in vis5d is limited to textplots at this time */

  GtkWidget *textplotdialog, *window3D;
  /*  
  window3D = lookup_widget(GTK_WIDGET(menuitem),"window3D");
  */
  textplotdialog = lookup_widget(GTK_WIDGET(menuitem),"TextPlotDialog");
  gtk_widget_show(textplotdialog);

}


void
on_vars3D_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}




