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

#ifndef TOPO_H
#define TOPO_H


#include "globals.h"




/* MJK 12.02.98 */ 
#define TOPO_BASE_COLOR       (PACK_COLOR (160, 160, 160, 255))


extern float elevation( Display_Context dtx, struct Topo *topo, float lat, float lon, int *water );


extern int init_topo( Display_Context dtx, char toponame[], int textureflag,
                      int hi_res );


extern void free_topo( struct Topo **topo );


extern void init_topo_color_table( unsigned int ct[], int size,
                                   float minght, float maxhgt );


extern void recolor_topo( Display_Context dtx, unsigned int ct[], int size );


extern void draw_topo( Display_Context dtx, int time,
                       int texture_flag, int flat_flag );


extern int read_topo( struct Topo *topo, char *filename );

void set_topo_sampling(struct Topo *topo, float latres, float lonres );

#endif

