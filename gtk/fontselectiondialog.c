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

GtkWidget *FontSelectionDialog=NULL;

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
