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

typedef enum {
  HSLICE=0,
  CHSLICE,
  VSLICE,
  CVSLICE,
  ISOSURF,
  VOLUME
} varcheckbutton;


GdkColor *
toggle_hslice(v5d_var_info *vinfo, gboolean toggleon)
{
  int times, numtimes,curtime;
  gchar labelstring[80];
  gchar *listentry[1];
  gchar varname[10];
  GdkColor *gcolor=NULL;
  GList *newnode;
  gchar *nstr[1];

  GtkWidget *HSliceControls;

  if(toggleon){
	 vis5d_get_ctx_numtimes( vinfo->v5d_data_context, &numtimes );
	 vis5d_get_ctx_timestep( vinfo->v5d_data_context,  &curtime);
	 for ( times = 0; times < numtimes; times++){
		vis5d_make_hslice( vinfo->v5d_data_context, times, vinfo->varid, times==curtime);
	 }

  
	 vis5d_get_ctx_var_name(vinfo->v5d_data_context,vinfo->varid,varname);


	 /*  
	 g_snprintf(labelstring,80,_("HSlice from %4.4g to %4.4g by %4.4g at %4.4g MB"),
					low,high,interval,pressure);
	 */

	 vis5d_enable_graphics(vinfo->v5d_data_context, VIS5D_HSLICE,
								  vinfo->varid, VIS5D_ON);

	 update_hslice_controls(vinfo);

  }else{
	 vis5d_enable_graphics(vinfo->v5d_data_context, VIS5D_HSLICE,
								  vinfo->varid, VIS5D_OFF);
  }
  return gcolor;

}



void
on_varbutton_toggled (GtkToggleButton *togglebutton,
							  gpointer         user_data)
{
  varcheckbutton which;
  v5d_var_info *vinfo;
  gboolean on;
  GdkColor *gcolor;

  which = GPOINTER_TO_INT(user_data);

  vinfo = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(togglebutton),"v5d_var_info");
  
  on = gtk_toggle_button_get_active(togglebutton);
  printf("here %d %d %d\n",which, vinfo->varid,on);
  switch(which){
  case HSLICE:
	 toggle_hslice(vinfo, on);
	 break;
  }

}

void
add_variable_toolbar(GtkWidget *VarDialog, v5d_var_info *vinfo){
  gchar varname[11];
  GtkWidget *VarNameLabel, *toolbar, *togglebutton, *vbox;
  int i;
  gchar *names[] = {"hslice","chslice","vslice","cvslice","isosurf","volume"};
  gchar *tooltip[] = {_("Horizontal Contour Slice"),
							 _("Shaded Horizontal Contour Slice"),
							 _("Vertical Contour Slice"),
							 _("Shaded Vertical Contour Slice"),
							 _("Isosurface"),
							 _("Volume")};

  toolbar = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_TEXT);
  gtk_widget_ref (toolbar);

  gtk_widget_show (toolbar);
  
  vbox = lookup_widget(VarDialog,"VarVBox");

  gtk_box_pack_start (GTK_BOX (vbox), toolbar, FALSE, FALSE, 0);

  vis5d_get_ctx_var_name(vinfo->v5d_data_context,vinfo->varid,varname);
  varname[10]='\0';
  for(i=strlen(varname); i<10; i++){
	 varname[i]=' ';
  }

  VarNameLabel = gtk_label_new (varname);
  gtk_widget_ref (VarNameLabel);

  gtk_widget_show (VarNameLabel);

  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar), VarNameLabel, NULL, NULL);
  gtk_misc_set_padding (GTK_MISC (VarNameLabel), 10, 0);

  for(i=0;i<6;i++){
	 GtkWidget *label, *togglebutton;
	 togglebutton = gtk_toggle_button_new_with_label (names[i]);
	 gtk_widget_set_usize (togglebutton, 20, 20);
	 label = GTK_BIN(togglebutton)->child;
	 gtk_widget_hide(label);
	 
	 gtk_widget_ref (togglebutton);
	 gtk_widget_show(togglebutton);
	 gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar), togglebutton,tooltip[i], NULL);
    
	 gtk_object_set_data(GTK_OBJECT (togglebutton), "v5d_var_info",(gpointer) vinfo);

	 gtk_signal_connect (GTK_OBJECT (togglebutton), "toggled",
								GTK_SIGNAL_FUNC (on_varbutton_toggled),
								GINT_TO_POINTER(i));
  }
  
}
