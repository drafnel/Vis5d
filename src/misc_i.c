/* misc.c */

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


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "v5d.h"

/* replacement for the common, but non-POSIX, strdup function if it is
 * not available: */
#ifndef HAVE_STRDUP
char *strdup(const char *s)
{
   int len = strlen(s) + 1;
   char *s2 = (char *) malloc( len );
   if (s2) {
      memcpy( s2, s, len );
   }
   return s2;
}
#endif

/* replacement for the BSD strncasecmp function if it is not available: */
#ifndef HAVE_STRNCASECMP
#  include <ctype.h>
int strncasecmp(const char *s1, const char *s2, size_t n)
{
     while (n-- > 0) {
	  char c1 = tolower(*s1++), c2 = tolower(*s2++);
	  if (!c1 || c1 != c2)
	       return (c1 - c2);
     }
     return 0;
}
#endif /* HAVE_STRNCASECMP */

void print_min_max( float *data, int n )
{
   int i;
   float min, max;
   int missing = 0;

   min = 1.0e30;
   max = -1.0e30;

   for (i=0;i<n;i++) {
      if (IS_MISSING( *data )) {
         /*printf("missing: %g %d\n", *data );*/
         missing++;
      }
      else {
         if (*data>max)  max = *data;
         if (*data<min)  min = *data;
      }
      data++;
   }

   printf("min=%g  max=%g  missing=%d\n", min, max, missing );
}
