#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "api.h"
#include "textplot.h"
#include "tp_interface.h"
#include "support.h"
#include "window3D.h"

extern GtkWidget *ColorSelectionDialog;


GtkWidget *new_TextPlotDialog(GtkWidget *window)
{
  GtkWidget *TextPlotDialog = create_TextPlotDialog();

  gtk_widget_ref(TextPlotDialog);
  gtk_object_set_data_full(GTK_OBJECT(window),
									"TextPlotDialog",TextPlotDialog,
									(GtkDestroyNotify) gtk_widget_unref );
  
  return TextPlotDialog;

}


void
on_irreg_variable_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gint i;
  GtkWidget *window3D;
  v5d_var_info *vinfo = (v5d_var_info *) user_data;
  
  window3D = lookup_widget(GTK_WIDGET(menuitem),"window3D");

  vis5d_enable_irregular_graphics(vinfo->v5d_data_context,VIS5D_TEXTPLOT ,VIS5D_ON);
  
  vis5d_set_text_plot( vinfo->v5d_data_context, vinfo->varid, 1.,10.,10.,1.);
  
  for(i=0;i<vinfo->numtimes;i++)
	 vis5d_make_text_plot( vinfo->v5d_data_context,i, i==vinfo->info->timestep);

}


void TextPlotDialog_add_variable(GtkWidget *TextPlotDialog, v5d_var_info *vinfo)
{
  GtkWidget *optionmenu, *menu, *item;

  optionmenu = lookup_widget(TextPlotDialog,"textplot_variables");
  menu = gtk_option_menu_get_menu(GTK_OPTION_MENU(optionmenu));

  item = gtk_menu_item_new_with_label(vinfo->vname);

  gtk_menu_append(GTK_MENU(menu),item);

  gtk_signal_connect (GTK_OBJECT(item), "activate", 
							 GTK_SIGNAL_FUNC (on_irreg_variable_activate),
							 (gpointer) vinfo);
  
  gtk_object_set_data(GTK_OBJECT(item),"vinfo",vinfo);

  gtk_widget_show(item);

}

void
on_tp_sb_changed                     (GtkEditable     *editable,
                                        gpointer         user_data)
{
  typedef enum { SPACING, FONTX, FONTY, FONTSPACE} tp_options;
  tp_options option;
  float value[4];
  int var, index, i, numtimes, timestep;
  GtkWidget *GtkGlArea, *TextPlotDialog;

  TextPlotDialog = lookup_widget(GTK_WIDGET(editable),"TextPlotDialog");


  index = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(TextPlotDialog),
															 "itx_index"));

  option = GPOINTER_TO_INT(user_data);
  GtkGlArea = gtk_object_get_data(GTK_OBJECT(TextPlotDialog),"GtkGlArea");
  

  vis5d_get_text_plot( index, &var, value, value+2, value+3, value+1);

  printf("%d %f %f %f %f\n",option, value[0],value[1],value[2],value[3]);

  value[option] = gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(editable));

  printf("%d %f %f %f %f\n",option, value[0],value[2],value[3],value[1]);

  vis5d_set_text_plot(index, var, value[0], value[2],value[3],value[1]);

  vis5d_get_itx_timestep(index, &timestep);
  vis5d_get_itx_numtimes(index, &numtimes);


  for(i=0;i<numtimes;i++)
	 vis5d_make_text_plot( index,i, i==timestep);


  glarea_draw(GtkGlArea, NULL, NULL);
}

void
on_tp_color_changed                    (GtkColorSelection *colorselection,
                                        gpointer         user_data)
{
}

void
on_Color_clicked                       (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *ColorSelectionDialog, *colorselection, *variable;
  GtkWidget *optionmenu, *menu, *item;
  v5d_var_info *vinfo;
  float v5dcolor[4];
  gdouble color[4];
  int i;

  optionmenu = lookup_widget(GTK_WIDGET(button),"textplot_variables");
  menu = gtk_option_menu_get_menu(GTK_OPTION_MENU(optionmenu));
  item = gtk_menu_get_active(GTK_MENU(menu));
  
  vinfo = (v5d_var_info *) gtk_object_get_data(GTK_OBJECT(item),"vinfo");
  

  ColorSelectionDialog = new_ColorSelectionDialog();
  colorselection = GTK_COLOR_SELECTION_DIALOG(ColorSelectionDialog)->colorsel;

  vis5d_get_color(vinfo->info->v5d_display_context, VIS5D_TEXTPLOT, vinfo->varid,
						v5dcolor,v5dcolor+1,v5dcolor+2,v5dcolor+3);
  for(i=0;i<4;i++){
	 color[i] = (gdouble) v5dcolor[i];
  }

  gtk_color_selection_set_color(GTK_COLOR_SELECTION(colorselection),color);

  gtk_signal_connect (GTK_OBJECT(colorselection), "color_changed",
							 GTK_SIGNAL_FUNC (on_tp_color_changed), (gpointer) vinfo);

  

}



void
on_tpokay_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_tpcancel_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{

}




