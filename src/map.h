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


#ifndef MAP_H
#define MAP_H


#include "globals.h"


extern int init_map( Display_Context dtx, char *mapname );

extern int draw_map( Display_Context dtx, int time, int flat );

/* MJK 12.04.98 */
extern float    get_z_off (Display_Context dtx, float zmin, float zmax);

extern int      bend_line_to_fit_surf (float *verts, int ncols, int nrows,
                                       int grid_scheme,
                                       float xmin, float ymin,
                                       float xmax, float ymax, float zoff,
                                       float *xyz_in, int n_in,
                                       float *xyz_out);

extern int      bend_line_to_fit_topo (Display_Context dtx,
                                       float *xyz_in, int n_in,
                                       float *xyz_out);

extern void     bend_map_seg_to_fit_topo (Display_Context dtx);

#endif
