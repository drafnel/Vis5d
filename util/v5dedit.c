/* v5dedit.c */
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
 * Program for editing .v5d file headers
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <v5d.h>



static void edit_varnames( v5dstruct *v )
{
   int var, n;
   char input[1000], newname[1000];

   while (1) {
      printf("\n");
      printf("Variable names\n");
      for (var=0;var<v->NumVars;var++) {
         printf("  %2d:  '%s'\n", var+1, v->VarName[var] );
      }
      printf("Enter number to change or <q> to quit: ");
      fgets(input,1000,stdin);
      if (input[0]=='q') {
         return;
      }
      n = atoi( input );
      if (n>=1 && n<=v->NumVars) {
         printf("Enter new name for %s: ", v->VarName[n-1] );
	 fgets(newname,1000,stdin);
         if (newname[0]) {
            strncpy( v->VarName[n-1], newname, 9 );
            v->VarName[n-1][9] = 0;
         }
      }
   }
}



static void edit_units( v5dstruct *v )
{
   int var, n;
   char input[1000], newunits[1000];

   while (1) {
      printf("\n");
      printf("       Variable    Units\n");
      for (var=0;var<v->NumVars;var++) {
         printf("  %2d:  %-10s  %s\n",
                var+1, v->VarName[var], v->Units[var] );
      }
      printf("Enter number to change or <q> to quit: ");
      fgets(input,1000,stdin);
      if (input[0]=='q') {
         return;
      }
      n = atoi( input );
      if (n>=1 && n<=v->NumVars) {
         printf("Enter new units for %s: ", v->VarName[n-1] );
	 fgets(newunits,1000,stdin);
         strncpy( v->Units[n-1], newunits, 19 );
         v->Units[n-1][19] = 0;
      }
   }
}



static void edit_lowlevs( v5dstruct *v )
{
   int i, var, n, l, maxnl, oldmaxnl;
   char input[1000], newlev[1000];

   oldmaxnl = 0;
   for (i=0;i<v->NumVars;i++) {
      if (v->Nl[i]>oldmaxnl) {
        oldmaxnl = v->Nl[i];
      }
   }

   while (1) {
      printf("\n");
      printf("Low levels\n");
      for (var=0;var<v->NumVars;var++) {
         printf("  %2d (%s): %4d\n", var+1, v->VarName[var], v->LowLev[var] );
      }
      printf("Enter number to change or <q> to quit: ");
      fgets(input,1000,stdin);
      if (input[0]=='q') {
        /* check for need to extend vertical coordinate system */
        maxnl = 0;
        for (i=0;i<v->NumVars;i++) {
          if (v->Nl[i]>maxnl) {
            maxnl = v->Nl[i];
          }
        }
        if (maxnl > oldmaxnl) {
          if (v->VerticalSystem == 2 || v->VerticalSystem == 3) {
            for (i=oldmaxnl; i<maxnl; i++) {
              v->VertArgs[i] = v->VertArgs[oldmaxnl-1] +
                 (i-oldmaxnl+1) * (v->VertArgs[oldmaxnl-1] - v->VertArgs[oldmaxnl-2]);
            }
          }
          printf("Must edit vertical coordinate system\n");
          printf("  - number of vertical levels increased.\n");
        }
        return;
      }
      n = atoi( input );
      if (n>=1 && n<=v->NumVars) {
         printf("Enter new low level for %s: ", v->VarName[n-1] );
         fgets(newlev,1000,stdin);
         l = atoi(newlev);
         if (l >= 0 && l <= MAXLEVELS - v->Nl[n-1]) v->LowLev[n-1] = l;
      }
   }
}



static void edit_times( v5dstruct *v )
{
   int time, n;
   char input[1000];

   while (1) {
      printf("\n");
      printf("Times and dates\n");
      for (time=0;time<v->NumTimes;time++) {
         printf("  %3d:  %06d  %05d\n",
                time+1,
                v->TimeStamp[time],
                v->DateStamp[time] );
      }
      printf("Enter number to change or <q> to quit: ");
      fgets(input,1000,stdin);
      if (input[0]=='q') {
         return;
      }
      n = atoi( input );
      if (n>=1 && n<=v->NumTimes) {
         printf("Enter new time: ");
         fgets(input,1000,stdin);
         if (input[0]) {
            v->TimeStamp[n-1] = atoi( input );
         }
         printf("Enter new date: ");
         fgets(input,1000,stdin);
         if (input[0]) {
            v->DateStamp[n-1] = atoi( input );
         }
      }
   }
}



static void edit_projection( v5dstruct *v )
{
   char input[1000];
   int i,n;

   while (1) {
      printf("\n");
      printf("Current projection:\n");
      switch (v->Projection) {
         case 0:
            printf("  1. Generic linear projection:\n");
            printf("  2. NorthBound = %g\n", v->ProjArgs[0] );
            printf("  3. WestBound = %g\n", v->ProjArgs[1] );
            printf("  4. RowInc = %g\n", v->ProjArgs[2] );
            printf("  5. ColInc = %g\n", v->ProjArgs[3] );
            break;
         case 1:
            printf("  1. Cylindrical Equidistant projection:\n");
            printf("  2. NorthBound = %g degrees\n", v->ProjArgs[0] );
            printf("  3. WestBound = %g degrees\n", v->ProjArgs[1] );
            printf("  4. RowInc = %g degrees\n", v->ProjArgs[2] );
            printf("  5. ColInc = %g degrees\n", v->ProjArgs[3] );
            break;
         case 2:
            printf("  1. Lambert Conformal\n");
            printf("  2. First standard latitude = %g degrees\n", v->ProjArgs[0] );
            printf("  3. Second standard latitude = %g degrees\n", v->ProjArgs[1] );
            printf("  4. Row of north/south pole = %g\n", v->ProjArgs[2] );
            printf("  5. Column of north/south pole = %g\n", v->ProjArgs[3] );
            printf("  6. Central Longitude = %g\n", v->ProjArgs[4] );
            printf("  7. ColInc = %g km\n", v->ProjArgs[5] );
            break;
         case 3:
            printf("  1. Polar Stereographic projection\n");
            printf("  2. Central Latitude = %g\n", v->ProjArgs[0] );
            printf("  3. Central Longitude = %g\n", v->ProjArgs[1] );
            printf("  4. Central Row = %g\n", v->ProjArgs[2] );
            printf("  5. Central Col = %g\n", v->ProjArgs[3] );
            printf("  6. ColInc = %g km\n", v->ProjArgs[4] );
            break;
         case 4:
            printf("  1. Rotated projection:\n");
            printf("  2. NorthBound = %g degrees\n", v->ProjArgs[0] );
            printf("  3. WestBound = %g degrees\n", v->ProjArgs[1] );
            printf("  4. RowInc = %g degrees\n", v->ProjArgs[2] );
            printf("  5. ColInc = %g degrees\n", v->ProjArgs[3] );
            printf("  6. Central Latitude = %g\n", v->ProjArgs[4] );
            printf("  7. Central Longitude = %g\n", v->ProjArgs[5] );
            printf("  8. Rotation = %g\n", v->ProjArgs[6] );
            break;
         /* ZLB 02-09-2000 */
         case -1:
            printf("  1. Generic nonequally spaced projection:\n");
            for (i=0;i<v->Nc;i++) {
               printf("%3d. column %3d:  %.4f\n", i+2, i+1, v->ProjArgs[i+v->Nr] );
            }
            for (i=0;i<v->Nr;i++) {
               printf("%3d. row %3d:  %.4f\n", i+2+v->Nr, i+1, v->ProjArgs[i] );
            }
            break;
         default:
            printf("  Bad projection number: %d\n", v->Projection );
            return;
      }
      printf("Enter number to change or <q> to quit: ");
      fgets(input,1000,stdin);
      if (input[0]=='q') {
         return;
      }
      n = atoi( input );
      if (n>0) {
         if (n==1) {
            printf("Projections\n");
            printf("  0. Generic nonequally spaced\n");
            printf("  1. Generic linear\n");
            printf("  2. Cylindrical equidistant\n");
            printf("  3. Lambert Conformal\n");
            printf("  4. Polar Sterographic\n");
            printf("  5. Rotated\n");
            printf("Enter selection or <q> to quit: ");
            fgets(input,1000,stdin);
            if (input[0]=='q') {
               return;
            }
            n = atoi( input );
            if (n>=1 && n<=5) {
               v->Projection = n-1;
            } else if (n==0) v->Projection = -1;
         }
         else {
            printf("New value: ");
            fgets(input,1000,stdin);
            v->ProjArgs[n-2] = atof( input );
         }
      }
   }

}



static void edit_vertsys( v5dstruct *v )
{
   char input[1000];
   int i, n, maxnl;
   float height;

   maxnl = 0;
   for (i=0;i<v->NumVars;i++) {
      if (v->Nl[i]>maxnl) {
         maxnl = v->Nl[i];
      }
   }

   while (1) {
      printf("\n");
      printf("Current vertical coordinate system:\n");
      switch (v->VerticalSystem) {
         case 0:
            printf("  0. Generic linear, equally spaced\n");
            printf("  1. Bottom Bound = %g\n", v->VertArgs[0] );
            printf("  2. Level Increment = %g\n", v->VertArgs[1] );
            break;
         case 1:
            printf("  0. Linear, equally spaced km\n");
            printf("  1. Bottom Bound = %g km\n", v->VertArgs[0] );
            printf("  2. Level Increment = %g km\n", v->VertArgs[1] );
            break;
         case 2:
            printf("  0. Linear, unequally spaced km\n");
            for (i=0;i<maxnl;i++) {
               printf("%3d. %.4f\n", i+1, v->VertArgs[i] );
            }
            break;
         case 3:
            printf("  0. Pressure, unequally spaced mb\n");
            for (i=0;i<maxnl;i++) {
               printf("%3d. %.4f\n", i+1, height_to_pressure(v->VertArgs[i]) );
            }
            break;
         default:
            printf("  Error, undefined vert. coord. system: %d\n",
                   v->VerticalSystem );
            return;
      }

      printf("Enter number of item to change or <q> to quit: ");
      fgets(input,1000,stdin);
      if (input[0]=='q') {
         return;
      }
      n = atoi(input);
      if (n==0) {
         printf("Vertical coordinate systems:\n");
         printf("  1. Generic linear, equally spaced\n");
         printf("  2. Linear, equally spaced km\n");
         printf("  3. Linear, unequally spaced km\n");
         printf("  4. Pressure, unequally spaced mb\n");
         printf("Enter new vertical coord. system: ");
         fgets(input,1000,stdin);
         n = atoi(input);
         if (n<1 || n>4) {
            printf("Only 1, 2, 3, or 4 accepted.\n");
         }
         else {
            if ((v->VerticalSystem==0 || v->VerticalSystem==1) && (n==3 || n==4)) {
               /* setup initial height values */
               float bottom = v->VertArgs[0];
               float hgtinc = v->VertArgs[1];
               for (i=0;i<maxnl;i++) {
                  v->VertArgs[i] = bottom+ i * hgtinc;
               }
            }
            v->VerticalSystem = n-1;
         }
      }
      else {
         printf("New value: ");
         fgets(input,1000,stdin);
         height = atof(input);
         if (v->VerticalSystem == 3) v->VertArgs[n-1] = pressure_to_height(height);
         else v->VertArgs[n-1] = height;
      }
   }

}




static void edit( v5dstruct *v )
{
   char input[1000];

   while (1) {
      printf("\n");
      printf("Main Menu\n");
      printf("  1. Variable names\n");
      printf("  2. Variable's units\n");
      printf("  3. Times and dates\n");
      printf("  4. Projection\n");
      printf("  5. Vertical coordinate system\n");
      printf("  6. Low levels\n");
      printf("Enter number to change or <q> to quit: ");
      fgets(input,1000,stdin);
      switch (input[0]) {
         case '1':
            edit_varnames(v);
            break;
         case '2':
            edit_units(v);
            break;
         case '3':
            edit_times(v);
            break;
         case '4':
            edit_projection(v);
            break;
         case '5':
            edit_vertsys(v);
            break;
         case '6':
            edit_lowlevs(v);
            break;
         case 'q':
            return;
         default:
            printf("Try again.\n\n");
      }
   }
}



main( argc, argv )
int argc;
char *argv[];
{
   v5dstruct v;
   char input[1000];

   if (argc==1) {
      printf("Usage\n");
      printf("   v5dedit filename.v5d\n");
      exit(1);
   }

   if (!v5dUpdateFile( argv[1], &v )) {
      printf("Error:  couldn't open %s for editing\n", argv[1] );
      exit(1);
   }

   edit( &v );

   printf("Save changes made (y/n)?" );
   fgets(input,1000,stdin);
   if (input[0]=='y') {
      v5dCloseFile( &v );
   }

   exit(0);
}
