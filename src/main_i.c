/* main.c */

/*
 * Vis5D system for visualizing five dimensional gridded data sets.
 * Copyright (C) 1990 - 2000 Bill Hibbard, Johan Kellum, Brian Paul,
 * Dave Santek, and Andre Battaiola.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * As a special exception to the terms of the GNU General Public
 * License, you are permitted to link Vis5D with (and distribute the
 * resulting source and executables) the LUI library (copyright by
 * Stellar Computer Inc. and licensed for distribution with Vis5D),
 * the McIDAS library, and/or the NetCDF library, where those
 * libraries are governed by the terms of their own licenses.
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

#include "../config.h"



#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "analyze_i.h"
#include "file_i.h"
#include "grid_i.h"
#include "gui_i.h"
#include "select_i.h"
#include "ui_i.h"
#include "../src/v5d.h"


extern int Debug_i;           /* -debug  in read_grid_i.c*/
char *path = NULL;   /* -path */


int main_irun( Display *guidpy, int argc, char *argv[] )
{
   struct grid_db *db;
   v5dstruct *v5dout;
   int i;
   int text_ui = 0;

   db = alloc_grid_db();
   v5dout = v5dNewStruct();

   Debug_i = 0;

   /* Read initial input files */
   if (argc>1) {
      for (i=1;i<argc;i++) {
         if (strcmp(argv[i],"-t")==0) {
            text_ui = 1;
         }
         else if (strcmp(argv[i],"-debug")==0) {
            Debug_i = 1;
         }
         else if (strcmp(argv[i],"-path")==0) {
            path = argv[i+1];
            i++;
         }
         else {
            get_file_info( argv[i], db );
         }
      }
   }


   analyze_grids( db );
   select_all( db, ALL_BITS, 1 );

   setup_defaults( db, v5dout, 1, 1, 1 );

   if (text_ui) {
      /* Text-based user interface */
      ui_loop( db, v5dout );
   }
   else {
      /* Graphical user interface */
      make_gui_i(guidpy);
      gui_loop( db, v5dout );
   }

   return 0;
}

