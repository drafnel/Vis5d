/* save.c */


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


/* Graphics Save & Restore:

   The file is a list of tagged blocks.  A block starts with
   a tag and length.  If the restore function finds an unknown tag, it
   can skip the block and continue.  This provides for backward
   compatibility with future formats.

   Note:  .SAVE files are written with native byte ordering.  That is,
   a .SAVE file made on a little endian computer (DEC) can't be read on
   a big endian computer (SGI), and vice-versa.
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "api.h"
#include "globals.h"
#include "grid.h"
#include "memory.h"
#include "misc.h"
#include "sync.h"
#include "topo.h"
#include "labels.h"


#ifndef SEEK_SET
#  define SEEK_SET 0
#endif
#ifndef SEEK_CUR
#  define SEEK_CUR 1
#endif



#define INT_1_SIZE  sizeof(int_1)
#define UINT_1_SIZE sizeof(uint_1)
#define INT_2_SIZE  sizeof(int_2)
#define UINT_2_SIZE sizeof(uint_2)
#define UINT_4_SIZE sizeof(uint_4)
#define INT_SIZE    sizeof(int)
#define UINT_SIZE   sizeof(unsigned int)
#define FLOAT_SIZE  sizeof(float)


#define TAG_SIZE 4

#define TAG_ISO_COLOR        11
#define TAG_HSLICE_COLOR     12
#define TAG_VSLICE_COLOR     13
#define TAG_CHSLICE_COLORS   14
#define TAG_CVSLICE_COLORS   15
#define TAG_TRAJ_COLOR       16
#define TAG_HWIND_COLOR      18
#define TAG_VWIND_COLOR      19
#define TAG_VOLUME_COLORS    20
#define TAG_HSTREAM_COLOR    21
#define TAG_ISO_COLOR_TABLE  22
#define TAG_VSTREAM_COLOR    23


#define TAG_CTM              30
#define TAG_LABEL            31
#define TAG_TOPO_COLORS      32
#define TAG_CLONE_TABLE      33   /* Obsolete, don't use */
#define TAG_OBS_34           34   /* OBSOLETE */
#define TAG_OBS_35           35   /* OBSOLETE, was TAG_EXT_FUNC */
#define TAG_VAR_TABLE_41     36
#define TAG_EXT_FUNC         37
#define TAG_EXPRESSION       38
#define TAG_VAR_TABLE        39

#define TAG_ISO_LEVEL        40
#define TAG_HSLICE_POS       41
#define TAG_VSLICE_POS       42
#define TAG_CHSLICE_POS      43
#define TAG_CVSLICE_POS      44
#define TAG_HWIND_POS        45
#define TAG_VWIND_POS        46
#define TAG_HSTREAM_POS      47
#define TAG_VSTREAM_POS      48

#define TAG_HSLICE           60
#define TAG_VSLICE           61
#define TAG_CHSLICE          62
#define TAG_CVSLICE          63
#define TAG_HWIND            64
#define TAG_VWIND            65
#define TAG_ISOSURF          66
#define TAG_TRAJ             67
#define TAG_HSTREAM          68
#define TAG_COLORED_ISOSURF  69
#define TAG_VSTREAM          70



/**
*** IMPORTANT:  If you add new tags, use new numbers which are higher than
*** existing tags since lower number ones may have been used in the past
*** and are now obsolete.
***/


#define FWRITE( BUF, SIZE, ITEMS, F )  if (fwrite(BUF,SIZE,ITEMS,F)!=ITEMS) { \
                                          fclose(F);              \
                                          /* unlock graphics */   \
                                          LOCK_OFF( GfxLock );    \
                                          return VIS5D_FAIL;      \
                                       }

#define WRITE_TAG( F, T )        fwrite( &T, sizeof(int), 1, F )
#define WRITE_INTS( F, I, N )    fwrite( I, sizeof(int), N, F )
#define WRITE_FLOATS( F, X, N )  fwrite( X, sizeof(float), N, F )
#define WRITE_BYTES( F, B, N )   fwrite( B, 1, N, f )

#define READ_INTS( F, I, N )     fread( I, sizeof(int), N, F )
#define READ_FLOATS( F, X, N )   fread( X, sizeof(int), N, F )
#define READ_BYTES( F, B, N )    fread( B, 1, N, F )


/* skip 'n' bytes in file 'f' */
static void skip( FILE *f, int n )
{
   if (n>0) {
      fseek( f, n, SEEK_CUR );
   }
}


static long start_pos = 0; /* position in file where block length is stored */



/*
 * Call this function before writing a block of "SAVE/RESTORE" data.
 */
static void begin_block( FILE *f, int tag )
{
   int zero = 0;

   if (start_pos) {
      printf("Error in begin_block()\n");
      return;
   }
   WRITE_INTS( f, &tag, 1 );   /* write the tag */
   start_pos = ftell( f );
   WRITE_INTS( f, &zero, 1 );   /* dummy length field to be filled in later */
}


/*
 * Call this function after writing a block of "SAVE/RESTORE" data.
 * Return:  number of bytes written between begin_block() and end_block()
 */
static int end_block( FILE *f )
{
   long end_pos;
   int length;

   if (start_pos==0) {
      printf("Error in end_block()\n");
      return 0;
   }
   end_pos = ftell( f );
   length = end_pos - start_pos - 4;
   fseek( f, start_pos, SEEK_SET );
   WRITE_INTS( f, &length, 1 );
   fseek( f, end_pos, SEEK_SET );
   start_pos = 0;

   return length;
}




static int save_isosurfaces( Context ctx, FILE *f )
{
   int iv, it;
   int neg_one = -1;

   for (iv=0;iv<ctx->NumVars;iv++) {
      for (it=0;it<ctx->NumTimes;it++) {
         if (ctx->Variable[iv]->SurfTable[it]->valid) {
            int numverts, numindex;

            begin_block( f, TAG_COLORED_ISOSURF );

            numverts = ctx->Variable[iv]->SurfTable[it]->numverts;
            numindex = ctx->Variable[iv]->SurfTable[it]->numindex;

            /* isosurface data */
            FWRITE( &iv, INT_SIZE, 1, f );  /* parm */
            FWRITE( &it, INT_SIZE, 1, f );  /* time */
            FWRITE( &ctx->Variable[iv]->SurfTable[it]->isolevel, FLOAT_SIZE, 1, f );
            FWRITE( &numverts, INT_SIZE, 1, f );  /* number of vertices */
            FWRITE( &numindex, INT_SIZE, 1, f );
            FWRITE( ctx->Variable[iv]->SurfTable[it]->verts, INT_2_SIZE, 3*numverts, f );
            FWRITE( ctx->Variable[iv]->SurfTable[it]->norms, INT_1_SIZE, 3*numverts, f );
#ifdef BIG_GFX
            FWRITE( ctx->Variable[iv]->SurfTable[it]->index, UINT_4_SIZE, numindex, f );
#else
            FWRITE( ctx->Variable[iv]->SurfTable[it]->index, UINT_2_SIZE, numindex, f );
#endif
            if (ctx->Variable[iv]->SurfTable[it]->colors) {
               FWRITE( &ctx->Variable[iv]->SurfTable[it]->colorvar, INT_SIZE, 1, f );
               FWRITE( ctx->Variable[iv]->SurfTable[it]->colors, UINT_1_SIZE,
                       numverts, f );
            }
            else {
               FWRITE( &neg_one, INT_SIZE, 1, f );
            }

            end_block(f);
         }
      }
   }
   return 0;
}

#ifdef LEAVEOUT

static int save_iso_colors( Context ctx, FILE *f )
{
   int iv;

   for (iv=0;iv<ctx->NumVars;iv++) {
      begin_block( f, TAG_ISO_COLOR );
      FWRITE( &iv, INT_SIZE, 1, f );
/* YO Color and the rest of these! */
      FWRITE( &ctx->dpy_ctx->Color[iv][ISOSURF], UINT_SIZE, 1, f );
      end_block(f);
   }
   for (iv=0;iv<ctx->NumVars;iv++) {
      begin_block( f, TAG_ISO_COLOR_TABLE );
      FWRITE( &iv, INT_SIZE, 1, f );
      FWRITE( &ctx->dpy_ctx->ColorTable[VIS5D_ISOSURF_CT]->Colors[iv], UINT_SIZE, 256, f );
      end_block(f);
   }
   return 0;
}


static int save_iso_level( Context ctx, FILE *f )
{
   int iv;
   
   for (iv=0;iv<ctx->NumVars;iv++) {
      begin_block( f, TAG_ISO_LEVEL );
      WRITE_INTS( f, &iv, 1 );
      WRITE_FLOATS( f, &ctx->IsoLevel[iv], 1 );
      end_block(f);
   }
   return 0;
}

#endif

static int save_hslices( Context ctx, FILE *f )
{
   int iv, it;
   int num1, num2, num3, num4;

   for (iv=0;iv<ctx->NumVars;iv++) {
      for (it=0;it<ctx->NumTimes;it++) {
         if (ctx->Variable[iv]->HSliceTable[it] && ctx->Variable[iv]->HSliceTable[it]->valid) {
            begin_block( f, TAG_HSLICE );
            num1 = ctx->Variable[iv]->HSliceTable[it]->num1;
            num2 = ctx->Variable[iv]->HSliceTable[it]->num2;
            num3 = ctx->Variable[iv]->HSliceTable[it]->num3;
            num4 = ctx->Variable[iv]->HSliceTable[it]->numboxverts;
            /* contour line data */
            FWRITE( &iv, INT_SIZE, 1, f );  /* parm */
            FWRITE( &it, INT_SIZE, 1, f );  /* time */
            FWRITE( &ctx->Variable[iv]->HSliceTable[it]->interval, FLOAT_SIZE, 1, f );
            FWRITE( &ctx->Variable[iv]->HSliceTable[it]->lowlimit, FLOAT_SIZE, 1, f );
            FWRITE( &ctx->Variable[iv]->HSliceTable[it]->highlimit, FLOAT_SIZE, 1, f );
            FWRITE( &ctx->Variable[iv]->HSliceTable[it]->level, FLOAT_SIZE, 1, f );
            FWRITE( &num1, INT_SIZE, 1, f );
            FWRITE( ctx->Variable[iv]->HSliceTable[it]->verts1, INT_2_SIZE, 3*num1, f );
            FWRITE( &num2, INT_SIZE, 1, f );
            FWRITE( ctx->Variable[iv]->HSliceTable[it]->verts2, INT_2_SIZE, 3*num2, f );
            FWRITE( &num3, INT_SIZE, 1, f );
            FWRITE( ctx->Variable[iv]->HSliceTable[it]->verts3, INT_2_SIZE, 3*num3, f );
            FWRITE( &num4, INT_SIZE, 1, f );
            FWRITE( ctx->Variable[iv]->HSliceTable[it]->boxverts, 3*FLOAT_SIZE, num4, f );
            end_block(f);
         }
      }
   }
   return 0;
}

#ifdef LEAVEOUT
static int save_hslice_colors( Context ctx, FILE *f )
{
   int iv;

   /* Save horizontal contour line slice colors */
   for (iv=0;iv<ctx->NumVars;iv++) {
      begin_block( f, TAG_HSLICE_COLOR );
      FWRITE( &iv, INT_SIZE, 1, f );
      FWRITE( &ctx->dpy_ctx->Color[iv][HSLICE], UINT_SIZE, 1, f );
      end_block(f);
   }
   return 0;
}


static int save_hslice_pos( Context ctx, FILE *f )
{
   int iv;

   for (iv=0;iv<ctx->NumVars;iv++) {
      begin_block( f, TAG_HSLICE_POS );
      WRITE_INTS( f, &iv, 1 );
      WRITE_FLOATS( f, &ctx->Variable[iv]->HSliceRequest->Interval, 1 );
      WRITE_FLOATS( f, &ctx->Variable[iv]->HSliceRequest->LowLimit, 1 );
      WRITE_FLOATS( f, &ctx->Variable[iv]->HSliceRequest->HighLimit, 1 );
      WRITE_FLOATS( f, &ctx->Variable[iv]->HSliceRequest->Level, 1 );
      end_block(f);
   }
   return 0;
}
#endif

static int save_vslices( Context ctx, FILE *f )
{
   int iv, it;
   int num1, num2, num3, num4;
     
   for (iv=0;iv<ctx->NumVars;iv++) {
      for (it=0;it<ctx->NumTimes;it++) {
         if (ctx->Variable[iv]->VSliceTable[it]->valid) {
            begin_block( f, TAG_VSLICE );
            num1 = ctx->Variable[iv]->VSliceTable[it]->num1;
            num2 = ctx->Variable[iv]->VSliceTable[it]->num2;
            num3 = ctx->Variable[iv]->VSliceTable[it]->num3;
            num4 = ctx->Variable[iv]->VSliceTable[it]->numboxverts;
            /* contour line data */
            FWRITE( &iv, INT_SIZE, 1, f );  /* parm */
            FWRITE( &it, INT_SIZE, 1, f );  /* time */
            FWRITE( &ctx->Variable[iv]->VSliceTable[it]->interval, FLOAT_SIZE, 1, f );
            FWRITE( &ctx->Variable[iv]->VSliceTable[it]->lowlimit, FLOAT_SIZE, 1, f );
            FWRITE( &ctx->Variable[iv]->VSliceTable[it]->highlimit, FLOAT_SIZE, 1, f );
            FWRITE( &ctx->Variable[iv]->VSliceTable[it]->r1, FLOAT_SIZE, 1, f );
            FWRITE( &ctx->Variable[iv]->VSliceTable[it]->c1, FLOAT_SIZE, 1, f );
            FWRITE( &ctx->Variable[iv]->VSliceTable[it]->r2, FLOAT_SIZE, 1, f );
            FWRITE( &ctx->Variable[iv]->VSliceTable[it]->c2, FLOAT_SIZE, 1, f );
            FWRITE( &num1, INT_SIZE, 1, f );
            FWRITE( ctx->Variable[iv]->VSliceTable[it]->verts1, INT_2_SIZE, 3*num1, f );
            FWRITE( &num2, INT_SIZE, 1, f );
            FWRITE( ctx->Variable[iv]->VSliceTable[it]->verts2, INT_2_SIZE, 3*num2, f );
            FWRITE( &num3, INT_SIZE, 1, f );
            FWRITE( ctx->Variable[iv]->VSliceTable[it]->verts3, INT_2_SIZE, 3*num3, f );
            FWRITE( &num4, INT_SIZE, 1, f );
            FWRITE( ctx->Variable[iv]->VSliceTable[it]->boxverts, 3*FLOAT_SIZE, num4, f );
            end_block(f);
         }
      }
   }
   return 0;
}

#ifdef LEAVEOUT

static int save_vslice_colors( Context ctx, FILE *f )
{
   int iv;

   /* save vertical contour slice colors */
   for (iv=0;iv<ctx->NumVars;iv++) {
      begin_block( f, TAG_VSLICE_COLOR );
      FWRITE( &iv, INT_SIZE, 1, f );
      FWRITE( &ctx->dpy_ctx->Color[iv][VSLICE], UINT_SIZE, 1, f );
      end_block(f);
   }
   return 0;
}



static int save_vslice_pos( Context ctx, FILE *f )
{
   int iv;
     
   for (iv=0;iv<ctx->NumVars;iv++) {
      begin_block( f, TAG_VSLICE_POS );
      WRITE_INTS( f, &iv, 1 );
      WRITE_FLOATS( f, &ctx->Variable[iv]->VSliceRequest->Interval, 1 );
      WRITE_FLOATS( f, &ctx->Variable[iv]->VSliceRequest->LowLimit, 1 );
      WRITE_FLOATS( f, &ctx->Variable[iv]->VSliceRequest->HighLimit, 1 );
      WRITE_FLOATS( f, &ctx->Variable[iv]->VSliceRequest->R1, 1 );
      WRITE_FLOATS( f, &ctx->Variable[iv]->VSliceRequest->R2, 1 );
      WRITE_FLOATS( f, &ctx->Variable[iv]->VSliceRequest->C1, 1 );
      WRITE_FLOATS( f, &ctx->Variable[iv]->VSliceRequest->C2, 1 );
      end_block(f);
   }
   return 0;
}

#endif

static int save_chslices( Context ctx, FILE *f )
{
   int it, iv;

   for (iv=0;iv<ctx->NumVars;iv++) {
      for (it=0;it<ctx->NumTimes;it++) {
         if (ctx->Variable[iv]->CHSliceTable[it]->valid) {
            int num;
            begin_block( f, TAG_CHSLICE );
            num = ctx->Variable[iv]->CHSliceTable[it]->rows
                * ctx->Variable[iv]->CHSliceTable[it]->columns;
            FWRITE( &iv, INT_SIZE, 1, f );  
            FWRITE( &it, INT_SIZE, 1, f );  
            FWRITE( &ctx->Variable[iv]->CHSliceTable[it]->level, FLOAT_SIZE, 1, f );
            FWRITE( &ctx->Variable[iv]->CHSliceTable[it]->rows, INT_SIZE, 1, f );
            FWRITE( &ctx->Variable[iv]->CHSliceTable[it]->columns, INT_SIZE, 1, f );
            FWRITE( ctx->Variable[iv]->CHSliceTable[it]->verts, INT_2_SIZE, 3*num, f );
            FWRITE( ctx->Variable[iv]->CHSliceTable[it]->color_indexes, UINT_1_SIZE, num, f );
            end_block(f);
         }
      }
   }
   return 0;
}

#ifdef LEAVEOUT

static int save_chslice_colors( Context ctx, FILE *f )
{
   int iv, size;

   /* save horizontal color slice colors */
   for (iv=0;iv<ctx->NumVars;iv++) {
      begin_block( f, TAG_CHSLICE_COLORS );
      size = 256;
      /* CHSLICE colors */
      FWRITE( &iv, INT_SIZE, 1, f );  /* parm */
      FWRITE( &ctx->dpy_ctx->Color[iv][CHSLICE], UINT_SIZE, 1, f );
      FWRITE( &size, INT_SIZE, 1, f );
/*Y0
      FWRITE( ctx->ColorTable[VIS5D_CHSLICE_CT]->Colors[iv], UINT_SIZE, size, f );
*/
      end_block(f);
   }
   return 0;
}


static int save_chslice_pos( Context ctx, FILE *f )
{
   int iv;

   for (iv=0;iv<ctx->NumVars;iv++) {
      begin_block( f, TAG_CHSLICE_POS );
      WRITE_INTS( f, &iv, 1 );  
      WRITE_FLOATS( f, &ctx->Variable[iv]->CHSliceRequest->Level, 1 );
      end_block(f);
   }
   return 0;
}

#endif

static int save_cvslices( Context ctx, FILE *f )
{
   int iv, it;

   for (iv=0;iv<ctx->NumVars;iv++) {
      for (it=0;it<ctx->NumTimes;it++) {
         if (ctx->Variable[iv]->CVSliceTable[it]->valid) {
            int num;
            begin_block( f, TAG_CVSLICE );
            num = ctx->Variable[iv]->CVSliceTable[it]->rows
                * ctx->Variable[iv]->CVSliceTable[it]->columns;
            FWRITE( &iv, INT_SIZE, 1, f );
            FWRITE( &it, INT_SIZE, 1, f );  
            FWRITE( &ctx->Variable[iv]->CVSliceTable[it]->r1, FLOAT_SIZE, 1, f );
            FWRITE( &ctx->Variable[iv]->CVSliceTable[it]->c1, FLOAT_SIZE, 1, f );
            FWRITE( &ctx->Variable[iv]->CVSliceTable[it]->r2, FLOAT_SIZE, 1, f );
            FWRITE( &ctx->Variable[iv]->CVSliceTable[it]->c2, FLOAT_SIZE, 1, f );
            FWRITE( &ctx->Variable[iv]->CVSliceTable[it]->rows, INT_SIZE, 1, f );
            FWRITE( &ctx->Variable[iv]->CVSliceTable[it]->columns, INT_SIZE, 1, f );
            FWRITE( ctx->Variable[iv]->CVSliceTable[it]->verts, INT_2_SIZE, 3*num, f );
            FWRITE( ctx->Variable[iv]->CVSliceTable[it]->color_indexes, UINT_1_SIZE, num, f );
            end_block(f);
         }
      }
   }
   return 0;
}
#ifdef LEAVEOUT

static int save_cvslice_colors( Context ctx, FILE *f )
{
   int iv, size;

   /* save vertical color slice colors */
   for (iv=0;iv<ctx->NumVars;iv++) {
      begin_block( f, TAG_CVSLICE_COLORS );
      size = 256;
      /* CVSLICE colors */
      FWRITE( &iv, INT_SIZE, 1, f );  /* parm */
/*Y0
      FWRITE( &ctx->dpy_ctx->Color[iv][CVSLICE], UINT_SIZE, 1, f );

      FWRITE( &size, INT_SIZE, 1, f );
      FWRITE( ctx->ColorTable[VIS5D_CVSLICE_CT]->Colors[iv], UINT_SIZE, size, f );
*/      end_block(f);
   }
   return 0;
}



static int save_cvslice_pos( Context ctx, FILE *f )
{
   int iv;

   for (iv=0;iv<ctx->NumVars;iv++) {
      begin_block( f, TAG_CVSLICE_POS );
      WRITE_INTS( f, &iv, 1 );
      WRITE_FLOATS( f, &ctx->Variable[iv]->CVSliceRequest->R1, 1 );
      WRITE_FLOATS( f, &ctx->Variable[iv]->CVSliceRequest->R2, 1 );
      WRITE_FLOATS( f, &ctx->Variable[iv]->CVSliceRequest->C1, 1 );
      WRITE_FLOATS( f, &ctx->Variable[iv]->CVSliceRequest->C2, 1 );
      end_block(f);
   }
   return 0;
}


static int save_trajectories( Context ctx, FILE *f )
{
#ifdef LEAVEOUT
   int i;

   /* save trajectories */
   for (i=0;i<ctx->NumTraj;i++) {
      int length = ctx->TrajTable[i].length;
      begin_block( f, TAG_TRAJ );
      FWRITE( &ctx->TrajTable[i].length, INT_SIZE, 1, f );
      FWRITE( &ctx->TrajTable[i].group, INT_SIZE, 1, f );
      FWRITE( &ctx->TrajTable[i].kind, INT_SIZE, 1, f );
      FWRITE( &ctx->NumTimes, INT_SIZE, 1, f );
      FWRITE( ctx->TrajTable[i].verts, INT_2_SIZE, 3*length, f );
      if (ctx->TrajTable[i].kind==1) {
         FWRITE( ctx->TrajTable[i].norms, INT_1_SIZE, 3*length, f );
      }
      FWRITE( ctx->TrajTable[i].start, UINT_2_SIZE, ctx->NumTimes, f );
      FWRITE( ctx->TrajTable[i].len, UINT_2_SIZE, ctx->NumTimes, f );
      end_block(f);
   }
#endif
   return 0;
}


static int save_traj_colors( Context ctx, FILE *f )
{
   int i;

   /* save traj colors */
   for (i=0;i<VIS5D_TRAJ_SETS;i++) {
      begin_block( f, TAG_TRAJ_COLOR );
      FWRITE( &i, INT_SIZE, 1, f );
/*Y0
      FWRITE( &ctx->TrajColor[i], UINT_SIZE, 1, f );
*/
      end_block(f);
   }
   return 0;
}

#endif
/* YO

static int save_hwind( Context ctx, FILE *f )
{
   int it, nb;
   int ws;


   for (ws=0;ws<VIS5D_WIND_SLICES;ws++) {
      for (it=0;it<ctx->NumTimes;it++) {
         if (ctx->HWindTable[ws][it].valid) {
            int nvect;
            begin_block( f, TAG_HWIND );
            nvect = ctx->HWindTable[ws][it].nvectors;
            nb = ctx->HWindTable[ws][it].numboxverts;
            FWRITE( &ws, INT_SIZE, 1, f );
            FWRITE( &it, INT_SIZE, 1, f );
            FWRITE( &ctx->HWindTable[ws][it].level, FLOAT_SIZE, 1, f );
            FWRITE( &ctx->HWindTable[ws][it].density, FLOAT_SIZE, 1, f );
            FWRITE( &ctx->HWindTable[ws][it].scale, FLOAT_SIZE, 1, f );
            FWRITE( &nvect, INT_SIZE, 1, f );
            FWRITE( ctx->HWindTable[ws][it].verts, INT_2_SIZE, 3*4*nvect, f );
            FWRITE( &nb, INT_SIZE, 1, f );
            FWRITE( ctx->HWindTable[ws][it].boxverts, 3*FLOAT_SIZE, nb, f );
            end_block(f);
         }
      }
   }
   return 0;
}



static int save_hwind_colors( Context ctx, FILE *f )
{
   int i;

   * save horizontal wind slice colors *

   for (i=0;i<VIS5D_WIND_SLICES;i++) {
      begin_block( f, TAG_HWIND_COLOR );
      FWRITE( &i, INT_SIZE, 1, f );
      FWRITE( &ctx->HWindColor[i], UINT_SIZE, 1, f );
      end_block(f);
   }
   return 0;
}



static int save_hwind_pos( Context ctx, FILE *f )
{
   int ws;

   for (ws=0;ws<VIS5D_WIND_SLICES;ws++) {
      begin_block( f, TAG_HWIND_POS );
      FWRITE( &ws, INT_SIZE, 1, f );
      FWRITE( &ctx->HWindLevel[ws], FLOAT_SIZE, 1, f );
      FWRITE( &ctx->HWindDensity[ws], FLOAT_SIZE, 1, f );
      FWRITE( &ctx->HWindScale[ws], FLOAT_SIZE, 1, f );
      end_block(f);
   }
   return 0;
}




static int save_vwind( Context ctx, FILE *f )
{
   int it;
   int ws;

   for (ws=0;ws<VIS5D_WIND_SLICES;ws++) {
      for (it=0;it<ctx->NumTimes;it++) {
         if (ctx->VWindTable[ws][it].valid) {
            int nvect, nb;
            begin_block( f, TAG_VWIND );
            nvect = ctx->VWindTable[ws][it].nvectors;
            nb = ctx->VWindTable[ws][it].numboxverts;
            FWRITE( &ws, INT_SIZE, 1, f );
            FWRITE( &it, INT_SIZE, 1, f );
            FWRITE( &ctx->VWindTable[ws][it].r1, FLOAT_SIZE, 1, f );
            FWRITE( &ctx->VWindTable[ws][it].c1, FLOAT_SIZE, 1, f );
            FWRITE( &ctx->VWindTable[ws][it].r2, FLOAT_SIZE, 1, f );
            FWRITE( &ctx->VWindTable[ws][it].c2, FLOAT_SIZE, 1, f );
            FWRITE( &ctx->VWindTable[ws][it].density, FLOAT_SIZE, 1, f );
            FWRITE( &ctx->VWindTable[ws][it].scale, FLOAT_SIZE, 1, f );
            FWRITE( &nvect, INT_SIZE, 1, f );
            FWRITE( ctx->VWindTable[ws][it].verts, INT_2_SIZE, 3*4*nvect, f );
            FWRITE( &nb, INT_SIZE, 1, f );
            FWRITE( ctx->VWindTable[ws][it].boxverts, 3*FLOAT_SIZE, nb, f );
            end_block(f);
         }
      }
   }
   return 0;
}


static int save_vwind_colors( Context ctx, FILE *f )
{
   int i;

   for (i=0;i<VIS5D_WIND_SLICES;i++) {
      begin_block( f, TAG_VWIND_COLOR );
      FWRITE( &i, INT_SIZE, 1, f );
      FWRITE( &ctx->VWindColor[i], UINT_SIZE, 1, f );
      end_block(f);
   }
   return 0;
}


static int save_vwind_pos( Context ctx, FILE *f )
{
   int ws;

   for (ws=0;ws<VIS5D_WIND_SLICES;ws++) {
      begin_block( f, TAG_VWIND_POS );
      FWRITE( &ws, INT_SIZE, 1, f );
      FWRITE( &ctx->VWindR1[ws], FLOAT_SIZE, 1, f );
      FWRITE( &ctx->VWindC1[ws], FLOAT_SIZE, 1, f );
      FWRITE( &ctx->VWindR2[ws], FLOAT_SIZE, 1, f );
      FWRITE( &ctx->VWindC2[ws], FLOAT_SIZE, 1, f );
      FWRITE( &ctx->VWindDensity[ws], FLOAT_SIZE, 1, f );
      FWRITE( &ctx->VWindScale[ws], FLOAT_SIZE, 1, f );
      end_block(f);
   }
   return 0;
}



static int save_hstream( Context ctx, FILE *f )
{
   int it, nb;
   int ws;

   for (ws=0;ws<VIS5D_WIND_SLICES;ws++) {
      for (it=0;it<ctx->NumTimes;it++) {
         if (ctx->HStreamTable[ws][it].valid) {
            int nline;
            begin_block( f, TAG_HSTREAM );
            nline = ctx->HStreamTable[ws][it].nlines;
            nb = ctx->HStreamTable[ws][it].numboxverts;
            FWRITE( &ws, INT_SIZE, 1, f );
            FWRITE( &it, INT_SIZE, 1, f );
            FWRITE( &ctx->HStreamTable[ws][it].level, FLOAT_SIZE, 1, f );
            FWRITE( &ctx->HStreamTable[ws][it].density, FLOAT_SIZE, 1, f );
            FWRITE( &nline, INT_SIZE, 1, f );
            FWRITE( ctx->HStreamTable[ws][it].verts, INT_2_SIZE, 3*nline, f );
            FWRITE( &nb, INT_SIZE, 1, f );
            FWRITE( ctx->HStreamTable[ws][it].boxverts, 3*FLOAT_SIZE, nb, f );
            end_block(f);
         }
      }
   }
   return 0;
}


static int save_hstream_colors( Context ctx, FILE *f )
{
   int i;

    save horizontal wind slice colors 
   for (i=0;i<VIS5D_WIND_SLICES;i++) {
      begin_block( f, TAG_HSTREAM_COLOR );
      FWRITE( &i, INT_SIZE, 1, f );
      FWRITE( &ctx->HStreamColor[i], UINT_SIZE, 1, f );
      end_block(f);
   }
   return 0;
}



static int save_hstream_pos( Context ctx, FILE *f )
{
   int ws;

   for (ws=0;ws<VIS5D_WIND_SLICES;ws++) {
      begin_block( f, TAG_HSTREAM_POS );
      FWRITE( &ws, INT_SIZE, 1, f );
      FWRITE( &ctx->HStreamLevel[ws], FLOAT_SIZE, 1, f );
      FWRITE( &ctx->HStreamDensity[ws], FLOAT_SIZE, 1, f );
      end_block(f);
   }
   return 0;
}

*/
#ifdef LEAVEOUT
static int save_volume_colors( Context ctx, FILE *f )
{
   int iv, size;

   /* save volume color tables */
   for (iv=0;iv<ctx->NumVars;iv++) {
      begin_block( f, TAG_VOLUME_COLORS );
      size = 256;
      /* CHSLICE colors */
      FWRITE( &iv, INT_SIZE, 1, f );  /* parm */
      FWRITE( &size, INT_SIZE, 1, f );
/* YO      FWRITE( ctx->ColorTable[VIS5D_VOLUME_CT]->Colors[iv], UINT_SIZE, size, f );
   */   end_block(f);
   }
   return 0;
}
#endif

/* YO
static int save_labels( Context ctx, FILE *f, int tag )
{
   struct label *lab;
   int bytes;

   for (lab=ctx->FirstLabel;lab;lab=lab->next) {
      fwrite( &tag, sizeof(int), 1, f );
      bytes = strlen(lab->text)+1 + 3*sizeof(int);
      fwrite( &bytes, 4, 1, f );
      fwrite( &lab->len, sizeof(int), 1, f );     * length of label *
      fwrite( lab->text, lab->len+1, 1, f );   * label text + '\0' *
      fwrite( &lab->x, sizeof(int), 1, f );       * position *
      fwrite( &lab->y, sizeof(int), 1, f );       * position *
   }
   return 0;
}
*/


/*
 * Save current graphics and colors to the 'savefile'
 * Input:  savefile - filename to save to.
 *         save_gfx - if non-zero save graphics, not just colors.
 *         save_traj - if non-zero save trajectories too.
 * Return:  0 for success,
 *          VIS5D_BAD_VALUE - if unable to open output file
 *          VIS5D_FAIL - if error while writing file.
 */
int save( Context ctx, char *savefile, int save_gfx, int save_traj )
{
   FILE *f;
   int it, iv, num;

   f = fopen(savefile,"w");
   if (!f)
      return VIS5D_BAD_VALUE;

   /* lock graphics */
   LOCK_ON( GfxLock );

   /* always save variable information */
   /* Only save information for regular variables and cloned variables. */
   /* When we find something else, stop because restoring the others */
   /* would be a major headache. */
/*
   for (num=0;num<NumVars;num++) {
      if (VarType[num]!=VIS5D_REGULAR && VarType[num]!=VIS5D_CLONE) {
         break;
      }
   }
*/
   num = ctx->NumVars;

   begin_block( f, TAG_VAR_TABLE );
   WRITE_INTS( f, &num, 1 );

   for (iv=0;iv<num;iv++) {
	  /*JPE: loop required for backword compatibility */
	  WRITE_INTS( f, &ctx->Variable[iv]->VarType, 1 );
	}
   for (iv=0;iv<num;iv++) {
	  /*JPE: loop required for backword compatibility */
	  WRITE_INTS( f, &ctx->Variable[iv]->CloneTable, 1 );
	}
   for (iv=0;iv<num;iv++) {
      WRITE_FLOATS( f, &ctx->Variable[iv]->MinVal, 1 );
      WRITE_FLOATS( f, &ctx->Variable[iv]->MaxVal, 1 );
      WRITE_BYTES( f, ctx->Variable[iv]->VarName, 10 );
      WRITE_INTS( f, &ctx->Nl[iv], 1 );
      WRITE_INTS( f, &ctx->Variable[iv]->LowLev, 1 );
   }
   end_block(f);

/*NEW*/
   /* always save external function variables */
   for (iv=0;iv<ctx->NumVars;iv++) {
      if (ctx->Variable[iv]->VarType==VIS5D_EXT_FUNC) {
         begin_block( f, TAG_EXT_FUNC );
         WRITE_INTS( f, &iv, 1 );
         WRITE_INTS( f, &ctx->NumTimes, 1 );
         for (it=0;it<ctx->NumTimes;it++) {
            float *griddata;
            griddata = get_grid( ctx, it, iv );
            WRITE_FLOATS( f, griddata, ctx->Nr*ctx->Nc*ctx->Nl[iv] );
            release_grid( ctx, it, iv, griddata );
         }
         end_block(f);
      }
   }
/*NEW*/
   /* always save type-in function variables */
   for (iv=0;iv<ctx->NumVars;iv++) {
      if (ctx->Variable[iv]->VarType==VIS5D_EXPRESSION) {
         begin_block( f, TAG_EXPRESSION );
         WRITE_INTS( f, &iv, 1 );

			/* JPE: ExpressionList is no longer fixed at 500 bytes */
			/* this may cause an error */
         WRITE_BYTES( f, ctx->Variable[iv]->ExpressionList, 500 );

         WRITE_INTS( f, &ctx->NumTimes, 1 );
         for (it=0;it<ctx->NumTimes;it++) {
            float *griddata;
            griddata = get_grid( ctx, it, iv );
            WRITE_FLOATS( f, griddata, ctx->Nr*ctx->Nc*ctx->Nl[iv] );
            release_grid( ctx, it, iv, griddata );
         }
         end_block(f);
      }
   }


   /* always save colors */
/* YO   save_iso_colors( ctx, f );
   save_hslice_colors( ctx, f );
   save_vslice_colors( ctx, f );
   save_chslice_colors( ctx, f );
   save_cvslice_colors( ctx, f );
   save_traj_colors( ctx, f );
   save_hwind_colors( ctx, f );
   save_vwind_colors( ctx, f );
   save_hstream_colors( ctx, f );
   save_volume_colors( ctx, f );
*/
   if (save_gfx || save_traj) {
      if (save_gfx) {
         /* save isosurfaces and all slices */
         save_isosurfaces( ctx, f );
         save_hslices( ctx, f );
         save_vslices( ctx, f );
         save_chslices( ctx, f );
         save_cvslices( ctx, f );
/* YO
         save_hwind( ctx, f );
         save_vwind( ctx, f );
         save_hstream( ctx, f );
*/      }
      if (save_traj) {
         /* save trajectories */
  /*       save_trajectories( ctx, f ); YO */
      }
   }
   else {
      /* save isolevels, slice positions, etc */
/*YO
      save_iso_level( ctx, f );
      save_hslice_pos( ctx, f );
      save_vslice_pos( ctx, f );
      save_chslice_pos( ctx, f );
      save_cvslice_pos( ctx, f );
      save_hwind_pos( ctx, f );
      save_vwind_pos( ctx, f );
      save_hstream_pos( ctx, f );
*/   }

   /* always save CTM */
   begin_block( f, TAG_CTM );
   /* WRITE_FLOATS( f, ctx->CTM, 16 ); YO */
   end_block( f );

   /* always save text labels */
   /* YO
save_labels( ctx, f, TAG_LABEL );
*/
   /* always save topography color table */
   begin_block( f, TAG_TOPO_COLORS );
/*   FWRITE( ctx->Topo_CT->Colors, UINT_SIZE, 256, f );  YO */
   end_block( f );

   /* unlock graphics */
   LOCK_OFF( GfxLock );

   fclose(f);
   return 0;
}



/*
 * Allocate a buffer of size 'bytes' then fill it by reading from the
 * given file.
 * Input:  ctx - the context
 *         f - the FILE pointer
 *         bytes - how many bytes to allocate and read
 * Return:  pointer to buffer of data read or NULL if error
 */
static void *alloc_and_read( Context ctx, FILE *f, int bytes )
{
   void *p;

   p = allocate( ctx, bytes );
   if (p) {
      if (fread( p, 1, bytes, f ) != bytes) {
         deallocate( ctx, p, bytes );
         p = NULL;
      }
   }
   return p;
}



static void restore_isosurf( Context ctx, FILE *f, int maxparm,
                             int blocklength )
{
   int iv, it, numverts, numindex;
   float level;

   fread( &iv, INT_SIZE, 1, f );
   if (iv>=maxparm) {
      skip( f, blocklength-INT_SIZE );
      return;
   }
   fread( &it, INT_SIZE, 1, f );
   fread( &level, FLOAT_SIZE, 1, f );
   fread( &numverts, INT_SIZE, 1, f );
   fread( &numindex, INT_SIZE, 1, f );
   if (iv>=ctx->NumVars || it>=ctx->NumTimes) {
#ifdef BIG_GFX
      skip(f, numverts*3*(INT_2_SIZE+INT_1_SIZE)+numindex*UINT_4_SIZE);
#else
      skip(f, numverts*3*(INT_2_SIZE+INT_1_SIZE)+numindex*UINT_2_SIZE);
#endif
      return;
   }
   recent( ctx, ISOSURF, iv );

   wait_read_lock( &ctx->Variable[iv]->SurfTable[it]->lock );

   /* deallocate old surface, if any */
   free_isosurface( ctx, it, iv );

   /* read isosurface */
   ctx->Variable[iv]->SurfTable[it]->verts = alloc_and_read(ctx,f,3*numverts*INT_2_SIZE);
   ctx->Variable[iv]->SurfTable[it]->norms = alloc_and_read(ctx,f,3*numverts*INT_1_SIZE);

#ifdef BIG_GFX
   ctx->Variable[iv]->SurfTable[it]->index = alloc_and_read(ctx,f,numindex*UINT_4_SIZE);
#else
   ctx->Variable[iv]->SurfTable[it]->index = alloc_and_read(ctx,f,numindex*UINT_2_SIZE);
#endif
   ctx->Variable[iv]->SurfTable[it]->isolevel = level;
   ctx->Variable[iv]->SurfTable[it]->numverts = numverts;
   ctx->Variable[iv]->SurfTable[it]->numindex = numindex;
   ctx->Variable[iv]->SurfTable[it]->colorvar = -1;
   ctx->Variable[iv]->SurfTable[it]->colors = NULL;
   ctx->Variable[iv]->SurfTable[it]->valid = 1;
   ctx->IsoLevel[iv] = level;
     
   done_read_lock( &ctx->Variable[iv]->SurfTable[it]->lock );
   return;
}




static void restore_colored_isosurf( Context ctx, FILE *f, int maxparm,
                                     int blocklength )
{
   int iv, it, numverts, numindex;
   float level;

   fread( &iv, INT_SIZE, 1, f );
   if (iv>=maxparm) {
      skip( f, blocklength-INT_SIZE );
      return;
   }
   fread( &it, INT_SIZE, 1, f );
   fread( &level, FLOAT_SIZE, 1, f );
   fread( &numverts, INT_SIZE, 1, f );
   fread( &numindex, INT_SIZE, 1, f );
   if (iv>=ctx->NumVars || it>=ctx->NumTimes) {
#ifdef BIG_GFX
      skip(f, numverts*3*(INT_2_SIZE+INT_1_SIZE)+numindex*UINT_4_SIZE);
#else
      skip(f, numverts*3*(INT_2_SIZE+INT_1_SIZE)+numindex*UINT_2_SIZE);
#endif
      return;
   }
   recent( ctx, ISOSURF, iv );

   wait_read_lock( &ctx->Variable[iv]->SurfTable[it]->lock );

   /* deallocate old surface, if any */
   free_isosurface( ctx, it, iv );

   /* read isosurface */
   ctx->Variable[iv]->SurfTable[it]->verts = alloc_and_read(ctx,f,3*numverts*INT_2_SIZE);
   ctx->Variable[iv]->SurfTable[it]->norms = alloc_and_read(ctx,f,3*numverts*INT_1_SIZE);

#ifdef BIG_GFX
   ctx->Variable[iv]->SurfTable[it]->index = alloc_and_read(ctx,f,numindex*UINT_4_SIZE);
#else
   ctx->Variable[iv]->SurfTable[it]->index = alloc_and_read(ctx,f,numindex*UINT_2_SIZE);
#endif
   fread( &ctx->Variable[iv]->SurfTable[it]->colorvar, INT_SIZE, 1, f );
   if (ctx->Variable[iv]->SurfTable[it]->colorvar>-1) {
      ctx->Variable[iv]->SurfTable[it]->colors = alloc_and_read(ctx,f,numverts*UINT_1_SIZE);
   }
   else {
      ctx->Variable[iv]->SurfTable[it]->colors = NULL;
   }

   ctx->Variable[iv]->SurfTable[it]->isolevel = level;
   ctx->Variable[iv]->SurfTable[it]->numverts = numverts;
   ctx->Variable[iv]->SurfTable[it]->numindex = numindex;
   ctx->Variable[iv]->SurfTable[it]->valid = 1;
   ctx->IsoLevel[iv] = level;
     
   done_read_lock( &ctx->Variable[iv]->SurfTable[it]->lock );
   return;
}




static void restore_hslice( Context ctx, FILE *f, int maxparm,
                            int blocklength )
{
   int iv, it, num1, num2, num3, num4;
   float interval, low, high, level;
   Display_Context dtx;
   
   dtx = ctx->dpy_ctx;
   fread( &iv, INT_SIZE, 1, f );
   if (iv>=maxparm) {
      skip( f, blocklength-INT_SIZE );
      return;
   }
   fread( &it, INT_SIZE, 1, f );
   fread( &interval, FLOAT_SIZE, 1, f );
   fread( &low, FLOAT_SIZE, 1, f );
   fread( &high, FLOAT_SIZE, 1, f );
   fread( &level, FLOAT_SIZE, 1, f );

   recent( ctx, HSLICE, iv );

	if(!ctx->Variable[iv]->HSliceTable[it]){
	  printf("Error HSlice not allocated for var=%d time=%d\n",iv,it);
	  return;
	}

   wait_read_lock( &ctx->Variable[iv]->HSliceTable[it]->lock );

   /* deallocate old slice, if any */
   free_hslice( ctx, it, iv );
            
   fread( &num1, INT_SIZE, 1, f );
   ctx->Variable[iv]->HSliceTable[it]->num1 = num1;
   ctx->Variable[iv]->HSliceTable[it]->verts1 = alloc_and_read(ctx,f,3*num1*INT_2_SIZE);

   fread( &num2, INT_SIZE, 1, f );
   ctx->Variable[iv]->HSliceTable[it]->num2 = num2;
   ctx->Variable[iv]->HSliceTable[it]->verts2 = alloc_and_read(ctx,f,3*num2*INT_2_SIZE);
            
   fread( &num3, INT_SIZE, 1, f );
   ctx->Variable[iv]->HSliceTable[it]->num3 = num3;
   ctx->Variable[iv]->HSliceTable[it]->verts3 = alloc_and_read(ctx,f,3*num3*INT_2_SIZE);
            
   fread( &num4, INT_SIZE, 1, f );
   ctx->Variable[iv]->HSliceTable[it]->numboxverts = num4;
   ctx->Variable[iv]->HSliceTable[it]->boxverts = alloc_and_read(ctx,f,num4*3*FLOAT_SIZE);

   ctx->Variable[iv]->HSliceTable[it]->interval = interval;
   ctx->Variable[iv]->HSliceTable[it]->lowlimit = low;
   ctx->Variable[iv]->HSliceTable[it]->highlimit = high;
   ctx->Variable[iv]->HSliceTable[it]->level = level;
   ctx->Variable[iv]->HSliceTable[it]->valid = 1;
   ctx->Variable[iv]->HSliceRequest->Interval = interval;
   ctx->Variable[iv]->HSliceRequest->LowLimit = low;
   ctx->Variable[iv]->HSliceRequest->HighLimit = high;
   ctx->Variable[iv]->HSliceRequest->Level = level;

/* YO     new_hslice_pos( dtx, level, &ctx->Variable[iv]->HSliceRequest->Z, &ctx->Variable[iv]->HSliceRequest->Hgt );*/

   done_read_lock( &ctx->Variable[iv]->HSliceTable[it]->lock );
   return;
}



static void restore_vslice( Context ctx, FILE *f, int maxparm,
                            int blocklength )
{
   int iv, it, num1, num2, num3, num4;
   float interval, low, high, r1,c1, r2,c2;

   fread( &iv, INT_SIZE, 1, f );
   if (iv>=maxparm) {
      skip( f, blocklength-INT_SIZE );
      return;
   }
   fread( &it, INT_SIZE, 1, f );
   fread( &interval, FLOAT_SIZE, 1, f );
   fread( &low, FLOAT_SIZE, 1, f );
   fread( &high, FLOAT_SIZE, 1, f );
   fread( &r1, FLOAT_SIZE, 1, f );
   fread( &c1, FLOAT_SIZE, 1, f );
   fread( &r2, FLOAT_SIZE, 1, f );
   fread( &c2, FLOAT_SIZE, 1, f );
             
   recent( ctx, VSLICE, iv );
   wait_read_lock( &ctx->Variable[iv]->VSliceTable[it]->lock );

   /* deallocate old slice, if any */
   free_vslice( ctx, it, iv );
             
   fread( &num1, INT_SIZE, 1, f );
   ctx->Variable[iv]->VSliceTable[it]->num1 = num1;
   ctx->Variable[iv]->VSliceTable[it]->verts1 = alloc_and_read(ctx,f,3*num1*INT_2_SIZE);
     
   fread( &num2, INT_SIZE, 1, f );
   ctx->Variable[iv]->VSliceTable[it]->num2 = num2;
   ctx->Variable[iv]->VSliceTable[it]->verts2 = alloc_and_read(ctx,f,3*num2*INT_2_SIZE);
             
   fread( &num3, INT_SIZE, 1, f );
   ctx->Variable[iv]->VSliceTable[it]->num3 = num3;
   ctx->Variable[iv]->VSliceTable[it]->verts3 = alloc_and_read(ctx,f,3*num3*INT_2_SIZE);

   fread( &num4, INT_SIZE, 1, f );
   ctx->Variable[iv]->VSliceTable[it]->numboxverts = num4;
   ctx->Variable[iv]->VSliceTable[it]->boxverts = alloc_and_read(ctx,f,num4*3*FLOAT_SIZE);

   ctx->Variable[iv]->VSliceTable[it]->r1 = r1;
   ctx->Variable[iv]->VSliceTable[it]->c1 = c1;
   ctx->Variable[iv]->VSliceTable[it]->r2 = r2;
   ctx->Variable[iv]->VSliceTable[it]->c2 = c2;
   ctx->Variable[iv]->VSliceTable[it]->interval = interval;
   ctx->Variable[iv]->VSliceTable[it]->lowlimit = low;
   ctx->Variable[iv]->VSliceTable[it]->highlimit = high;
   ctx->Variable[iv]->VSliceTable[it]->valid = 1;
   ctx->Variable[iv]->VSliceRequest->Interval = interval;
   ctx->Variable[iv]->VSliceRequest->LowLimit = low;
   ctx->Variable[iv]->VSliceRequest->HighLimit = high;
   ctx->Variable[iv]->VSliceRequest->R1 = r1;
   ctx->Variable[iv]->VSliceRequest->R2 = c1;
   ctx->Variable[iv]->VSliceRequest->C1 = r2;
   ctx->Variable[iv]->VSliceRequest->C2 = c2;

   new_vslice_pos( ctx, r1, c1, &ctx->Variable[iv]->VSliceRequest->X1, &ctx->Variable[iv]->VSliceRequest->Y1,
                   &ctx->Variable[iv]->VSliceRequest->Lat1, &ctx->Variable[iv]->VSliceRequest->Lon1 );
   new_vslice_pos( ctx, r2, c2, &ctx->Variable[iv]->VSliceRequest->X2, &ctx->Variable[iv]->VSliceRequest->Y2,
                   &ctx->Variable[iv]->VSliceRequest->Lat2, &ctx->Variable[iv]->VSliceRequest->Lon2 );

   done_read_lock( &ctx->Variable[iv]->VSliceTable[it]->lock );
   return;
}



static void restore_chslice( Context ctx, FILE *f, int maxparm,
                             int blocklength )
{
   int iv, it, num;
   Display_Context dtx;

   dtx = ctx->dpy_ctx;

   fread( &iv, INT_SIZE, 1, f );
   if (iv>=maxparm) {
      skip( f, blocklength-INT_SIZE );
      return;
   }
   fread( &it, INT_SIZE, 1, f );
   recent( ctx, CHSLICE, iv );
   wait_read_lock( &ctx->Variable[iv]->CHSliceTable[it]->lock );

   /* deallocate old slice */
   free_chslice( ctx, it, iv );

   fread( &ctx->Variable[iv]->CHSliceTable[it]->level, FLOAT_SIZE, 1, f );
   fread( &ctx->Variable[iv]->CHSliceTable[it]->rows, INT_SIZE, 1, f );
   fread( &ctx->Variable[iv]->CHSliceTable[it]->columns, INT_SIZE, 1, f );

   num = ctx->Variable[iv]->CHSliceTable[it]->rows * ctx->Variable[iv]->CHSliceTable[it]->columns;
   ctx->Variable[iv]->CHSliceTable[it]->verts = alloc_and_read(ctx,f,3*num*INT_2_SIZE);
   ctx->Variable[iv]->CHSliceTable[it]->color_indexes =
                                       alloc_and_read(ctx,f,num*UINT_1_SIZE);

   ctx->Variable[iv]->CHSliceTable[it]->valid = 1;
   ctx->Variable[iv]->CHSliceRequest->Level = ctx->Variable[iv]->CHSliceTable[it]->level;

/* YO     new_hslice_pos( dtx, ctx->Variable[iv]->CHSliceTable[it]->level,*/
/* YO                     &ctx->Variable[iv]->CHSliceRequest->Z, &ctx->Variable[iv]->CHSliceRequest->Hgt);*/

   done_read_lock( &ctx->Variable[iv]->CHSliceTable[it]->lock );
   return;
}





static void restore_cvslice( Context ctx, FILE *f, int maxparm,
                             int blocklength )
{
   int iv, it, num;

   fread( &iv, INT_SIZE, 1, f );
   if (iv>=maxparm) {
      skip( f, blocklength-INT_SIZE );
      return;
   }
   fread( &it, INT_SIZE, 1, f );
   recent( ctx, CVSLICE, iv );
   wait_read_lock( &ctx->Variable[iv]->CVSliceTable[it]->lock );

   /* deallocate old slice */
   free_cvslice( ctx, it, iv );

   fread( &ctx->Variable[iv]->CVSliceTable[it]->r1, FLOAT_SIZE, 1, f );
   fread( &ctx->Variable[iv]->CVSliceTable[it]->c1, FLOAT_SIZE, 1, f );
   fread( &ctx->Variable[iv]->CVSliceTable[it]->r2, FLOAT_SIZE, 1, f );
   fread( &ctx->Variable[iv]->CVSliceTable[it]->c2, FLOAT_SIZE, 1, f );
   fread( &ctx->Variable[iv]->CVSliceTable[it]->rows, INT_SIZE, 1, f );
   fread( &ctx->Variable[iv]->CVSliceTable[it]->columns, INT_SIZE, 1, f );

   num = ctx->Variable[iv]->CVSliceTable[it]->rows * ctx->Variable[iv]->CVSliceTable[it]->columns;
   ctx->Variable[iv]->CVSliceTable[it]->verts = alloc_and_read(ctx,f,3*num*INT_2_SIZE);
   ctx->Variable[iv]->CVSliceTable[it]->color_indexes =
                                       alloc_and_read(ctx,f,num*UINT_1_SIZE);
   ctx->Variable[iv]->CVSliceTable[it]->valid = 1;
   ctx->Variable[iv]->CVSliceRequest->R1 = ctx->Variable[iv]->CVSliceTable[it]->r1;
   ctx->Variable[iv]->CVSliceRequest->R2 = ctx->Variable[iv]->CVSliceTable[it]->c1;
   ctx->Variable[iv]->CVSliceRequest->C1 = ctx->Variable[iv]->CVSliceTable[it]->r2;
   ctx->Variable[iv]->CVSliceRequest->C2 = ctx->Variable[iv]->CVSliceTable[it]->c2;

   new_vslice_pos( ctx, ctx->Variable[iv]->CVSliceTable[it]->r1,
                   ctx->Variable[iv]->CVSliceTable[it]->c1,
                   &ctx->Variable[iv]->CVSliceRequest->X1, &ctx->Variable[iv]->CVSliceRequest->Y1,
                   &ctx->Variable[iv]->CVSliceRequest->Lat1, &ctx->Variable[iv]->CVSliceRequest->Lon1 );
   new_vslice_pos( ctx, ctx->Variable[iv]->CVSliceTable[it]->r2,
                   ctx->Variable[iv]->CVSliceTable[it]->c2,
                   &ctx->Variable[iv]->CVSliceRequest->X2, &ctx->Variable[iv]->CVSliceRequest->Y2,
                   &ctx->Variable[iv]->CVSliceRequest->Lat2, &ctx->Variable[iv]->CVSliceRequest->Lon2 );

   done_read_lock( &ctx->Variable[iv]->CVSliceTable[it]->lock );
   return;
}



static void restore_traj( Context ctx, FILE *f, int blocklength )
{
   int length, kind, group, bytes;
   struct traj *t;

   if (ctx->dpy_ctx->NumTraj<MAXTRAJ) {
      fread( &length, INT_SIZE, 1, f );
      fread( &group, INT_SIZE, 1, f );
      recent( ctx, TRAJ, group );
      fread( &kind, INT_SIZE, 1, f );
      fread( &ctx->NumTimes, INT_SIZE, 1, f );
/*YO*/
      t = ctx->dpy_ctx->TrajTable[ctx->dpy_ctx->NumTraj];
      t->length = length;
      t->group = group;
      t->kind = kind;
      t->verts = alloc_and_read(ctx,f,3*length*INT_2_SIZE);
      if (kind==1) {
         /* read ribbon normals */
         t->norms = alloc_and_read(ctx,f,3*length*UINT_1_SIZE);
      }
      bytes = ctx->NumTimes * UINT_2_SIZE;
      t->start = alloc_and_read(ctx,f,bytes);
      t->len = alloc_and_read(ctx,f,bytes);
      ctx->dpy_ctx->NumTraj++;
   }
   else {
      skip( f, blocklength );
   }
}




static void restore_hwind( Context ctx, FILE *f, int blocklength )
{
   int ws, it;

   fread( &ws, INT_SIZE, 1, f );  /* which slice */
   fread( &it, INT_SIZE, 1, f );  /* which time */
   if (ws<VIS5D_WIND_SLICES && it<ctx->NumTimes) {
      recent( ctx, HWIND, ws );
/*       wait_read_lock( &ctx->HWindTable[ws][it].lock ); 
      free_hwind( ctx, it, ws );

      fread( &ctx->HWindTable[ws][it].level, FLOAT_SIZE, 1, f );
      fread( &ctx->HWindTable[ws][it].density, FLOAT_SIZE, 1, f );
      fread( &ctx->HWindTable[ws][it].scale, FLOAT_SIZE, 1, f );
      ctx->HWindLevel[ws] = ctx->HWindTable[ws][it].level;
      ctx->HWindDensity[ws] = ctx->HWindTable[ws][it].density;
      ctx->HWindScale[ws] = ctx->HWindTable[ws][it].scale;

      fread( &nvect, INT_SIZE, 1, f );
      ctx->HWindTable[ws][it].nvectors = nvect;

      bytes = 3 * 4 * nvect * INT_2_SIZE;
      ctx->HWindTable[ws][it].verts = alloc_and_read( ctx, f, bytes );

      ctx->HWindTable[ws][it].valid = 1;

      fread( &nb, INT_SIZE, 1, f );
      ctx->HWindTable[ws][it].numboxverts = nb;
      ctx->HWindTable[ws][it].boxverts = alloc_and_read(ctx,f,nb*3*FLOAT_SIZE);

      new_hslice_pos( ctx, ctx->HWindLevel[ws],
                      &ctx->HWindZ[ws], &ctx->HWindHgt[ws] ); 

      done_read_lock( &ctx->HWindTable[ws][it].lock ); */
   }
   else {
      skip( f, blocklength - 2*INT_SIZE );
   }
}




#ifdef LEAVEOUT
static void restore_vwind( Context ctx, FILE *f, int blocklength )
{
   int ws, it;

   fread( &ws, INT_SIZE, 1, f );  /* which slice */
   fread( &it, INT_SIZE, 1, f );  /* which time */
   if (ws<VIS5D_WIND_SLICES && it<ctx->NumTimes) {
/* YO
      recent( ctx, VWIND, ws );
      wait_read_lock( &ctx->VWindTable[ws][it].lock );
      free_vwind( ctx, it, ws );

      fread( &ctx->VWindTable[ws][it].r1, FLOAT_SIZE, 1, f );
      fread( &ctx->VWindTable[ws][it].c1, FLOAT_SIZE, 1, f );
      fread( &ctx->VWindTable[ws][it].r2, FLOAT_SIZE, 1, f );
      fread( &ctx->VWindTable[ws][it].c2, FLOAT_SIZE, 1, f );
      fread( &ctx->VWindTable[ws][it].density, FLOAT_SIZE, 1, f );
      fread( &ctx->VWindTable[ws][it].scale, FLOAT_SIZE, 1, f );
      ctx->VWindR1[ws] = ctx->VWindTable[ws][it].r1;
      ctx->VWindC1[ws] = ctx->VWindTable[ws][it].c1;
      ctx->VWindR2[ws] = ctx->VWindTable[ws][it].r2;
      ctx->VWindC2[ws] = ctx->VWindTable[ws][it].c2;
      ctx->VWindDensity[ws] = ctx->VWindTable[ws][it].density;
      ctx->VWindScale[ws] = ctx->VWindTable[ws][it].scale;

      fread( &nvect, INT_SIZE, 1, f );
      ctx->VWindTable[ws][it].nvectors = nvect;
      bytes = 3 * 4 * nvect * INT_2_SIZE;

      ctx->VWindTable[ws][it].verts = alloc_and_read(ctx,f,bytes);

      fread( &nb, INT_SIZE, 1, f );
      ctx->VWindTable[ws][it].numboxverts = nb;
      ctx->VWindTable[ws][it].boxverts = alloc_and_read(ctx,f,3*FLOAT_SIZE*nb);

      new_vslice_pos( ctx, ctx->VWindR1[ws], ctx->VWindC1[ws],
                      &ctx->VWindX1[ws], &ctx->VWindY1[ws],
                      &ctx->VWindLat1[ws], &ctx->VWindLon1[ws] );
      new_vslice_pos( ctx, ctx->VWindR2[ws], ctx->VWindC2[ws],
                      &ctx->VWindX2[ws], &ctx->VWindY2[ws],
                      &ctx->VWindLat2[ws], &ctx->VWindLon2[ws] );

      done_read_lock( &ctx->VWindTable[ws][it].lock );
*/   }
   else {
      skip( f, blocklength - 2*INT_SIZE );
   }
   return;
}




static void restore_stream( Context ctx, FILE *f, int blocklength )
{
   int ws, it;

   fread( &ws, INT_SIZE, 1, f );  /* which slice */
   fread( &it, INT_SIZE, 1, f );  /* which time */
   if (ws<VIS5D_WIND_SLICES && it<ctx->NumTimes) {
/* YO
      recent( ctx, HSTREAM, ws );
      wait_read_lock( &ctx->HStreamTable[ws][it].lock );
      free_hstream( ctx, it, ws );

      fread( &ctx->HStreamTable[ws][it].level, FLOAT_SIZE, 1, f );
      fread( &ctx->HStreamTable[ws][it].density, FLOAT_SIZE, 1, f );
      ctx->HStreamLevel[ws] = ctx->HStreamTable[ws][it].level;
      ctx->HStreamDensity[ws] = ctx->HStreamTable[ws][it].density;

      fread( &nline, INT_SIZE, 1, f );
      ctx->HStreamTable[ws][it].nlines = nline;

      bytes = 3 * nline * INT_2_SIZE;
      ctx->HStreamTable[ws][it].verts = alloc_and_read( ctx, f, bytes );

      ctx->HStreamTable[ws][it].valid = 1;

      fread( &nb, INT_SIZE, 1, f );
      ctx->HStreamTable[ws][it].numboxverts = nb;
      ctx->HStreamTable[ws][it].boxverts = alloc_and_read(ctx,f,nb*3*FLOAT_SIZE);


      done_read_lock( &ctx->HStreamTable[ws][it].lock );
*/   }
   else {
      skip( f, blocklength - 2*INT_SIZE );
   }
}
#endif

static void restore_label( Context ctx, FILE *f )
{
  printf("WARNING restore_label in file save.c doesnt do anything except print this message\n");
/* YO
   int len, x, y;
   char text[MAX_LABEL];
   fread( &len, sizeof(int), 1, f );
   fread( text, 1, len+1, f );
   fread( &x, sizeof(int), 1, f );
   fread( &y, sizeof(int), 1, f );
   vis5d_make_label( ctx->context_index, x, y, text );
*/
}


/*
 * Restore the graphics saved by save() above.
 * Input:  savefile - filename to restore from.
 * Return:  0 for success,
 *          VIS5D_BAD_VALUE if unable to open file
 *          VIS5D_FAIL if error while reading file.
 */
int restore( Context ctx, char *savefile )
{
   FILE *f;
   int tag, blocklength;
   int maxparm, var, add_parms;
   /*int prevtag;*/  /* for debugging */
   int abort = 0;

   /* Determine which variables (and associated graphics) can be restored. */
   /* We can only restore cloned/computed variables if we haven't made any */
   /* such variables yet in the current session. */
   for (var=0;var<ctx->NumVars;var++) {
      if (ctx->Variable[var]->VarType!=VIS5D_REGULAR) {
         break;
      }
   }
   if (var==ctx->NumVars) {
      maxparm = MAXVARS;
      add_parms = 1;
   }
   else {
      maxparm = var;
      add_parms = 0;
   }

   /* Now, we'll only restore variables whose number is < maxparm. */
   /* i.e. only restore variables of type EXT_REGULAR. */


   f = fopen(savefile,"r");
   if (!f)
     return VIS5D_BAD_VALUE;


   while (!feof(f) && !abort) {
      /*prevtag = tag;*/
      if (READ_INTS( f, &tag, 1) == 0)
        break;
      if (READ_INTS( f, &blocklength, 1 ) == 0)
        break;

      switch (tag) {
         case TAG_CLONE_TABLE:
            /* Obsolete */
            printf("Obsolete tag: %d\n", tag );
            skip( f, blocklength );
            break;

         case TAG_VAR_TABLE_41: {
            /* Vis5D v4.1 */
            int np;
            if (add_parms) {
               READ_INTS( f, &np, 1 );

               for (var=0;var<np;var++) {
					  /* JPE: Loop required for backword compatibility */
					  READ_INTS( f, &ctx->Variable[var]->VarType, 1 );
					}
               for (var=0;var<np;var++) {
					  /* JPE: Loop required for backword compatibility */
					  READ_INTS( f, &ctx->Variable[var]->CloneTable, 1 );
					}
               for (var=0;var<np;var++) {
                  READ_FLOATS( f, &ctx->Variable[var]->MinVal, 1 );
                  READ_FLOATS( f, &ctx->Variable[var]->MaxVal, 1 );
                  READ_BYTES( f, ctx->Variable[var]->VarName, 10 );
                  READ_INTS( f, &ctx->Nl[var], 1 );
                  ctx->Variable[var]->LowLev = 0;
               }
               ctx->NumVars = np;
            }
            else {
               skip( f, blocklength );
            }
         } break;

         case TAG_VAR_TABLE: {
            int np;
            if (add_parms) {
               READ_INTS( f, &np, 1 );
               for (var=0;var<np;var++) {
					  /* JPE: Loop required for backword compatibility */
					  READ_INTS( f, &ctx->Variable[var]->VarType, 1 );
					}
               for (var=0;var<np;var++) {
					  /* JPE: Loop required for backword compatibility */
					  READ_INTS( f, &ctx->Variable[var]->CloneTable, 1 );
					}

               for (var=0;var<np;var++) {
                  READ_FLOATS( f, &ctx->Variable[var]->MinVal, 1 );
                  READ_FLOATS( f, &ctx->Variable[var]->MaxVal, 1 );
                  READ_BYTES( f, ctx->Variable[var]->VarName, 10 );
                  READ_INTS( f, &ctx->Nl[var], 1 );
                  READ_INTS( f, &ctx->Variable[var]->LowLev, 1 );
               }
               ctx->NumVars = np;
            }
            else {
               skip( f, blocklength );
            }
         } break;

/*NEW*/
         case TAG_EXT_FUNC: {
            int var, ntimes, it;
            float *grid;
            if (add_parms) {
               READ_INTS( f, &var, 1 );
               READ_INTS( f, &ntimes, 1 );
               grid = allocate( ctx,ctx->Nr*ctx->Nc*ctx->Nl[var]*FLOAT_SIZE );
               for (it=0;it<ntimes;it++) {
                  READ_FLOATS( f, grid, ctx->Nr*ctx->Nc*ctx->Nl[var] );
                  install_new_grid( ctx, it, var, grid, ctx->Nl[var],
                                    ctx->Variable[var]->LowLev );
               }
               deallocate(ctx,grid,ctx->Nr*ctx->Nc*ctx->Nl[var]*FLOAT_SIZE );
            }
            else {
               skip( f, blocklength );
            }
         } break;

/*NEW*/
         case TAG_EXPRESSION: {
            int var, ntimes, it;
            float *grid;
            if (add_parms) {
               READ_INTS( f, &var, 1 );

					/* needs to be 500 for backward compatibility */
					ctx->Variable[var]->ExpressionList = (char *) malloc(500);
               READ_BYTES( f, ctx->Variable[var]->ExpressionList, 500 );

               READ_INTS( f, &ntimes, 1 );
               grid = allocate( ctx, ctx->Nr*ctx->Nc*ctx->Nl[var]*FLOAT_SIZE );
               for (it=0;it<ntimes;it++) {
                  READ_FLOATS( f, grid, ctx->Nr*ctx->Nc*ctx->Nl[var] );
                  install_new_grid( ctx, it, var, grid, ctx->Nl[var],
                                    ctx->Variable[var]->LowLev);
               }
               deallocate( ctx, grid,
                           ctx->Nr*ctx->Nc*ctx->Nl[var]*FLOAT_SIZE );
            }
            else {
               skip( f, blocklength );
            }
         } break;



         case TAG_ISOSURF:
            restore_isosurf( ctx, f, maxparm, blocklength );
            break;

         case TAG_COLORED_ISOSURF:
            restore_colored_isosurf( ctx, f, maxparm, blocklength );
            break;

         case TAG_ISO_COLOR: {
            int var;
            fread( &var, INT_SIZE, 1, f );
            if (var>=maxparm)
               skip( f, blocklength-INT_SIZE );
            else
               fread( &ctx->dpy_ctx->Color[var][ISOSURF], UINT_SIZE, 1, f );
         } break;

         case TAG_ISO_COLOR_TABLE: {
/*YO            int var;
            fread( &var, INT_SIZE, 1, f );
            if (var>=maxparm)
               skip( f, blocklength-INT_SIZE );
            else

               fread( &ctx->ColorTable[VIS5D_ISOSURF_CT]->Colors[var], UINT_SIZE, 256, f );
*/
         } break;

         case TAG_ISO_LEVEL: {
            int var;
            float x;
            READ_INTS( f, &var, 1 );
            READ_FLOATS( f, &x, 1 );
            if (var<maxparm) {
               ctx->IsoLevel[var] = x;
            }
         } break;

         case TAG_HSLICE:
            restore_hslice( ctx, f, maxparm, blocklength );
            break;

         case TAG_HSLICE_COLOR: {
            int var;
            unsigned int color;
            fread( &var, INT_SIZE, 1, f );
            fread( &color, UINT_SIZE, 1, f );
            if (var<maxparm) {
/*YO
               ctx->Color[var][HSLICE] = color;
*/
            }
         } break;

         case TAG_HSLICE_POS: {
            int var;
            float interval, low, high, level;
            READ_INTS( f, &var, 1 );
            READ_FLOATS( f, &interval, 1 );
            READ_FLOATS( f, &low, 1 );
            READ_FLOATS( f, &high, 1 );
            READ_FLOATS( f, &level, 1 );
            if (var<maxparm) {
               ctx->Variable[var]->HSliceRequest->Interval = interval;
               ctx->Variable[var]->HSliceRequest->LowLimit = low;
               ctx->Variable[var]->HSliceRequest->HighLimit = high;
               ctx->Variable[var]->HSliceRequest->Level = level;
/* YO                 new_hslice_pos( ctx, level, &ctx->Variable[var]->HSliceRequest->Z,*/
/* YO                                 &ctx->Variable[var]->HSliceRequest->Hgt );*/
            }
         } break;

         case TAG_VSLICE:
            restore_vslice( ctx, f, maxparm, blocklength );
            break;

         case TAG_VSLICE_COLOR: {
            int var;
            unsigned int color;
            fread( &var, INT_SIZE, 1, f );
            fread( &color, UINT_SIZE, 1, f );
            if (var<maxparm) {
/*YO
               ctx->Color[var][VSLICE] = color;
*/
            }
         } break;

         case TAG_VSLICE_POS: {
            int var;
            float interval, low, high, r1,c1, r2,c2;
            READ_INTS( f, &var, 1 );
            READ_FLOATS( f, &interval, 1 );
            READ_FLOATS( f, &low, 1 );
            READ_FLOATS( f, &high, 1 );
            READ_FLOATS( f, &r1, 1 );
            READ_FLOATS( f, &c1, 1 );
            READ_FLOATS( f, &r2, 1 );
            READ_FLOATS( f, &c2, 1 );
            if (var<maxparm) {
               ctx->Variable[var]->VSliceRequest->Interval = interval;
               ctx->Variable[var]->VSliceRequest->LowLimit = low;
               ctx->Variable[var]->VSliceRequest->HighLimit = high;
               ctx->Variable[var]->VSliceRequest->R1 = r1;
               ctx->Variable[var]->VSliceRequest->R2 = c1;
               new_vslice_pos( ctx, r1, c1,
                               &ctx->Variable[var]->VSliceRequest->X1, &ctx->Variable[var]->VSliceRequest->Y1,
                               &ctx->Variable[var]->VSliceRequest->Lat1, &ctx->Variable[var]->VSliceRequest->Lon1 );
               ctx->Variable[var]->VSliceRequest->C1 = r2;
               ctx->Variable[var]->VSliceRequest->C2 = c2;
               new_vslice_pos( ctx, r2, c2,
                               &ctx->Variable[var]->VSliceRequest->X2, &ctx->Variable[var]->VSliceRequest->Y2,
                               &ctx->Variable[var]->VSliceRequest->Lat2, &ctx->Variable[var]->VSliceRequest->Lon2 );
            }
         } break;

         case TAG_CHSLICE:
            restore_chslice( ctx, f, maxparm, blocklength );
            break;

         case TAG_CHSLICE_COLORS: {
            int var, size;
            fread( &var, INT_SIZE, 1, f );
            if (var>=maxparm) {
               skip( f, blocklength-INT_SIZE );
            }
            else {
/*YO
               fread( &ctx->Color[var][CHSLICE], UINT_SIZE, 1, f );
*/
               fread( &size, INT_SIZE, 1, f );
/*YO
               fread( ctx->ColorTable[VIS5D_CHSLICE_CT]->Colors[var], UINT_SIZE, size, f );
*/
            }
         } break;

         case TAG_CHSLICE_POS: {
            int var;
            float level;
            READ_INTS( f, &var, 1 );
            READ_FLOATS( f, &level, 1 );
            if (var<maxparm) {
               ctx->Variable[var]->CHSliceRequest->Level = level;
/* YO                 new_hslice_pos( ctx, level, &ctx->Variable[var]->CHSliceRequest->Z,*/
/* YO                                 &ctx->Variable[var]->CHSliceRequest->Hgt );*/
            }
         } break;

         case TAG_CVSLICE:
            restore_cvslice( ctx, f, maxparm, blocklength );
            break;

         case TAG_CVSLICE_COLORS: {
            int var, size;
            fread( &var, INT_SIZE, 1, f );
            if (var>=maxparm) {
               skip( f, blocklength-INT_SIZE );
            }
            else {
/*YO
               fread( &ctx->Color[var][CVSLICE], UINT_SIZE, 1, f );
*/
               fread( &size, INT_SIZE, 1, f );
/*YO
               fread( ctx->ColorTable[VIS5D_CVSLICE_CT]->Colors[var], UINT_SIZE, size, f );
*/
            }
         } break;

         case TAG_CVSLICE_POS: {
            int var;
            float r1,c1, r2,c2;
            READ_INTS( f, &var, 1 );
            READ_FLOATS( f, &r1, 1 );
            READ_FLOATS( f, &c1, 1 );
            READ_FLOATS( f, &r2, 1 );
            READ_FLOATS( f, &c2, 1 );
            if (var<maxparm) {
               ctx->Variable[var]->CVSliceRequest->R1 = r1;
               ctx->Variable[var]->CVSliceRequest->R2 = c1;
               new_vslice_pos( ctx, r1, c1,
                               &ctx->Variable[var]->CVSliceRequest->X1, &ctx->Variable[var]->CVSliceRequest->Y1,
                               &ctx->Variable[var]->CVSliceRequest->Lat1, &ctx->Variable[var]->CVSliceRequest->Lon1 );
               ctx->Variable[var]->CVSliceRequest->C1 = r2;
               ctx->Variable[var]->CVSliceRequest->C2 = c2;
               new_vslice_pos( ctx,r2, c2,
                               &ctx->Variable[var]->CVSliceRequest->X2, &ctx->Variable[var]->CVSliceRequest->Y2,
                               &ctx->Variable[var]->CVSliceRequest->Lat2, &ctx->Variable[var]->CVSliceRequest->Lon2 );
            }
         } break;

         case TAG_TRAJ:
            restore_traj( ctx, f, blocklength );
            break;
         
         case TAG_TRAJ_COLOR: {
            /* restore traj colors */
            int grp;
            fread( &grp, INT_SIZE, 1, f );
            if (grp<VIS5D_TRAJ_SETS) {
/*YO
               fread( &ctx->TrajColor[grp], UINT_SIZE, 1, f );
*/
            }
            else {
               skip( f, UINT_SIZE );
            }
         } break;
         
         case TAG_CTM:
            /* restore ctm */
/*YO
            fread( ctx->CTM, FLOAT_SIZE, 16, f );
*/
            break;
         
         case TAG_LABEL:
            restore_label( ctx, f );
            break;

         case TAG_HWIND:
            restore_hwind( ctx, f, blocklength );
            break;

         case TAG_HWIND_COLOR: {
            int ws;
            fread( &ws, INT_SIZE, 1, f );
            if (ws<VIS5D_WIND_SLICES) {
/*YO
               fread( &ctx->HWindColor[ws], UINT_SIZE, 1, f );
*/
            }
            else {
               skip( f, UINT_SIZE);
            }
         } break;

         case TAG_HWIND_POS: {
            int ws;
            READ_INTS( f, &ws, 1 );
/*YO
            READ_FLOATS( f, &ctx->HWindLevel[ws], 1 );
*/
/*YO
            READ_FLOATS( f, &ctx->HWindDensity[ws], 1 );
*/
/*YO
            READ_FLOATS( f, &ctx->HWindScale[ws], 1 );
*/
/*YO
              new_hslice_pos( ctx, ctx->HWindLevel[ws],
                              &ctx->HWindZ[ws], &ctx->HWindHgt[ws] );
*/
         } break;

         case TAG_VWIND:
/*YO
            restore_vwind( ctx, f, blocklength );
*/
            break;

         case TAG_VWIND_COLOR: {
            int ws;
            fread( &ws, INT_SIZE, 1, f );
            if (ws<VIS5D_WIND_SLICES) {
/*YO
               fread( &ctx->VWindColor[ws], UINT_SIZE, 1, f );
*/
            }
            else {
               skip( f, UINT_SIZE);
            }
         } break;

         case TAG_VWIND_POS: {

/*YO             int ws;
            READ_INTS( f, &ws, 1 );
            READ_FLOATS( f, &ctx->VWindR1[ws], 1 );
            READ_FLOATS( f, &ctx->VWindC1[ws], 1 );
            new_vslice_pos( ctx, ctx->VWindR1[ws], ctx->VWindC1[ws],
                            &ctx->VWindX1[ws], &ctx->VWindY1[ws],
                            &ctx->VWindLat1[ws], &ctx->VWindLon1[ws] );
            READ_FLOATS( f, &ctx->VWindR2[ws], 1 );
            READ_FLOATS( f, &ctx->VWindC2[ws], 1 );
            new_vslice_pos( ctx, ctx->VWindR2[ws], ctx->VWindC2[ws],
                            &ctx->VWindX2[ws], &ctx->VWindY2[ws],
                            &ctx->VWindLat2[ws], &ctx->VWindLon2[ws] );
            READ_FLOATS( f, &ctx->VWindDensity[ws], 1 );
            READ_FLOATS( f, &ctx->VWindScale[ws], 1 );
 */
         } break;

         case TAG_HSTREAM:
/*YO
            restore_stream( ctx, f, blocklength );
*/
            break;

         case TAG_HSTREAM_COLOR: {
            int ws;
            fread( &ws, INT_SIZE, 1, f );
            if (ws<VIS5D_WIND_SLICES) {
/*YO
               fread( &ctx->HStreamColor[ws], UINT_SIZE, 1, f );
*/
            }
            else {
               skip( f, UINT_SIZE);
            }
         } break;

         case TAG_HSTREAM_POS: {

/*YO            int ws;
            READ_INTS( f, &ws, 1 );
            READ_FLOATS( f, &ctx->HStreamLevel[ws], 1 );
            READ_FLOATS( f, &ctx->HStreamDensity[ws], 1 );
 YO              new_hslice_pos( ctx, ctx->HStreamLevel[ws],
 YO                              &ctx->HStreamZ[ws], &ctx->HStreamHgt[ws] );
*/         } break;

         case TAG_TOPO_COLORS:
/*YO
            fread( ctx->Topo_CT->Colors, UINT_SIZE, 256, f );
*/
            break;
        
         case TAG_VOLUME_COLORS: {
            int var, size;
            fread( &var, INT_SIZE, 1, f );
            if (var>=maxparm) {
               skip( f, blocklength-INT_SIZE );
            }
            else {
               fread( &size, INT_SIZE, 1, f );
/*YO
               fread( ctx->ColorTable[VIS5D_VOLUME_CT]->Colors[var], UINT_SIZE, size, f );
*/
            }
         } break;

         default:
            /* skip bytes */
            if (tag<=0 || blocklength<=0) {
               printf("Bad .SAVE file, restore aborted\n");
               abort = 1;
            }
            else {
               printf("skipping block (tag=%d,len=%d)\n", tag, blocklength );
               skip( f, blocklength );
            }

      } /*switch */

   } /*while*/

   fclose(f);
   if (abort) {
      return VIS5D_FAIL;
   }
   else {
      return 0;
   }
}

