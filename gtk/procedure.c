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
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <glib.h>
#include "procedure.h"
#include <api.h>
/* define enumeration values to be returned for specific symbols */
/* a SCOPE_SYMBOL is one which will change the current parser scope */

enum {
  SCOPE_SYMBOL_IMAGE  = G_TOKEN_LAST + 1,
  SYMBOL_NAME,
  SCOPE_SYMBOL_HSLICE,
    SYMBOL_VAR,
    SYMBOL_MIN,
    SYMBOL_MAX,
    SYMBOL_HEIGHT,
    SYMBOL_INT,
    SYMBOL_STIPPLE,
    SYMBOL_WIDTH,
    SYMBOL_COLOR,
  SCOPE_SYMBOL_CHSLICE,
    SYMBOL_COLOR_TABLE,
  SCOPE_SYMBOL_TEXTPLOT,
    SYMBOL_SPACING,
    SYMBOL_FONTX,
    SYMBOL_FONTY,
    SYMBOL_FONTSPACE,
  NULL_SYMBOL    /* should always be last - just a pointer to the end*/
};


/* symbol array */
typedef const struct {
  gchar *symbol_name;
  guint  symbol_token;
  guint  symbol_context;
} Symbols; 

Symbols symbols[] = {
  { "image", SCOPE_SYMBOL_IMAGE, 0,},
  { "name",  SYMBOL_NAME,  SCOPE_SYMBOL_IMAGE,},
  { "hslice", SCOPE_SYMBOL_HSLICE,  SCOPE_SYMBOL_IMAGE,},
    { "var", SYMBOL_VAR,  SCOPE_SYMBOL_HSLICE,},
    { "min", SYMBOL_MIN,  SCOPE_SYMBOL_HSLICE,},
    { "max", SYMBOL_MAX,  SCOPE_SYMBOL_HSLICE,},
    { "height", SYMBOL_HEIGHT,  SCOPE_SYMBOL_HSLICE,},
    { "interval", SYMBOL_INT,  SCOPE_SYMBOL_HSLICE,},
    { "stipple", SYMBOL_STIPPLE, SCOPE_SYMBOL_HSLICE,},
    { "width", SYMBOL_WIDTH, SCOPE_SYMBOL_HSLICE,},
    { "color", SYMBOL_COLOR, SCOPE_SYMBOL_HSLICE,},
  { "chslice", SCOPE_SYMBOL_CHSLICE,  SCOPE_SYMBOL_IMAGE,},
    { "var", SYMBOL_VAR,  SCOPE_SYMBOL_CHSLICE,},
    { "min", SYMBOL_MIN,  SCOPE_SYMBOL_CHSLICE,},
    { "max", SYMBOL_MAX,  SCOPE_SYMBOL_CHSLICE,},
    { "height", SYMBOL_HEIGHT,  SCOPE_SYMBOL_CHSLICE,},
    { "color_table", SYMBOL_COLOR_TABLE, SCOPE_SYMBOL_CHSLICE,},
  { "textplot", SCOPE_SYMBOL_TEXTPLOT, SCOPE_SYMBOL_IMAGE,},
    { "var", SYMBOL_VAR, SCOPE_SYMBOL_TEXTPLOT,},
    { "spacing", SYMBOL_SPACING, SCOPE_SYMBOL_TEXTPLOT,},
    { "fontx", SYMBOL_FONTX, SCOPE_SYMBOL_TEXTPLOT,},
    { "fonty", SYMBOL_FONTY, SCOPE_SYMBOL_TEXTPLOT,},
    { "fontspace", SYMBOL_FONTSPACE, SCOPE_SYMBOL_TEXTPLOT,},
    { "color", SYMBOL_COLOR, SCOPE_SYMBOL_TEXTPLOT,},
  { NULL, 0, 0,},
};

static int context_depth=0, context[8];

static guint 
token_string(GScanner *scanner, gchar **str)
{
  g_scanner_get_next_token (scanner);
  if(scanner->token != G_TOKEN_STRING)
	 return G_TOKEN_STRING;
  *str = g_strdup(scanner->value.v_string);
  /* Next token should be a semi-colon */
  g_scanner_get_next_token (scanner);
  if(scanner->token!=';')
	 return ';';

  return G_TOKEN_NONE;
}

static guint
token_float(GScanner *scanner, float *fval)
{
  gboolean negate = FALSE;


  /* convert G_TOKEN_INT to G_TOKEN_FLOAT */
  scanner->config->int_2_float = TRUE;


	 /* feature optional '-' */
  g_scanner_peek_next_token (scanner);
  if (scanner->next_token == '-')
	 {
		g_scanner_get_next_token (scanner);
		negate = !negate;
	 }
  
  /* expect a float (ints are converted to floats on the fly) */
  g_scanner_get_next_token (scanner);

  /* convert G_TOKEN_INT to G_TOKEN_FLOAT */
  scanner->config->int_2_float = FALSE;

  if (scanner->token != G_TOKEN_FLOAT)
	 return G_TOKEN_FLOAT;

  *fval = negate ? - scanner->value.v_float : scanner->value.v_float;
  
  /* make sure the next token is a ';' strip in any case*/
  if (g_scanner_get_next_token (scanner) != ';')
	 {
		/* not so, eat up the non-semicolon and error out */
		return ';';
	 }
  return G_TOKEN_NONE;
}

static guint
token_float_list(GScanner *scanner, gint nitems, float *fval)
{
  gint i;
  gchar token;
  /* first token should be { */
  g_scanner_peek_next_token (scanner);
  if (scanner->next_token == '{')
	 g_scanner_get_next_token (scanner);
  else
	 return '{';

  /* convert G_TOKEN_INT to G_TOKEN_FLOAT */
  scanner->config->int_2_float = TRUE;


  for(i=0;i<nitems;i++){
	 gboolean negate = FALSE;
	 /* feature optional '-' */
	 g_scanner_peek_next_token (scanner);
	 if (scanner->next_token == '-')
		{
		  g_scanner_get_next_token (scanner);
		  negate = !negate;
		}
  
	 /* expect a float (ints are converted to floats on the fly) */
	 g_scanner_get_next_token (scanner);


	 if (scanner->token != G_TOKEN_FLOAT)
		return G_TOKEN_FLOAT;

	 fval[i] = negate ? - scanner->value.v_float : scanner->value.v_float;
  
	 /* make sure the next token is a ',' or '}' strip in any case*/
	 token = g_scanner_get_next_token (scanner);
	 if (i<nitems-1 && token != ',')
	 {
		/* not so, eat up the non-comma and error out */
		return ',';
	 }else if(i==nitems-1 && token != '}')
	 {
		return '}';
	 }
  }
  /* do not convert G_TOKEN_INT to G_TOKEN_FLOAT */
  scanner->config->int_2_float = FALSE;

  /* make sure the next token is a ';' strip in any case*/
  if (g_scanner_get_next_token (scanner) != ';')
	 {
		/* not so, eat up the non-semicolon and error out */
		return ';';
	 }
  return G_TOKEN_NONE;


}

static guint
token_int(GScanner *scanner, guint *fval)
{
  g_scanner_peek_next_token (scanner);

  g_scanner_get_next_token (scanner);
  if (scanner->token != G_TOKEN_INT)
	 return G_TOKEN_INT;

  *fval = scanner->value.v_int;
  
  /* make sure the next token is a ';' strip in any case*/
  if (g_scanner_get_next_token (scanner) != ';')
	 {
		/* not so, eat up the non-semicolon and error out */
		return ';';
	 }
  return G_TOKEN_NONE;
}

Image *image_add_item(Image *image, gpointer item, gint itemtype, gchar *imagename)
{
  if(image==NULL){
	 image =  g_new(Image,1);
	 image->vinfo_array = g_array_new(FALSE,TRUE,sizeof(v5d_var_info *)); 
	 image->items=g_ptr_array_new();
	 image->item_type=g_array_new(FALSE,TRUE,sizeof(gint));
  }
  if(item){  
	 g_ptr_array_add(image->items,(gpointer) item);
	 g_array_append_val(image->item_type,itemtype);
  }
  if(imagename){
	 image->name = g_strdup(imagename);
  }
  return image;
}

static guint
parse_textplot_symbol(GScanner *scanner, Image *oneimage, guint symbol)
{
  float val;
  gchar var[15];

  textplotcontrols *onetextplot;

  if(g_array_index(oneimage->item_type,gint,oneimage->item_type->len-1)
	  != TEXTPLOT){
	 g_print("Expected TEXTPLOT found %d\n",oneimage->item_type->data[oneimage->item_type->len-1]);
	 return G_TOKEN_SYMBOL;
  }
  onetextplot = (textplotcontrols *) g_ptr_array_index(oneimage->items,oneimage->items->len-1);

  switch (symbol){
  case SYMBOL_VAR:
	 return token_string(scanner,&onetextplot->var);
  case SYMBOL_SPACING:
	 return token_float(scanner, &onetextplot->spacing);
  case SYMBOL_FONTX:
	 return token_float(scanner, &onetextplot->fontx);
  case SYMBOL_FONTY:
	 return token_float(scanner, &onetextplot->fonty);
  case SYMBOL_FONTSPACE:
	 return token_float(scanner, &onetextplot->fontspace);
  case SYMBOL_COLOR:
	 return token_float_list(scanner, 4, onetextplot->color);
  default:
	 return G_TOKEN_SYMBOL;
  }

}

static guint
parse_hslice_symbol(GScanner *scanner, Image *oneimage, guint symbol)
{
  guint tmp;
  hslicecontrols *onehslice;
  if(g_array_index(oneimage->item_type,gint,oneimage->item_type->len-1)
	  != HSLICE){
	 g_print("Expected HSLICE found %d\n",oneimage->item_type->data[oneimage->item_type->len-1]);
	 return G_TOKEN_SYMBOL;
  }
  onehslice = (hslicecontrols *) g_ptr_array_index(oneimage->items,oneimage->items->len-1);
  
  switch (symbol){
  case SYMBOL_VAR:
	 return token_string(scanner,&onehslice->var);
  case SYMBOL_MIN:
	 return token_float(scanner, &onehslice->min);
  case SYMBOL_MAX:
	 return token_float(scanner, &onehslice->max);
  case SYMBOL_INT:
	 return token_float(scanner, &onehslice->interval);
  case SYMBOL_HEIGHT:
	 return token_float(scanner, &onehslice->height);
  case SYMBOL_STIPPLE:
	 symbol = token_int(scanner, &tmp);		
	 onehslice->stipple = (gushort) tmp;
	 return symbol;
  case SYMBOL_WIDTH:
	 return token_int(scanner, &onehslice->linewidth);
  case SYMBOL_COLOR:
	 {
		gfloat color[4];
		gint i;
		symbol = token_float_list(scanner, 4, color);
		for(i=0;i<4;i++)
		  onehslice->color[i]=(gdouble) color[i];
		return symbol;
	 }
	 
  default:
	 return G_TOKEN_SYMBOL;
  }	 
  return G_TOKEN_NONE;
}


static guint
parse_chslice_symbol(GScanner *scanner, Image *oneimage, guint symbol)
{
  hslicecontrols *onehslice;
  if(g_array_index(oneimage->item_type,gint,oneimage->item_type->len-1)
	  != CHSLICE){
	 g_print("Expected CHSLICE found %d\n",oneimage->item_type->data[oneimage->item_type->len-1]);
	 return G_TOKEN_SYMBOL;
  }
  onehslice = (hslicecontrols *) g_ptr_array_index(oneimage->items,oneimage->items->len-1);
  
  switch (symbol){
  case SYMBOL_VAR:
	 return token_string(scanner,&onehslice->var);
  case SYMBOL_MIN:
	 return token_float(scanner, &onehslice->min);
  case SYMBOL_MAX:
	 return token_float(scanner, &onehslice->max);
  case SYMBOL_HEIGHT:
	 return token_float(scanner, &onehslice->height);
  case SYMBOL_COLOR_TABLE:
	 if(!onehslice->sample)
		onehslice->sample = g_new0(preview_area, 1);	
	 return token_string(scanner,&(onehslice->sample->name));
  default:
	 return G_TOKEN_SYMBOL;
  }	 
  return G_TOKEN_NONE;
}


static guint
parse_symbol (GScanner *scanner, GList **ProcedureList)
{
  guint symbol;
  Image *oneimage;
  hslicecontrols *onehslice;
  textplotcontrols *onetextplot;

  /* expect a valid symbol */
  g_scanner_get_next_token (scanner);
  symbol = scanner->token;

  if(scanner->token == '}'){    
	 g_scanner_set_scope(scanner,context[--context_depth]);

	 return G_TOKEN_NONE;
  }
  
  if (symbol < SCOPE_SYMBOL_IMAGE ||
		symbol >= NULL_SYMBOL)
	 return G_TOKEN_SYMBOL;


  g_scanner_get_next_token (scanner);


  switch (context[context_depth]){
  case 0:
	 if(scanner->token != '{')
		return '{';
	 context[++context_depth] = symbol;
	 g_scanner_set_scope(scanner,context[context_depth]);

	 oneimage = image_add_item(NULL, NULL, 0, NULL);
    *ProcedureList = g_list_append(*ProcedureList, (gpointer) oneimage);
	 
	 return G_TOKEN_NONE;
	 break;
  case SCOPE_SYMBOL_IMAGE:
	 if(*ProcedureList==NULL)
		return SCOPE_SYMBOL_IMAGE;
	 oneimage = (Image *) g_list_last(*ProcedureList)->data;

	 switch (symbol){
	 case SYMBOL_NAME:
		if(scanner->token != '=')
		  return '=';
		return token_string(scanner,&oneimage->name);
		break;
	 case SCOPE_SYMBOL_HSLICE:
	 case SCOPE_SYMBOL_CHSLICE:
		if(scanner->token != '{')
		  return '{';
		context[++context_depth] = symbol;
		g_scanner_set_scope(scanner,context[context_depth]);
		onehslice = g_new0(hslicecontrols,1);
		onehslice->var=NULL;
		if(symbol==SCOPE_SYMBOL_HSLICE){
		  image_add_item(oneimage, (gpointer) onehslice, HSLICE, NULL);
		}else{
		  image_add_item(oneimage, (gpointer) onehslice, CHSLICE, NULL);
		}
		return G_TOKEN_NONE;
		break;
	 case SCOPE_SYMBOL_TEXTPLOT:
		if(scanner->token != '{')
		  return '{';
		context[++context_depth] = symbol;
		g_scanner_set_scope(scanner,context[context_depth]);
		onetextplot = g_new(textplotcontrols,1);
		onetextplot->var=NULL;
		onetextplot->label=NULL;
		onetextplot->spacing=1.0;
		onetextplot->fontspace=1.0;
		onetextplot->fontx=10.;
		onetextplot->fonty=10.;
		onetextplot->color[0]=-1.;
		image_add_item(oneimage, (gpointer) onetextplot, TEXTPLOT, NULL);

		return G_TOKEN_NONE;
		break;
	 default:
		return G_TOKEN_SYMBOL;
		break;
	 }
  case SCOPE_SYMBOL_HSLICE:
	 /* expect '=' */
	 if(scanner->token != '=')
		return '=';
    oneimage = (Image *) g_list_last(*ProcedureList)->data;
	 return parse_hslice_symbol(scanner, oneimage, symbol);
	 break;
  case SCOPE_SYMBOL_CHSLICE:
	 /* expect '=' */
	 if(scanner->token != '=')
		return '=';
    oneimage = (Image *) g_list_last(*ProcedureList)->data;
	 return parse_chslice_symbol(scanner, oneimage, symbol);
	 break;
  case SCOPE_SYMBOL_TEXTPLOT:
	 /* expect '=' */
	 if(scanner->token != '=')
		return '=';
    oneimage = (Image *) g_list_last(*ProcedureList)->data;
	 return parse_textplot_symbol(scanner, oneimage, symbol);
	 break;
  default:
	 /* flag an unrecognized context */
	 printf("bad context %d\n",symbol);
  }
  return G_TOKEN_NONE;
}

void 
print_textplotcontrols(FILE *fp, textplotcontrols *textplot)
{
  gchar name[12];
  gint varid;
  float spacing, fontx, fonty, fontspace, v5dcolor[4];

  /*
  vis5d_get_text_plot(vinfo->v5d_data_context,&varid, &spacing,
							 &fontx, &fonty, &fontspace);
  if(varid != vinfo->varid){
	 fprintf(stderr,"Varid does not match in %s line %d\n",__FILE__,__LINE__);
	 return;
  }
  
  vis5d_get_itx_var_name(vinfo->v5d_data_context,varid, name);

  vis5d_get_color(vinfo->info->v5d_display_context, VIS5D_TEXTPLOT, varid,
						v5dcolor,v5dcolor+1,v5dcolor+2,v5dcolor+3);
  */


  fprintf(fp,"  textplot {\n");
  fprintf(fp,"    var = \"%s\";\n",textplot->var);    
  fprintf(fp,"    spacing = %f;\n",textplot->spacing);
  fprintf(fp,"    fontx = %f;\n",textplot->fontx);
  fprintf(fp,"    fonty = %f;\n",textplot->fonty);
  fprintf(fp,"    fontspace = %f;\n",textplot->fontspace);
  fprintf(fp,"    color = { %g, %g, %g, %g };\n",
			 textplot->color[0],
			 textplot->color[1],
			 textplot->color[2],
			 textplot->color[3]);
  fprintf(fp,"  }\n");


}

void 
print_hslicecontrols(FILE *fp, hslicecontrols *hslice)
{
  if(! hslice->var)
	 {
		g_print("hslicecontrols is invalid without a variable name\n");
		return;
	 }
  if(hslice->interval){
	 /* this is a contour isoline slice */
	 fprintf(fp,"  hslice {\n");
	 fprintf(fp,"    interval = %g;\n",hslice->interval);
	 fprintf(fp,"    stipple = 0x%x;\n",(unsigned short) hslice->stipple);
	 fprintf(fp,"    width = %d;\n",hslice->linewidth);
	 fprintf(fp,"    color = {%g, %g, %g, %g};\n"
				,hslice->color[0],hslice->color[1],
				hslice->color[2],hslice->color[3]);
  }else{
	 /* this is a contour color filled slice */
	 fprintf(fp,"  chslice {\n");
	 if(hslice->sample && hslice->sample->name)
		fprintf(fp,"    color_table = \"%s\";\n",hslice->sample->name);
  }
  fprintf(fp,"    var = \"%s\";\n",hslice->var);
  fprintf(fp,"    min = %g;\n",hslice->min);
  fprintf(fp,"    max = %g;\n",hslice->max);
  fprintf(fp,"    height = %g;\n",hslice->height);

  fprintf(fp,"  }\n");
}

void
print_ProcedureList(GList *ProcedureList,gchar *filename)
{
  GList *nextimage;
  GArray *item_types;
  GPtrArray *items;
  FILE *fp;
  gchar *name;
  gint i;

  fp = fopen(filename,"w");

  nextimage = ProcedureList;
  
  while(nextimage){
	 fprintf(fp,"image { \n");
	 if( (name= ((Image *)nextimage->data)->name)){
		fprintf(fp,"  name=\"%s\";\n",name);
	 }
	 items = (GPtrArray *) ((Image *)nextimage->data)->items;
	 item_types = (GArray *) ((Image *)nextimage->data)->item_type;
	 for(i=0;i<items->len;i++){
		switch(g_array_index(item_types,gint,i)){
		case CHSLICE:
		case HSLICE:
		  print_hslicecontrols(fp, (hslicecontrols *) g_ptr_array_index(items,i));
		  break;
		case TEXTPLOT:
		  print_textplotcontrols(fp, (textplotcontrols *) g_ptr_array_index(items,i));
		  break;
		default:
		  g_print("What is this %d\n",g_array_index(item_types,gint,i));
		}
	 }
	 fprintf(fp,"}\n");
	 nextimage = g_list_next(nextimage);
  }
  fclose(fp);
}




GList *procedure_parse_file(int filedescriptor)
{
  GScanner *scanner;
  guint expected_token;
  GList *ProcedureList=NULL;

  Symbols *symbol_p = symbols;  

  scanner = g_scanner_new (NULL);

  /* adjust lexing behaviour to suit our needs
	*/
  /* convert non-floats (octal values, hex values...) to G_TOKEN_INT */
  scanner->config->numbers_2_int = TRUE;
  /* don't return G_TOKEN_SYMBOL, but the symbol's value */
  scanner->config->symbol_2_token = TRUE;
  
  /* load symbols into the scanner */
  while (symbol_p->symbol_name)
	 {
		g_scanner_scope_add_symbol (scanner,symbol_p->symbol_context,
									 symbol_p->symbol_name,
									 GINT_TO_POINTER (symbol_p->symbol_token));
		symbol_p++;
	 }
  

  g_scanner_input_file(scanner,filedescriptor);

  /* give the error handler an idea on how the input is named */
  scanner->input_name = "test text";

  /* scanning loop, we parse the input until its end is reached,
	* the scanner encountered a lexing error, or our sub routine came
	* across invalid syntax
	*/
  do
	 {
		expected_token = parse_symbol (scanner, &ProcedureList);

		g_scanner_peek_next_token (scanner);
	 }
  while (expected_token == G_TOKEN_NONE &&
			scanner->next_token != G_TOKEN_EOF &&
			scanner->next_token != G_TOKEN_ERROR);
  
  /* give an error message upon syntax errors */
  if (expected_token != G_TOKEN_NONE )
	 g_scanner_unexp_token (scanner, expected_token, NULL, "symbol", NULL, NULL, TRUE);
  
  /* finish parsing */

  g_scanner_destroy (scanner);
  
  
  return ProcedureList;
}

void hslice_free(hslicecontrols *hslice){
  if(hslice){
	 if(hslice->var)
		g_free(hslice->var);
	 g_free(hslice);
  }
}



void procedure_free_image(Image *image)
{
  gint i;
  gint type;
  if(image){
	 if(image->name)
		g_free(image->name);
		

	 for(i=0;i<image->items->len;i++){
		type = g_array_index(image->item_type,gint,i);
		switch (type){
		case CHSLICE:
		case HSLICE:
		  hslice_free((hslicecontrols *) g_ptr_array_index(image->items,i));
		  break;
		default:
		  g_print("Unrecogized type in free %d\n",type);
		  
		}
	 }
	 g_array_free(image->item_type,TRUE);
	 
	 g_ptr_array_free(image->items,TRUE);
  }		 				

}

void procedure_free(GList *Procedure)
{
  GList *imagelist;
  Image *image;
 

  imagelist = g_list_first(Procedure);
  while(imagelist){
	 image = (Image *) imagelist->data;
	 
	 procedure_free_image(image);
	 
	 imagelist = g_list_next(imagelist);
  }
  g_list_free(Procedure);

}


GList *procedure_open_file(char *filename)
{
  int fd;
  GList *ProcedureList;

  fd = open(filename,O_RDONLY);
  ProcedureList = procedure_parse_file(fd);
  close(fd);
  return(ProcedureList);
}

