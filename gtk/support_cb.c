/*
 * Vis5d+/Gtk user interface 
 * Copyright (C) 2001 James P Edwards
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "api.h"
#include "window3D.h"
#include "support_cb.h"
#include "interface.h"
#include "support.h"
#include "ProcedureDialog.h"

GtkWidget *FileSelectionDialog=NULL;
GtkWidget *FontSelectionDialog=NULL;
GtkWidget *ColorSelectionDialog=NULL;

void variable_ctree_add_var(GtkCTree *ctree, gchar *name, v5d_var_info *vinfo)
{
  GtkCTreeNode *node;
  gchar *nstr[1];
  nstr[0] = name;
  node = gtk_ctree_insert_node(ctree,NULL,NULL,nstr,0,NULL,NULL,NULL,NULL,1,0);
  gtk_ctree_node_set_row_data(ctree,node,(gpointer) vinfo);
}

void
load_data_file  (GtkWidget *window3D, gchar *filename)
{
  gint dc;
  gint numvars, i;
  
  v5d_var_info *vinfo;
  v5d_info *info;

  
  /* todo: should check for errors here */
  info = (v5d_info *) lookup_widget(window3D,"v5d_info");

  dc = vis5d_load_v5dfile(info->v5d_display_context,0,filename,"context");

  if(dc==VIS5D_FAIL){
	 /* TODO: message dialog - open failed */
	 return;
  }

  vis5d_get_dtx_timestep(info->v5d_display_context  ,&info->timestep);
  /* returns Numtimes - lasttime is one less */
  vis5d_get_dtx_numtimes(info->v5d_display_context, &info->numtimes);

  glarea_draw(info->GtkGlArea,NULL,NULL);

  vis5d_get_ctx_numvars(dc,&numvars);
  {
	 float vertargs[MAXVERTARGS];
	 vis5d_get_dtx_vertical(info->v5d_display_context, &(info->vcs), vertargs);
  }
  
  for(i=0;i < numvars; i++){
	 vinfo = (v5d_var_info *) g_malloc(sizeof(v5d_var_info));
	 
	 vinfo->hs = NULL;
	 vinfo->chs = NULL;
	 vinfo->varid=i;
	 vinfo->v5d_data_context=dc;
	 vinfo->info = info;
	 vis5d_get_ctx_var_name(dc,i,vinfo->vname);
	 vinfo->maxlevel = vis5d_get_levels(dc, i);
	 vinfo->VarGraphicsDialog=NULL;
	 
	 g_ptr_array_add(info->vinfo_array,(gpointer) vinfo);

	 variable_menu_add_variable(window3D, vinfo);
  }

  /* make the menu widgets sensitive */
  gtk_widget_set_sensitive(lookup_widget(window3D,"options1"),TRUE);
  gtk_widget_set_sensitive(lookup_widget(window3D,"procedures1"),TRUE); 
  gtk_widget_set_sensitive(lookup_widget(window3D,"variables"),TRUE);
  vis5d_get_dtx_numtimes(info->v5d_display_context, &info->numtimes);
  if(info->numtimes>1)
	 gtk_widget_set_sensitive(lookup_widget(window3D,"toolbar1"),TRUE);
	 
}


void
on_fileselect_ok                       (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *filesel, *window3D, *save_procedure;
  gint what;
  gchar *filename;
  v5d_info *info;
  int hires;
  filesel = gtk_widget_get_toplevel (GTK_WIDGET (button));
  filename = gtk_file_selection_get_filename (GTK_FILE_SELECTION (filesel));

  what =  GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(filesel), "OpenWhat"));

  window3D = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(filesel), "window3D"));

  if(window3D==NULL){
	 fprintf(stderr,"Could not find window3D widget\n");
	 exit ;
  }
  info = (v5d_info *) lookup_widget(window3D,"v5d_info");
  switch(what){
  case DATA_FILE:
	 load_data_file(window3D,filename);  
	 break;
  case TOPO_FILE:
	 hires = vis5d_graphics_mode(info->v5d_display_context,VIS5D_HIRESTOPO,VIS5D_GET);
	 vis5d_init_topo(info->v5d_display_context,filename,hires);
	 vis5d_load_topo_and_map(info->v5d_display_context);
	 break;
  case MAP_FILE:
	 vis5d_init_map(info->v5d_display_context,filename);
	 vis5d_load_topo_and_map(info->v5d_display_context);
	 break;
  case PROCEDURE_FILE:
	 if(info->ProcedureDialog)
		gtk_widget_destroy(info->ProcedureDialog);
    info->ProcedureDialog = new_ProcedureDialog(info, filename);
	 gtk_window_set_transient_for(GTK_WINDOW(info->ProcedureDialog),GTK_WINDOW(window3D));
	 break;
  case SAVE_PROCEDURE_FILE:
	 if(! info->ProcedureDialog){
		printf("Error: ProcedureDialog not defined\n");
		return;
	 }
	 save_procedure = lookup_widget(info->ProcedureDialog, "saveProcedure");
	 on_saveProcedure_activate(GTK_MENU_ITEM(save_procedure), filename);
	 break;
  default:
	 g_print("open what ? %d\n",what);
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

GtkWidget *new_ColorSelectionDialog()
{
  if(! ColorSelectionDialog)
	 {
		GtkWidget *colorselection;
		ColorSelectionDialog = create_colorselectiondialog1();
		colorselection = GTK_COLOR_SELECTION_DIALOG(ColorSelectionDialog)->colorsel;
		gtk_color_selection_set_opacity(GTK_COLOR_SELECTION(colorselection),TRUE);
	 }
  gtk_widget_show(ColorSelectionDialog);
  return ColorSelectionDialog;
}

void Invoke_VerifyDialog(gchar *label, GtkSignalFunc on_okay, gpointer okay_data,
								 GtkSignalFunc on_cancel, gpointer cancel_data)
{

  GtkWidget *VerifyDialog, *labelwidget, *cancel;

  VerifyDialog = create_VerifyDialog();

  labelwidget = lookup_widget(VerifyDialog,"label1");
  if(label)
	 gtk_label_set_text(GTK_LABEL(labelwidget), label);

  if(on_okay)
	 gtk_signal_connect (GTK_OBJECT (lookup_widget(VerifyDialog,"button1")), "clicked",
								on_okay,okay_data);
  if(on_cancel)
	 gtk_signal_connect (GTK_OBJECT (lookup_widget(VerifyDialog,"button2")), "clicked",
							 on_cancel,cancel_data);

  gtk_widget_show(VerifyDialog );
}
