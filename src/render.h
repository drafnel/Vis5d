/* render.h */

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

#ifndef RENDER_H
#define RENDER_H


#include "globals.h"



/* MJK 12.02.98 */
extern void float2string (Display_Context dtx, int icoord, float f, char *str);


extern void plot_string( char *str, float startx, float starty, float startz,
                         float base[], float up[], int rjustify );


extern void render_3d_only( Display_Context dtx, int GoTime );

extern void render_2d_only( Display_Context dtx );

extern void render_sounding_only( Display_Context dtx, int pixmapflag );


extern void render_everything( Display_Context dtx, int GoTime );

extern int check_for_valid_time( Context ctx, int dtxcurtime);

#endif

