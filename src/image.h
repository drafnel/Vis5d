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

#ifndef IMAGE_H
#define IMAGE_H


#include "globals.h"


extern void init_image( Display_Context dtx );


extern void define_texture( Display_Context dtx, int time, int width, int height,
                            int components, void *image );

extern int use_texture( Display_Context dtx, int time );


extern int read_texture_image( Display_Context dtx, char *filename );


extern int read_texture_sequence( Display_Context dtx, char *name );


extern int read_texture_areas( Display_Context dtx, int first );

extern int texture_quadmeshnorm( int rows, int cols, float vert[][3],
                                 float norm[][3], float texcoord[][2] );

#endif


