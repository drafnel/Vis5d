#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <stdio.h>
#include <string.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "ProcedureDialog.h"
#include "PD_interface.h"
#include "support_cb.h"
#include "support.h"
#include "procedure.h"
#include "window3D.h"

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
  gtk_ctree_node_set_row_data(ctree,node, image);
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

  PD = create_ProcedureDialog();

  if(filename)
	 ProcedureList = procedure_open_file(filename);

  
  gtk_object_set_data_full(GTK_OBJECT(PD),"ProcedureList",
									(gpointer) ProcedureList, (GtkDestroyNotify) procedure_free);

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

  if(enable){
	 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Hslicebutton), TRUE);
  }else if(Hslicebutton){
	 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Hslicebutton), FALSE);
  }
  

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

  /* we need to clear all currently enabled graphics */

  vinfo = (v5d_var_info *) g_ptr_array_index(image->vinfo_array,0);

  for(i=0;i<image->item_type->len;i++){
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
  return FALSE;
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

  GtkWidget *INdialog;

  INdialog = create_ImageNameDialog();
  
  gtk_object_set_data(GTK_OBJECT(INdialog),"ProcedureDialog",user_data);

  gtk_widget_show(INdialog);

  
  

}


void
on_delete_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
  /* delete the currently selected item from the procedure */
  GtkCTree     *ctree;
  GtkCTreeNode *node;
  GdkEventKey event;

  ctree = GTK_CTREE(user_data);

  event.keyval = GDK_Down;

  node = GTK_CTREE_NODE(gtk_object_get_data(GTK_OBJECT(ctree),"SelectedNode"));

  /* select the next item or the previous item if that doesn't work */
  if(!on_ProcedureCtree_key_press_event(GTK_WIDGET(ctree),&event,NULL)){
	 event.keyval = GDK_Up;
	 on_ProcedureCtree_key_press_event(GTK_WIDGET(ctree),&event,NULL);
  }
  gtk_ctree_remove_node(ctree,node);

}


void
on_close_clicked                       (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *PD=GTK_WIDGET(user_data);
  v5d_info *info;

  info = gtk_object_get_data(GTK_OBJECT(PD),"v5d_info");
  
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

  INdialog = lookup_widget(GTK_WIDGET(button),"ImageNameDialog");

  PD = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(INdialog),"ProcedureDialog"));

  info = (v5d_info *) gtk_object_get_data(GTK_OBJECT(PD), "v5d_info");
  ProcedureList = (GList *) gtk_object_get_data(GTK_OBJECT(PD), "ProcedureList");
  label_item = info->graph_label_list;
  ctree = GTK_CTREE(lookup_widget(PD,"ProcedureCtree"));
  node = GTK_CTREE_NODE(gtk_object_get_data(GTK_OBJECT(ctree),"SelectedNode"));


  while(label_item){
	 label = (graph_label *) label_item->data;
	 vinfo = (v5d_var_info *) label->data;
	 switch(label->gtype){
	 case HSLICE:
		ProcedureList = procedure_add_item(ProcedureList, (gpointer) vinfo->hs, HSLICE, NewImage, imagename);
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

  procedure_ctree_add_image(ctree, node, 
									 info->vinfo_array,
									 (Image *) ProcedureList->data );


  
  gtk_widget_destroy(INdialog);

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

}


void
on_openProcedure_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_saveProcedure_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_save_asProcedure_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

