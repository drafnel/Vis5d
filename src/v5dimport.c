 
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
#include <stdlib.h>
#include <X11/Xlib.h>
#include "api.h"

/* About this file ...

   Long, long ago v5dimport existed only as a standalone program separate
   from vis5d. Then it was copied into this "src" directory, renaming
   each *.h or *.c source file to *_i.h or *_i.c, renaming a few global
   variables to also add "_i" so that they wouldn't conflict with other
   identically-named vis5d variables, and it was turned into a subroutine
   called "main_irun" that could be called from an "IMPORT..." button in
   vis5d itself.

   This was a nice idea, but some people are used to running v5dimport as
   a standalone program, so this one simple source file compiles to an
   executable that gives them that capability.
*/
extern char toponame[]; /* resample_i.c */

#define TOPO_FILE "EARTH.TOPO"
int main_irun( Display *guidpy, int standalone, int argc, char *argv[] );

int main( int argc, char *argv[] )
{
  int result;

  Display* guidpy = XOpenDisplay( NULL );

  Vis5dDataPath[0]=0;  /* from api.c & api.h */

  if (getenv("VIS5D_PATH") ){
	 strcpy( Vis5dDataPath, getenv("VIS5D_PATH") );
  }
#ifdef DATA_PREFIX
  if (Vis5dDataPath[0] == 0) { /* SGJ 1/11/00 */
	 strcpy(Vis5dDataPath, DATA_PREFIX);
  }
#endif

  if (getenv("VIS5D_TOPO") ){
	 sprintf( toponame,"%s/%s",Vis5dDataPath,getenv("VIS5D_TOPO") );
  }else{
	 sprintf( toponame,"%s/EARTH.TOPO",Vis5dDataPath );
  }	 
    

  printf("Setting toponame %s\n",toponame);
  result = main_irun( guidpy, 1, argc, argv );
  return result;
}

