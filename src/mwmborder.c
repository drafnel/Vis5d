/* mwmborder.c */


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
 * This function shows how to remove the border, title bar, resize button,
 * etc from a Motif window frame from inside an Xlib-based application.
 *
 * Brian Paul  19 Sep 1995   brianp@ssec.wisc.edu
 */


#include <X11/Xlib.h>
#include <X11/Xatom.h>

#ifdef HAVE_X11_XM_MWMUTIL_H
#  include <X11/Xm/MwmUtil.h>
#else

/* bit definitions for MwmHints.flags */
#define MWM_HINTS_FUNCTIONS       (1L << 0)
#define MWM_HINTS_DECORATIONS     (1L << 1)
#define MWM_HINTS_INPUT_MODE      (1L << 2)
#define MWM_HINTS_STATUS          (1L << 3)

/* bit definitions for MwmHints.decorations */
#define MWM_DECOR_ALL             (1L << 0)
#define MWM_DECOR_BORDER          (1L << 1)
#define MWM_DECOR_RESIZEH         (1L << 2)
#define MWM_DECOR_TITLE           (1L << 3)
#define MWM_DECOR_MENU            (1L << 4)
#define MWM_DECOR_MINIMIZE        (1L << 5)
#define MWM_DECOR_MAXIMIZE        (1L << 6)

typedef struct
{
    unsigned long        flags;
    unsigned long        functions;
    unsigned long        decorations;
    long                 inputMode;
    unsigned long        status;
} PropMotifWmHints;

#define PROP_MOTIF_WM_HINTS_ELEMENTS  5

#endif /* ! HAVE_X11_XM_MWMUTIL_H */



/*
 * Specify which Motif window manager border decorations to put on a
 * top-level window.  For example, you can specify that a window is not
 * resizabe, or omit the titlebar, or completely remove all decorations.
 * Input:  dpy - the X display
 *         w - the X window
 *         flags - bitwise-OR of the MWM_DECOR_xxx symbols in X11/Xm/MwmUtil.h
 *                 indicating what decoration elements to enable.  Zero would
 *                 be no decoration.
 */
void set_mwm_border( Display *dpy, Window w, unsigned long flags )
{
   PropMotifWmHints motif_hints;
   Atom prop, proptype;

   /* setup the property */
   motif_hints.flags = MWM_HINTS_DECORATIONS;
   motif_hints.decorations = flags;

   /* get the atom for the property */
   prop = XInternAtom( dpy, "_MOTIF_WM_HINTS", True );
   if (!prop) {
      /* something went wrong! */
      return;
   }

   /* not sure this is correct, seems to work, XA_WM_HINTS didn't work */
   proptype = prop;

   XChangeProperty( dpy, w,                         /* display, window */
                    prop, proptype,                 /* property, type */
                    32,                             /* format: 32-bit datums */
                    PropModeReplace,                /* mode */
                    (unsigned char *) &motif_hints, /* data */
                    PROP_MOTIF_WM_HINTS_ELEMENTS    /* nelements */
                  );
}


void no_border( Display *dpy, Window w )
{
   set_mwm_border( dpy, w, 0 );
}

