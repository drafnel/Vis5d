/* outlgrid.c */
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



/*
 * Create the file OUTLGRID which is a map outline file defining
 * lines of constant latitude and longitude for the entire earth.
 */




#include <stdio.h>

#include <binio.h>


struct polyline {
        int        minlat;
        int        maxlat;
        int        minlon;
        int        maxlon;
        int        start;
        int        len;
};


#define MAXLINES 100000
struct polyline MapLine[MAXLINES];
int NumLines = 0;



struct vertex {
        int        lat, lon;
};


#define MAXVERTS 100000
struct vertex VertexList[MAXVERTS];
int NumVertices = 0;


extern int F77_FUNC(transform,TRANSFORM)();

main( argc, argv )
int argc;
char *argv[];
{
   int flag;

   make_map();

   write_mapfile( "OUTLGRID" );

   exit(0);
}


/* These values should be factors of 180.0: */
#define DELTA_LAT 10.0
#define DELTA_LON 10.0

/* How many line segments between DELTA degrees: */
#define SEGMENTS 5.0


make_map()
{
   float lat, lon;
   int len, i;

   NumLines = 0;
   NumVertices = 0;

   /* lines of constant latitude */
   for (lat=-90.0+DELTA_LAT;lat<90.0;lat+=DELTA_LAT) {
      MapLine[NumLines].minlat = (int) (lat * 10000.0);
      MapLine[NumLines].maxlat = (int) (lat * 10000.0);
      MapLine[NumLines].minlon = (int) (-180.0 * 10000.0);
      MapLine[NumLines].maxlon = (int) (180.0 * 10000.0);
      MapLine[NumLines].start = NumVertices;
      len = 0;
      for (lon=-180.0;lon<=180.0;lon+=DELTA_LON/SEGMENTS) {
         VertexList[NumVertices].lat = (int) (lat * 10000.0);
         VertexList[NumVertices].lon = (int) (lon * 10000.0);
         NumVertices++;
         len++;
      }
      MapLine[NumLines].len = len;
      NumLines++;
   }

   /* lines of constant longitude */
   for (lon=-180.0;lon<180.0;lon+=DELTA_LON) {
      MapLine[NumLines].minlat = (int) (-90.0 * 10000.0);
      MapLine[NumLines].maxlat = (int) (90.0 * 10000.0);
      MapLine[NumLines].minlon = (int) (lon * 10000.0);
      MapLine[NumLines].maxlon = (int) (lon * 10000.0);
      MapLine[NumLines].start = NumVertices;
      len = 0;
      for (lat=-90.0;lat<=90.0;lat+=DELTA_LAT/SEGMENTS) {
         VertexList[NumVertices].lat = (int) (lat * 10000.0);
         VertexList[NumVertices].lon = (int) (lon * 10000.0);
         NumVertices++;
         len++;
      }
      MapLine[NumLines].len = len;
      NumLines++;
   }

   printf("NumLines=%d\n", NumLines);
   printf("NumVertices=%d\n", NumVertices);

   /* recompute Mapline.start and .len as file positions */
   for (i=0;i<NumLines;i++) {
      MapLine[i].start = NumLines*6 + MapLine[i].start*2 + 1;
      MapLine[i].len *= 2;
   }
}





write_mapfile( filename )
char *filename;
{
   int f;

   f = open( filename, O_CREAT | O_WRONLY );
   if (f<0) {
      printf("Error:  unable to open %s for writing\n", filename );
      exit(0);
   }

   write_int4( f, NumLines );

   if (write_int4_array( f, MapLine, 6*NumLines ) < 6*NumLines) {
      printf("Error:  bad file write (disk full?)\n");
      exit(0);
   }

   if (write_int4_array( f, VertexList, 2*NumVertices ) < 2*NumVertices) {
      printf("Error:  bad file write (disk full?)\n");
      exit(0);
   }

   close( f );
}

