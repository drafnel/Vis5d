/* record.h */

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
 
 
#ifndef RECORD_H
#define RECORD_H

#include "globals.h"
#include "api.h"
#include "irregular_v5d.h"

extern int init_record_cache( Irregular_Context itx, int maxbytes, float *ratio );

extern int open_recordfile(Irregular_Context itx, char *filename);

extern int initially_open_recordfile( char filename[], irregular_v5dstruct *iv );

extern void preload_irregular_cache( Irregular_Context itx );

extern void load_geo_data( Irregular_Context itx );

extern void get_record_location( Irregular_Context itx, int time, int rec,
                           float *lat, float *lon, float *alt);

extern void get_record_locations( Irregular_Context itx, int time,
                           float *lat, float *lon, float *alt);


extern void get_all_record_numerical_data( Irregular_Context itx, int time,
                                int var, double *data);

extern void get_all_record_char_data( Irregular_Context itx, int time,
                                int var, char *data);

extern void get_some_record_numerical_data( Irregular_Context itx, int time,
                                int var, int ploton[], double *data);

extern void get_some_record_char_data( Irregular_Context itx, int time,
                                int var, int ploton[], char *data);

#endif


