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


#ifndef FSL_H 
#define FSL_H

/* MJK 12.01.98 */

int follow_slice_link (int index, int *p_type, int *p_num);

int move_linked_slices (int index, int type, int num);

int make_linked_slices (int index, int time, int type, int num, int urgent);

int enable_linked_slices (int index, int type, int num, int mode);

int enable_linked_sfc_slices (int index, int type, int num, int mode);

int sync_linked_slices (int index, int type, int num);

int check_view_side (Context ctx, int type, int num);

int flip_vslice_end_for_end (Context ctx, int time, int var);



#endif

