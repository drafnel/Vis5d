#ifndef	_sgi_driver_h_
#define	_sgi_driver_h_

/*
 *      $Id: sgidump.h,v 1.1 2001/10/09 19:22:21 jedwards Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1997			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		sgidump.h
 *
 *	Author:		Don Middleton
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Mon Jul 14 20:14:57 MDT 1997
 *
 *	Description:	Provides *working* windowdump code for
 *                      SGI OS6.X and later.
 */
#include <GL/gl.h>
#include <X11/Xlib.h>

extern void	SGI_Dump(
	Display	*display,
	int	scr,
	Window	window,
	FILE	*out,
	GLenum	oglbuf
);

#endif
