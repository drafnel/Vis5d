/* v5dstats.c */
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
 * Print statistics about data in a .v5d file.
 */


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <v5d.h>



int main( int argc, char *argv[] )
{
   v5dstruct v;
   int time, var;

   if (argc!=2) {
      printf("Usage:\n");
      printf("   v5dstats file\n");
      exit(0);
   }

   if (!v5dOpenFile( argv[1], &v )) {
      printf("Error: couldn't open %s for reading\n", argv[1] );
      exit(0);
   }

   printf("Time  Variable Units      MinValue     MaxValue    MeanValue      Std Dev #Miss\n");
   printf("-------------------------------------------------------------------------------\n");
   
   for (time=0; time<v.NumTimes; time++) {

      for (var=0; var<v.NumVars; var++) {
         int i, nrncnl;
         float *data;
         float min, max, sum, sumsum;
         int missing, good;

         nrncnl = v.Nr * v.Nc * v.Nl[var];

         data = (float *) malloc( nrncnl * sizeof(float) );
         if (!v5dReadGrid( &v, time, var, data )) {
            printf("Error while reading grid (time=%d,var=%s)\n",
                   time+1, v.VarName[var] );
            exit(0);
         }

         min = MISSING;
         max = -MISSING;
         missing = 0;
         good = 0;
         sum = 0.0;
         sumsum = 0.0;

         for (i=0;i<nrncnl;i++) {
/*
            if (data[i]!=data[i]) {
               printf("bad: %g\n", data[i]);
            }
*/
            if ( IS_MISSING(data[i]) ) {
               missing++;
            }
            else {
               good++;
               if (data[i]<min) {
                  min = data[i];
               }
               if (data[i]>max) {
                  max = data[i];
               }
               sum += data[i];
               sumsum += data[i]*data[i];
            }
         }

         free( data );

         if (good==0) {
            /* all missing */
            printf("%4d  %-8s %-5s  all missing values\n",
                   time+1, v.VarName[var], v.Units[var] );
         }
         else {
            float mean = sum / good;
            float tmp = (sumsum - sum*sum/good) / (good-1);
            float sd;
            if (tmp<0.0) {
               sd = 0.0;
            }
            else {
               sd = sqrt( tmp );
            }
            printf("%4d  %-8s %-5s %13g%13g%13g%13g  %4d\n",
                   time+1, v.VarName[var], v.Units[var],
                   min,  max,  mean, sd,  missing );
         }

      }
      printf("\n");
   }

   v5dCloseFile( &v );
   return 0;
}
