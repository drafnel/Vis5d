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
