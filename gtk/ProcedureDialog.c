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
#include <string.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "api.h"
#include "ProcedureDialog.h"
#include "PD_interface.h"
#include "support_cb.h"
#include "support.h"
#include "procedure.h"
#include "window3D.h"
#include "VarGraphicsControls.h"


v5d_var_info *vinfo_array_find_var_by_name(GPtrArray *vinfo_array, gchar *name)
{
  int i;
  v5d_var_info *vinfo;

  for(i=0;i<vinfo_array->len;i++)
	 {
		vinfo = (v5d_var_info *) g_ptr_array_index(vinfo_array,i);
		if(strncmp(name,vinfo->vname,10)==0)
		  return vinfo;
	 }
  return NULL;
}

void procedure_ctree_add_image(GtkCTree *ctree, 
										 GtkCTreeNode *pnode,
										 GPtrArray *vinfo_array, 
										 Image *image)
{
  gchar *nstr[3];
  GtkCTreeNode *node;
  gint i, type;
  v5d_var_info *vinfo;

  nstr[0] = image->name;
  nstr[1]=NULL;
  nstr[2]=NULL;


  node = gtk_ctree_insert_node(ctree,NULL,pnode,nstr,0,NULL,NULL,NULL,NULL,0,0);
  gtk_ctree_node_set_row_data(ctree,node,(gpointer) image);
  for(i=0;i<image->item_type->len;i++){
	 hslicecontrols *hs;
	 type = g_array_index(image->item_type,gint, i);
	 switch(type){
	 case HSLICE:
		nstr[0] = NULL;
		nstr[1] = g_strdup("HSlice");
		hs = (hslicecontrols *) g_ptr_array_index(image->items,i);
		nstr[2] = hs->var;
		vinfo = vinfo_array_find_var_by_name(vinfo_array, hs->var);
		
		g_ptr_array_add(image->vinfo_array,(gpointer) vinfo);
		
		gtk_ctree_node_set_selectable(ctree,
												gtk_ctree_insert_node(ctree,node,NULL,nstr,0,NULL,NULL,NULL,NULL,1,0),
												FALSE);
		break;
	 }
  }
}


GtkWidget * new_ProcedureDialog(v5d_info *info, gchar *filename)
{
  GtkWidget *PD;
  GtkCTree *ctree;
  GList *ProcedureList=NULL, *listitem;
  GtkWidget *paste;

  PD = create_ProcedureDialog();

  if(filename){
	 ProcedureList = procedure_open_file(filename);
  }
	 
  
  gtk_object_set_data(GTK_OBJECT(PD),"ProcedureList",
									(gpointer) ProcedureList);

  gtk_object_set_data(GTK_OBJECT(PD),"ProcedureFileName",(gpointer) filename);

  ctree = GTK_CTREE(lookup_widget(PD,"ProcedureCtree"));
  /* Hide the image sub columns until tree is expanded */

  gtk_clist_set_column_visibility (GTK_CLIST(ctree), 1, FALSE);
  gtk_clist_set_column_visibility (GTK_CLIST(ctree), 2, FALSE);


  listitem = ProcedureList;

  while(listitem!=NULL){

	 Image *image = (Image *) listitem->data;

	 procedure_ctree_add_image(ctree, NULL, info->vinfo_array, image);

	 listitem = g_list_next(listitem);
  }
  gtk_object_set_data(GTK_OBJECT(PD),"v5d_info",(gpointer) info);


  paste = lookup_widget(PD, "paste1");
  gtk_widget_set_sensitive(paste,FALSE);

  gtk_widget_show(PD);

  return PD;
}

void
vinfo_toggle_hslice_from_procedure(v5d_var_info *vinfo, hslicecontrols *hs, gint enable)
{
  GtkWidget *Hslicebutton;

  if(vinfo==NULL) {
	 /* TODO: var was not found by name in dataset - a dialog should be open which
		 gives the user the choice of selecting from the available variables 
		 or canceling this operation */
	 return;
  }

  /* the variable selected callback */
  if(enable)
	 on_variable_activate(NULL, vinfo);
  if(vinfo->VarGraphicsDialog)
	 Hslicebutton = lookup_widget(vinfo->VarGraphicsDialog,"Hslicebutton");


  if(Hslicebutton){
	 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Hslicebutton), enable);
  }
  if(! (enable && hs) ) 
	 return;

  if(hs->color[0]+hs->color[1]+hs->color[2]+hs->color[3] > 0){
	 int i;
	 for(i=0;i<4;i++)
		vinfo->hs->color[i] = hs->color[i];
	 
	 vis5d_set_color( vinfo->info->v5d_display_context, 
							VIS5D_HSLICE, vinfo->varid,
							(float) vinfo->hs->color[0], 
							(float) vinfo->hs->color[1], 
							(float) vinfo->hs->color[2], 
							(float) vinfo->hs->color[3] );
  }
  
  if(hs->max > hs->min){
	 if(hs->interval > 0)
		vinfo->hs->interval = hs->interval;
	 if(hs->level > 0)
		vinfo->hs->level = hs->level;
	 vinfo->hs->max = hs->max;
	 vinfo->hs->min = hs->min;
  }
  if(hs->height>0){
	 vinfo->hs->height = hs->height;
	 vis5d_height_to_gridlevel( vinfo->v5d_data_context,0, vinfo->varid, 
										 vinfo->hs->height, &vinfo->hs->level);
  }
  update_hslice_controls(vinfo, HSLICE);
  glarea_draw(vinfo->info->GtkGlArea, NULL, NULL);
}


void
on_ProcedureCtree_tree_select_row      (GtkCTree        *ctree,
                                        GList           *node,
                                        gint             column,
                                        gpointer         user_data)
{
  Image *image;
  gint i;
  v5d_var_info *vinfo;
  hslicecontrols *hs;
  gint type;

  image = (Image *) gtk_ctree_node_get_row_data(ctree,GTK_CTREE_NODE(node));

  vinfo = (v5d_var_info *) g_ptr_array_index(image->vinfo_array,0);

  for(i=0;i<image->item_type->len;i++){
	 /* when is this not the case? */
	 if(image->item_type->data){
		type = g_array_index(image->item_type,gint, i);

		vinfo = (v5d_var_info *) g_ptr_array_index(image->vinfo_array,i);

		gtk_object_set_data(GTK_OBJECT(ctree),"SelectedNode",(gpointer) node);

		switch(type){
		case HSLICE: 
		  hs = (hslicecontrols *) g_ptr_array_index(image->items,i);
		  vinfo_toggle_hslice_from_procedure(vinfo,hs, TRUE);
		  break;
		default:
		  printf ("Unrecognized graphic in procedure %d\n",type);
		}
	 }
  }

}


void
on_ProcedureCtree_tree_unselect_row    (GtkCTree        *ctree,
                                        GList           *node,
                                        gint             column,
                                        gpointer         user_data)
{
  Image *image;
  gint i;
  v5d_var_info *vinfo;
  hslicecontrols *hs;
  gint type;

  image = (Image *) gtk_ctree_node_get_row_data(ctree,GTK_CTREE_NODE(node));

  for(i=0;i<image->item_type->len;i++){
	 type = g_array_index(image->item_type,gint, i);
	 vinfo = (v5d_var_info *) g_ptr_array_index(image->vinfo_array,i);

	 gtk_object_remove_data(GTK_OBJECT(ctree),"SelectedNode");

	 switch(type){
	 case HSLICE: 
		hs = (hslicecontrols *) g_ptr_array_index(image->items,i);
		vinfo_toggle_hslice_from_procedure(vinfo,hs, FALSE);
		break;
	 default:
		printf ("Unrecognized graphic in procedure %d\n",type);
	 }
  }


}


gboolean
on_ProcedureCtree_key_press_event      (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data)
{
  GtkCTree     *ctree;
  GtkCTreeNode *node, *newnode=NULL;


  ctree = GTK_CTREE(widget);
  node = GTK_CTREE_NODE(gtk_object_get_data(GTK_OBJECT(ctree),"SelectedNode"));

  if(! node){
	 node = gtk_ctree_node_nth(ctree,0);
	 printf("selected node not found 0x%x\n",node);
  }
  switch(event->keyval){
  case GDK_Up:
	 newnode = GTK_CTREE_NODE_PREV (node);
    while(newnode && !gtk_ctree_node_get_selectable( ctree, newnode)){
		newnode = GTK_CTREE_NODE_PREV (newnode);
	 }
	 break;
  case GDK_Down:
	 newnode = GTK_CTREE_NODE_NEXT (node);
    while(newnode && !gtk_ctree_node_get_selectable( ctree, newnode)){
		newnode = GTK_CTREE_NODE_NEXT (newnode);
	 }
	 break;
  default:
	 break;
  }

  if(newnode){
	 gtk_ctree_unselect(ctree,node);
	 gtk_ctree_select(ctree, newnode);
	 return TRUE;
  }
  return TRUE; /* disable all other callbacks? */
}


void
on_ProcedureCtree_tree_expand          (GtkCTree        *ctree,
                                        GList           *node,
                                        gpointer         user_data)
{

  gtk_clist_set_column_visibility (GTK_CLIST(ctree), 1, TRUE);
  gtk_clist_set_column_visibility (GTK_CLIST(ctree), 2, TRUE);

}


void
on_ProcedureCtree_tree_collapse        (GtkCTree        *ctree,
                                        GList           *node,
                                        gpointer         user_data)
{

  gtk_clist_set_column_visibility (GTK_CLIST(ctree), 1, FALSE);
  gtk_clist_set_column_visibility (GTK_CLIST(ctree), 2, FALSE);

}


void
on_add_clicked                         (GtkButton       *button,
                                        gpointer         user_data)
{

  
  

}


void
on_close_clicked                       (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *PD=GTK_WIDGET(user_data);
  v5d_info *info;
  GList *ProcedureList;

  info = gtk_object_get_data(GTK_OBJECT(PD),"v5d_info");
  ProcedureList = (GList *) gtk_object_get_data(GTK_OBJECT(PD), "ProcedureList");

  procedure_free( ProcedureList );
  
  gtk_widget_destroy(PD);
  info->ProcedureDialog=NULL;

}




void
on_okay_clicked                        (GtkButton       *button,
                                        gpointer         user_data)
{
  /* add the currently displayed graphics to the current procedure 
     after the selected procedure item */
  v5d_info *info;
  v5d_var_info *vinfo;
  gchar *imagename=NULL;
  graph_label *label;
  GList *label_item;
  GList *ProcedureList;
  GtkWidget *PD, *INdialog;
  gboolean NewImage=TRUE;
  GtkCTreeNode *node;
  GtkCTree *ctree;
  Image *image;
  gint position;

  INdialog = lookup_widget(GTK_WIDGET(button),"ImageNameDialog");

  PD = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(INdialog),"ProcedureDialog"));
  ProcedureList = (GList *) gtk_object_get_data(GTK_OBJECT(PD), "ProcedureList");

  info = (v5d_info *) gtk_object_get_data(GTK_OBJECT(PD), "v5d_info");

  label_item = info->graph_label_list;
  ctree = GTK_CTREE(lookup_widget(PD,"ProcedureCtree"));
  node = GTK_CTREE_NODE(gtk_object_get_data(GTK_OBJECT(ctree),"SelectedNode"));
  
  position = g_list_index(ProcedureList ,gtk_ctree_node_get_row_data(ctree,node));
  if(position>=0) position++;

  while(label_item){
	 label = (graph_label *) label_item->data;
	 vinfo = (v5d_var_info *) label->data;
	 switch(label->gtype){
	 case HSLICE:
		ProcedureList = procedure_add_item(ProcedureList, (gpointer) vinfo->hs, 
													  HSLICE, NewImage, imagename, position );
		break;
	 default:
		break;
	 }
	 NewImage=FALSE;
	 label_item = g_list_next(label_item);
  }
  ProcedureList = g_list_last(ProcedureList);
  image = (Image *) ProcedureList->data;
  image->name = g_strdup(  gtk_editable_get_chars(GTK_EDITABLE(user_data),0,-1));

  /* GtkCTreeRow(node)->sibling puts the new item after rather than before node */
  procedure_ctree_add_image(ctree, GTK_CTREE_ROW(node)->sibling , 
									 info->vinfo_array,
									 (Image *) ProcedureList->data );

  /* Now sibling should be the new node */
  gtk_ctree_unselect(ctree, node);
  gtk_ctree_select(ctree,  GTK_CTREE_ROW(node)->sibling);

  
  gtk_widget_destroy(INdialog);

  ProcedureList = g_list_first(ProcedureList);
  
  gtk_object_set_data(GTK_OBJECT(PD), "ProcedureList", (gpointer) ProcedureList);



}


void
on_cancel_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{

  gtk_widget_destroy(lookup_widget(GTK_WIDGET(button),"ImageNameDialog"));
}



void
on_capture1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  /* capture the currently displayed graphics and add to the procedure */
  GtkWidget *INdialog;

  INdialog = create_ImageNameDialog();

  
  gtk_object_set_data(GTK_OBJECT(INdialog),"ProcedureDialog",
							 (gpointer) lookup_widget(GTK_WIDGET(menuitem),"ProcedureDialog"));

  gtk_widget_show(INdialog);
}


void
on_cut2_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_copy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *PD;
  GList *KillRing;
  GtkCTree *ctree;
  GtkCTreeNode *node;
  gpointer image;

  PD = lookup_widget(GTK_WIDGET(menuitem), "ProcedureDialog");
  KillRing = gtk_object_get_data(GTK_OBJECT(PD),"KillRing");

  ctree = GTK_CTREE(lookup_widget(PD,"ProcedureCtree"));
  node = GTK_CTREE_NODE(gtk_object_get_data(GTK_OBJECT(ctree),"SelectedNode"));
  image = gtk_ctree_node_get_row_data(ctree,node);

  if(KillRing==NULL){
	 GtkWidget *paste = lookup_widget(PD, "paste1");
	 gtk_widget_set_sensitive(paste,TRUE);
  }

  KillRing = g_list_append(KillRing, image);

  gtk_object_set_data(GTK_OBJECT(PD),"KillRing", (gpointer) KillRing);

}


void
on_paste1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{



}


void
on_delete1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *PD;
  GtkCTree *ctree;
  GtkCTreeNode *node;
  Image *image;
  GList *ProcedureList;

  PD = lookup_widget(GTK_WIDGET(menuitem), "ProcedureDialog");
  ctree = GTK_CTREE(lookup_widget(PD,"ProcedureCtree"));
  node = GTK_CTREE_NODE(gtk_object_get_data(GTK_OBJECT(ctree),"SelectedNode"));

  ProcedureList = (GList *) gtk_object_get_data(GTK_OBJECT(PD), "ProcedureList");

  image = gtk_ctree_node_get_row_data(ctree,node);

  ProcedureList = g_list_remove(ProcedureList, (gpointer) image);

  procedure_free_image(image);

  gtk_object_set_data(GTK_OBJECT(PD), "ProcedureList", (gpointer) ProcedureList);

  gtk_ctree_unselect(ctree, node);

  if(GTK_CTREE_ROW(node)->sibling)
	 gtk_ctree_select(ctree, GTK_CTREE_ROW(node)->sibling);

  gtk_ctree_remove_node(ctree,node);

  

}


void
on_openProcedure_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  v5d_info *info;
  GList *ProcedureList;
  GtkWidget *PD;

  PD = lookup_widget(GTK_WIDGET(menuitem), "ProcedureDialog");

  info = (v5d_info *) gtk_object_get_data(GTK_OBJECT(PD),"v5d_info");

  ProcedureList = (GList *) gtk_object_get_data(GTK_OBJECT(PD), "ProcedureList");
  
  /* in window3D.c */
  if(ProcedureList)
	 procedure_free(ProcedureList);


  on_openprocedure_activate(menuitem,(gpointer) lookup_widget(info->GtkGlArea,"window3D") );


}

void
on_write_procedure                         (GtkButton       *button,
														  gpointer         user_data)
{
  GtkWidget *PD, *VerifyDialog;
  GList *ProcedureList;
  gchar *filename; 

  PD = GTK_WIDGET(user_data);

  ProcedureList = (GList *) gtk_object_get_data(GTK_OBJECT(PD), "ProcedureList");

  filename = (gchar *) gtk_object_get_data(GTK_OBJECT(PD), "ProcedureFileName");

  if(filename)
	 print_ProcedureList(ProcedureList, filename);

  VerifyDialog = lookup_widget(GTK_WIDGET(button),"VerifyDialog");
  if(VerifyDialog)
	 gtk_widget_destroy(VerifyDialog);
  
}

void
on_saveProcedure_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *PD;
  GList *ProcedureList;
  gchar *filename;

  PD = lookup_widget(GTK_WIDGET(menuitem), "ProcedureDialog");
  ProcedureList = (GList *) gtk_object_get_data(GTK_OBJECT(PD), "ProcedureList");

  filename = (gchar *) gtk_object_get_data(GTK_OBJECT(PD), "ProcedureFileName");

  if(filename){
	 /* file already exists dialog */
	 gchar string[240];
	 g_snprintf(string, 240, _("Overwrite existing file %s?"),filename);
	 Invoke_VerifyDialog( string, GTK_SIGNAL_FUNC (on_write_procedure), (gpointer) PD,
								 NULL, NULL);
  }else{
	 /*
	 if(FileSelectionDialog == NULL)
		FileSelectionDialog = create_fileselection1();

	 gtk_grab_add(FileSelectionDialog);

	 gtk_window_set_title(GTK_WINDOW(FileSelectionDialog),_("Save Procedure As..."));

	 gtk_object_set_data(GTK_OBJECT(FileSelectionDialog),"OpenWhat" , 
								GINT_TO_POINTER(SAVE_PROCEDURE_FILE));
	 */
  }
	
}


void
on_save_asProcedure_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *PD;
  gchar *filename;

  PD = lookup_widget(GTK_WIDGET(menuitem), "ProcedureDialog");
  
  gtk_object_set_data(GTK_OBJECT(PD), "ProcedureFileName", NULL);
 
  on_saveProcedure_activate(menuitem, user_data);

}

