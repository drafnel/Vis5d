/* misc.h */

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

#ifndef MISC_i_H
#define MISC_i_H


extern char *str_dup( char *s );


extern void print_min_max( float *data, int n );

/* Case-insensitive versions of strcmp and strncmp are available on
   UNIX, on windows the same routines are called _stricmp and _strnicmp.
   "You say strncasecmp. I say _strnicmp. Let's call the whole thing off!"
   An autoconf guru should handle this in config.h via the configure script
   instead of here... I haven't tested this on Win32...
*/
#ifdef WIN32
#define strcasecmp( sz1, sz2 ) _stricmp ( sz1, sz2 )
#define strncasecmp( sz1, sz2, size ) _strnicmp ( sz1, sz2, size )
#endif

#endif

