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

GtkWidget *FileSelectionDialog=NULL;

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
  gchar vname[10];
  v5d_var_info *vinfo;
  v5d_info *info;
  GtkWidget *VarDialog;

  GtkCTree *hs_ctree;
  
  
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
  /* create but do not show tools */
  info->HSliceControls = create_HSliceControls();
  /* point back to info */  
  gtk_object_set_data(GTK_OBJECT(info->HSliceControls),"v5d_info",(gpointer) info);
  hs_ctree = GTK_CTREE(lookup_widget(info->HSliceControls,"hslicectree"));

  /*
  VarDialog = create_VarDialog();
  */
  for(i=0;i < numvars; i++){
	 vinfo = (v5d_var_info *) g_malloc(sizeof(v5d_var_info));
	 
	 vinfo->hc = NULL;
	 vinfo->varid=i;
	 vinfo->v5d_data_context=dc;
	 vinfo->info = info;
	 vis5d_get_ctx_var_name(dc,i,vname);
	 vinfo->maxlevel = vis5d_get_levels(dc, i);

	 variable_ctree_add_var(hs_ctree, vname, vinfo);
	 
    /*
	 add_variable_toolbar(VarDialog , vinfo);
	 */
  }
  /*
  gtk_widget_show(VarDialog);
  */
}


void
on_fileselect_ok                       (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *filesel, *window3D;
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


