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

void
load_data_file  (v5d_info *info, gchar *filename)
{
  gint dc;
  
  /* todo: should check for errors here */

  printf("on load display=%d\n",info->v5d_display_context);
  dc = vis5d_load_v5dfile(info->v5d_display_context,0,filename,"context");

  if(dc==VIS5D_FAIL){
	 /* TODO: message dialog - open failed */
	 return;
  }

  vis5d_get_dtx_timestep(info->v5d_display_context  ,&info->timestep);
  /* returns Numtimes - lasttime is one less */
  vis5d_get_dtx_numtimes(info->v5d_display_context, &info->numtimes);

  glarea_draw(info->GtkGlArea,NULL,NULL);

  if(info->VarSelectionDialog==NULL){
	 info->VarSelectionDialog = create_VarSelectionDialog();
	 /* set pointers to the info structure from the VarSelectionDialog */
	 gtk_object_set_data(GTK_OBJECT(info->VarSelectionDialog), "v5d_info", info);
  }
  gtk_widget_show(info->VarSelectionDialog);

  VarSelectionDialog_Append(info->VarSelectionDialog,info,filename,dc);

}


void
VarSelectionDialog_Append(GtkWidget *window,v5d_info *info, char *fname, int dc)
{
  GtkCTree *ctree;
  GtkCTreeNode *pnode, *node;
  gint numvars, i;
  gchar vname[10];
  gchar *nstr[1];
  v5d_var_info *vinfo;

  ctree = GTK_CTREE(lookup_widget(window,"VariableCTree"));

  
  nstr[0] = fname;

  pnode = gtk_ctree_insert_node(ctree,NULL,NULL,nstr,0,NULL,NULL,NULL,NULL,0,1);
  gtk_ctree_node_set_selectable(ctree,pnode,0);

  vis5d_get_ctx_numvars(dc,&numvars);

  for(i=0;i < numvars; i++){
	 vinfo = (v5d_var_info *) g_malloc(sizeof(v5d_var_info));
	 
	 vinfo->VarGraphicsDialog = NULL;
	 vinfo->varid=i;
	 vinfo->v5d_data_context=dc;
	 vinfo->info = info;
	 vis5d_get_ctx_var_name(dc,i,vname);
	 nstr[0] = vname;
	 node = gtk_ctree_insert_node(ctree,pnode,NULL,nstr,0,NULL,NULL,NULL,NULL,1,0);

	 gtk_ctree_node_set_row_data(ctree,node,(gpointer) vinfo);

  }

}




void
on_fileselect_ok                       (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *filesel, *window3D;
  gchar *what;
  gchar *filename;
  v5d_info *info;

  filesel = gtk_widget_get_toplevel (GTK_WIDGET (button));
  filename = gtk_file_selection_get_filename (GTK_FILE_SELECTION (filesel));

  what = (gchar *) gtk_object_get_data(GTK_OBJECT(filesel), "OpenWhat");

  window3D = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(filesel), "window3D"));

  if(window3D==NULL){
	 fprintf(stderr,"Could not find window3D widget\n");
	 exit ;
  }
  info = (v5d_info *) lookup_widget(window3D,"v5d_info");

  if(what==NULL) return;

  if(strcmp(what,"data")==0){
	 load_data_file(info,filename);  
  }else if(strcmp(what,"topo")==0){
	 int hires = vis5d_graphics_mode(info->v5d_display_context,VIS5D_HIRESTOPO,VIS5D_GET);
	 vis5d_init_topo(info->v5d_display_context,filename,hires);
	 vis5d_load_topo_and_map(info->v5d_display_context);
  }else if(strcmp(what,"map")==0){
	 vis5d_init_map(info->v5d_display_context,filename);
	 vis5d_load_topo_and_map(info->v5d_display_context);
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
on_topography1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  char v5dstr[V5D_MAXSTRLEN];
  GtkWidget *window3D;
  v5d_info *info;

  window3D = lookup_widget(GTK_WIDGET(menuitem),"window3D");
  if(!window3D) return;
  info = (v5d_info*) gtk_object_get_data(GTK_OBJECT(window3D), "v5d_info");
  if(!info) return;

  if(FileSelectionDialog == NULL)
	 FileSelectionDialog = create_fileselection1();

  gtk_window_set_title(GTK_WINDOW(FileSelectionDialog),_("Select Topography File"));
  gtk_grab_add(FileSelectionDialog);
  gtk_object_set_data(GTK_OBJECT(FileSelectionDialog),"OpenWhat" ,"topo");
  
  vis5d_get_topo(info->v5d_display_context , (char *) v5dstr);
  if(v5dstr[0]=='/'){
	 gtk_file_selection_set_filename(GTK_FILE_SELECTION(FileSelectionDialog),v5dstr);
  }else{
	 gtk_file_selection_set_filename(GTK_FILE_SELECTION(FileSelectionDialog),DATA_PREFIX );
  }
  gtk_widget_show (FileSelectionDialog);
  gtk_grab_add(FileSelectionDialog);
  gtk_window_set_transient_for(GTK_WINDOW(FileSelectionDialog),GTK_WINDOW(window3D));
}


void
on_map2_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  char v5dstr[V5D_MAXSTRLEN];
  GtkWidget *window3D;
  v5d_info *info;

  window3D = lookup_widget(GTK_WIDGET(menuitem),"window3D");
  if(!window3D) return;
  info = (v5d_info*) gtk_object_get_data(GTK_OBJECT(window3D), "v5d_info");
  if(!info) return;

  if(FileSelectionDialog == NULL)
	 FileSelectionDialog = create_fileselection1();

  gtk_window_set_title(GTK_WINDOW(FileSelectionDialog),_("Select Map File"));
  gtk_grab_add(FileSelectionDialog);
  gtk_object_set_data(GTK_OBJECT(FileSelectionDialog),"OpenWhat" ,"map");

  vis5d_get_map(info->v5d_display_context , (char *) v5dstr);
  if(v5dstr[0]=='/'){
	 gtk_file_selection_set_filename(GTK_FILE_SELECTION(FileSelectionDialog),v5dstr);
  }else{
	 gtk_file_selection_set_filename(GTK_FILE_SELECTION(FileSelectionDialog),DATA_PREFIX );
  }  
 
  gtk_widget_show (FileSelectionDialog);
  gtk_grab_add(FileSelectionDialog);
  gtk_window_set_transient_for(GTK_WINDOW(FileSelectionDialog),GTK_WINDOW(window3D));
}

