#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "textplot.h"
#include "tp_interface.h"
#include "support.h"

GtkWidget *new_TextPlotDialog(GtkWidget *window)
{
  GtkWidget *TextPlotDialog = create_TextPlotDialog();

  gtk_widget_ref(TextPlotDialog);
  gtk_object_set_data_full(GTK_OBJECT(window),
									"TextPlotDialog",TextPlotDialog,
									(GtkDestroyNotify) gtk_widget_unref );
  
  return TextPlotDialog;

}

void TextPlotDialog_add_variable(GtkWidget *TextPlotDialog, v5d_var_info *vinfo)
{
  GtkWidget *optionmenu, *menu, *item;

  optionmenu = lookup_widget(TextPlotDialog,"textplot_variables");
  menu = gtk_option_menu_get_menu(GTK_OPTION_MENU(optionmenu));

  item = gtk_menu_item_new_with_label(vinfo->vname);

  printf("Adding Item %s\n",vinfo->vname);

  gtk_menu_append(GTK_MENU(menu),GTK_MENU_ITEM(item));
  
  gtk_object_set_data(GTK_OBJECT(item),"vinfo",vinfo);

}
