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


typedef struct {
  gint v5d_display_context;

  float beginx,beginy;  /* position of mouse */

  float zoom;           /* field of view in degrees */

  GtkWidget *VarSelectionDialog;
  GtkWidget *GtkGlArea;
  int animate;
  guint32 animate_speed;
  gint timeout_id;
  gint stepsize;
  int timestep;
  int numtimes;

} v5d_info;

typedef struct {
  gint v5d_data_context;
  gint varid;
  v5d_info *info;

  GtkWidget *VarGraphicsDialog;
  gint clistrow;
} v5d_var_info;


GtkWidget *fileselection=NULL;
GtkWidget *graph_menu=NULL;

void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *window3D;
  if(fileselection == NULL)
	 fileselection = create_fileselection1();

  /* This is the only window that should accept input */
  gtk_grab_add(fileselection);

  gtk_window_set_title(fileselection,_("Open Data File"));

  window3D=lookup_widget(GTK_WIDGET (menuitem),"window3D");

  gtk_object_set_data(GTK_OBJECT(fileselection),"window3D" , window3D);

  gtk_object_set_data(GTK_OBJECT(fileselection),"OpenWhat" , "data");

  gtk_widget_show (fileselection);

  gtk_window_set_transient_for(GTK_WINDOW(fileselection),GTK_WINDOW( window3D));
}


void
on_import1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_exit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

  v5d_info *info = (v5d_info*)gtk_object_get_data(GTK_OBJECT(lookup_widget(GTK_WIDGET(menuitem),"window3D")),"v5d_info");

  if(info->timeout_id){
	 gtk_timeout_remove(info->timeout_id);
  }
  free(info);

  vis5d_terminate(1);
  gtk_main_quit();

}


void
on_help1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

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
load_data_file  (v5d_info *info, gchar *filename)
{
  gint dc;
  
  /* todo: should check for errors here */

  dc = vis5d_load_v5dfile(info->v5d_display_context,0,filename,"context");

  if(dc==VIS5D_FAIL){
	 /* TODO: message dialog - open failed */
	 return;
  }

  vis5d_get_dtx_timestep(info->v5d_display_context  ,&info->timestep);
  /* returns Numtimes - lasttime is one less */
  vis5d_get_dtx_numtimes(info->v5d_display_context, &info->numtimes);

  glarea_draw(info->GtkGlArea,NULL,NULL);

  if(info->VarSelectionDialog==NULL){
	 info->VarSelectionDialog = create_VarSelectionDialog();
	 /* set pointers to the info structure from the VarSelectionDialog */
	 gtk_object_set_data(GTK_OBJECT(info->VarSelectionDialog), "v5d_info", info);
  }
  gtk_widget_show(info->VarSelectionDialog);

  VarSelectionDialog_Append(info->VarSelectionDialog,info,filename,dc);

}






void
on_fileselect_ok                       (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *filesel, *glarea, *window3D;
  gchar *what;
  gchar *filename;
  v5d_info *info;

  filesel = gtk_widget_get_toplevel (GTK_WIDGET (button));
  filename = gtk_file_selection_get_filename (GTK_FILE_SELECTION (filesel));

  what = (gchar *) gtk_object_get_data(GTK_OBJECT(filesel), "OpenWhat");

  window3D = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(filesel), "window3D"));
  printf(" window3d= 0x%x\n",(unsigned int) window3D);

  if(window3D==NULL){
	 fprintf(stderr,"Could not find window3D widget\n");
	 exit -1;
  }

  if(what==NULL) return;

  if(strcmp(what,"data")==0){
	 info = lookup_widget(window3D,"v5d_info");
	 load_data_file(info,filename);  
  }else if(strcmp(what,"topo")==0){
	 GtkWidget *Prefs, *topo_entry;

	 Prefs=GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(filesel),"PrefsDialog"));
	 topo_entry = lookup_widget(Prefs, "Topo_entry");
	 gtk_entry_set_text(GTK_ENTRY(topo_entry),filename);

  }else if(strcmp(what,"map")==0){
	 GtkWidget *Prefs, *map_entry;

	 Prefs=GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(filesel),"PrefsDialog"));
	 map_entry = lookup_widget(Prefs, "Map_entry");
	 gtk_entry_set_text(GTK_ENTRY(map_entry),filename);
  }
  gtk_widget_hide (filesel);
  /* This is the only window that should accept input */
  gtk_grab_remove(filesel);

}


void
on_fileselect_cancel                   (GtkButton       *button,
                                        gpointer         user_data)
{
  /* just hide the window instead of closing it */
  GtkWidget *filesel = gtk_widget_get_toplevel (GTK_WIDGET (button));
  gtk_widget_hide (filesel);
  gtk_grab_remove(filesel);
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
  if(fileselection == NULL)
	 fileselection = create_fileselection1();

  sprintf(title,_("Open %s File"),(gchar *) user_data);

  gtk_window_set_title(fileselection,title);
  gtk_grab_add(fileselection);

  /* TODO: Need to set the default directory? */
  Prefs = gtk_widget_get_toplevel (GTK_WIDGET (button));

  gtk_object_set_data(GTK_OBJECT(fileselection),"OpenWhat" ,user_data);

  
  if(! strncmp("map",(gchar *) user_data,3)){
	 char v5dstr[V5D_MAXSTRLEN];
	 v5d_info *info = (v5d_info*) gtk_object_get_data(GTK_OBJECT(Prefs), "v5d_info");
	 vis5d_get_map(info->v5d_display_context , (char *) v5dstr);
	 if(v5dstr[0]=='/'){
		gtk_file_selection_set_filename(fileselection,v5dstr);
	 }else{
		gtk_file_selection_set_filename(fileselection,DATA_PREFIX );
	 }
  }else if(! strncmp("topo",(gchar *) user_data,4)){
	 char v5dstr[V5D_MAXSTRLEN];
	 v5d_info *info = (v5d_info*) gtk_object_get_data(GTK_OBJECT(Prefs), "v5d_info");
	 vis5d_get_topo(info->v5d_display_context , (char *) v5dstr);
	 if(v5dstr[0]=='/'){
		gtk_file_selection_set_filename(fileselection,v5dstr);
	 }else{
		gtk_file_selection_set_filename(fileselection,DATA_PREFIX );
	 }
  }
  gtk_object_set_data(GTK_OBJECT(fileselection),"PrefsDialog" ,Prefs );

  gtk_widget_show (fileselection);
  gtk_grab_add(fileselection);
  gtk_window_set_transient_for(GTK_WINDOW(fileselection),GTK_WINDOW(Prefs));

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
	 GdkColor *black, *gcolor;
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
on_ColorSelectionOk_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_Cancel_Clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{

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



