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

#ifdef HAVE_LIBPNG

#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#include <X11/Xlib.h>

#include <png.h>

#include "pngdump.h"

static void die( char *msg )
{
        fflush(stdout);
        fflush(stderr);
        fprintf(stderr, "vis5d: error: %s\n", msg);
        exit(1);
}

#define CHECK(cond, msg) { if (!(cond)) { die(msg); } }

/* required for older versions of libpng: */
#ifndef png_jmpbuf
#  define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif

void png_dump(Display	*dpy,
	      int	screen,
	      Window	window,
	      FILE	*out,
	      GLenum	oglbuf)
{
     XWindowAttributes win_info;
     int height, width, row;
     png_byte *image;
     png_bytep *row_pointers;
     png_structp png_ptr;
     png_infop info_ptr;

     /******************* get image data **********************/

     glReadBuffer(oglbuf);
     glFinish();
     
     CHECK(XGetWindowAttributes(dpy, window, &win_info),
	   "can't get target window attributes");
     width      = win_info.width;
     height     = win_info.height;

     image = (png_byte *) malloc(sizeof(png_byte) * width*height * 4);
     CHECK(image, "out of memory");
     glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image);
     
     row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * height);
     for (row = 0; row < height; ++row)
	  row_pointers[row] = image + (height-1 - row) * width * 4;

     /******************* create and write png file ***********/

     /* Create and initialize the png_struct with the desired error
        handler functions.  If you want to use the default stderr and
        longjump method, you can supply NULL for the last three
        parameters.  We also check that the library version is
        compatible with the one used at compile time, in case we are
        using dynamically linked libraries.  REQUIRED. */
     png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,NULL);
     CHECK(png_ptr, "png_create_write_struct failed");

     /* Allocate/initialize the image information data.  REQUIRED */
     info_ptr = png_create_info_struct(png_ptr);
     CHECK(info_ptr, "png_create_info_struct failed");

     /* Set error handling.  REQUIRED if you aren't supplying your own 
        error handling functions in the png_create_write_struct() call. */
     if (setjmp(png_jmpbuf(png_ptr))) {
	  /* If we get here, we had a problem writing the file */
	  die("error writing png file");
     }
     /* set up the output control if you are using standard C streams */
     png_init_io(png_ptr, out);

     /* Set the image information here.  Width and height are up to
       2^31, bit_depth is one of 1, 2, 4, 8, or 16, but valid values
       also depend on the color_type selected. color_type is one of
       PNG_COLOR_TYPE_GRAY, PNG_COLOR_TYPE_GRAY_ALPHA,
       PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB, or
       PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either
       PNG_INTERLACE_NONE or PNG_INTERLACE_ADAM7, and the
       compression_type and filter_type MUST currently be
       PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED */

     png_set_IHDR(png_ptr, info_ptr, width, height, 8 /* bit_depth */ ,
		  PNG_COLOR_TYPE_RGB,
		  PNG_INTERLACE_NONE,
		  PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

     /* Write the file header information.  REQUIRED */
     png_write_info(png_ptr, info_ptr);

     /* Get rid of ALPHA bytes: pack RGBA into RGB (4 channels -> 3
      * channels). The second parameter is not used. */
     png_set_filler(png_ptr, 0, PNG_FILLER_AFTER);


     /* write out the entire image data in one call */
     png_write_image(png_ptr, row_pointers);

     /* It is REQUIRED to call this to finish writing the rest of the file */
     png_write_end(png_ptr, info_ptr);

     /* clean up after the write, and free any memory allocated */
     png_destroy_write_struct(&png_ptr, (png_infopp) NULL);

     free(row_pointers);
     free(image);
}

#endif /* HAVE_LIBPNG */
