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


GtkWidget *ColorSelectionDialog=NULL;

GtkWidget *new_ColorSelectionDialog()
{
  if(! ColorSelectionDialog)
	 {
		GtkWidget *colorselection;
		ColorSelectionDialog = create_colorselectiondialog1();
		colorselection = GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(ColorSelectionDialog)->colorsel);
		gtk_color_selection_set_opacity(colorselection,TRUE);
	 }
  gtk_widget_show(ColorSelectionDialog);
  return ColorSelectionDialog;
}


void
on_ColorSelectionOk_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_widget_hide(ColorSelectionDialog);
}


void
on_ColorSelectionCancel_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
  /* TODO: need to restore original color */
  gtk_widget_hide(ColorSelectionDialog);

}
