/*
 *      $Id: sgidump.c,v 1.2 2001/11/05 11:28:45 jedwards Exp $
 */
/*
 *	File:		sgidump.c
 *
 *	Author:		Don Middleton
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Mon Jul 14 20:15:52 MDT 1997
 *
 *	Description:	Provides *working* windowdump code
 *                      for SGI OS6.X and later.
 */
/*
Vis5D system for visualizing five dimensional gridded data sets
Copyright (C) 1990 - 1996 Bill Hibbard, Brian Paul, Dave Santek,
and Andre Battaiola.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include "../config.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "sgidump.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#include <assert.h>
#include <X11/Xlib.h>
#include <X11/Xmd.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int	SGIWrite(
	FILE		*fp,
	int		nx,
	int		ny,
	unsigned char	*buf);

static char *program_name = "xdump";

/*
 * Standard fatal error routine - call like printf but maximum of 7 arguments.
 * Does not require dpy or screen defined.
 */
/* VARARGS1 */
static void Fatal_Error( char *msg )
{
        fflush(stdout);
        fflush(stderr);
        fprintf(stderr, "%s: error: %s\n", program_name, msg);
        exit(1);
}

void
SGI_Dump(
	Display	*display,
	int	scr,
	Window	window,
	FILE	*out,
	GLenum	oglbuf
)
{
    Display		*dpy;
    int			screen;
    unsigned long	swaptest = 1;
    XColor		*colors;
    unsigned		buffer_size;
    int			win_name_size;
    int			header_size;
    int			ncolors, i;
    char		*win_name;
    Bool		got_win_name;
    XWindowAttributes	win_info;
    XImage		*image;
    int			absx, absy, x, y;
    unsigned		width, height;
    int			dwidth, dheight;
    int			bw;
    Window		dummywin;
    static unsigned char	*buf;
    static int		last_nx = -1;
    static int		last_ny = -1;
    int			length, status;

    dpy = display;
    screen = scr;

    glReadBuffer(oglbuf);
    glFinish();

    /*
     * Get the parameters of the window being dumped.
     */
    if(!XGetWindowAttributes(dpy, window, &win_info)) 
      Fatal_Error("Can't get target window attributes.");

    /* handle any frame window */
    if (!XTranslateCoordinates (dpy, window, RootWindow (dpy, screen), 0, 0,
                                &absx, &absy, &dummywin)) {
        fprintf (stderr, 
                 "%s:  unable to translate window coordinates (%d,%d)\n",
                 program_name, absx, absy);
        exit (1);
    }
    win_info.x = absx;
    win_info.y = absy;
    width      = win_info.width;
    height     = win_info.height;
    bw         = 0;

    /* Allocate or adjust buffer for imagery. */

    if (buf == (unsigned char *) NULL) {
	buf = (unsigned char *) calloc(1, width*height*3);
        if (buf == (unsigned char *) NULL) {
		perror("SGI_Dump()");
		return;
	}
    }
    else {
	if (last_nx != width || last_ny != height) {
		(void) free(buf);
		buf = (unsigned char *) calloc(1, width*height*3);
	        if (buf == (unsigned char *) NULL) {
			perror("SGI_Dump()");
			return;
		}
	}
    }
    last_nx = width ; last_ny = height;

    (void) glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buf);

    status = SGIWrite(out, win_info.width, win_info.height, buf);
    if (status != 0) {
	(void) fprintf(stderr,
		"SGI_Dump(): Error encoding/writing SGI rasterfile\n");
    }

    return;
}

#define SGI_MAGIC 		0732

typedef enum {
	SGI_CM_NORMAL,		/* file contains rows of values which 
				 * are either RGB values (zsize == 3) 
				 * or greyramp values (zsize == 1) */
	SGI_CM_DITHERED,
	SGI_CM_SCREEN,		/* File contains data which is a screen
				 * image; getrow() returns buffer which 
				 * can be displayed writepixels(). */
	SGI_CM_COLORMAP		/* File has colormap. */
} SgiColormapType;

#define SGI_TYPEMASK		0xff00
#define SGI_BPPMASK		0x00ff
#define SGI_TYPE_VERBATIM	0x0000
#define SGI_TYPE_RLE		0x0100
#define SGI_ISRLE(type)		(((type) & SGI_TYPEMASK) == SGI_TYPE_RLE)
#define SGI_ISVERBATIM(type)	(((type) & SGI_TYPEMASK) == SGI_TYPE_VERBATIM)
#define SGI_BPP(type)		((type) & SGI_BPPMASK)
#define RLE(bpp)		(SGI_TYPE_RLE | (bpp))
#define VERBATIM(bpp)		(SGI_TYPE_VERBATIM | (bpp))

/*
SGI reserves 512 bytes for the header, but the structure itself
is not (yet) that large.
*/

#ifdef	ArchAlpha
typedef	unsigned int	UInt32_T;
typedef	unsigned short	UInt16_T;
typedef	int		Int32_T;
typedef	short		Int16_T;
#else
typedef	unsigned long	UInt32_T;
typedef	unsigned short	UInt16_T;
typedef	long		Int32_T;
typedef	short		Int16_T;
#endif

#define RAS_SGI_RESERVED	512

#ifdef ArchCray
#define B32     :32
#define B16     :16
#else
#define B32
#define B16
#endif /* ArchCray */


/*
**	Format of SGI file header. Note field types indicate field sizes.
**
*/
typedef	struct	{
	UInt16_T	imagic B16;
	UInt16_T 	type B16;
	UInt16_T 	dim B16;
	UInt16_T 	xsize B16;
	UInt16_T 	ysize B16;
	UInt16_T 	zsize B16;
	UInt32_T 	min B32;
	UInt32_T 	max B32;
	UInt32_T	wastebytes B32;	
	char 		name[80];
	SgiColormapType	colormap B32;
} SGIFileHeader_T;
	
int	SGIWrite(
	FILE		*fp,
	int		nx,
	int		ny,
	unsigned char	*buf
) {
	SGIFileHeader_T		header;
	int			x, y, i, nb;
	unsigned long		swaptest = 1;
	static unsigned char	*tmpbuf;
	static int		tmpbuf_size = 0;
	unsigned char		*iptr, *optr;

	if (tmpbuf == (unsigned char *) NULL) {
		if (nx > RAS_SGI_RESERVED) {
			tmpbuf_size = nx;
		}
		else {
			tmpbuf_size = RAS_SGI_RESERVED;
		}
		tmpbuf = (unsigned char *) calloc(1, tmpbuf_size);
		if (! tmpbuf) {
			(void) fprintf(stderr,
				"sgidump: Memory allocation error\n");
			return(-1);
		}
	}

	
	header.type       = SGI_TYPE_VERBATIM | 1;
	header.imagic     = SGI_MAGIC;
	header.dim        = 3;
	header.xsize      = nx; 
	header.ysize      = ny;
	header.zsize      = 3; /* RGB image. */
	header.min        = 0;
	header.max        = 255;
	header.wastebytes = 0;
	header.colormap   = SGI_CM_NORMAL;
	sprintf(header.name,"Created by Vis5d+ version %s",VERSION);
	/*	strcpy(header.name,"no name"); */

	/* Write the header. */

	nb = fwrite(&header, sizeof(header),1, fp);
	if (nb != 1) {
		(void) fprintf(stderr,
			"sgidump: Error writing header\n");
		return(-1);
	}

	/* Write bytes remaining before actual image data. */

	memset(tmpbuf, 0,RAS_SGI_RESERVED-sizeof(header)); 
	nb = fwrite(tmpbuf, 1, (RAS_SGI_RESERVED-sizeof(header)), fp);
	if (nb != (RAS_SGI_RESERVED-sizeof(header))) { 
		(void) fprintf(stderr,
			"sgidump: Error writing header\n");
		return(-1);
	}

	/*
	** SGI_VERBATIM rasterfiles are scan-plane interleaved,
	** so we write out the red plane, the green plane, and then
	** the blue plane. There is no compression done here.
	*/

	for(i=0; i<3; i++) {
		for(y=ny-1; y>=0; y--) {
			iptr = &buf[((ny-y-1)*nx*3)+i];
			optr = tmpbuf;
			for(x=0; x<nx; x++) {
				*optr = *iptr;
				optr += 1;
				iptr += 3;
			}
			nb = fwrite(tmpbuf, 1, nx, fp);
			if (nb != nx) {
				(void) fprintf(stderr,
					"sgidump: Error writing header\n");
				return(-1);
			}
		}
	}

	return(0);
}
