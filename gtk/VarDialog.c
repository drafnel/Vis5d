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




GdkColor *
toggle_hslice(v5d_var_info *vinfo, gboolean toggleon)
{
  int times, numtimes,curtime;
  gchar labelstring[80];
  gchar *listentry[1];

  GdkColor *gcolor=NULL;
  GList *newnode;
  gchar *nstr[1];

  GtkWidget *HSliceControls;

  if(toggleon){
	 vis5d_get_ctx_numtimes( vinfo->v5d_data_context, &numtimes );
	 vis5d_get_ctx_timestep( vinfo->v5d_data_context,  &curtime);
	 for ( times = 0; times < numtimes; times++){
		vis5d_make_hslice( vinfo->v5d_data_context, times, vinfo->varid, times==curtime);
	 }

  
	 vis5d_get_ctx_var_name(vinfo->v5d_data_context,vinfo->varid,vinfo->vname);


	 /*  
	 g_snprintf(labelstring,80,_("HSlice from %4.4g to %4.4g by %4.4g at %4.4g MB"),
					low,high,interval,pressure);
	 */

	 vis5d_enable_graphics(vinfo->v5d_data_context, VIS5D_HSLICE,
								  vinfo->varid, VIS5D_ON);

	 update_hslice_controls(vinfo);

  }else{
	 vis5d_enable_graphics(vinfo->v5d_data_context, VIS5D_HSLICE,
								  vinfo->varid, VIS5D_OFF);
  }
  return gcolor;

}



