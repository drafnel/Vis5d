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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
extern int errno;

#include "api.h"
#include "ProcedureDialog.h"
#include "PD_interface.h"
#include "support_cb.h"
#include "support.h"
#include "procedure.h"
#include "window3D.h"
#include "VarGraphicsControls.h"

extern GtkWidget *FileSelectionDialog;

v5d_var_info *vinfo_array_find_var_by_name(GPtrArray *vinfo_array, gchar *name)
{
  int i;
  v5d_var_info *vinfo;

  /* BUG?: names are not unique */

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
  gchar var[12];
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
	 textplotcontrols *textplot;
	 type = g_array_index(image->item_type,gint, i);
	 nstr[1]=NULL;
	 nstr[0] = NULL;
	 switch(type){
	 case CHSLICE:
		nstr[1] = g_strdup("CHSlice");
	 case HSLICE:
		if(nstr[1]==NULL)
		  nstr[1] = g_strdup("HSlice");
		hs = (hslicecontrols *) g_ptr_array_index(image->items,i);
		if(hs==NULL){
		  printf("Error: expected hslice here\n");
		  return;
		}
		nstr[2] = hs->var;
		vinfo = vinfo_array_find_var_by_name(vinfo_array, hs->var);
		g_array_append_val(image->vinfo_array, vinfo);
		gtk_ctree_node_set_selectable(ctree,
												gtk_ctree_insert_node(ctree,node,NULL,nstr,0,NULL,NULL,NULL,NULL,1,0),
												FALSE);
		break;
	 case TEXTPLOT:
		nstr[1] = g_strdup("TextPlot");
		textplot = (textplotcontrols *) g_ptr_array_index(image->items,i);
		vinfo = vinfo_array_find_var_by_name(vinfo_array, textplot->var);

		vis5d_get_itx_var_name(vinfo->v5d_data_context,vinfo->varid, var);
		nstr[2] = var;
      
		g_array_append_val(image->vinfo_array, vinfo);
		gtk_ctree_node_set_selectable(ctree,
												gtk_ctree_insert_node(ctree,node,NULL,nstr,0,NULL,NULL,NULL,NULL,1,0),
												FALSE);
		break;
	 default:
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
  }else{
	 GtkWidget *save;
	 save = lookup_widget(PD, "saveProcedure");
	 gtk_widget_set_sensitive(save, FALSE);
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
vinfo_toggle_textplot_from_procedure(v5d_var_info *vinfo, textplotcontrols *textplot, gint enable)
{
  GtkWidget *window3D, *TextPlotDialog;
  if(vinfo==NULL) {
	 return;
  }
  if(enable){
	 GtkWidget *menu, *optionmenu, *menuitem;
	 window3D = lookup_widget(vinfo->info->GtkGlArea, "window3D");
	 TextPlotDialog = new_TextPlotDialog(window3D);
	 gtk_widget_show(TextPlotDialog);

	 optionmenu = lookup_widget(TextPlotDialog,"textplot_variables");
	 menu = gtk_option_menu_get_menu(GTK_OPTION_MENU(optionmenu));

	 /* FIXTHIS:  This is not a good method of finding the variable to
		 activate if more than one irregular data set is opened */

	 gtk_menu_set_active(GTK_MENU(menu),vinfo->varid);
	 menuitem = gtk_menu_get_active(GTK_MENU(menu));

	 gtk_menu_item_activate(menuitem );
	 /*
	 vis5d_set_text_plot(vinfo->v5d_data_context,vinfo->varid,textplot->spacing,
								textplot->fontspace,textplot->fontx,textplot->fonty);
	 */

  }
  


	 
  glarea_draw(vinfo->info->GtkGlArea, NULL, NULL);

}

void 
vinfo_toggle_chslice_from_procedure(v5d_var_info *vinfo, hslicecontrols *hs, gint enable)
{
  GtkWidget *CHslicebutton;
  if(vinfo==NULL) {
	 /* TODO: var was not found by name in dataset - a dialog should be open which
		 gives the user the choice of selecting from the available variables 
		 or canceling this operation */
	 return;
  }
  if(enable)
	 on_variable_activate(NULL, vinfo);
  if(vinfo->VarGraphicsDialog)
	 CHslicebutton = lookup_widget(vinfo->VarGraphicsDialog,"CHslicebutton");

  if(CHslicebutton){
	 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(CHslicebutton), enable);
  }
  if(! (enable && hs) ) 
	 return;


  if(hs->max > hs->min){
	 if(hs->level > 0)
		vinfo->chs->level = hs->level;
	 vinfo->chs->max = hs->max;
	 vinfo->chs->min = hs->min;
  }
  if(hs->height>0){
	 vinfo->chs->height = hs->height;
	 vis5d_height_to_gridlevel( vinfo->v5d_data_context,0, vinfo->varid, 
										 vinfo->chs->height, &vinfo->chs->level);
  }
  update_hslice_controls(vinfo, CHSLICE);
  
  if(hs->sample && hs->sample->name){
	 gradient_update_sample_from_name(vinfo->chs->sample,hs->sample->name); 
  }
  

  glarea_draw(vinfo->info->GtkGlArea, NULL, NULL);
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
  textplotcontrols *textplot;
  gint type;

  image = (Image *) gtk_ctree_node_get_row_data(ctree,GTK_CTREE_NODE(node));

  vinfo = g_array_index(image->vinfo_array,v5d_var_info *, 0);

  for(i=0;i<image->item_type->len;i++){
	 /* when is this not the case? */
	 if(image->item_type->data){
		type = g_array_index(image->item_type,gint, i);

		vinfo = g_array_index(image->vinfo_array,v5d_var_info *, i);

		gtk_object_set_data(GTK_OBJECT(ctree),"SelectedNode",(gpointer) node);

		switch(type){
		case CHSLICE:
		  hs = (hslicecontrols *) g_ptr_array_index(image->items,i);
		  vinfo_toggle_chslice_from_procedure(vinfo,hs, TRUE);
		  break;
		case HSLICE: 
		  hs = (hslicecontrols *) g_ptr_array_index(image->items,i);
		  vinfo_toggle_hslice_from_procedure(vinfo,hs, TRUE);
		  break;
		case TEXTPLOT:
        textplot = (textplotcontrols *) g_ptr_array_index(image->items,i);
		  vinfo_toggle_textplot_from_procedure(vinfo,textplot, TRUE);
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
	 vinfo = g_array_index(image->vinfo_array,v5d_var_info *, i);

	 gtk_object_remove_data(GTK_OBJECT(ctree),"SelectedNode");

	 switch(type){
	 case CHSLICE: 
		hs = (hslicecontrols *) g_ptr_array_index(image->items,i);
		vinfo_toggle_chslice_from_procedure(vinfo,hs, FALSE);
		break;
	 case HSLICE: 
		hs = (hslicecontrols *) g_ptr_array_index(image->items,i);
		vinfo_toggle_hslice_from_procedure(vinfo,hs, FALSE);
		break;
	 case TEXTPLOT:
		printf ("Got a textplot %d\n",__LINE__);
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
on_close_clicked                       (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *PD=GTK_WIDGET(user_data);
  v5d_info *info;
  GList *ProcedureList;

  info = gtk_object_get_data(GTK_OBJECT(PD),"v5d_info");
  ProcedureList = (GList *) gtk_object_get_data(GTK_OBJECT(PD), "ProcedureList");

  /* gets stuck when you try to close a procedure opened with new */
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
  GtkCTreeNode *node, *prevnode, *newnode;
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
  /* GtkCTreeRow(node)->sibling puts the new item after rather than before node */
  if(node)
	 prevnode = GTK_CTREE_ROW(node)->sibling;
  else
	 prevnode = NULL;

  image = gtk_ctree_node_get_row_data(ctree,node);
  position = g_list_index(ProcedureList , image);
  if(position>=0) position++;

  image = NULL;
  while(label_item){
	 
	 label = (graph_label *) label_item->data;
	 vinfo = (v5d_var_info *) label->data;
	 switch(label->gtype){
	 case CHSLICE:
		image = image_add_item(image,(gpointer) vinfo->chs,CHSLICE,imagename);
		break;
	 case HSLICE:
		image = image_add_item(image,(gpointer) vinfo->hs,HSLICE,imagename);
		break;
	 case TEXTPLOT:
		image = image_add_item(image,(gpointer) vinfo , label->gtype,imagename);
		break;
	 default:
		break;
	 }
	 label_item = g_list_next(label_item);
  }
  ProcedureList = g_list_insert(ProcedureList,(gpointer) image, position);

  image->name = g_strdup(  gtk_editable_get_chars(GTK_EDITABLE(user_data),0,-1));

  
  procedure_ctree_add_image(ctree, prevnode, 
									 info->vinfo_array,image );

  /* Now sibling should be the new node */
  if(node){
	 gtk_ctree_unselect(ctree, node);
    newnode = GTK_CTREE_ROW(node)->sibling;
  }else{
	 newnode = gtk_ctree_node_nth(ctree,0);
  }
  gtk_ctree_select(ctree,  newnode);

  
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
Procedure_remove_image(GtkWidget *ProcedureDialog, GList *KillRing, gboolean killringadd)
{
  GtkCTree *ctree;
  GtkCTreeNode *node, *newnode;
  gpointer image;
  GList *ProcedureList;

  ctree = GTK_CTREE(lookup_widget(ProcedureDialog,"ProcedureCtree"));
  node = GTK_CTREE_NODE(gtk_object_get_data(GTK_OBJECT(ctree),"SelectedNode"));
  image = gtk_ctree_node_get_row_data(ctree,node);

  


  gtk_ctree_unselect(ctree, node);
  newnode = GTK_CTREE_ROW(node)->sibling;

  if(! newnode){
	 newnode = GTK_CTREE_NODE_PREV(node);
	 if(newnode)
		while(GTK_CTREE_ROW(newnode)->parent)
		  newnode = GTK_CTREE_ROW(newnode)->parent;
  }	 
  if(newnode)
	 gtk_ctree_select(ctree, newnode);

  gtk_ctree_remove_node(ctree,node);


  ProcedureList = (GList *) gtk_object_get_data(GTK_OBJECT(ProcedureDialog), "ProcedureList");

  ProcedureList = g_list_remove(ProcedureList, (gpointer) image);

  gtk_object_set_data(GTK_OBJECT(ProcedureDialog), "ProcedureList", (gpointer) ProcedureList);

  if(killringadd){
	 if(KillRing==NULL){
		GtkWidget *paste = lookup_widget(ProcedureDialog, "paste1");
		gtk_widget_set_sensitive(paste,TRUE);
	 }
	 
	 KillRing = g_list_append(KillRing, image);

	 gtk_object_set_data(GTK_OBJECT(ProcedureDialog),"KillRing", (gpointer) KillRing);

  }else{
	 procedure_free_image(image);
  }

}

void
on_cut2_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *PD;
  GList *KillRing;
  PD = lookup_widget(GTK_WIDGET(menuitem), "ProcedureDialog");
  KillRing = gtk_object_get_data(GTK_OBJECT(PD),"KillRing");
  
  Procedure_remove_image(PD, KillRing, TRUE);

}


void
on_copy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *PD;
  GList *KillRing=NULL;
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
  GtkWidget *PD;
  GList *ProcedureList, *KillRing=NULL, *listitem;
  GtkCTree *ctree;
  GtkCTreeNode *node;
  Image * image;
  v5d_info *info;
  gint position;

  PD = lookup_widget(GTK_WIDGET(menuitem), "ProcedureDialog");
  KillRing = gtk_object_get_data(GTK_OBJECT(PD),"KillRing");
  if(!KillRing)
	 return;


  ProcedureList = (GList *) gtk_object_get_data(GTK_OBJECT(PD), "ProcedureList");
  info = (v5d_info *) gtk_object_get_data(GTK_OBJECT(PD), "v5d_info");
  ctree = GTK_CTREE(lookup_widget(PD,"ProcedureCtree"));
  node = GTK_CTREE_NODE(gtk_object_get_data(GTK_OBJECT(ctree),"SelectedNode"));
  image = (Image *) gtk_ctree_node_get_row_data(ctree,node);
  position = g_list_index(ProcedureList , (gpointer) image);
  if(position>=0) position++;

  listitem = g_list_last(KillRing);

  KillRing = g_list_remove_link(KillRing, listitem);

  image = (Image *) listitem->data;

  ProcedureList = g_list_insert(ProcedureList,(gpointer) image, position);

  procedure_ctree_add_image(ctree, GTK_CTREE_ROW(node)->sibling , 
									 info->vinfo_array, image);

  gtk_object_set_data(GTK_OBJECT(PD), "ProcedureList",(gpointer) ProcedureList );
  g_list_free_1(listitem);
  gtk_object_set_data(GTK_OBJECT(PD),"KillRing" , (gpointer) KillRing);

  if(KillRing==NULL){
	 gtk_widget_set_sensitive(GTK_WIDGET(menuitem),FALSE);
  }
  


}


void
on_delete1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *PD;
  GList *KillRing;
  PD = lookup_widget(GTK_WIDGET(menuitem), "ProcedureDialog");
  KillRing = gtk_object_get_data(GTK_OBJECT(PD),"KillRing");
  
  Procedure_remove_image(PD, KillRing, FALSE);
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
  if(button){
	 VerifyDialog = lookup_widget(GTK_WIDGET(button),"VerifyDialog");
	 if(VerifyDialog)
		gtk_widget_destroy(VerifyDialog);
  }
}

void
on_saveProcedure_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *PD;
  GList *ProcedureList;
  gchar *filename;


  gtk_widget_set_sensitive(GTK_WIDGET(menuitem), TRUE);
  PD = lookup_widget(GTK_WIDGET(menuitem), "ProcedureDialog");
  ProcedureList = (GList *) gtk_object_get_data(GTK_OBJECT(PD), "ProcedureList");

  filename = (gchar *) user_data;
  if(! filename){
	 filename = (gchar *) gtk_object_get_data(GTK_OBJECT(PD), "ProcedureFileName");
  }


  if(filename){
	 gchar string[240];
	 struct stat filestat;

	 gtk_object_set_data(GTK_OBJECT(PD), "ProcedureFileName", (gpointer) filename);
	 /* TODO: could handle other situations here - such as no write permission, etc. */

	 if(stat(filename, &filestat)==-1 && errno== ENOENT){
		/* file does not exist */
		on_write_procedure(NULL ,PD);
	 }else{
		/* file already exists dialog */
		g_snprintf(string, 240, _("Overwrite existing file %s?"),filename);
		Invoke_VerifyDialog( string, GTK_SIGNAL_FUNC (on_write_procedure), (gpointer) PD,
									NULL, NULL);
	 }

  }else{
	 printf("No filename provided \n");
  }
}


void
on_save_asProcedure_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  if(FileSelectionDialog == NULL)
	 FileSelectionDialog = create_fileselection1();

  gtk_grab_add(FileSelectionDialog);

 
  gtk_window_set_title(GTK_WINDOW(FileSelectionDialog),_("Save Procedure As..."));

  gtk_object_set_data(GTK_OBJECT(FileSelectionDialog),"OpenWhat" , 
							 GINT_TO_POINTER(SAVE_PROCEDURE_FILE));
  gtk_widget_show(FileSelectionDialog);
}

