#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <glib.h>
#include "procedure.h"

/* define enumeration values to be returned for specific symbols */
/* a SCOPE_SYMBOL is one which will change the current parser scope */

enum {
  SCOPE_SYMBOL_IMAGE  = G_TOKEN_LAST + 1,
  SYMBOL_NAME,
  SCOPE_SYMBOL_HSLICE,
    SYMBOL_VAR,
    SYMBOL_MIN,
    SYMBOL_MAX,
    SYMBOL_INT,
    SYMBOL_LEV,
    SYMBOL_STIPPLE,
    SYMBOL_WIDTH,
    SYMBOL_COLOR
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
    { "level", SYMBOL_LEV,  SCOPE_SYMBOL_HSLICE,},
    { "interval", SYMBOL_INT,  SCOPE_SYMBOL_HSLICE,},
    { "stipple", SYMBOL_STIPPLE, SCOPE_SYMBOL_HSLICE,},
    { "width", SYMBOL_WIDTH, SCOPE_SYMBOL_HSLICE,},
    { "color", SYMBOL_COLOR, SCOPE_SYMBOL_HSLICE,},

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

GList *procedure_add_item(GList *Procedure, gpointer item, gint itemtype, gboolean NewImage, gchar *imagename)
{
  Image *oneimage;
  if(Procedure && NewImage==FALSE){
    oneimage = (Image *) g_list_last(Procedure)->data;
  }else{
	 oneimage = g_new(Image,1); /* *) g_malloc(sizeof(Image)); */
	 oneimage->items=g_ptr_array_new();
	 oneimage->item_type=g_array_new(FALSE,TRUE,sizeof(gint));
  }
  
  g_ptr_array_add(oneimage->items,(gpointer) item);
  g_array_append_val(oneimage->item_type,itemtype);

  if(imagename){
	 oneimage->name = g_strdup(imagename);
  }

  if(NewImage==TRUE){
	 g_list_append(Procedure, (gpointer) oneimage);
  }
  return Procedure;
}



static guint
parse_symbol (GScanner *scanner, GList **ProcedureList)
{
  guint symbol;
  guint tmp;
  Image *oneimage;
  hslicecontrols *onehslice;
  
  /* expect a valid symbol */
  g_scanner_get_next_token (scanner);
  symbol = scanner->token;

  if(scanner->token == '}'){    
	 g_scanner_set_scope(scanner,context[--context_depth]);

	 printf("leaving scope\n");
	 /*
	 sleep(4);
	 */
	 return G_TOKEN_NONE;
  }
  
  if (symbol < SCOPE_SYMBOL_IMAGE ||
		symbol > SYMBOL_COLOR)
	 return G_TOKEN_SYMBOL;


  g_scanner_get_next_token (scanner);


  switch (context[context_depth]){
  case 0:
	 if(scanner->token != '{')
		return '{';
	 context[++context_depth] = symbol;
	 g_scanner_set_scope(scanner,context[context_depth]);

		printf("new image\n");
		/*
		sleep(4);
		*/
	 oneimage = g_new(Image,1); /* *) g_malloc(sizeof(Image));*/
	 oneimage->name = NULL;
	 oneimage->items=g_ptr_array_new();
	 oneimage->item_type=g_array_new(FALSE,TRUE,sizeof(gint));
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
		if(scanner->token != '{')
		  return '{';

		printf("new hslice\n");
		/*
		sleep(4);
		*/
		context[++context_depth] = symbol;
		g_scanner_set_scope(scanner,context[context_depth]);
		onehslice = g_new0(hslicecontrols,1); /* *) g_malloc(sizeof(hslicecontrols));*/
		onehslice->var=NULL;
		*ProcedureList = procedure_add_item(*ProcedureList,(gpointer) onehslice, HSLICE, FALSE, NULL);

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
	 case SYMBOL_LEV:
		return token_float(scanner, &onehslice->level);
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
	 break;
  default:
	 /* flag an unrecognized context */
	 printf("bad context %d\n",context);
  }

  return G_TOKEN_NONE;
}

void 
print_hslicecontrols(FILE *fp, hslicecontrols *hslice)
{
  if(! hslice->var)
	 {
		g_print("hslicecontrols is invalid without a variable name\n");
		return;
	 }
  fprintf(fp,"  hslice {\n");
  fprintf(fp,"    var = \"%s\";\n",hslice->var);
  fprintf(fp,"    min = %g;\n",hslice->min);
  fprintf(fp,"    max = %g;\n",hslice->max);
  fprintf(fp,"    level = %g;\n",hslice->level);
  if(hslice->interval){
	 /* this is a contour isoline slice */
	 fprintf(fp,"    interval = %g;\n",hslice->interval);
	 fprintf(fp,"    stipple = 0x%x;\n",(unsigned short) hslice->stipple);
	 fprintf(fp,"    width = %d;\n",hslice->linewidth);
	 fprintf(fp,"    color = [%g, %g, %g, %g];\n"
				,hslice->color[0],hslice->color[1],
				hslice->color[2],hslice->color[3]);
  }else{
	 /* this is a contour color filled slice */

  }
  fprintf(fp,"  }\n");
}

int 
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
	 if(name=((Image *)nextimage->data)->name){
		fprintf(fp,"  name=\"%s\";\n",name);
	 }
	 items = (GPtrArray *) ((Image *)nextimage->data)->items;
	 item_types = (GArray *) ((Image *)nextimage->data)->item_type;
	 for(i=0;i<items->len;i++){
		switch(g_array_index(item_types,gint,i)){
		case HSLICE:
		  print_hslicecontrols(fp, (hslicecontrols *) g_ptr_array_index(items,i));
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
  if(hslice->var)
	 free(hslice->var);
  free(hslice);
}


void procedure_free(GList *Procedure)
{
  GList *imagelist;
  Image *image;
  gint i;

  imagelist = g_list_first(Procedure);
  while(imagelist){
	 image = (Image *) imagelist->data;
	 if(image){
		if(image->name)
		  free(image->name);
		

		for(i=0;i<image->items->len;i++)
		  switch (g_array_index(image->item_type,gint,i)){
		  case SCOPE_SYMBOL_HSLICE:
			 hslice_free((hslicecontrols *) g_ptr_array_index(image->items,i));
			 break;
		  default:
			 g_print("Unrecogized type in free\n");

		  }
	 }		 				
	 g_array_free(image->item_type,TRUE);
	 
	 g_ptr_array_free(image->items,TRUE);
	 
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

