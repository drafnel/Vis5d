/* proj.h */

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


/*
 * Map projection / coordinate transformation
 */


#ifndef PROJ_i_H
#define PROJ_i_H


extern int rowcol_to_latlon_i( float row, float col,
                             float *lat, float *lon,
                             struct projection *proj );



extern int latlon_to_rowcol_i( float lat, float lon,
                             float *row, float *col,
                             struct projection *proj );



extern float proj_resolution( struct projection *proj );


extern int height_to_level( float height, float *level,
                            struct vcs *vcs, float topo_elev );


extern int level_to_height( float level, float *height,
                            struct vcs *vcs, float topo_elev );


#endif
