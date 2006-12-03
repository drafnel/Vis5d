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

#ifndef SOUNDING_H
#define SOUNDING_H


#include "globals.h"

extern int reload_sounding_data( Display_Context dtx );

extern int draw_sounding( Display_Context dtx, int time);

extern int  make_soundGFX_window( Display_Context dtx, const char *title, int xpos, int ypos,
                           int width, int height, Window ctrlwindow, const char *wdpy_name);

extern void do_pixmap_art( Display_Context dtx );

extern void resize_snd_window( Display_Context dtx, int width, int height,
                               int x, int y);

extern unsigned long SND_AllocateColorInt( int r, int g, int b );

#endif
