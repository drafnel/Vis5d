#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "api.h"
#include "textplot.h"
#include "tp_interface.h"
#include "support.h"
#include "window3D.h"
#include "graph_labels.h"

extern GtkWidget *ColorSelectionDialog;


GtkWidget *new_TextPlotDialog(GtkWidget *window)
{
  GtkWidget *TextPlotDialog;

  TextPlotDialog = gtk_object_get_data(GTK_OBJECT(window),"TextPlotDialog");
  if(TextPlotDialog==NULL){

	 TextPlotDialog = create_TextPlotDialog();

	 gtk_widget_ref(TextPlotDialog);
	 gtk_object_set_data_full(GTK_OBJECT(window),
									  "TextPlotDialog",TextPlotDialog,
									  (GtkDestroyNotify) gtk_widget_unref );
  }
  return TextPlotDialog;

}

void 
textplot_label(v5d_var_info *vinfo)
{
  gchar text[100], name[15];
  graph_label *label;

  vis5d_get_itx_var_name(vinfo->v5d_data_context,vinfo->varid, name);

  g_snprintf(text,100,"SYNOP %s ",name);

  if(vinfo->textplot->label == NULL){
	 vinfo->textplot->label = add_label(vinfo->info,text , TEXTPLOT);
  }
  label = vinfo->textplot->label;
  
  if(vinfo->textplot->color[0]==-1){
	 vis5d_get_color( vinfo->info->v5d_display_context, VIS5D_TEXTPLOT, 
							vinfo->varid, 
							vinfo->textplot->color,
							vinfo->textplot->color+1, 
							vinfo->textplot->color+2, 
							vinfo->textplot->color+3);
  }
  vis5d_set_color( vinfo->info->v5d_display_context, VIS5D_LABEL, 
						 label->labelid, 
						 vinfo->textplot->color[0],
						 vinfo->textplot->color[1],
						 vinfo->textplot->color[2],
						 vinfo->textplot->color[3]);

  label->data = (gpointer) vinfo;

  update_graph_labels(vinfo->info);
  
}


void
on_irreg_variable_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gint i, active;
  GtkWidget *window3D;
  v5d_var_info *vinfo = (v5d_var_info *) user_data;
  v5d_var_info *prev_vinfo=NULL;
  
  window3D = lookup_widget(GTK_WIDGET(menuitem),"window3D");
  prev_vinfo = gtk_object_get_data(GTK_OBJECT(window3D),"active_textplot_var");

  if(prev_vinfo && prev_vinfo->textplot && prev_vinfo!=vinfo)
  {
	 delete_label(prev_vinfo->info, prev_vinfo->textplot->label);
	 prev_vinfo->textplot->label=NULL;
	 vinfo->textplot = prev_vinfo->textplot;
	 prev_vinfo->textplot=NULL;
  }

  if(vinfo->textplot==NULL){
	 vinfo->textplot = g_new(textplotcontrols, 1);
	 vinfo->textplot->var=NULL;
	 vinfo->textplot->label=NULL;
	 vinfo->textplot->spacing=1.0;
	 vinfo->textplot->fontspace=1.0;
	 vinfo->textplot->fontx=10.;
	 vinfo->textplot->fonty=10.;
	 vinfo->textplot->color[0]=-1.;
  }
  


  vis5d_enable_irregular_graphics(vinfo->v5d_data_context,VIS5D_TEXTPLOT ,VIS5D_ON);
  
  vis5d_set_text_plot( vinfo->v5d_data_context, vinfo->varid,
							  vinfo->textplot->spacing,
							  vinfo->textplot->fontx,
							  vinfo->textplot->fonty,
							  vinfo->textplot->fontspace);
  
  for(i=0;i<vinfo->numtimes;i++)
	 vis5d_make_text_plot( vinfo->v5d_data_context,i, i==vinfo->info->timestep);

  if(vinfo->textplot->color[0]!=-1.0){
	 vis5d_set_color(vinfo->info->v5d_display_context, VIS5D_TEXTPLOT,
						  vinfo->varid,
						  vinfo->textplot->color[0], 
						  vinfo->textplot->color[1], 
						  vinfo->textplot->color[2], 
						  vinfo->textplot->color[3] );
  }

  textplot_label(vinfo);

  gtk_object_set_data(GTK_OBJECT(window3D),"active_textplot_var",(gpointer) vinfo);


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
  v5d_var_info *vinfo;
  gdouble color[4];
  gint i;

  vinfo = (v5d_var_info *) user_data;

  gtk_color_selection_get_color(GTK_COLOR_SELECTION(colorselection),color);

  for(i=0;i<4;i++)
	 vinfo->textplot->color[i] = (float) color[i];


  vis5d_set_color(vinfo->info->v5d_display_context, VIS5D_TEXTPLOT,
						vinfo->varid,
						vinfo->textplot->color[0], 
						vinfo->textplot->color[1], 
						vinfo->textplot->color[2], 
						vinfo->textplot->color[3] );

  vis5d_set_color( vinfo->info->v5d_display_context, VIS5D_LABEL,
						 vinfo->textplot->label->labelid, 
						 vinfo->textplot->color[0],
						 vinfo->textplot->color[1],
						 vinfo->textplot->color[2],
						 vinfo->textplot->color[3]);


  glarea_draw(vinfo->info->GtkGlArea, NULL, NULL);


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
  if(vinfo->textplot->color[0]==-1.){
	 vis5d_get_color(vinfo->info->v5d_display_context, VIS5D_TEXTPLOT, 
						  vinfo->varid,
						  vinfo->textplot->color,
						  vinfo->textplot->color+1,
						  vinfo->textplot->color+2,
						  vinfo->textplot->color+3);
  }
  for(i=0;i<4;i++){
	 color[i] = (gdouble) vinfo->textplot->color[i];
  }

  gtk_color_selection_set_color(GTK_COLOR_SELECTION(colorselection),color);

  gtk_signal_connect (GTK_OBJECT(colorselection), "color_changed",
							 GTK_SIGNAL_FUNC (on_tp_color_changed), (gpointer) vinfo);

  

}


void
on_tpclose_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *TextPlotDialog;

  TextPlotDialog = lookup_widget(GTK_WIDGET(button),"TextPlotDialog");
  gtk_widget_hide(TextPlotDialog);
}

