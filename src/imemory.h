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


#ifndef IMEMORY_H
#define IMEMORY_H


#include "globals.h"



#define NULL_TYPE 0
#define GRID_TYPE 1
#define IXPLANE_TYPE 2
#define PTFLAG_TYPE 3
#define PTAUX_TYPE 4
#define PCUBE_TYPE 5
#define POLFVERT_TYPE 6
#define NXA_TYPE 7
#define PNX_TYPE 8
#define TRISTRIPE_TYPE 9
#define VETPOL_TYPE 10
#define CVX_TYPE 11
#define CVY_TYPE 12
#define CVZ_TYPE 13
#define CNX_TYPE 14
#define CNY_TYPE 15
#define CNZ_TYPE 16
#define PTS_TYPE 17
#define HSLICE_TYPE 18
#define VSLICE_TYPE 19
#define MHRECT_TYPE 20
#define MVRECT_TYPE 21
#define CVX1H_TYPE 22
#define CVY1H_TYPE 23
#define CVZ1H_TYPE 24
#define CVX2H_TYPE 25
#define CVY2H_TYPE 26
#define CVZ2H_TYPE 27
#define CVX3H_TYPE 28
#define CVY3H_TYPE 29
#define CVZ3H_TYPE 30
#define CVX1V_TYPE 31
#define CVY1V_TYPE 32
#define CVZ1V_TYPE 33
#define CVX2V_TYPE 34
#define CVY2V_TYPE 35
#define CVZ2V_TYPE 36
#define CVX3V_TYPE 37
#define CVY3V_TYPE 38
#define CVZ3V_TYPE 39
#define VXH_TYPE 40
#define VYH_TYPE 41
#define VZH_TYPE 42
#define INDEXESH_TYPE 43
#define VXV_TYPE 44
#define VYV_TYPE 45
#define VZV_TYPE 46
#define INDEXESV_TYPE 47
#define WINDXH_TYPE 48
#define WINDYH_TYPE 49
#define WINDZH_TYPE 50
#define WINDXV_TYPE 51
#define WINDYV_TYPE 52
#define WINDZV_TYPE 53
#define TRAJX_TYPE 54
#define TRAJY_TYPE 55
#define TRAJZ_TYPE 56
#define TRAJXR_TYPE 57
#define TRAJYR_TYPE 58
#define TRAJZR_TYPE 59
#define START_TYPE 60
#define LEN_TYPE 61
#define STREAM1_TYPE 62
#define STREAM2_TYPE 63
#define STREAM3_TYPE 64
#define SOUND_TYPE 65
#define UWIND_TYPE 66
#define VWIND_TYPE 67
#define VERTDATA_TYPE 68

extern int init_irregular_memory( Irregular_Context itx, int bytes );

extern int i_init_shared_memory( Irregular_Context itx, void *start, int bytes );

extern int i_reinit_memory( Irregular_Context itx );

extern void *i_allocate( Irregular_Context itx, int bytes );

extern void *i_allocate_type( Irregular_Context itx, int bytes, int type );

extern void *i_pallocate( Irregular_Context itx, int bytes );

extern void i_deallocate( Irregular_Context itx, void *addr, int bytes );

extern int i_mem_available( Irregular_Context itx );




#endif
