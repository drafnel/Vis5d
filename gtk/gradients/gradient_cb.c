#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <stdio.h>
#include <gtk/gtk.h>

#include <gtkgl/gtkglarea.h>
#include "support_cb.h"
#include "window3D.h"
#include "support.h"
#include "interface.h"

#include "gradient_cb.h"
#include "gradient_header.h"
#include "gradient.h"
#include "gradientP.h"



GtkWidget *
new_GtkGradient()
{
  GtkWidget *GtkGradient, *clist;
  GdkGC *gc;
  gint select_pos;

  GtkGradient = create_GtkGradient();
  gtk_widget_show(GtkGradient);
  gradients_init(FALSE);
  clist = lookup_widget(GtkGradient, "clist1");

  gc = gdk_gc_new(GtkGradient->window);
  gdk_rgb_init();

  select_pos = gradient_clist_init (GtkGradient, gc,
												clist,
												gradients_get_standard_gradient());
  return GtkGradient;
}



void
on_Edit_clicked                        (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *TopLevel, *clist;

  TopLevel = lookup_widget(GTK_WIDGET(button), "GtkGradient");
  clist = lookup_widget(TopLevel, "clist1");


  printf("OOPS: This function is not yet implemented\n");
  
  
}



void
on_Close_clicked                       (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *TopLevel;

  TopLevel = lookup_widget(GTK_WIDGET(button), "GtkGradient");
  gtk_widget_destroy(TopLevel);
  
}


void
gradient_update_sample(preview_area *sample, gradient_t *curr_gradient)
{
  gint i, j;
  gdouble r,g,b,a;

  for(i=0;i<255;i++){
	 gradient_get_color_at(curr_gradient, (gdouble) i/254., &r, &g, &b, &a);
	 
	 sample->colors[i] = PACK_COLOR((gint) (r*255.0), (gint) (g*255.0), 
											  (gint) (b*255.0), (gint) (a*255.0));
	 
  }
}

void
gradient_update_sample_from_name(preview_area *sample, gchar *name)
{
  gradient_t *curr_gradient;

  if(! gradients_list)
	 gradients_init (FALSE);

  curr_gradient = gradient_list_get_gradient(gradients_list, name);

  gradient_update_sample(sample, curr_gradient);

}




void
on_clist1_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  GtkWidget *Gradient, *glarea;
  gradient_t *curr_gradient;
  preview_area *sample;

  curr_gradient = (gradient_t *) gtk_clist_get_row_data (clist, row); 
  
  Gradient = lookup_widget(GTK_WIDGET(clist),"GtkGradient");
  sample = (preview_area *) gtk_object_get_data(GTK_OBJECT(Gradient),"Sample");
  glarea = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(Gradient),"GtkGlArea"));

  if(! sample && sample->colors)
	 return;

  gradient_update_sample(sample, curr_gradient);

  gradient_preview_update(sample, FALSE);
  /*
  sample->name = g_strdup(curr_gradient->name);
  */
  if(glarea)
	 glarea_draw(glarea,NULL,NULL);
}


void gradient_preview_update(preview_area *sample, gboolean resize)
{
  gint x, y, i, wid, heig, f;
  guchar color[3];

  if (! GTK_WIDGET_DRAWABLE (sample->preview))
	 return;

  wid = sample->preview->allocation.width;
  heig = sample->preview->allocation.height;

  if (resize)
    {
		g_free (sample->buffer);
      sample->buffer = g_new(guchar, 3 * wid);
    }
  
  i = 0;
  if(sample->ncolors==1)
	 {
		color[0] = (guchar) UNPACK_RED(sample->colors[0]);
		color[1] = (guchar) UNPACK_GREEN(sample->colors[0]);
		color[2] = (guchar) UNPACK_BLUE(sample->colors[0]);
	 }
  
  for (y = 0; y < heig; y++)
    {
      i = 0;
      for (x = 0; x < wid; x++)
		  {
			 int j;
			 if(sample->ncolors>1 )
				{
				  f = x*255/wid;
				  color[0] = (guchar) UNPACK_RED(sample->colors[f]);
				  color[1] = (guchar) UNPACK_GREEN(sample->colors[f]);
				  color[2] = (guchar) UNPACK_BLUE(sample->colors[f]);
				}
			 for(j=0;j<3;j++)
				sample->buffer[i++] = color[j];

		  }
		
      gtk_preview_draw_row (GTK_PREVIEW (sample->preview), sample->buffer, 0, y, wid);
    }
  gtk_widget_queue_draw (sample->preview);
}
