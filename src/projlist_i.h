/* projlist.h */

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


#ifndef PROJLIST_H
#define PROJLIST_H


#include "grid.h"


extern struct projection *new_projection( struct grid_db *db,
                                          int kind, int nr, int nc,
                                          float *args );


extern void free_projection( struct grid_db *db,
                             struct projection *proj );


extern int lookup_proj( struct grid_db *db, struct projection *proj );


extern char **sprint_projection_list( struct grid_db *db );


extern void print_projection_list( struct grid_db *db );


extern struct vcs *new_vcs( struct grid_db *db, int kind,
                            int nl, int lowlev, float *args );


extern void free_vcs( struct grid_db *db, struct vcs *vcs );


extern int make_vcs_list( struct vcs **vcslist, int max_vcs );


extern int lookup_vcs( struct grid_db *db, struct vcs *vcs );


extern char **sprint_vcs_list( struct grid_db *db );


extern void print_vcs_list( struct grid_db *db );


#endif
