/*
 *      $Id: graphics.scenes.c,v 1.1 2001/10/09 19:22:20 jedwards Exp $
 */
/*
 *	File:		graphics.scenes.c
 *
 *	Author:		Don Middleton
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Mon Jul 14 20:17:58 MDT 1997
 *
 *	Description:	NCAR VIS5D Enhancment
 *                      Switchboard for creating "scene files"
 *                      of various flavors (e.g. VRML, POV, Wavefront, etc.)
 */

/*
Vis5D system for visualizing five dimensional gridded data sets
Copyright (C) 1990-1996 Bill Hibbard, Brian Paul, Dave Santek,
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



#include "api.h"
#include "graphics.scenes.h"
#include "graphics.vrml.h"


/*
 * Function:	save_3d_scene
 *
 * Description:	Currently only supports VRML
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
int save_3d_scene(Display_Context dtx, char *filename, int format)
{
	int	status;

	switch(format){

		case VIS5D_VRML:
			vrml_save(dtx, filename);
			break;
		default:
			return(0);
	}

	return(1);
}

