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

GtkWidget* create_glarea (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2 ) {

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
  /*
  int x = event->x;
  int y = event->y;
  */
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
		info->timestep = info->numtimes-info->timestep;
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
	 GtkWidget *window3D, *clist;
	 GtkStyle *clist_style;
	 GtkRcStyle mystyle;

	 /* set up v5d_info */
	 v5d_info *info ;

	 window3D = gtk_widget_get_toplevel(widget);

	 
	 info = (v5d_info *) g_malloc(sizeof(v5d_info));

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

		vis5d_initialize(0);
		/*
		vis5d_set_verbose_level(VERBOSE_DISPLAY);
		*/
		vis5d_noexit(1);

		info->v5d_display_context = vis5d_alloc_display_context();

		vis5d_set_BigWindow(Xdisplay, Xwindow, glcontext);
		
		vis5d_init_opengl_window(info->v5d_display_context,Xdisplay, Xwindow, glcontext);

		vis5d_init_path(DATA_PREFIX);
		vis5d_graphics_mode(info->v5d_display_context,VIS5D_BOX,VIS5D_ON);
		vis5d_graphics_mode(info->v5d_display_context,VIS5D_CLOCK,VIS5D_OFF);
		vis5d_graphics_mode(info->v5d_display_context,VIS5D_MAP,VIS5D_ON);

		vis5d_signal_redraw(info->v5d_display_context,3);


		info->timeout_id = gtk_timeout_add(info->animate_speed, (GtkFunction) (_glarea_draw),(gpointer) info);

	 }

  }

  return ;

}
