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


extern GtkWidget *FileSelectionDialog;
extern GtkWidget *FontSelectionDialog;
gboolean vis5d_initialized=FALSE;

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
	 
  gtk_widget_set_sensitive(lookup_widget(window3D,"vars2d"),FALSE);
  gtk_widget_set_sensitive(lookup_widget(window3D,"vars3d"),FALSE);

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
/*
void
on_hslice_activate (GtkMenuItem     *menuitem,
							 gpointer         user_data)
{
  GtkWidget *window3D;
  v5d_info *info;

  window3D = lookup_widget(GTK_WIDGET(menuitem),"window3D");
  info = gtk_object_get_data(GTK_OBJECT(window3D),"v5d_info");
  if(info)
 	 gtk_widget_show(info->HSliceControls);

}
*/
void
on_chslice_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_vslice_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}



void
on_isosurface_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_volume_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

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
	 float xangle,yangle,view[7];

	 yangle = (x - info->beginx) * 200.0/ area.width;
	 xangle = (y - info->beginy) * 200.0/ area.height;

	 vis5d_get_view(info->v5d_display_context,view,view+1,
						 view+2,view+3,view+4,view+5,view+6);

	 vis5d_set_view(info->v5d_display_context,
						 xangle+view[0],yangle+view[1],view[2],view[3],
						 view[4],view[5],view[6]);

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
    info->zoom += ((y - info->beginy) / area.height) ;
	 /*    if (info->zoom < 0.5) info->zoom = 0.5;
			 if (info->zoom > 120) info->zoom = 120;
	 */
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
	 vis5d_make_timestep_graphics(info->v5d_display_context, info->timestep);
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

	 
	 info = (v5d_info *) g_malloc(sizeof(v5d_info));

	 info->graph_label_list=NULL;
	 info->beginx = 0;
	 info->beginy = 0;
	 info->animate=0;
	 info->stepsize=1;
	 info->v5d_display_context=-1;
	 info->VarSelectionDialog=NULL;
	 info->GtkGlArea=widget;

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
on_variable_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  v5d_var_info *vinfo = (v5d_var_info *) user_data;

  if(vinfo->VarGraphicsDialog){
	 /* the dialog already exist what to do ? */
	 gtk_widget_show(vinfo->VarGraphicsDialog);
  }else{
	 vinfo->VarGraphicsDialog = new_VarGraphicsControls();	 
	 
	 gtk_widget_hide_on_delete(vinfo->VarGraphicsDialog);

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
	 gtk_widget_show(vinfo->VarGraphicsDialog);
  }

}


void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

void
variable_menu_add_variable(GtkWidget *window3D, v5d_var_info *vinfo)
{
  GtkWidget *variables_menu, *variable, *tearoff;
  gpointer tmp;

  if(vinfo->maxlevel == 1){
	 variables_menu = lookup_widget(window3D,"vars2d_menu");
	 gtk_widget_set_sensitive(lookup_widget(window3D,"vars2d"),TRUE);
	 tmp = gtk_object_get_data(GTK_OBJECT(window3D),"tearoff_2d");
	 if(tmp)
		tearoff = GTK_WIDGET(tmp);
	 else{
		variable = lookup_widget(window3D,"var2d");
		if(variable){
		  gtk_widget_destroy(variable);  
		  gtk_object_remove_data(GTK_OBJECT(window3D),"var2d");
		}
		tearoff =  gtk_tearoff_menu_item_new();
		gtk_widget_ref(tearoff);
		gtk_widget_show(tearoff);
		gtk_container_add (GTK_CONTAINER (variables_menu), tearoff);
		gtk_object_set_data_full(GTK_OBJECT(window3D), "tearoff_2d", 
										 tearoff, (GtkDestroyNotify) gtk_widget_unref);
		
	 }
  }
  else{
	 variables_menu = lookup_widget(window3D,"vars3d_menu");
	 gtk_widget_set_sensitive(lookup_widget(window3D,"vars3d"),TRUE);
	 tmp = gtk_object_get_data(GTK_OBJECT(window3D),"tearoff_3d");
	 if(tmp)
		tearoff = GTK_WIDGET(tmp);
	 else{
		variable = lookup_widget(window3D,"var3d");
		if(variable){
		  gtk_widget_destroy(variable);
		  gtk_object_remove_data(GTK_OBJECT(window3D),"var3d");
		}
		tearoff =  gtk_tearoff_menu_item_new();
		gtk_widget_ref(tearoff);
		gtk_widget_show(tearoff);
		gtk_container_add (GTK_CONTAINER (variables_menu), tearoff);
		gtk_object_set_data_full(GTK_OBJECT(window3D), "tearoff_3d", 
										 tearoff, (GtkDestroyNotify) gtk_widget_unref);
	 }
  }
  

  /* get rid of the glade generated place holder and add tearoff */
  /* glade cannot handle tearoffs at this time                   */

  variable = gtk_menu_item_new_with_label (vinfo->vname);
  gtk_widget_ref (variable);
  gtk_widget_show (variable);
  gtk_container_add (GTK_CONTAINER (variables_menu), variable);

  gtk_signal_connect (GTK_OBJECT (variable), "activate",
                      GTK_SIGNAL_FUNC (on_variable_activate),
                      (gpointer) vinfo);

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
  printf("Setting animate speed to %f frames per second\n",1000.0/ (float) info->animate_speed);

  gtk_timeout_remove(info->timeout_id);
  info->timeout_id = gtk_timeout_add(info->animate_speed, (GtkFunction) (_glarea_draw),(gpointer) info);
}

