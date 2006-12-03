/*
 *      $Id: graphics.vrml.c,v 1.3 2006/12/03 06:42:28 stevengj Exp $
 */
/*
 *	File:		graphics.vrml.c
 *
 *	Authors:	Original Version
 *			Dr. Wang Hongqing
 *                      Laboratory for Severe Storms Research
 *                      Peking Univ, Beijing, China
 *
 *			VRML2.0 Version
 *			Don Middleton
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *			Changes to support Vis5d version 5.2
 *			Jeff Boote
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *			Michael Boettinger, DKRZ, Hamburg, 2000/09/21:
 *			(based on old code by Don)
 *			horizontal and vertical contour slices
 *			horizontal and vertical stream slices
 *			vis5d clock
 *			changed formatted output of floating point numbers 
 *			from 6.2f to 5.3f to improve resolution
 *
 *	Date:		Tue Mar 25 17:59:20 MST 1997
 *
 *	Description:	Driver for VIS5D VRML 2.0 output
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



/*
 * NOTES:
 *   1. lmcolor( LMC_COLOR ) is the default mode.
 *   2. lighting is normally disabled.
 */

#include "../config.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "api.h"
#include "proj.h"
#include "misc.h"
#include "render.h"
#include "graphics.vrml.h"
#include "graphics.h"

static FILE	*fp = (FILE *) NULL;

static	int	indentLevel = 0;

#define VERT(Z) (dtx->VerticalSystem==VERT_NONEQUAL_MB ? height_to_pressure(Z) : (Z))
#define TICK_SIZE 0.05

static void bl()
{
	int	i;

	for(i=0; i<indentLevel; i++) (void) putc(' ', fp);
}

static void pushLevel()
{
	indentLevel++;
}

static void popLevel()
{
	if (indentLevel <= 0) {
		(void) fprintf(stderr,
			"Warning: Popping VRML indent level below zero\n");
		indentLevel = 0;
	}
	else {
		indentLevel--;
	}
}
	
static void vrml_header()
{
	indentLevel = 0;
	bl();fprintf(fp, "#VRML V2.0 utf8\n");
	bl();fprintf(fp, "# Produced by %s %s VRML Driver\n\n",PACKAGE,VERSION);

	bl();fprintf(fp, "WorldInfo {\n");
	pushLevel();
	bl();fprintf(fp, "title \"VIS5D VRML Output\"\n");
	bl();fprintf(fp, "info [ \"VRML Driver Author: Don Middleton, NCAR\"]\n");
	popLevel();
	bl();fprintf(fp, "}\n\n");
	fflush(fp);

	bl();fprintf(fp, "NavigationInfo {\n");
	pushLevel();
	bl();fprintf(fp, "headlight TRUE\n");
	bl();fprintf(fp, "avatarSize       [ 0.25, 1.8, 0.75 ]\n");
	bl();fprintf(fp, "speed		.3\n");
	bl();fprintf(fp, "type		\"EXAMINE\"\n");
	popLevel();
	bl();fprintf(fp, "}\n\n");
	fflush(fp);

	bl();fprintf(fp, "Background {\n");
	pushLevel();
	bl();fprintf(fp, "skyColor	.0 .0 .0\n");
	popLevel();
	bl();fprintf(fp, "}\n\n");
	fflush(fp);
}

static void vrml_light()
{
	bl();fprintf(fp, "\nPointLight {\n");
	pushLevel();
	bl();fprintf(fp, "intensity 1\n");
	bl();fprintf(fp, "location -10.0 10.0 5.0\n");
	bl();fprintf(fp, "color 0.8 0.8 0.8\n");
	popLevel();
	bl();fprintf(fp, "} # End of PointLight\n");
}


void vrml_text( float x, float y, float z,
		unsigned int color, char *str)
{
	float red, green, blue;
	char flag[1];

#ifdef VRML_DEBUG
	(void)fprintf(stderr, "vrml_text()\n");
#endif
	
	flag[0] = '"';

	red = UNPACK_RED(color)/255.0;
	green = UNPACK_GREEN(color)/255.0;
	blue = UNPACK_BLUE(color)/255.0;
	
	bl();fprintf(fp, "\n#vrml_text--------\n");
	bl();fprintf(fp, "Transform {\n");
	pushLevel();
	bl();fprintf(fp, "translation %f  %f  %f\n", x, y, z);
	bl();fprintf(fp, "children [\n");
	pushLevel();
	bl();fprintf(fp, "Shape {\n");
	bl();fprintf(fp, "appearance Appearance {\n");
	pushLevel();
	bl();fprintf(fp, "material Material {\n");
	pushLevel();
	bl();fprintf(fp, "emissiveColor %5.3f %5.3f %5.3f\n", red, green, blue);
	bl();fprintf(fp, "diffuseColor %5.3f %5.3f %5.3f\n", red, green, blue);
	popLevel();
	bl();fprintf(fp, "} #End Material\n");
	popLevel();
	bl();fprintf(fp, "} #End Appearance\n");

	pushLevel();
	bl();fprintf(fp, "#AsciiText\n");
	bl();fprintf(fp, "geometry Text {\n");
	pushLevel();
	bl();fprintf(fp, "string  %c%s%c\n", flag[0], str, flag[0]);

 	bl();fprintf(fp, "#Define Font.\n");
	bl();fprintf(fp, "fontStyle FontStyle {\n");
	pushLevel();
	bl();fprintf(fp, "size 0.06\n");
	popLevel();
	bl();fprintf(fp, "} #End of define font.\n"); 

	popLevel();
	bl();fprintf(fp, "} #End of AsciiText.\n");
	popLevel();
	bl();fprintf(fp, "} #End Shape\n");
	popLevel();
	bl();fprintf(fp, "] #End children\n");
	popLevel();
	bl();fprintf(fp, "} #End of vrml_text: Transform.\n\n");	


#ifdef VRML_DEBUG
	(void)fprintf(stderr, "vrml_text()\n");
#endif
}
void vrml_polyline2d( short verts[][2], int n, unsigned int color, 
		                 int WinWidth, int WinHeight)
{
        /*	float		v[3];*/
	int		i;
	float		red, green, blue;
	static char	*myname = "vrml_polyline2d";

#ifdef VRML_DEBUG
	(void)fprintf(stderr, "vrml_polyline2d()\n");
#endif
	red = UNPACK_RED(color)/255.0;
	green = UNPACK_GREEN(color)/255.0;
	blue = UNPACK_BLUE(color)/255.0;

	fprintf(fp, "\n");
	bl();fprintf(fp, "Shape { # %s\n", myname);

	bl();fprintf(fp, "appearance Appearance {\n");
	pushLevel();
	bl();fprintf(fp, "material Material {\n");
	pushLevel();
	bl();fprintf(fp, "emissiveColor %5.3f %5.3f %5.3f\n", red, green, blue);
	bl();fprintf(fp, "diffuseColor %5.3f %5.3f %5.3f\n", red, green, blue);
	popLevel();
	bl();fprintf(fp, "}\n");
	popLevel();
	bl();fprintf(fp, "}\n");

	bl();fprintf(fp, "    #Draw the 2D Polyline\n");
	bl();fprintf(fp, "    geometry IndexedLineSet {\n");
	bl();fprintf(fp, "    #Points\n");
	bl();fprintf(fp, "    coord Coordinate {         \n");
	bl();fprintf(fp, "       point [   # the list of points\n");
	for (i=0;i<n;i++) {
		if ( i != n-1 )
		 {
		         bl();fprintf(fp, "%5.3f %5.3f %5.3f,\n", 
		            2*(verts[i][0]-WinWidth/2)/(float)WinWidth-0.2, 2*(WinHeight/2-verts[i][1])/(float)WinHeight+0.2, 0.0);
		 }
		 else
		 {
		         bl();fprintf(fp, "            %5.3f %5.3f %5.3f\n", 
		            2*(verts[i][0]-WinWidth/2)/(float)WinWidth-0.2, 2*(WinHeight/2-verts[i][1])/(float)WinHeight+0.2, 0.0);
		 }
	}
	bl();fprintf(fp, "       ] #End of points\n");
	bl();fprintf(fp, "       # Total point = %d\n", n);
	bl();fprintf(fp, "    } #End of Coordinate\n");

	bl();fprintf(fp, "\n");
	bl();fprintf(fp, "       coordIndex [\n");
	for (i=0; i<n; i++)
	{
		  if ( i == 0 )
		  {
		      bl();fprintf(fp, "            %d, ", i);
		  }
		  else
		  {
		      bl();fprintf(fp, "%d, ", i);
		  }
	}
	bl();fprintf(fp, "%d\n", -1);
	bl();fprintf(fp, "       ] #End of coordIndex\n");
	bl();fprintf(fp, "    } #End of IndexedLineSet\n"); 
	bl();fprintf(fp, "} #End of Draw 2D Polyline.\n");

#ifdef VRML_DEBUG
	(void)fprintf(stderr, "vrml_polyline2d() done\n");
#endif
}

static void vrml_disjoint_polylines(float verts[][3], int n, unsigned int color)
{
	register int i;
	float red, green, blue;

	red = (color & 0x000000ff)/255.0;
	green = ((color >> 8) & 0x000000ff)/255.0;
	blue =  ((color >> 16) & 0x000000ff)/255.0;
	
	bl();fprintf(fp, "\n# VIS5D DisJointPolyLine\n");
	bl();fprintf(fp, "Shape {\n");

	bl();fprintf(fp, "  appearance Appearance {\n");

	bl();fprintf(fp, "    # Material\n");
	bl();fprintf(fp, "    material Material {\n");
	bl();fprintf(fp, "       diffuseColor %f  %f  %f\n", red, green, blue);
	bl();fprintf(fp, "    } #\n\n");

	bl();fprintf(fp, "  } # Appearance\n");

	bl();fprintf(fp, "  geometry IndexedLineSet {\n");
	bl();fprintf(fp, "    coord Coordinate {\n");
	bl();fprintf(fp, "    #Points\n");
	bl();fprintf(fp, "      point [   # the list of points\n");

	for (i=0;i<n;i++) {
		if ( i != n-1 )
		 {
		         bl();fprintf(fp, "            %5.3f %5.3f %5.3f,\n", 
		            verts[i][0], verts[i][1], verts[i][2]);
		 }
		 else
		 {
		         bl();fprintf(fp, "            %5.3f %5.3f %5.3f\n", 
		            verts[i][0], verts[i][1], verts[i][2]);
		 }
	}
	bl();fprintf(fp, "       ] #End of points\n");
	bl();fprintf(fp, "       # Total point = %d\n", n);
	bl();fprintf(fp, "    } #End of Coordinate\n");

	bl();fprintf(fp, "\n");
	bl();fprintf(fp, "    coordIndex [\n");
	for (i=0;i<n;i+=2) {
		if ( i != n-2 )
		{
		    bl();fprintf(fp, "            %d, %d, -1,\n", i, i+1);
		}
		else
		{
		    bl();fprintf(fp, "            %d, %d, -1 \n", i, i+1);
		}
	}
	bl();fprintf(fp, "    ] #End of coordIndex\n");
	bl();fprintf(fp, "  } #End of IndexedLineSet\n"); 
	bl();fprintf(fp, "} #End of Shape Draw DisJointPolyline\n");
}

static void vrml_polylines_float(int n,
			 float verts[][3],
			 unsigned int color)
{
	int		i;
	float		red, green, blue;
	/*
	const char	*myname = "vrml_polylines_float";
	*/
	red   = UNPACK_RED(color)/255.0;
	green = UNPACK_GREEN(color)/255.0;
	blue  = UNPACK_BLUE(color)/255.0;

	bl();fprintf(fp, "Shape {\n");
	pushLevel();

	bl();fprintf(fp, "appearance Appearance {\n");
	pushLevel();

	bl();fprintf(fp, "material Material {\n");
	pushLevel();
	bl();fprintf(fp, "emissiveColor %5.3f %5.3f %5.3f\n", red, green, blue);
	bl();fprintf(fp, "diffuseColor %5.3f %5.3f %5.3f\n", red, green, blue);
	popLevel();
	bl();fprintf(fp, "}\n");

	popLevel();
	bl();fprintf(fp, "}\n");

	bl();fprintf(fp, "# VIS5D geometry for polyline\n");
	bl();fprintf(fp, "geometry IndexedLineSet {\n");
	pushLevel();

	/* Output the Coordinate node. */

	bl();fprintf(fp, "coord Coordinate {\n");
	pushLevel();
	bl();fprintf(fp, "point [\n");
	pushLevel();

	for (i=0;i<n;i++ ) {
		bl();fprintf(fp, "%5.3f %5.3f %5.3f",
			verts[i][0], verts[i][1], verts[i][2]);
		if ( i != n-1 ) bl();fprintf(fp, ",");
		bl();fprintf(fp, "\n");
	}
	popLevel();
	bl();fprintf(fp, "] # End of %d points\n", n);
	popLevel();
	bl();fprintf(fp, "} # End of Coordinate\n");
	bl();fprintf(fp, "\n");

	/* Output the coordIndex field. */

	bl();fprintf(fp, "coordIndex [\n");

	pushLevel();
	for (i=0; i<n; i++)
	{
		bl();fprintf(fp, "%d, ", i);
	}
	bl();fprintf(fp, "%d\n", -1);

	popLevel();
	bl();fprintf(fp, "] # End of coordIndex\n");

	popLevel();
	bl();fprintf(fp, "} # End of IndexedLineSet\n"); 

	popLevel();
	bl();fprintf(fp, "} # End of Shape\n"); 
}

static void vrml_polylines( int n, int_2 verts[][3], unsigned int color)
{
	int	i;
	float	r, g, b, a;

	r = UNPACK_RED(color)/255.0;
	g = UNPACK_GREEN(color)/255.0;
	b = UNPACK_BLUE(color)/255.0;
	a = UNPACK_ALPHA(color)/255.0;

	pushLevel();
	bl();fprintf(fp, "Shape {\n");

	pushLevel();
	bl();fprintf(fp, "appearance Appearance {\n");

	pushLevel();
	bl();fprintf(fp, "material Material {\n");

	pushLevel();
	bl();fprintf(fp, "emissiveColor %f  %f  %f\n", r, g, b);
	bl();fprintf(fp, "ambientIntensity .1\n");

	popLevel();
	bl();fprintf(fp, "}\n");
	popLevel();
	bl();fprintf(fp, "}\n");

	bl();fprintf(fp, "geometry IndexedLineSet {\n");

	pushLevel();
	bl();fprintf(fp, "coord Coordinate {\n");

	pushLevel();
	bl();fprintf(fp, "point [\n");

	pushLevel();
	for (i=0;i<n;i++ ) {
		bl();fprintf(fp, "%5.3f %5.3f %5.3f,\n", 
			verts[i][0]/VERTEX_SCALE,
			verts[i][1]/VERTEX_SCALE,
			verts[i][2]/VERTEX_SCALE);
	}
	popLevel();
	bl();fprintf(fp, "] # End of points (%d)\n", n);

	popLevel();	
	bl();fprintf(fp, "} # End of Coordinate\n");

	bl();fprintf(fp, "coordIndex [\n");

	pushLevel();
	for (i=0; i<n-1; i++) {
		bl();fprintf(fp, "%d, %d, -1\n", i, i+1);
	}

	popLevel();
	bl();fprintf(fp, "] # End of coordIndex\n");

	popLevel();
	bl();fprintf(fp, "} # End of IndexedLineSet\n"); 

	popLevel();
	bl();fprintf(fp, "} # End of Shape\n");
}

static void vrml_colored_polylines(int n, int_2 verts[][3],
	                   uint_1 color_indexes[],
	                   unsigned int color_table[])
{
	int		i;
	unsigned int	color;
	float		r, g, b;
	char		*myname = "vrml_colored_polylines";

	pushLevel();
	bl();fprintf(fp, "Shape { # %s\n", myname);

	pushLevel();
	bl();fprintf(fp, "geometry IndexedLineSet {\n");

	pushLevel();
	bl();fprintf(fp, "coord Coordinate {\n");

	pushLevel();
	bl();fprintf(fp, "point [\n");

	pushLevel();
	for (i=0;i<n;i++ ) {
		bl();fprintf(fp, "%5.3f %5.3f %5.3f,\n", 
			verts[i][0]/VERTEX_SCALE,
			verts[i][1]/VERTEX_SCALE,
			verts[i][2]/VERTEX_SCALE);
	}
	popLevel();
	bl();fprintf(fp, "] # End of points (%d)\n", n);

	popLevel();	
	bl();fprintf(fp, "} # End of Coordinate\n");

	bl();fprintf(fp, "coordIndex [\n");

	pushLevel(); bl();
	for (i=0; i<n; i++) {
		fprintf(fp, "%d,", i);
	}
	fprintf(fp, "\n");

	popLevel();
	bl();fprintf(fp, "] # End of coordIndex\n");

	bl();fprintf(fp, "colorPerVertex TRUE\n");

	bl();fprintf(fp, "color Color {\n");

	pushLevel();
	bl();fprintf(fp, "color [\n");

	for (i=0;i<n;i++ ) {
		color = color_table[color_indexes[i]];
		r = UNPACK_RED(color)/255.0;
		g = UNPACK_GREEN(color)/255.0;
		b = UNPACK_BLUE(color)/255.0;
		bl();fprintf(fp, "%5.3f %5.3f %5.3f,\n", r, g, b);
	}

	popLevel();
	bl();fprintf(fp, "] # End of colors\n");

	popLevel();
	bl();fprintf(fp, "} # End of Color Node\n");

	bl();fprintf(fp, "colorIndex [\n");

	pushLevel(); bl();

	for (i=0; i<n; i++) {
		fprintf(fp, "%d,", i);
	}

	fprintf(fp, "\n");

	popLevel();
	bl();fprintf(fp, "]\n");

	popLevel();
	bl();fprintf(fp, "} # End of IndexedLineSet\n");

	popLevel();
	bl();fprintf(fp, "} # End of Shape\n");
}

static void vrml_multi_lines( int n, float verts[][3], unsigned int color)
{
	int		i;
	float		r, g, b, a;
	const char	*myname = "vrml_multi_lines";

	r = UNPACK_RED(color)/255.0;
	g = UNPACK_GREEN(color)/255.0;
	b = UNPACK_BLUE(color)/255.0;
	a = UNPACK_ALPHA(color)/255.0;

	pushLevel();
	bl();fprintf(fp, "Shape { # VIS5D Begin %s \n", myname);

	pushLevel();
	bl();fprintf(fp, "appearance Appearance {\n");

	pushLevel();
	bl();fprintf(fp, "material Material {\n");

	pushLevel();
	bl();fprintf(fp, "emissiveColor %5.3f %5.3f %5.3f\n", r, g, b);
	bl();fprintf(fp, "diffuseColor %5.3f %5.3f %5.3f\n", r, g, b);

	popLevel();
	bl();fprintf(fp, "}\n");

	popLevel();
	bl();fprintf(fp, "}\n");

	bl();fprintf(fp, "geometry IndexedLineSet {\n");

	pushLevel();
	bl();fprintf(fp, "coord Coordinate {\n");

	pushLevel();
	bl();fprintf(fp, "point [\n");

	pushLevel();
	for (i=0;i<n;i++) {
		/*
		This looks wrong, we should be filtering out the 999.0 vertices.
		DM 4/27/98
		*/
		bl();fprintf(fp, "%5.3f %5.3f %5.3f,\n", 
			verts[i][0], verts[i][1], verts[i][2]);
	}

	popLevel();
	bl();fprintf(fp, "] # End of points (n=%d)\n", n);

	popLevel();
	bl();fprintf(fp, "} # End of Coordinate\n");

	bl();fprintf(fp, "coordIndex [\n");
	 
	pushLevel();
	for (i = 0; i < n ; i++) {
		if ( i == 0 ) {
		   if (verts[i][0] != -999.0) {
		      bl();fprintf(fp, "%d, ", i);
		   }
		}
		else {
		   if (verts[i][0]==-999.0) {
		      /* start new line */
		      if ( i == n-1) {
		          bl();fprintf(fp, "-1 \n");
		      }
		      else {
		          bl();fprintf(fp, "-1,\n");
		      }
		   }
		   else if (verts[i-1][0]==-999.0) {
		      if ( i == n-1) {
		          bl();fprintf(fp, "%d, -1\n", i);
		      }
		      else {
		          bl();fprintf(fp, "%d, ", i);
		      }
		   }
		   else {
		      if ( i == n-1) {  
		          bl();fprintf(fp, "%d, -1\n", i);
		      }
		      else {          
		          bl();fprintf(fp, "%d, ", i);
		      }
		   }
		}
	}

	popLevel();
	bl();fprintf(fp, "] # End of %s coordIndex\n", myname);

	popLevel();
	bl();fprintf(fp, "} # End of %s IndexedLineSet\n", myname); 

	popLevel();
	bl();fprintf(fp, "} # End of %s Shape\n", myname);
}

static void vrml_wind_lines(int nvectors,
		     int_2 verts[][3],
		     unsigned int color)
{
	int	i, j;
	float	red, green, blue;

	red   = UNPACK_RED(color)/255.0;
	green = UNPACK_GREEN(color)/255.0;
	blue  = UNPACK_BLUE(color)/255.0;

	bl();fprintf(fp, "\n");
	bl();fprintf(fp, "# *** Begin vrml_wind_lines\n");
	bl();fprintf(fp, "Transform {\n");

	pushLevel();
	bl();fprintf(fp, "children [\n");

	pushLevel();
	bl();fprintf(fp, "Shape {\n");

	pushLevel();
	bl();fprintf(fp, "appearance Appearance {\n");

	pushLevel();
	bl();fprintf(fp, "material Material {\n");

	pushLevel();
	bl();fprintf(fp, "emissiveColor %f %f %f\n", red, green, blue);

	popLevel();
	bl();fprintf(fp, "} # End of Material\n");

	popLevel();
	bl();fprintf(fp, "} # End of Appearance\n");

	bl();fprintf(fp, "geometry IndexedLineSet {\n");

	pushLevel();
	bl();fprintf(fp, "coord Coordinate {\n");

	pushLevel();
	bl();fprintf(fp, "point [\n");

	pushLevel();
	for (i = 0; i<nvectors; i++) {
		if (i != nvectors-1) {
			for ( j = 0; j < 4; j++) {
				if (verts[i*4+j][2]/VERTEX_SCALE >  1.1 ||
					verts[i*4+j][2]/VERTEX_SCALE < -1.1) {
					(void) fprintf(stderr,
						"Bogus Z for %d is %f\n",
						i,verts[i*4+j][2]/VERTEX_SCALE);
				}
				bl();fprintf(fp, "%5.3f %5.3f %5.3f,\n", 
					verts[i*4+j][0]/VERTEX_SCALE,
					verts[i*4+j][1]/VERTEX_SCALE,
					verts[i*4+j][2]/VERTEX_SCALE);
			}
		 }
		 else {
			/* Last vector to be drawn */
			for ( j = 0; j < 3; j++) {
				bl();fprintf(fp, "%5.3f %5.3f %5.3f,\n", 
					verts[i*4+j][0]/VERTEX_SCALE,
					verts[i*4+j][1]/VERTEX_SCALE,
					verts[i*4+j][2]/VERTEX_SCALE);
			}
			bl();fprintf(fp, "%5.3f %5.3f %5.3f \n", 
				verts[i*4+3][0]/VERTEX_SCALE,
				verts[i*4+3][1]/VERTEX_SCALE,
				verts[i*4+3][2]/VERTEX_SCALE);
		 } 
	}

	popLevel();
	bl();fprintf(fp, "] # End of %d Points, %d wind vectors\n",
			4*nvectors, nvectors);

	popLevel();
	bl();fprintf(fp, "} # End of Coordinate\n");
	bl();fprintf(fp, "\n");
	
	bl();fprintf(fp, "coordIndex [\n");

	pushLevel();
	for ( i = 0; i<nvectors;i++) {
		j = i*4;
		if ( i != nvectors-1) {
			/* The vector? */
			bl();fprintf(fp, "%d, %d, -1,\n", j, j+1);
			/* The arrowhead? */
			bl();fprintf(fp, "%d, %d, %d, -1,\n", j+2, j+1, j+3);
		}
		else {
			bl();fprintf(fp, "%d, %d, -1,\n", j, j+1);
			bl();fprintf(fp, "%d, %d, %d, -1\n", j+2, j+1, j+3);
		}
	} 

	popLevel();
	bl();fprintf(fp, "] # End of coordIndex\n");

	popLevel();
	bl();fprintf(fp, "} # End of IndexedLineSet\n"); 

	popLevel();
	bl();fprintf(fp, "} # End of Shape\n"); 

	popLevel();
	bl();fprintf(fp, "] # End of children\n"); 

	popLevel();
	bl();fprintf(fp, "} # *** End vrml_wind_lines\n");
}

void vrml_disjoint_lines(
			int		n,
			int_2		verts[][3],
			unsigned int	color
)
{
	/* enabled version  MiB 2000/09/19   */
        int i;
	float red, green, blue;
	
	red   = UNPACK_RED(color)/255.0;
	green = UNPACK_GREEN(color)/255.0;
	blue  = UNPACK_BLUE(color)/255.0;

	
	bl();fprintf(fp, "\n#Draw DisJoint-Lines\n");
	bl();fprintf(fp, "Shape {\n");
	pushLevel();
	bl();fprintf(fp, "appearance Appearance {\n");
	pushLevel();
	bl();fprintf(fp, "material Material {\n");
	pushLevel();
	bl();fprintf(fp, "emissiveColor %f  %f  %f\n", red, green, blue);
	bl();fprintf(fp, "diffuseColor %f  %f  %f\n", red, green, blue);
	popLevel();
	bl();fprintf(fp, "} #End of Material\n\n");
	popLevel();
	bl();fprintf(fp, "} #End of Appearance\n");

	bl();fprintf(fp, "\n");
	bl();fprintf(fp, "#Draw the DisJoint-lines\n");
	bl();fprintf(fp, "geometry IndexedLineSet {\n");
	pushLevel();
	bl();fprintf(fp, "#Points\n");
	bl();fprintf(fp, "coord Coordinate {         \n");
	pushLevel();
	bl();fprintf(fp, "point [   # the list of points\n");
	pushLevel();

	for (i=0;i<n;i++) {
		if ( i != n-1 )
		 {
		         bl();fprintf(fp, "            %5.3f %5.3f %5.3f,\n", 
				verts[i][0]/VERTEX_SCALE,
				verts[i][1]/VERTEX_SCALE,
				verts[i][2]/VERTEX_SCALE);
		 }
		 else
		 {
		         bl();fprintf(fp, "            %5.3f %5.3f %5.3f\n", 
				verts[i][0]/VERTEX_SCALE,
				verts[i][1]/VERTEX_SCALE,
				verts[i][2]/VERTEX_SCALE);
		 }
	}
	popLevel();
	bl();fprintf(fp, "] #End of points\n");
	bl();fprintf(fp, "# Total point = %d\n", n);
	popLevel();
	bl();fprintf(fp, "} #End of Coordinate\n");
	bl();fprintf(fp, "coordIndex [\n");
	pushLevel();
	for (i=0;i<n;i+=2 ) {
		 if (i != n-2)
		 {
		     bl();fprintf(fp, "            %d, %d, -1,\n", i, i+1);
		 }
		 else
		 {
		     bl();fprintf(fp, "            %d, %d, -1 \n", i, i+1);
		 }
	}
	popLevel();
	bl();fprintf(fp, "] #End of coordIndex\n");
	popLevel();
	bl();fprintf(fp, "} #End of IndexedLineSet\n"); 
	popLevel();
	bl();fprintf(fp, "} #End of Draw DisJoint-lines\n");
}

static void vrml_colored_quadmesh(int rows, int columns,
			int_2 verts[][3],
			uint_1 color_indexes[],
			unsigned int color_table[],
		        int alphavalue)
{
	register	int i, j, base1, base2;
	int		j0, j1, j2, j3;
	float		red, green, blue;
	const char	*myname = "vrml_colored_quadmesh";

	fprintf(fp, "\n");
	bl();fprintf(fp, "Shape { # VIS5D %s\n", myname);

	pushLevel();
	bl();fprintf(fp, "appearance Appearance {\n");
	
	pushLevel();
	bl();fprintf(fp, "material Material {\n");
	pushLevel();
	bl();fprintf(fp, "transparency %f\n", (1.0 - ((float)alphavalue / 255.0)));
	popLevel();
	bl();fprintf(fp, "} # End of Material\n\n");

	popLevel();
	bl();fprintf(fp, "} # End of Appearance\n");
	bl();fprintf(fp, "\n");

	bl();fprintf(fp, "# Geometry for the colored slice.\n");
	bl();fprintf(fp, "geometry IndexedFaceSet {\n");

	pushLevel();
	bl();fprintf(fp, "solid FALSE\n");
	bl();fprintf(fp, "colorPerVertex TRUE\n\n");

	bl();fprintf(fp, "coord Coordinate {\n");

	pushLevel();
	bl();fprintf(fp, "point [   # the list of points\n");

	for (i=0;i<rows*columns;i++) {
		if ( i != rows*columns-1)
		{
		      bl();fprintf(fp, "%5.3f %5.3f %5.3f,\n", 
		      	verts[i][0]/VERTEX_SCALE,
			verts[i][1]/VERTEX_SCALE,
			verts[i][2]/VERTEX_SCALE);
		}
		else
		{
		      bl();fprintf(fp, "%5.3f %5.3f %5.3f \n", 
		         verts[i][0]/VERTEX_SCALE,
			 verts[i][1]/VERTEX_SCALE,
			 verts[i][2]/VERTEX_SCALE);
		} 
	}
 
	bl();fprintf(fp, "] # End of Coordinate list (total points = %d)\n",
		rows*columns);
	popLevel();
	bl();fprintf(fp, "} # End of Coordinate\n");

	/* Now bl();fprintfoduce the coordinate indices. */

	bl();fprintf(fp, "\n");
	bl();fprintf(fp, "coordIndex [\n");
	pushLevel();

	for (i=0;i<rows-1;i++) {
		base1 = i * columns;
		base2 = (i+1) * columns;

		for (j = 1; j < columns; j++) {
		   j0 = base1+j-1; j1 = base2+j-1; j2 = base2+j; j3 = base1+j;
		   if (i == rows-2 && j == columns-1) {
			bl();fprintf(fp,
					"%d, %d, %d, %d, -1 \n",
					j0, j1, j2, j3);
		   }
		   else {
		   	bl();fprintf(fp,
					"%d, %d, %d, %d, -1,\n",
					j0, j1, j2, j3);
		   }
		}
	}
	popLevel();
	bl();fprintf(fp, "] # End of coordIndex\n");
		
	bl();fprintf(fp, "\n");
	bl();fprintf(fp, "# List of Vertex Colors\n");
	bl();fprintf(fp, "color Color {\n");
	pushLevel();
	bl();fprintf(fp, "color [\n");
	pushLevel();

	for (i=0;i<rows*columns;i++ ) {
		unsigned int color;

		color = color_table[color_indexes[i]];
		red   = UNPACK_RED(color)/255.0;
		green = UNPACK_GREEN(color)/255.0;
		blue  = UNPACK_BLUE(color)/255.0;
	
		if ( i != rows*columns-1 ) {
		        bl();fprintf(fp, "%5.3f %5.3f %5.3f,\n", red, green, blue);
		}
		else {
		        bl();fprintf(fp, "%5.3f %5.3f %5.3f \n", red, green, blue); 
		}
	}
	popLevel();
	bl();fprintf(fp, "] # End of colors (total colors %d)\n", rows*columns);
	popLevel();
	bl();fprintf(fp, "} # End of Color Node\n");

	bl();fprintf(fp, "\n");
	bl();fprintf(fp, "colorIndex [\n");
	pushLevel();

	for (i=0;i<rows-1;i++) {
		base1 = i * columns;
		base2 = (i+1) * columns;

		for (j = 1; j < columns; j++) {
		   j0 = base1+j-1; j1 = base2+j-1; j2 = base2+j; j3 = base1+j;
		   if (i == rows-2 && j == columns-1) {
		      bl();fprintf(fp, "%d, %d, %d, %d, -1 \n", j0, j1, j2, j3);
		   }
		   else {
		      bl();fprintf(fp, "%d, %d, %d, %d, -1,\n", j0, j1, j2, j3);
		   }
		}
	} 
	popLevel();
	bl();fprintf(fp, "] # End of colorIndex\n"); 

	popLevel();
	bl();fprintf(fp, "} # End of IndexedFaceSet\n");       

	popLevel();
	bl();fprintf(fp, "} # End of Colored QuadMesh Shape.\n");
}

static void vrml_colored_topomesh(
			int rows,
			int columns,
			float verts[][3],
			float norms[][3],
			uint_1 color_indexes[],
			unsigned int color_table[])
{
	register	int i, j, base1, base2;
	int		j0, j1, j2, j3;
	float		red, green, blue;

	fprintf(fp, "\n");
	bl();fprintf(fp, "# VIS5D Topography\n");
	bl();fprintf(fp, "Shape { # Topography Shape\n\n");

	/* Output the appearance of the topo. */

	pushLevel();
	bl();fprintf(fp, "appearance Appearance {\n");
	pushLevel();
	bl();fprintf(fp, "material Material {\n");
	pushLevel();
	bl();fprintf(fp, "emissiveColor .2 .2 .2\n");
	popLevel();
	bl();fprintf(fp, "} # End of Material\n\n");
	popLevel();
	bl();fprintf(fp, "} # End of Appearance\n");
	bl();fprintf(fp, "\n");

	bl();fprintf(fp, "# Geometry for the topo.\n");
	bl();fprintf(fp, "geometry IndexedFaceSet {\n");

	pushLevel();
	bl();fprintf(fp, "solid FALSE\n");
	bl();fprintf(fp, "colorPerVertex TRUE\n\n");

	bl();fprintf(fp, "coord Coordinate {\n");
	pushLevel();

	bl();fprintf(fp, "point [ # Topography points\n");
	pushLevel();

	for (i=0;i<rows*columns;i++) {
		if ( i != rows*columns-1)
		{
		      bl();fprintf(fp, "%5.3f %5.3f %5.3f,\n", 
		      	verts[i][0], verts[i][1], verts[i][2]);
		}
		else
		{
		      bl();fprintf(fp, "%5.3f %5.3f %5.3f \n", 
		         verts[i][0], verts[i][1], verts[i][2]);
		} 
	}
 
	popLevel();
	bl();fprintf(fp,
		"] # End of Coordinate list (total points = %d)\n",
		rows*columns);
	popLevel();
	bl();fprintf(fp, "} # End of Coordinate\n");

	/* Output the coordinate indices. */

	fprintf(fp, "\n");
	bl();fprintf(fp, "coordIndex [\n");
	pushLevel();

	for (i=0;i<rows-1;i++) {
		base1 = i * columns;
		base2 = (i+1) * columns;

		for (j = 1; j < columns; j++) {
		   j0 = base1+j-1; j1 = base2+j-1; j2 = base2+j; j3 = base1+j;
		   if (i == rows-2 && j == columns-1) {
			bl();fprintf(fp,
				"%d, %d, %d, %d, -1 \n",
				j0, j1, j2, j3);
		   }
		   else {
		   	bl();fprintf(fp,
				"%d, %d, %d, %d, -1,\n",
				j0, j1, j2, j3);
		   }
		}
	}
	bl();fprintf(fp, "] # End of coordIndex\n");
	bl();fprintf(fp, "\n");

	/* Output the vertex colors. */

	bl();fprintf(fp, "# List of Vertex Colors\n");
	bl();fprintf(fp, "color Color {\n");
	pushLevel();
	bl();fprintf(fp, "color [\n");
	pushLevel();

	for (i=0;i<rows*columns;i++ ) {
		unsigned int color;

		color = color_table[color_indexes[i]];
		red   = UNPACK_RED(color)/255.0;
		green = UNPACK_GREEN(color)/255.0;
		blue  = UNPACK_BLUE(color)/255.0;
	
		if ( i != rows*columns-1 ) {
		        bl();fprintf(fp,
				"%5.3f %5.3f %5.3f,\n",
				red, green, blue);
		}
		else {
		        bl();fprintf(fp,
				"%5.3f %5.3f %5.3f \n",
				red, green, blue); 
		}
	}
	popLevel();
	bl();fprintf(fp, "] # End of colors (total colors %d)\n", rows*columns);

	popLevel();
	bl();fprintf(fp, "} # End of Color Node\n");
	bl();fprintf(fp, "\n");

	/* Output the color indices. */

	bl();fprintf(fp, "colorIndex [\n");
	pushLevel();

	for (i=0;i<rows-1;i++) {
		base1 = i * columns;
		base2 = (i+1) * columns;

		for (j = 1; j < columns; j++) {
		   j0 = base1+j-1; j1 = base2+j-1; j2 = base2+j; j3 = base1+j;
		   if (i == rows-2 && j == columns-1) {
		      bl();fprintf(fp,
				"%d, %d, %d, %d, -1 \n",
				j0, j1, j2, j3);
		   }
		   else {
		      bl();fprintf(fp,
				"%d, %d, %d, %d, -1,\n",
				j0, j1, j2, j3);
		   }
		}
	} 
	popLevel();
	bl();fprintf(fp, "] # End of topo colorIndex\n"); 

	popLevel();
	bl();fprintf(fp, "} # End of topo IndexedFaceSet\n");       

	popLevel();
	bl();fprintf(fp, "} # End of topo Shape.\n");
}

#ifdef BIG_GFX
static void vrml_isosurface(int n, uint_4 *index, int_2 verts[][3],
			int_1 norms[][3], unsigned int color)
#else
static void vrml_isosurface(int n, uint_2 *index, int_2 verts[][3],
			int_1 norms[][3], unsigned int color)
#endif
{
	int		i, count, maxvert;
	int		i1, i2, i3;
	float		r, g, b, a;
	float		vs, ns;
	const char	*myname = "vrml_isosurface";

	r = UNPACK_RED(color)/255.0;
	g = UNPACK_GREEN(color)/255.0;
	b = UNPACK_BLUE(color)/255.0;
	a = UNPACK_ALPHA(color)/255.0;

        vs = 1.0/VERTEX_SCALE;
        ns = 1.0/NORMAL_SCALE;

	pushLevel();
	bl();fprintf(fp, "Shape { # Begin %s Shape\n", myname);

	pushLevel();
	bl();fprintf(fp, "appearance Appearance {\n");

	pushLevel();
	bl();fprintf(fp, "material Material {\n");

	pushLevel();
	bl();fprintf(fp, "diffuseColor %f  %f  %f\n", r, g, b);
	bl();fprintf(fp, "ambientIntensity .1\n");
	bl();fprintf(fp, "transparency %f\n", 1.0-a);

	popLevel();
	bl();fprintf(fp, "} # End of Material\n");

	popLevel();
	bl();fprintf(fp, "} # End of appearance\n\n");

	bl();fprintf(fp, "# Geometry for isosurface\n");
	bl();fprintf(fp, "geometry IndexedFaceSet {\n");

	pushLevel();
	bl();fprintf(fp, "ccw		FALSE\n");
#ifdef DEAD /* Turn off the normals, there are problems (reversed?) */
	bl();fprintf(fp, "normalPerVertex TRUE\n\n");
#endif
	bl();fprintf(fp, "creaseAngle     1.57\n\n");
	bl();fprintf(fp, "solid		FALSE\n");
#ifdef DEAD
	bl();fprintf(fp, "convex		FALSE\n");
#endif

	bl();fprintf(fp, "coord Coordinate {\n");

	pushLevel();
	bl();fprintf(fp, "point [   # the list of points\n");

	maxvert = 0;
	for(i=0; i<n; i++) {
#ifdef VRML_DEBUG
		(void) fprintf(stderr,
			"maxvert=%d  index[%d]=%d\n",
			maxvert, i, index[i]);
#endif /* VRML_DEBUG */
		if (maxvert < index[i]) {
			maxvert = index[i];
		}
	}

	pushLevel();
	for (i=0; i<=maxvert; i++) {

	  if (i != maxvert) {
            bl();fprintf(fp, "%5.3f %5.3f %5.3f,\n", 
 	      verts[i][0]*vs, verts[i][1]*vs, verts[i][2]*vs);
          }
          else {
            bl();fprintf(fp, "%5.3f %5.3f %5.3f \n",
              verts[i][0]*vs, verts[i][1]*vs, verts[i][2]*vs);
          } 
        }

	popLevel();
	bl();fprintf(fp, "] # End of Points (nvertices=%d)\n", maxvert+1);
	popLevel();
	bl();fprintf(fp, "} # End of Coordinate \n");
	bl();fprintf(fp, "\n");

	/**** Render the indexed triangle strip ****/

	bl();fprintf(fp, "# Isosurface triangle strip\n");
	bl();fprintf(fp, "coordIndex [\n");

	count = 0;
	pushLevel();
	for (i=0; i<n-3; i++) {
		
		if (i%2 == 0) {
		  i1 = index[i];
		  i2 = index[i+1];
		  i3 = index[i+2];
		}
		else {
		  i1 = index[i+1];
		  i2 = index[i];
		  i3 = index[i+2];
		}

		/*
		Degenerate faces i.e. ones where the vertices
		are coincident, cause VRML many problems. Leave 'em out.
		*/
		if (i1 != i2 && i2 != i3 && i1 != i3) {
			bl();fprintf(fp, "%d, %d, %d, -1", i1, i2, i3);

			if (i != n-2) {
				bl();fprintf(fp, ",\n");
			}
			else {
				bl();fprintf(fp, "\n");
			}

			count++;
		}
	}
	popLevel();
	bl();fprintf(fp, "] # End of coordIndex (npolys = %d)\n", count);


#ifdef DEAD /* The normals just don't work right, maybe some reversed.  */
	bl();fprintf(fp, "normal Normal {\n");
	pushLevel();
	bl();fprintf(fp, "vector [   # Normal vectors for isosurface\n");
	pushLevel();
	for (i=0; i<=maxvert; i++) {

		if( i != maxvert)
		{
		      bl();fprintf(fp, "%5.3f %5.3f %5.3f,\n", 
		         ((float)norms[i][0])*ns,
			 ((float)norms[i][1])*ns,
			 ((float)norms[i][2])*ns);
		}
		else
		{
		      bl();fprintf(fp, "%5.3f %5.3f %5.3f\n", 
		         ((float)norms[i][0])*ns,
			 ((float)norms[i][1])*ns,
			 ((float)norms[i][2])*ns);
		} 
	}

	popLevel();
	bl();fprintf(fp, "] # End of vector (n=%d)\n", n);
	popLevel();
	bl();fprintf(fp, "} # End of Normal\n\n");
	
	bl();fprintf(fp, "\n");
	bl();fprintf(fp, "normalIndex [\n");

	pushLevel();
	for (i=0; i<n-3; i++) {
		
		if (i%2 == 0) {
		  i1 = index[i];
		  i2 = index[i+1];
		  i3 = index[i+2];
		}
		else {
		  i1 = index[i+1];
		  i2 = index[i];
		  i3 = index[i+2];
		}


		if (i1 != i2 && i2 != i3 && i1 != i3) {
		  bl();fprintf(fp, "%d, %d, %d, -1", i1, i2, i3);

		  if (i != n-2) {
		    bl();fprintf(fp, ",\n");
		  }
		  else {
		    bl();fprintf(fp, "\n");
		  }

		  count++;
		}
	}
	popLevel();
	bl();fprintf(fp, "] # End of normalIndex\n");
#endif

	popLevel();
	bl();fprintf(fp, "} # End of %s Shape geometry\n", myname); 

	popLevel();
	bl();fprintf(fp, "} # End of %s Shape\n", myname);
}

static void vrml_colored_isosurface( int n,
#ifdef BIG_GFX
		uint_4 *index,
#else
		uint_2 *index,
#endif
		int_2 verts[][3],
		int_1 norms[][3],
		uint_1 color_indexes[],
		unsigned int color_table[],
		int alpha)
{
	int		i, count, maxvert;
	int		i1, i2, i3;
	float		r, g, b;
	float		vs, ns;
	const char	*myname = "vrml_colored_isosurface";

        vs = 1.0/VERTEX_SCALE;
        ns = 1.0/NORMAL_SCALE;

	pushLevel();
	bl();fprintf(fp, "Shape {\n");

	pushLevel();
	bl();fprintf(fp, "appearance Appearance {\n");

	pushLevel();
	bl();fprintf(fp, "material NULL\n");
	bl();fprintf(fp, "texture NULL\n");
	bl();fprintf(fp, "textureTransform NULL\n");

	popLevel();
	bl();fprintf(fp, "}\n");

	bl();fprintf(fp, "# Geometry for isosurface\n");
	bl();fprintf(fp, "geometry IndexedFaceSet {\n");

	pushLevel();
	bl();fprintf(fp, "colorPerVertex TRUE\n");
	bl();fprintf(fp, "normalPerVertex TRUE\n");
	bl();fprintf(fp, "ccw		FALSE\n");
#ifdef DEAD /* Turn off the normals, there are problems (reversed?) */
	bl();fprintf(fp, "normalPerVertex TRUE\n\n");
#endif
	bl();fprintf(fp, "creaseAngle     1.57\n\n");
	bl();fprintf(fp, "solid		FALSE\n");
#ifdef DEAD
	bl();fprintf(fp, "convex		FALSE\n");
#endif

	bl();fprintf(fp, "# Point Color\n");
	bl();fprintf(fp, "color Color {\n");

	pushLevel();
	bl();fprintf(fp, "color [\n");

	pushLevel();
	for (i=0;i<n;i++ ) {
		unsigned int rgb;

		rgb = color_table[color_indexes[i]];
		r = UNPACK_RED(rgb)/255.0;
		g = UNPACK_GREEN(rgb)/255.0;
		b = UNPACK_BLUE(rgb)/255.0;
		bl();fprintf(fp, "%5.3f %5.3f %5.3f,\n", r, g, b);
	}

	popLevel();
	bl();fprintf(fp, "] # End of colors\n");

	popLevel();
	bl();fprintf(fp, "} # End of Color\n");

	bl();fprintf(fp, "coord Coordinate {\n");

	pushLevel();
	bl();fprintf(fp, "point [   # the list of points\n");

	maxvert = 0;
	for(i=0; i<n; i++) {
#ifdef VRML_DEBUG
		(void) fprintf(stderr,
			"maxvert=%d  index[%d]=%d\n",
			maxvert, i, index[i]);
#endif /* VRML_DEBUG */
		if (maxvert < index[i]) {
			maxvert = index[i];
		}
	}

	pushLevel();
	for (i=0; i<=maxvert; i++) {

	  if (i != maxvert) {
            bl();fprintf(fp, "%5.3f %5.3f %5.3f,\n", 
 	      verts[i][0]*vs, verts[i][1]*vs, verts[i][2]*vs);
          }
          else {
            bl();fprintf(fp, "%5.3f %5.3f %5.3f \n",
              verts[i][0]*vs, verts[i][1]*vs, verts[i][2]*vs);
          } 
        }

	popLevel();
	bl();fprintf(fp, "] # End of Points (nvertices=%d)\n", maxvert+1);
	popLevel();
	bl();fprintf(fp, "} # End of Coordinate \n");
	bl();fprintf(fp, "\n");

	/**** Render the indexed triangle strip ****/

	bl();fprintf(fp, "# Isosurface triangle strip\n");
	bl();fprintf(fp, "coordIndex [\n");

	count = 0;
	pushLevel();
	for (i=0; i<n-3; i++) {
		
		if (i%2 == 0) {
		  i1 = index[i];
		  i2 = index[i+1];
		  i3 = index[i+2];
		}
		else {
		  i1 = index[i+1];
		  i2 = index[i];
		  i3 = index[i+2];
		}

		/*
		Degenerate faces i.e. ones where the vertices
		are coincident, cause VRML many problems. Leave 'em out.
		*/
		if (i1 != i2 && i2 != i3 && i1 != i3) {
			bl();fprintf(fp, "%d, %d, %d, -1", i1, i2, i3);

			if (i != n-2) {
				fprintf(fp, ",\n");
			}
			else {
				fprintf(fp, "\n");
			}

			count++;
		}
	}
	popLevel();
	bl();fprintf(fp, "] # End of coordIndex (npolys = %d)\n", count);


#ifdef DEAD /* The normals just don't work right, maybe some reversed.  */
	bl();fprintf(fp, "normal Normal {\n");
	pushLevel();
	bl();fprintf(fp, "vector [   # Normal vectors for isosurface\n");
	pushLevel();
	for (i=0; i<=maxvert; i++) {

		if( i != maxvert)
		{
		      bl();fprintf(fp, "%5.3f %5.3f %5.3f,\n", 
		         ((float)norms[i][0])*ns,
			 ((float)norms[i][1])*ns,
			 ((float)norms[i][2])*ns);
		}
		else
		{
		      bl();fprintf(fp, "%5.3f %5.3f %5.3f\n", 
		         ((float)norms[i][0])*ns,
			 ((float)norms[i][1])*ns,
			 ((float)norms[i][2])*ns);
		} 
	}

	popLevel();
	bl();fprintf(fp, "] # End of vector (n=%d)\n", n);
	popLevel();
	bl();fprintf(fp, "} # End of Normal\n\n");
	
	bl();fprintf(fp, "\n");
	bl();fprintf(fp, "normalIndex [\n");

	pushLevel();
	for (i=0; i<n-3; i++) {
		
		if (i%2 == 0) {
		  i1 = index[i];
		  i2 = index[i+1];
		  i3 = index[i+2];
		}
		else {
		  i1 = index[i+1];
		  i2 = index[i];
		  i3 = index[i+2];
		}


		if (i1 != i2 && i2 != i3 && i1 != i3) {
		  bl();fprintf(fp, "%d, %d, %d, -1", i1, i2, i3);

		  if (i != n-2) {
		    bl();fprintf(fp, ",\n");
		  }
		  else {
		    bl();fprintf(fp, "\n");
		  }

		  count++;
		}
	}
	popLevel();
	bl();fprintf(fp, "] # End of normalIndex\n");
#endif

	popLevel();
	bl();fprintf(fp, "} # End of %s geometry\n", myname); 

	popLevel();
	bl();fprintf(fp, "} # End of %s Shape\n", myname);
}


/*** vrml_string  ******************************************************
	write a string in 3-D.  At this time, only strings of digits,
	periods, and dashes are implemented.
	Input: f - the string to plot.
		    startx, y, z - the point in 3-D to start at.
		    base - vector indicating text baseline.
		    up - vector indicating ubl();fprintfight direction for text.
		    rjustify - non-zero value indicates right justify the text.
**********************************************************************/
static void vrml_string( char *str, float startx, float starty, float startz,
		             float base[], float up[], int rjustify, 
		             unsigned int color)
{
	static float zero[] = { 0,0, 0,.8, .4,.8, .4,0, 0,0 },
		one[] = { 0,0, 0,.8 },
		two[] = { .4,0, 0,0, 0,.4, .4,.4, .4,.8, 0,.8 },
		three[] = { 0,0, .4,0, .4,.4, 0,.4, .4,.4, .4,.8, 0,.8 },
		four[] = { 0,.8, 0,.4, .4,.4, .4,.8, .4,0 },
		five[] = { 0,0, .4,0, .4,.4, 0,.4, 0,.8, .4,.8 },
		six[] = { .4,.8, 0,.8, 0,0, .4,0, .4,.4, 0,.4 },
		seven[] = { 0,.7, 0,.8, .4,.8, .4,0 },
		eight[] = { 0,0, 0,.8, .4,.8, .4,0, 0,0, 0,.4, .4,.4 },
		nine[] = { .4,.4, 0,.4, 0,.8, .4,.8, .4,0 },
		dash[] = { 0,.4, .4,.4 },
		dot[] = { 0,0, 0,.1, .1,.1, .1,0, 0,0 };

	static float *index[12] = { zero, one, two, three, four, five, six,
		                         seven, eight, nine, dash, dot };

	static float width[12] = { 0.6, 0.2, 0.6, 0.6, 0.6, 0.6, 0.6, 0.6,
		0.6, 0.6, 0.6, 0.3 };
	static int verts[12] = { 5, 2, 6, 7, 5, 6, 6, 4, 7, 5, 2, 5 };

	float *temp, plot[100][3];
	float cx, cy, cz;
	int i, j, k, len;
	const char	*myname = "vrml_string";

	fprintf(fp, "# *** Begin %s:%s\n", myname,str);

	cx = startx;  cy = starty;  cz = startz;
	len = strlen(str);

	if (rjustify) {
		/* draw right justified text */
		for (i=len-1; i>=0; i--) {
		   if (str[i]=='-')
		      k = 10;
		   else if (str[i]=='.')
		      k = 11;
		   else if (str[i]>='0' && str[i]<='9')
		      k = str[i] - '0';
		   else
		      continue;
		   /* calculate position for this char */
		   cx += width[k]*base[0];
		   cy += width[k]*base[1];
		   cz += width[k]*base[2];
		   /* make the vertex array for this character */
		   temp = index[k];
		   for (j=0; j<verts[k]; j++) {
		      float x, y;
		      x = *temp++;
		      y = *temp++;
		      plot[j][0] = cx - x*base[0] + y*up[0];
		      plot[j][1] = cy - x*base[1] + y*up[1];
		      plot[j][2] = cz - x*base[2] + y*up[2];
		   }
		   vrml_polylines_float( verts[k], plot, color );
		}

	}
	else {
		/* draw left justified text */
		for (i=0; i<len; i++) {
		   if (str[i]=='-')
		      k = 10;
		   else if (str[i]=='.')
		      k = 11;
		   else if (str[i]>='0' && str[i]<='9')
		      k = str[i] - '0';
		   else
		      continue;
		   /* make the vertex array for this character */
		   temp = index[k];
		   for (j=0; j<verts[k]; j++) {
		      float x, y;
		      x = *temp++;
		      y = *temp++;
		      plot[j][0] = cx + x*base[0] + y*up[0];
		      plot[j][1] = cy + x*base[1] + y*up[1];
		      plot[j][2] = cz + x*base[2] + y*up[2];
		   }
		   vrml_polylines_float( verts[k], plot, color );
		   /* calculate position for next char */
		   cx += width[k]*base[0];
		   cy += width[k]*base[1];
		   cz += width[k]*base[2];
		}
	}

	fprintf(fp, "# *** End %s\n", myname);
}

/*
 * Write the tick mark for a horizontal slice.
 * Input:  level - grid level
 *         z - graphics z coord
 *         height - geographic height coord
 *         color - color.
 */
static void vrml_horizontal_slice_tick(
			Display_Context dtx,
			float level,
			float z,
			float height,
			unsigned int color)
{
	/*
	 * VERTPRIME is defined in render.c - but we need it here.
	 */
#ifndef	VERTPRIME
#define	VERTPRIME(Z) (dtx->VerticalSystem==VERT_NONEQUAL_MB ? height_to_pressure(Z) : (Z))
#endif
	float	v[2][3];
	static	float base[3] = { 0.035, -0.035, 0.0 };
	static	float up[3] = { 0.0, 0.0, 0.07 };
	char	str[1000];
	float	red, green, blue;

	red   = UNPACK_RED(color)/255.0;
	green = UNPACK_GREEN(color)/255.0;
	blue  = UNPACK_BLUE(color)/255.0;

	bl();fprintf(fp, "\n");
	bl();fprintf(fp, "# **** VIS5D vrml_horizontal_slice_tick\n");
 
	/* Produce the vertices for the tick mark. */
	v[0][0] = dtx->Xmax;
	v[0][1] = dtx->Ymin;
	v[0][2] = z;
	v[1][0] = dtx->Xmax + 0.05;
	v[1][1] = dtx->Ymin - 0.05;
	v[1][2] = z;

	vrml_polylines_float( 2, v, color);

	/* Produce the textual label */
	if (dtx->CoordFlag) {
		float2string(dtx, 2, level+1.0, str );
	}
	else {
		float2string(dtx, 2, VERTPRIME(height), str );
	}

	vrml_string( str, dtx->Xmax+0.07, dtx->Ymin-0.07, z, base,up, 0, 
		           color);

	bl();fprintf(fp, "# *** End horizontal_slice_tick\n");
}

/*
 * Write a tick mark for a vertical slice.
 * Input:  row, col - position in grid coords
 *         x, y - position in graphics coords
 *         lat,lon - position in geographic coords
 *         color - color.
 *         fp -  file stream.
 */
static void vrml_vertical_slice_tick(
	Display_Context	dtx,
	float		row,
	float		col,
	float		x,
	float		y,
	float		lat,
	float		lon,
	unsigned int	color)
{
	float v[2][3];
	/* base and up vectors for drawing 3-D text */
	static float b2[3] = { 0.05, 0.0, 0.0 }, u2[3] = { 0.0, 0.05, 0.0 };
	static float b3[3] = { -0.05, 0.0, 0.0 }, u3[3] = { 0.0, 0.05, 0.0 };
	char str[1000];
	float red, green, blue;

	red = (color & 0x000000ff)/255.0;
	green = ((color >> 8) & 0x000000ff)/255.0;
	blue =  ((color >> 16) & 0x000000ff)/255.0;

	bl();fprintf(fp, "\n#Draw vertical_slice_tick\n");

	if (col==0.0) {
		/* draw on top-west edge */
		v[0][0] = x;
		v[0][1] = y;
		v[0][2] = dtx->Zmax;
		v[1][0] = x-0.05;
		v[1][1] = y;
		v[1][2] = dtx->Zmax;
		vrml_polylines_float( 2, v, color);
		if (dtx->CoordFlag) {
		   float2string(dtx, 1, row+1, str );
		}
		else {
		   float2string(dtx, 1, lat, str );
		}
		vrml_string( str, x-0.07, y, dtx->Zmax, b3, u3, 1, color);
	}
	else if (col==(float)(dtx->Nc-1)) {
		/* draw on top-east edge */
		v[0][0] = x;
		v[0][1] = y;
		v[0][2] = dtx->Zmax;
		v[1][0] = x+0.05;
		v[1][1] = y;
		v[1][2] = dtx->Zmax;
		vrml_polylines_float( 2, v, color );
		if (dtx->CoordFlag) {
		   float2string( dtx, 1, row+1, str );
		}
		else {
		   float2string( dtx, 1, lat, str );
		}
		vrml_string( str, x+0.07, y, dtx->Zmax, b2, u2, 0, color);
	}
	else if (row==0.0) {
		/* draw on top-north edge */
		v[0][0] = x;
		v[0][1] = y;
		v[0][2] = dtx->Zmax;
		v[1][0] = x;
		v[1][1] = y+0.05;
		v[1][2] = dtx->Zmax;
		vrml_polylines_float( 2, v, color );
		if (dtx->CoordFlag) {
		   float2string( dtx, 0, col+1.0, str );
		}
		else {
		   float2string( dtx, 0, lon, str );
		}
		vrml_string( str, x-0.07, y+0.07, dtx->Zmax, b2,u2, 0, color );
	}
	else {
		/* draw on top-south edge */
		v[0][0] = x;
		v[0][1] = y;
		v[0][2] = dtx->Zmax;
		v[1][0] = x;
		v[1][1] = y-0.05;
		v[1][2] = dtx->Zmax;
		vrml_polylines_float( 2, v, color );
		if (dtx->CoordFlag) {
		   float2string( dtx, 0, col+1.0, str );
		}
		else {
		   float2string( dtx, 0, lon, str );
		}
		vrml_string( str, x-0.07, y-0.12, dtx->Zmax, b2,u2, 0, color );
	}

	fprintf(fp, "#End of vertical_slice_tick\n");

}



/*############################## OBJECTS ####################################*/
/*
 * Write the 3-D box.
 * Input:  it - time step.
 */
static void vrml_box( Display_Context dtx, int it)
{
	/* base and up vectors for text drawn along x,y,z axes. */
	static float bx[3] = { 0.05, 0.0, 0.0 }, ux[3] = {0.0, 0.05, 0.05 };
	static float by[3] = { -0.035, 0.0, -0.035 }, uy[3] = {0.0, 0.07, 0.0 };
	static float bz[3] = { -0.035, -0.035, 0.0 }, uz[3] = {0.0, 0.0, 0.07 };
	float		x1, y1, z1, x2, y2, z2;
	char		str[100];
	float		r, g, b;
	unsigned int	color;
	const char	*myname = "vrml_box";

	/* set depth cueing & line color */
	color = dtx->BoxColor;


	r = UNPACK_RED(color)/255.0;
	g = UNPACK_GREEN(color)/255.0;
	b = UNPACK_BLUE(color)/255.0;
	
	pushLevel();
	bl();fprintf(fp, "Transform { # %s\n", myname);

	pushLevel();
	bl();fprintf(fp, "children [ # %s\n", myname);

	if(dtx->Reversed){
		vrml_multi_lines(dtx->NumBoxVerts,dtx->BoxVerts,
							PACK_COLOR(0,0,0,255));
	}
	else{
		vrml_multi_lines(dtx->NumBoxVerts,dtx->BoxVerts,dtx->BoxColor);
	}

	if (dtx->TickMarks) {
		/* Draw axis labels. */
		if (dtx->CoordFlag) {
		   x1 = 1.0;
		   x2 = (float) dtx->Nc;
		   y1 = 1.0;
		   y2 = (float) dtx->Nr;
		   z1 = 1.0;
		   z2 = (float) dtx->MaxNl;
		}
		else {
		   x1 = dtx->WestBound;
		   x2 = dtx->EastBound;
		   y1 = dtx->NorthBound;
		   y2 = dtx->SouthBound;
#ifdef LEVELTYPES
		   z1 = dtx->BottomCoordinate;
		   z2 = dtx->TopCoordinate;
#else
		   z1 = dtx->BottomBound;
		   z2 = dtx->TopBound;
#endif
		   z1 = VERT(z1);
		   z2 = VERT(z2);
		}

		if (dtx->CursorX - dtx->Xmin > 0.1 || dtx->DisplayCursor==0) {
		   float2string( dtx, 0, x1, str );
		   vrml_string(str,
			dtx->Xmin-0.02, dtx->Ymin-0.1, dtx->Zmin-0.125,
			bx, ux, 0, color );
		}

		if (dtx->Xmax - dtx->CursorX > 0.1 || dtx->DisplayCursor==0) {
		   float2string( dtx, 0, x2, str );
		   vrml_string(str,
		   dtx->Xmax-0.05, dtx->Ymin-0.1, dtx->Zmin-0.125,
		   bx, ux, 0, color );
		}

		if (dtx->Ymax - dtx->CursorY > 0.1 || dtx->DisplayCursor==0) {
		   float2string( dtx, 1, y1, str );
		   vrml_string(str,
		   dtx->Xmin-0.075, dtx->Ymax-0.03, dtx->Zmin-0.075,
		   by, uy, 1, color );
		}

		if (dtx->CursorY - dtx->Ymin > 0.1 || dtx->DisplayCursor==0) {
		   float2string( dtx, 2, y2, str );
		   vrml_string( str,
		   dtx->Xmin-0.075, dtx->Ymin-0.02, dtx->Zmin-0.075,
		   by, uy, 1, color );
		}

		if (dtx->CursorZ - dtx->Zmin > 0.1 || dtx->DisplayCursor==0) {
		   float2string( dtx, 2, z1, str );
		   vrml_string(str,
		   dtx->Xmin-0.07, dtx->Ymin-0.07, dtx->Zmin+0.005,
		   bz, uz, 1, color );
		}

		if (dtx->Zmax-dtx->CursorZ > 0.1 || dtx->DisplayCursor==0) {
		   float2string( dtx, 2, z2, str );
		   vrml_string(str,
		   dtx->Xmin-0.07, dtx->Ymin-0.07, dtx->Zmax+0.005,
		   bz, uz, 1, color );
		}
	}

	popLevel();
	bl();fprintf(fp, "] # End %s children\n", myname);

	popLevel();
	bl();fprintf(fp, "} # End %s Transform\n", myname);
}

/*
 * write all horizontal wind vector slices which are selected for display.
 * Input:  ctx - the context
 *         time - the timestep
 */

static void vrml_hwind_slices( Context ctx, int time)
{
	Display_Context	dtx;
	int		w;
	const char	*myname = "vrml_hwind_slices";

	dtx = ctx->dpy_ctx;

	bl();fprintf(fp, "\n");
	bl();fprintf(fp, "# **** Begin %s\n", myname);

	for (w=0;w<VIS5D_WIND_SLICES;w++) {
		if (dtx->DisplayHWind[w] && dtx->HWindTable[w][time].valid &&
				ctx->context_index == dtx->Uvarowner[w]) {
			wait_read_lock(&dtx->HWindTable[w][time].lock);

			/* write the bounding box */
			vrml_polylines_float(
				  dtx->HWindTable[w][time].numboxverts,
				  (void *) dtx->HWindTable[w][time].boxverts,
				  dtx->HWindColor[w]);

			/* draw wind vectors */
			vrml_wind_lines(dtx->HWindTable[w][time].nvectors/4,
				(void *) dtx->HWindTable[w][time].verts,
				dtx->HWindColor[w]);

			done_read_lock(&dtx->HWindTable[w][time].lock);
	   	}

		/* draw position label */
		if (dtx->DisplayBox && !dtx->CurvedBox) {
			vrml_horizontal_slice_tick(dtx, dtx->HWindLevel[w],
					dtx->HWindZ[w], dtx->HWindHgt[w],
					dtx->HWindColor[w]);
		}
	}

	bl();fprintf(fp, "# *** End %s\n", myname);
}

/*
 * write all vertical wind vector slices which are selected for display.
 * Input:  ctx - the context
 *         time - the timestep
 */
static void vrml_vwind_slices( Context ctx, int time)
{
	Display_Context	dtx;
	int w;

	dtx = ctx->dpy_ctx;

	bl();fprintf(fp, "# *** Begin vrml_vwind_slices\n");

	for (w=0;w<VIS5D_WIND_SLICES;w++) {
		if (dtx->DisplayVWind[w] && dtx->VWindTable[w][time].valid &&
				ctx->context_index == dtx->Uvarowner[w]) {
			wait_read_lock(&dtx->VWindTable[w][time].lock);
			recent( ctx, VWIND, w );

			/* draw the bounding box */
			bl();fprintf(fp, "# *** Begin bounding box\n");
			vrml_polylines_float(
				dtx->VWindTable[w][time].numboxverts,
				(void *) dtx->VWindTable[w][time].boxverts,
				dtx->VWindColor[w]);		 
			bl();fprintf(fp, "# *** End bounding box\n");

			/* draw wind vectors */
			vrml_wind_lines( dtx->VWindTable[w][time].nvectors/4,
					(void *) dtx->VWindTable[w][time].verts,
					dtx->VWindColor[w]);

			done_read_lock( &dtx->VWindTable[w][time].lock );

			if (dtx->DisplayBox && !dtx->CurvedBox) {
				/* position labels */
				float r1p, r2p, c1p, c2p, lp;
				float x1,y1,z1;
				float x2,y2,z2;
				float zbot, ztop;
				float vert[4][3];
				zbot = gridlevelPRIME_to_zPRIME(dtx, time,
					dtx->Uvar[w],(float) dtx->LowLev);
				ztop = gridlevelPRIME_to_zPRIME(dtx, time,
						dtx->Uvar[w],
						(float)(dtx->Nl + dtx->LowLev));
	
				r1p = dtx->VWindR1[w];
				c1p = dtx->VWindC1[w];
				r2p = dtx->VWindR2[w];
				c2p = dtx->VWindC2[w];
				lp= 0.0;
				gridPRIME_to_xyzPRIME(dtx,time,dtx->Uvar[w],
							1,&r1p,&c1p,
							&lp,&x1,&y1,&z1);
				gridPRIME_to_xyzPRIME(dtx,time,dtx->Uvar[w],1,
							&r2p,&c2p,
							&lp,&x2,&y2,&z2);
				vrml_vertical_slice_tick(dtx,r1p,c1p,x1,y1,
					dtx->VWindLat1[w],dtx->VWindLon1[w],
					dtx->VWindColor[w]);		 
				vrml_vertical_slice_tick(dtx,r2p,c2p,x2,y2,
					dtx->VWindLat2[w],dtx->VWindLon2[w],
					dtx->VWindColor[w]);		 
				/*
				 * draw small markers at midpoint of top and
				 * bottom edges
				 */
				vert[0][0] = vert[1][0] = vert[2][0] =
					vert[3][0] =
					(dtx->VWindX1[w]+dtx->VWindX2[w])*0.5;
				vert[0][1] = vert[1][1] = vert[2][1] =
					vert[3][1] =
					(dtx->VWindY1[w]+dtx->VWindY2[w])*0.5;
				vert[0][2] = ztop + TICK_SIZE;
				vert[1][2] = ztop;
				vert[2][2] = zbot;
				vert[3][2] = zbot - TICK_SIZE;
	
				vrml_disjoint_polylines( vert, 4,
							dtx->VWindColor[w]);
			}
		}
	}

	bl();fprintf(fp, "# *** End vrml_vwind_slices\n");
}

/*
 * Write all trajectories which are selected for display.
 * Input:  ctx - the context
 *         it  - the timestep
 */
static void
vrml_trajectories(
	Context	ctx,
	int	it)
{
	Display_Context	dtx;
	int i, len, start;

	dtx = ctx->dpy_ctx;

	for (i=0;i<dtx->NumTraj;i++) {
		struct traj *t = dtx->TrajTable[i];

		if(t->ctx_owner == ctx->context_index &&
			dtx->DisplayTraj[t->group] && cond_read_lock(&t->lock)){

			assert( t->lock==1 );

			recent( ctx, TRAJ, t->group );

			start = t->start[it];
			len = t->len[it];
			if (start!=0xffff && len>0) {
				if (t->kind==0) {
					/* draw as line segments */
					int colorvar = t->colorvar;
					if (colorvar>=0) {
						/* draw colored trajectory */
						vrml_colored_polylines( len,
						(void *) (t->verts + start*3),
						(void *)(t->colors + start),
						dtx->ColorTable[VIS5D_TRAJ_CT]->Colors[
							t->colorvarowner*
							MAXVARS+colorvar]);
					}
					else {
					/* monocolored */
						vrml_polylines( len,
						(void *) (t->verts + start*3),
						dtx->TrajColor[t->group]);
					}
				}
			}
			done_read_lock( &t->lock );
		}
	}
}

/*
 * Draw the map.
 * Input:  time - time step
 *         flat - 1 = draw flat map
 *                  0 = draw raised map
 * Return:  nothing.
 */
int vrml_map2( Display_Context dtx, int time, int flat, unsigned int color)
{
	int	i, j, k;
	float	red, green, blue;
	float	*verts;
	const char	*myname = "vrml_map2";

	red   = UNPACK_RED(color)/255.0;
	green = UNPACK_GREEN(color)/255.0;
	blue  = UNPACK_BLUE(color)/255.0;

	bl();fprintf(fp, "\n");
	bl();fprintf(fp, "# *** Begin %s\n", myname);
	bl();fprintf(fp, "\n");
	bl();fprintf(fp, "Transform {\n");

	pushLevel();
	bl();fprintf(fp, "children Shape {\n");

	pushLevel();
	bl();fprintf(fp, "appearance Appearance {\n");

	pushLevel();
	bl();fprintf(fp, "material Material {\n");

	pushLevel();
	bl();fprintf(fp, "emissiveColor %f %f %f\n", red, green, blue);

	popLevel();
	bl();fprintf(fp, "} # End of Material\n");  

	popLevel();
	bl();fprintf(fp, "} # End of Appearance\n\n");  

	bl();fprintf(fp, "geometry IndexedLineSet {\n");

	pushLevel();
	bl();fprintf(fp, "coord Coordinate {\n");

	pushLevel();
	bl();fprintf(fp, "point [\n");

	if (flat) {
		/* draw a flat map */
		bl();fprintf(fp, "# %s flat rendition\n", myname);
		for (i=0;i<dtx->SegCount;i++) {
			verts = (float *)(dtx->FlatMapVert+dtx->Start[i]);
			for (j=0; j<dtx->Len[i]; j++) {
				if (i == dtx->SegCount-1 && j == dtx->Len[i]-1){
					bl();fprintf(fp, "%5.3f %5.3f %5.3f\n", 
					*(verts+j*3+0),
					*(verts+j*3+1),
					*(verts+j*3+2));
				}
				else {
					bl();fprintf(fp, "%5.3f %5.3f %5.3f,\n", 
					*(verts+j*3+0),
					*(verts+j*3+1),
					*(verts+j*3+2));
				}
			}
			bl();fprintf(fp,"# Map Segment %d includes %d points\n",
				i, dtx->Len[i]);
		}

		popLevel();
		bl();fprintf(fp, "] # End of points\n");

		popLevel();
		bl();fprintf(fp, "} #End of Coordinate\n");

		bl();fprintf(fp, "\n");
		bl();fprintf(fp, "coordIndex [\n");

		pushLevel();
		k = 0;
		for (i=0;i<dtx->SegCount;i++) {
			for (j=0; j<dtx->Len[i]; j++) {
				if ( j == 0 ) {
					bl();fprintf(fp, "%d,", k);
				}
				else {
					bl();fprintf(fp, "%d, ", k);
				}
				k++;
			}
			if (i == dtx->SegCount-1) {  
				bl();fprintf(fp, "%d \n", -1);
			}
			else {
				bl();fprintf(fp, "%d,\n", -1);
			}
		}

		popLevel();
		bl();fprintf(fp, "] # End of coordIndex with %d points\n", k+1);

		popLevel();
		bl();fprintf(fp, "} # End of IndexedLineSet\n");
	}
	else {
		bl();fprintf(fp, "# %s non-flat rendition\n", myname);
		/* draw a map with heights */
		/* NOTE: ain't fixed up yet! */
		for (i=0;i<dtx->SegCount;i++) {
		   verts = (float *)(dtx->MapVert+dtx->Start[i]);
		   for (j=0; j<dtx->Len[i]; j++)
		   {
		       if (i == dtx->SegCount-1 && j == dtx->Len[i]-1)
		       {
		           bl();fprintf(fp, "            %5.3f %5.3f %5.3f \n", 
		              *(verts+j*3+0), *(verts+j*3+1), *(verts+j*3+2));
		       }
		       else
		       {
		           bl();fprintf(fp, "            %5.3f %5.3f %5.3f,\n", 
		              *(verts+j*3+0), *(verts+j*3+1), *(verts+j*3+2));
		       }
		   }
		   bl();fprintf(fp, "# Map Segment %d includes %d points\n",
			i, dtx->Len[i]);
		}

		popLevel();
		bl();fprintf(fp, "] # End of points\n");

		popLevel();
		bl();fprintf(fp, "] # End of Coordinate\n");

		bl();fprintf(fp, "\n");
		bl();fprintf(fp, "coordIndex [\n");

		pushLevel();
		k = 0;
		for (i=0;i<dtx->SegCount;i++) {
		   for (j=0; j<dtx->Len[i]; j++) {
		       if ( j == 0 ) {
		          bl();fprintf(fp, "%d,",k);
		       }
		       else {
		          bl();fprintf(fp, "%d, ", k);
		       }
		       k++;
		   }
		   if (i == dtx->SegCount-1) {  
		       bl();fprintf(fp, "%d \n", -1);
		   }
		   else {
		       bl();fprintf(fp, "%d,\n", -1);
		   }
		}

		popLevel();
		bl();fprintf(fp, "] #End of coordIndex with %d points\n",k+1);

		popLevel();
		bl();fprintf(fp, "} #End of IndexedLineSet\n");
	}

	popLevel();
	bl();fprintf(fp, "} # End Shape\n");

	popLevel();
	bl();fprintf(fp, "} # End %s\n", myname);
	return 0;
}

/*
 * Write the topography.
 *	Generate the VRML code for the VIS5D topography. This will
 *	simply be a Transform node with single child - a colored quadmesh.
 *	The quadmesh will be colored, possibly lit, and opaque.
 * Input:  time - the timestep number
 */
static void
vrml_topo(
	Display_Context	dtx,
	int		time)
{
	uint_1		*indexes;
	unsigned int	*color_table;

	(void) fprintf(fp, "\n");
	bl();fprintf(fp, "# VIS5D Topography\n");


	if (dtx->topo->TopoColorVar<0) {
		color_table = dtx->ColorTable[VIS5D_TOPO_CT]->Colors[MAXVARS*VIS5D_MAX_CONTEXTS];
		indexes     = dtx->topo->TopoIndexes[MAXTIMES];
	}
	else {
		color_table = dtx->ColorTable[VIS5D_TOPO_CT]->Colors[
			dtx->topo->TopoColorVarOwner*VIS5D_MAX_CONTEXTS +
				dtx->topo->TopoColorVar];
		indexes     = dtx->topo->TopoIndexes[time];
		if (!indexes) {
			indexes = dtx->topo->TopoIndexes[MAXTIMES];
		}
	}
	/* draw the topo as a per-vertex colored, lit quadmesh */
	vrml_colored_topomesh(dtx->topo->qrows,dtx->topo->qcols,(void *)dtx->topo->TopoVertex,
				(void *)dtx->topo->TopoNormal,indexes,color_table);
}

/*
 * Write all isosurfaces selected for display.
 * Input:  ctx - the context
 *         dtxtime
 *         ctxtime
 */
static void
vrml_isosurfaces(
	Context		ctx,
	int		dtxtime,
	int		ctxtime
)
{
	int		var, alpha;
	Display_Context	dtx;
	int		time, colorvar, cvowner;
	const char	*myname = "vrml_isosurfaces";

	dtx = ctx->dpy_ctx;

	fprintf(fp, "\n# Begin %s\n", myname);
	for(var=0;var<ctx->NumVars;var++){
		if(ctx->SameIsoColorVarOwner[var] || ctx->IsoColorVar[var] < 0){
			time = ctxtime;
		}
		else{
			time = dtxtime;
		}

		if (ctx->DisplaySurf[var] && ctx->Variable[var]->SurfTable[time]->valid) {
			wait_read_lock( &ctx->Variable[var]->SurfTable[time]->lock );

			recent( ctx, ISOSURF, var );
			colorvar = ctx->Variable[var]->SurfTable[time]->colorvar;
			cvowner = ctx->Variable[var]->SurfTable[time]->cvowner;

			alpha = UNPACK_ALPHA(dtx->Color[ctx->context_index *
						MAXVARS + var][ISOSURF]); 

			if(ctx->Variable[var]->SurfTable[time]->colors){
				vrml_colored_isosurface(
					ctx->Variable[var]->SurfTable[time]->numindex,
					ctx->Variable[var]->SurfTable[time]->index,
					(void*)ctx->Variable[var]->SurfTable[time]->verts,
					(void*)ctx->Variable[var]->SurfTable[time]->norms,
					(void*)ctx->Variable[var]->SurfTable[time]->colors,
					dtx->ColorTable[VIS5D_ISOSURF_CT]->Colors[
						cvowner * MAXVARS + colorvar],
					alpha);
			}
			else {
				vrml_isosurface(
					ctx->Variable[var]->SurfTable[time]->numindex,
					ctx->Variable[var]->SurfTable[time]->index,
					(void *)ctx->Variable[var]->SurfTable[time]->verts,
					(void *)ctx->Variable[var]->SurfTable[time]->norms,
					dtx->Color[ctx->context_index *
							MAXVARS + var][0]);
			}
			done_read_lock(&ctx->Variable[var]->SurfTable[time]->lock);
		}
	}
	fprintf(fp, "# End %s\n", myname);
}

/*
 * Write all horizontal contour slices selected for display.
 * Input:  ctx - the context
 *         time - the time step
 *         labels - draw labels flag.
 */
void vrml_hslices(
	Context	ctx,
	int	time,
	int	labels
)
{
	Display_Context	dtx;
	int var;

	bl();fprintf(fp, "\n#----------------- horizontal contour slices -----------------\n");
	bl();fprintf(fp, "#Draw horizontal contour slices\n");
	bl();fprintf(fp, "Transform {\n");
	bl();fprintf(fp, "  children [\n");

	dtx = ctx->dpy_ctx;

	for (var=0;var<ctx->NumVars;var++) {
		if (ctx->DisplayHSlice[var] &&
				ctx->Variable[var]->HSliceTable[time]->valid){

			wait_read_lock(&ctx->Variable[var]->HSliceTable[time]->lock);
			recent( ctx, HSLICE, var );

			/* draw main contour lines */
			vrml_disjoint_lines( ctx->Variable[var]->HSliceTable[time]->num1,
				(void *)ctx->Variable[var]->HSliceTable[time]->verts1,
				dtx->Color[var][HSLICE]);

			if (labels) {
				/* draw contour labels */
				vrml_disjoint_lines(
				ctx->Variable[var]->HSliceTable[time]->num3,
				(void *)ctx->Variable[var]->HSliceTable[time]->verts3,
				dtx->Color[var][HSLICE]);
			}
			else{
				/* draw hidden contour lines */
				vrml_disjoint_lines(
					ctx->Variable[var]->HSliceTable[time]->num2,
				(void *)ctx->Variable[var]->HSliceTable[time]->verts2,
				dtx->Color[var][HSLICE]);
			}

			/* draw the bounding box */
			vrml_polylines_float(
				ctx->Variable[var]->HSliceTable[time]->numboxverts,
				(void *) ctx->Variable[var]->HSliceTable[time]->boxverts,
				dtx->Color[var][HSLICE]);

			done_read_lock( &ctx->Variable[var]->HSliceTable[time]->lock );

			/* draw position label */
			if (dtx->DisplayBox && !dtx->CurvedBox) {
				vrml_horizontal_slice_tick(dtx,
					ctx->Variable[var]->HSliceRequest->Level,
					ctx->Variable[var]->HSliceRequest->Z,
					ctx->Variable[var]->HSliceRequest->Hgt,
					dtx->Color[var][HSLICE]);
			}
		}
	}

	bl();fprintf(fp, " ]  # end children\n");
	bl();fprintf(fp, "} #End of Draw horizontal contour slices\n");
}

/*
 * Write all vertical contour slices selected for display.
 * Input:  ctx - the context
 *         time - the time step
 *         labels - draw labels flag.
 */
void vrml_vslices(
	Context	ctx,
	int	time,
	int	labels
)
{
	Display_Context	dtx;
	int var;

	bl();fprintf(fp, "\n#----------------- vertical contour slices -------------------\n");
	bl();fprintf(fp, "#Draw vertical contour slices\n");
	bl();fprintf(fp, "Transform {\n");
	bl();fprintf(fp, "  children [\n");
 
	dtx = ctx->dpy_ctx;

	for (var=0;var<ctx->NumVars;var++) {
		if (ctx->DisplayVSlice[var]&&ctx->Variable[var]->VSliceTable[time]->valid){

			wait_read_lock(&ctx->Variable[var]->VSliceTable[time]->lock);

			recent( ctx, VSLICE, var );

			/* draw main contour lines */
			vrml_disjoint_lines( ctx->Variable[var]->VSliceTable[time]->num1,
				(void*) ctx->Variable[var]->VSliceTable[time]->verts1,
				dtx->Color[var][VSLICE]);
			if (labels) {
				/* draw contour labels */
				vrml_disjoint_lines(
					ctx->Variable[var]->VSliceTable[time]->num3,
					(void*)ctx->Variable[var]->VSliceTable[time]->verts3,
					dtx->Color[var][VSLICE]);
			}
			else {
				/* draw hidden contour lines */
				vrml_disjoint_lines(
					ctx->Variable[var]->VSliceTable[time]->num2,
					(void*) ctx->Variable[var]->VSliceTable[time]->verts2,
					dtx->Color[var][VSLICE]);
			}

			/* draw the bounding box */
			vrml_polylines_float(
				ctx->Variable[var]->VSliceTable[time]->numboxverts,
				(void *) ctx->Variable[var]->VSliceTable[time]->boxverts,
				dtx->Color[var][VSLICE]);

			done_read_lock( &ctx->Variable[var]->VSliceTable[time]->lock );

			if (dtx->DisplayBox && !dtx->CurvedBox) {
				/* draw position labels */
				float vert[4][3];
				float zbot, ztop;

				zbot = gridlevel_to_z(ctx,time,var,
					(float)ctx->Variable[var]->LowLev);
				ztop = gridlevel_to_z(ctx, time, var,
					(float)(ctx->Nl[var] - 1 +
							ctx->Variable[var]->LowLev));
				set_color( dtx->Color[var][VSLICE] );
				vrml_vertical_slice_tick(dtx,ctx->Variable[var]->VSliceRequest->R1,
					ctx->Variable[var]->VSliceRequest->C1,ctx->Variable[var]->VSliceRequest->X1,
					ctx->Variable[var]->VSliceRequest->Y1,ctx->Variable[var]->VSliceRequest->Lat1,
					ctx->Variable[var]->VSliceRequest->Lon1,
					dtx->Color[var][VSLICE]);
				vrml_vertical_slice_tick(dtx,ctx->Variable[var]->VSliceRequest->R2,
					ctx->Variable[var]->VSliceRequest->C2,ctx->Variable[var]->VSliceRequest->X2,
					ctx->Variable[var]->VSliceRequest->Y2,ctx->Variable[var]->VSliceRequest->Lat2,
					ctx->Variable[var]->VSliceRequest->Lon2,
					dtx->Color[var][VSLICE]);

				/*
				 * draw small markers at midpoint of top
				 * and bottom edges
				 */

				vert[0][0] = vert[1][0] = vert[2][0] =
				vert[3][0] = (ctx->Variable[var]->VSliceRequest->X1 +
						ctx->Variable[var]->VSliceRequest->X2) * 0.5;
				vert[0][1] = vert[1][1] = vert[2][1] =
				vert[3][1] = (ctx->Variable[var]->VSliceRequest->Y1 +
						ctx->Variable[var]->VSliceRequest->Y2) * 0.5;
				vert[0][2] = ztop+TICK_SIZE;
				vert[1][2] = ztop;
				vert[2][2] = zbot;
				vert[3][2] = zbot-TICK_SIZE;

				set_line_width(5); /* WLH 3-5-96 */

				/*
				 * Made color arg compatible with OpenGL
				 * 	don092596
				 */
				vrml_disjoint_polylines(vert,4,
					dtx->Color[var][VSLICE]);
				set_line_width(dtx->LineWidth); /* WLH 3-5-96 */
			}
		}
	}

	bl();fprintf(fp, " ]  # end children\n");
	bl();fprintf(fp, "} #End of Draw vertical contour slices\n");
}


/*
 * write all horizontal colored slices selected for display.
 * Input:  ctx - the context
 *         time - the timestep
 */
static void
vrml_chslices(
	Context		ctx,
	int		time
)
{
	int		var, alpha;
	Display_Context	dtx;

	bl();fprintf(fp, "# VIS5D colored horizontal slices\n");
	bl();fprintf(fp, "Transform {\n");

	pushLevel();
	bl();fprintf(fp, "children [\n");
	
	dtx = ctx->dpy_ctx;
	for (var=0;var<ctx->NumVars;var++) {
		if(ctx->DisplayCHSlice[var] &&
					 ctx->Variable[var]->CHSliceTable[time]->valid){
			wait_read_lock(&ctx->Variable[var]->CHSliceTable[time]->lock);

			recent( ctx, CHSLICE, var );

			alpha = get_alpha(dtx->ColorTable[VIS5D_CHSLICE_CT]->Colors[
				ctx->context_index * MAXVARS + var],255);

			vrml_colored_quadmesh(
				ctx->Variable[var]->CHSliceTable[time]->rows,
				ctx->Variable[var]->CHSliceTable[time]->columns,
				(void *)ctx->Variable[var]->CHSliceTable[time]->verts,
				ctx->Variable[var]->CHSliceTable[time]->color_indexes,
				dtx->ColorTable[VIS5D_CHSLICE_CT]->Colors[ctx->context_index *
								 MAXVARS + var],
				alpha);

			done_read_lock(&ctx->Variable[var]->CHSliceTable[time]->lock);

			/* draw position label */
		}
	}

	bl();fprintf(fp,"] # End of children of chslice Transform\n");
	popLevel();

	bl();fprintf(fp,
		"} # End of Draw horizontal colored slices Transform.\n");
}

/*
 * Draw all vertical colored slices selected for display.
 * Input:  ctx - the context
 *         time - the timestep
 */
static void
vrml_cvslices(
	Context		ctx,
	int		time
)
{
	int var, alpha;
	float r1, r2, c1, c2, l, x1, x2,y1,y2,z1,z2;
	float r1p, r2p, c1p, c2p, lp;
	Display_Context dtx;

	bl();fprintf(fp, "\n# VIS5D Vertical Colored Slices\n");
	bl();fprintf(fp, "#Draw vertical colored slices\n");
	bl();fprintf(fp, "Transform {\n");

	pushLevel();
	bl();fprintf(fp, "children [\n");

	dtx = ctx->dpy_ctx;
	for(var=0;var<ctx->NumVars;var++){
		if(ctx->DisplayCVSlice[var] &&
					ctx->Variable[var]->CVSliceTable[time]->valid){

			wait_read_lock(&ctx->Variable[var]->CVSliceTable[time]->lock);

			recent( ctx, CVSLICE, var );

			alpha = get_alpha(dtx->ColorTable[VIS5D_CVSLICE_CT]->Colors[
				ctx->context_index * MAXVARS + var], 255);

			vrml_colored_quadmesh(
				ctx->Variable[var]->CVSliceTable[time]->rows,
				ctx->Variable[var]->CVSliceTable[time]->columns,
				(void *)ctx->Variable[var]->CVSliceTable[time]->verts,
				ctx->Variable[var]->CVSliceTable[time]->color_indexes,
				dtx->ColorTable[VIS5D_CVSLICE_CT]->Colors[ctx->context_index * 
								MAXVARS + var],
				alpha);
			done_read_lock(&ctx->Variable[var]->CVSliceTable[time]->lock);

			if(dtx->DisplayBox && !dtx->CurvedBox) {
				/* draw position labels */
				float		zbot, ztop;
				float		vert[4][3];
				unsigned int	color;

				zbot = gridlevelPRIME_to_zPRIME(ctx->dpy_ctx,
						time,var,
						(float)ctx->dpy_ctx->LowLev);
				ztop = gridlevelPRIME_to_zPRIME(ctx->dpy_ctx,
						time,var,
						(float)(ctx->dpy_ctx->Nl - 1 +
							ctx->dpy_ctx->LowLev));
				color = dtx->Color[ctx->context_index *
							MAXVARS + var][CVSLICE];

				r1p = ctx->Variable[var]->CVSliceRequest->R1;
				c1p = ctx->Variable[var]->CVSliceRequest->C1;
				r2p = ctx->Variable[var]->CVSliceRequest->R2;
				c2p = ctx->Variable[var]->CVSliceRequest->C2;
				lp= 0.0;
				gridPRIME_to_xyzPRIME(ctx->dpy_ctx,time,var,1,
						&r1p,&c1p,&lp,&x1,&y1,&z1);
				gridPRIME_to_xyzPRIME(ctx->dpy_ctx,time,var,1,
						&r2p,&c2p,&lp,&x2,&y2,&z2);

				vrml_vertical_slice_tick(ctx->dpy_ctx,
						r1p,c1p,x1,y1,
						ctx->Variable[var]->CVSliceRequest->Lat1,
						ctx->Variable[var]->CVSliceRequest->Lon1,
						color);
				vrml_vertical_slice_tick(ctx->dpy_ctx,
						r2p,c2p,x2,y2,
						ctx->Variable[var]->CVSliceRequest->Lat2,
						ctx->Variable[var]->CVSliceRequest->Lon2,
						color);

				/*
				 * draw small markers at midpoint of
				 * top and bottom edges.
				 */
				vert[0][0] = vert[1][0] = vert[2][0] =
						vert[3][0] = (x1 + x2)*0.5;
				vert[0][1] = vert[1][1] = vert[2][1] =
						vert[3][1] = (y1 +y2)*0.5;
				vert[0][2] = ztop+TICK_SIZE;
				vert[1][2] = ztop;
				vert[2][2] = zbot;
				vert[3][2] = zbot-TICK_SIZE;
				vrml_disjoint_polylines(vert,4,color);
			}
		}
	}
	bl();fprintf(fp,"] # End of children of cvslice Transform\n");
	popLevel();
	bl();fprintf(fp, "} # End of VIS5D vertical Colored Slices\n");
}

/*
 * Write all horizontal stream vector slices which are selected for display.
 * Input:  ctx - the context
 *         time - the timestep
 */
void vrml_hstream_slices(
	Context	ctx,
	int	time
)
{
	Display_Context	dtx;
	int w, lock;

	bl();fprintf(fp, "\n#------------ Draw horizontal stream vector slices -----------\n");
	bl();fprintf(fp, "#Draw horizontal stream vector slices\n");
	bl();fprintf(fp, "Transform {\n");
	bl();fprintf(fp, " children [\n");
	
	dtx = ctx->dpy_ctx;	
	for (w=0;w<VIS5D_WIND_SLICES;w++) {

		if (dtx->DisplayHStream[w] && dtx->HStreamTable[w][time].valid){

		   wait_read_lock(&dtx->HStreamTable[w][time].lock);
		  
		   recent( ctx, HSTREAM, w );

		   /* draw the bounding box */
		   vrml_polylines_float(
		       dtx->HStreamTable[w][time].numboxverts,
                       (void *) dtx->HStreamTable[w][time].boxverts,
		       dtx->HStreamColor[w]);


		   /* draw main contour lines */
		   vrml_disjoint_lines(
		       dtx->HStreamTable[w][time].nlines,
		       (void *) dtx->HStreamTable[w][time].verts,
		       dtx->HStreamColor[w]);
		   
		   done_read_lock( &dtx->HStreamTable[w][time].lock );


		   /* draw position label */
		   if (dtx->DisplayBox && !dtx->CurvedBox) {
		      vrml_horizontal_slice_tick( dtx, dtx->HStreamLevel[w],
													 dtx->HStreamZ[w],
													 dtx->HStreamHgt[w],
													 dtx->HStreamColor[w]);
		   }
		}
	}

	bl();fprintf(fp, "] #End children\n");
	bl();fprintf(fp, "} #End of Draw horizontal stream vector slices.\n");
}

void vrml_vstream_slices(
	Context	ctx,
	int	time
)
{
	/* Added by Michael Boettinger, DKRZ 09/19/2000 */
	Display_Context	dtx;
	int w, lock;

	bl();fprintf(fp, "\n#------------ Draw vertical stream vector slices -----------\n");
	bl();fprintf(fp, "#Draw vertical stream vector slices\n");
	bl();fprintf(fp, "Transform {\n");
	bl();fprintf(fp, " children [\n");

	dtx = ctx->dpy_ctx;
	
	for (w=0;w<VIS5D_WIND_SLICES;w++) {

		if (dtx->DisplayVStream[w] && dtx->VStreamTable[w][time].valid){
		   
		   wait_read_lock(&dtx->VStreamTable[w][time].lock);

		   recent( ctx, VSTREAM, w );

		   /* draw the bounding box */
		   vrml_polylines_float(
		       dtx->VStreamTable[w][time].numboxverts,
                       (void *) dtx->VStreamTable[w][time].boxverts,
		       dtx->VStreamColor[w]);


		   /* draw main contour lines */
		   vrml_disjoint_lines(
		       dtx->VStreamTable[w][time].nlines,
		       (void *) dtx->VStreamTable[w][time].verts,
		       dtx->VStreamColor[w]);
		   
		   done_read_lock( &dtx->VStreamTable[w][time].lock );
		  

		   /* draw position label */

		   if (dtx->DisplayBox && !dtx->CurvedBox) {
		      /* position labels */
		      float zbot, ztop;
		      float vert[4][3];
		      zbot = gridlevel_to_z(ctx, time, ctx->Uvar[0],
		                            (float) ctx->Variable[ctx->Uvar[0]]->LowLev);
		      ztop = gridlevel_to_z(ctx, time, dtx->Uvar[0],
		                            (float) (ctx->Nl[ctx->Uvar[0]]-1
		                                     +ctx->Variable[dtx->Uvar[0]]->LowLev));
		      vrml_vertical_slice_tick( dtx, dtx->VStreamR1[w], dtx->VStreamC1[w],
		                                 dtx->VStreamX1[w], dtx->VStreamY1[w],
		                                 dtx->VStreamLat1[w], dtx->VStreamLon1[w],
		                                 dtx->VStreamColor[w]);
		      vrml_vertical_slice_tick( dtx, dtx->VStreamR2[w], dtx->VStreamC2[w],
		                                 dtx->VStreamX2[w], dtx->VStreamY2[w],
		                                 dtx->VStreamLat2[w], dtx->VStreamLon2[w],
		                                 dtx->VStreamColor[w]);
		      /* draw small markers at midpoint of top and bottom edges */
		      vert[0][0] = vert[1][0] = vert[2][0] = vert[3][0] =
		                            (dtx->VStreamX1[w] + dtx->VStreamX2[w]) * 0.5;
		      vert[0][1] = vert[1][1] = vert[2][1] = vert[3][1] =
		                            (dtx->VStreamY1[w] + dtx->VStreamY2[w]) * 0.5;
		      vert[0][2] = ztop+TICK_SIZE;
		      vert[1][2] = ztop;
		      vert[2][2] = zbot;
		      vert[3][2] = zbot-TICK_SIZE;
		      set_line_width(5); /* WLH 3-5-96 */
		 /* Made color arg compatible with OpenGL don,092596 */
		      vrml_disjoint_polylines( vert, 4, dtx->VStreamColor[w]);
		      set_line_width(dtx->LineWidth); /* WLH 3-5-96 */
		   }

		}
	}

	bl();fprintf(fp, "] #End children\n");
	bl();fprintf(fp, "} #End of Draw vertical stream vector slices.\n");
}

/*
 * Write the clock in the upper-left corner of the 3-D window.
 * Input:  ctx - the vis5d context
 *         c - the color to use.
 */
void vrml_clock(
	Context		ctx,
	unsigned int	c
)
{
	Display_Context	dtx;
	static char day[7][20] = {"Sunday", "Monday", "Tuesday", "Wednesday",
		                       "Thursday", "Friday", "Saturday" };
	static float twopi = 2.0 * 3.141592;
	short pp[8][2];
	float ang;
	char str[12];
	int i;
	float x, y, z;

	dtx = ctx->dpy_ctx;

	bl();fprintf(fp, "\n# VIS5D Clock\n\n");
	bl();fprintf(fp, "Transform {\n");
	pushLevel();
	bl();fprintf(fp, "children [\n");
	
	/* Draw the clock. */
	if (ctx->NumTimes)
		ang = (float) ctx->CurTime / (float) ctx->NumTimes;
	else
		ang = 0.0;

	pp[0][1] = 50;
	pp[0][0] = 50;
	pp[1][1] = 50 - 40 * cos(twopi * ang);
	pp[1][0] = 50 + 40 * sin(twopi * ang);
	pp[2][1] = pp[1][1] + 1;
	pp[2][0] = pp[1][0] + 1;
	pp[3][1] = pp[0][1] + 1;
	pp[3][0] = pp[0][0] + 1;
	pp[4][1] = pp[0][1] - 1;
	pp[4][0] = pp[0][0] + 1;
	pp[5][1] = pp[1][1] - 1;
	pp[5][0] = pp[1][0] + 1;

	vrml_polyline2d( pp, 6, c, dtx->WinWidth, dtx->WinHeight);

	i = ctx->TimeStamp[ctx->CurTime];
	strcpy(str,"");
	sprintf( str, "%02d:%02d:%02d", i/3600, (i/60)%60, i%60 );
	x = 2*(100-dtx->WinWidth/2)/(float)dtx->WinWidth-0.2;
	y = 2*(dtx->WinHeight/2 - (dtx->gfx[WINDOW_3D_FONT]->FontHeight+5))/(float)dtx->WinHeight+0.2;
	z = 0.0;
	vrml_text( x, y, z, c, str);

	sprintf( str, "%05d", v5dDaysToYYDDD( ctx->DayStamp[ctx->CurTime] ) );
	x = 2*(100-dtx->WinWidth/2)/(float)dtx->WinWidth-0.2;
	y = 2*(dtx->WinHeight/2 - (2*dtx->gfx[WINDOW_3D_FONT]->FontHeight+10))/(float)dtx->WinHeight+0.2;
	z = 0.0;
	vrml_text( x, y, z, c, str);

	sprintf( str, "%d of %d", ctx->CurTime+1, ctx->NumTimes );
	x = 2*(100-dtx->WinWidth/2)/(float)dtx->WinWidth-0.2;
	y = 2*(dtx->WinHeight/2 - (3*dtx->gfx[WINDOW_3D_FONT]->FontHeight+15))/(float)dtx->WinHeight+0.2;
	z = 0.0;
	vrml_text( x, y, z, c, str);

	if (ctx->NumTimes == 1 ||
		 ((ctx->Elapsed[ctx->NumTimes-1] - ctx->Elapsed[0])
		   / (ctx->NumTimes - 1)) < 48*3600 ) {
	  /* Print day of week */
	  x = 2*(100-dtx->WinWidth/2)/(float)dtx->WinWidth-0.2;
	  y = 2*(dtx->WinHeight/2 - (4*dtx->gfx[WINDOW_3D_FONT]->FontHeight+20))/(float)dtx->WinHeight+0.2;
	  z = 0.0;
	  vrml_text( x, y, z, c, day[ (ctx->DayStamp[ctx->CurTime]+0) % 7 ]);
	}
	bl();fprintf(fp, "] #End children.\n");
	popLevel();
	bl();fprintf(fp, "} #End of Draw Clock.\n");
}

void vrml_save(Display_Context dtx, const char *filename)
{
	Context	ctx;
	int yo, spandex;
	const char	*myname = "vrml_save";

	/* Open the output file. */

	fp = fopen(filename, "w");
	if (fp == (FILE *) NULL) {
		(void) fprintf(stderr, "Error: Opening \"%s\"\n", filename);
		return;
	}

	vrml_header();
	vrml_light();

	/*
	Output the top-level Transform node. All other objects
	beneath this will be Shapes, representing the various
	graphical objects produced by VIS5D.
	*/

	bl();fprintf(fp, "Transform { # %s\n", myname);

	pushLevel();
	bl();fprintf(fp, "children [ # %s\n", myname);

	fflush(fp);

	/* vrml_textplots - not done */

	/* vrml_hslices - MiB */
	for (yo= 0; yo < dtx->numofctxs; yo++){
		spandex = dtx->TimeStep[dtx->CurTime].owners[yo];
		ctx = dtx->ctxpointerarray[yo];
		if (check_for_valid_time(ctx, dtx->CurTime)){
			vrml_hslices(ctx,ctx->CurTime,dtx->ContnumFlag);
		}
	}
	fflush(fp);

	/* vrml_vslices - MiB */
	for (yo= 0; yo < dtx->numofctxs; yo++){
		spandex = dtx->TimeStep[dtx->CurTime].owners[yo];
		ctx = dtx->ctxpointerarray[yo];
		if (check_for_valid_time(ctx, dtx->CurTime)){
			vrml_vslices(ctx,ctx->CurTime,dtx->ContnumFlag);
		}
	}
	fflush(fp);

	for (yo= 0; yo < dtx->numofctxs; yo++){
		spandex = dtx->TimeStep[dtx->CurTime].owners[yo];
		ctx = dtx->ctxpointerarray[yo];
		if (check_for_valid_time(ctx, dtx->CurTime)){
			vrml_hwind_slices(ctx,ctx->CurTime);
		}
	}
	fflush(fp);

	for (yo= 0; yo < dtx->numofctxs; yo++){
		spandex = dtx->TimeStep[dtx->CurTime].owners[yo];
		ctx = dtx->ctxpointerarray[yo];
		if (check_for_valid_time(ctx, dtx->CurTime)){
			vrml_vwind_slices(ctx,ctx->CurTime);
		}
	}
	fflush(fp);

	/* vrml_hstream_slices - MiB */
	for (yo= 0; yo < dtx->numofctxs; yo++){
		spandex = dtx->TimeStep[dtx->CurTime].owners[yo];
		ctx = dtx->ctxpointerarray[yo];
		if (check_for_valid_time(ctx, dtx->CurTime)){
			vrml_hstream_slices(ctx,ctx->CurTime);
		}
	}
	fflush(fp);

	/* vrml_vstream_slices - MiB */
	for (yo= 0; yo < dtx->numofctxs; yo++){
		spandex = dtx->TimeStep[dtx->CurTime].owners[yo];
		ctx = dtx->ctxpointerarray[yo];
		if (check_for_valid_time(ctx, dtx->CurTime)){
			vrml_vstream_slices(ctx,ctx->CurTime);
		}
	}
	fflush(fp);

	if (dtx->topo->TopoFlag && dtx->topo->DisplayTopo) {
		vrml_topo( dtx, dtx->CurTime);
	}

	if (dtx->MapFlag && dtx->DisplayMap) {
		if(dtx->DisplaySfcMap)
			vrml_map2( dtx, dtx->CurTime, 0, dtx->DarkMapColor);
		else
			vrml_map2( dtx, dtx->CurTime, 1, dtx->LightMapColor);
	}

	for (yo= 0; yo < dtx->numofctxs; yo++){
		spandex = dtx->TimeStep[dtx->CurTime].owners[yo];
		ctx = dtx->ctxpointerarray[yo];
		if (check_for_valid_time(ctx, dtx->CurTime)){
			vrml_trajectories( ctx, ctx->CurTime);
		}
	}    
	fflush(fp);

	for (yo= 0; yo < dtx->numofctxs; yo++){
		spandex = dtx->TimeStep[dtx->CurTime].owners[yo];
		ctx = dtx->ctxpointerarray[yo];
		if (check_for_valid_time(ctx, dtx->CurTime)){
			vrml_isosurfaces(ctx,dtx->CurTime,ctx->CurTime);
		}
	}
	fflush(fp);

	for (yo= 0; yo < dtx->numofctxs; yo++){
		spandex = dtx->TimeStep[dtx->CurTime].owners[yo];
		ctx = dtx->ctxpointerarray[yo];
		if(check_for_valid_time(ctx,dtx->CurTime)){
			vrml_chslices(ctx,ctx->CurTime);
		}
	}
	fflush(fp);

	for(yo= 0; yo < dtx->numofctxs; yo++){
		spandex = dtx->TimeStep[dtx->CurTime].owners[yo];
		ctx = dtx->ctxpointerarray[yo];
		if(check_for_valid_time(ctx,dtx->CurTime)){
			vrml_cvslices(ctx,ctx->CurTime);
		}
	}

	if (dtx->DisplayBox) {
		vrml_box( dtx, dtx->CurTime);
	}


	if (dtx->DisplayClock) {
		 vrml_clock( ctx, dtx->BoxColor);
	}

	popLevel();
	bl();fprintf(fp, "] # End %s children\n", myname);

	popLevel();
	bl();fprintf(fp, "} # End %s Transform\n", myname);

	(void) fclose(fp);
}
