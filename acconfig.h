/* Here we define the default values for any preprocessor symbols used
   by autoconf but not known by autoheader.  This file is used to
   create config.h.in by running autoheader.  In most cases, they will
   default to being undefined, which is specified here by #undef.
   Remember, the actual values of these symbols that we use will be
   set by the configure script, and output into config.h. */

#ifndef CONFIG_H
#define CONFIG_H 1
@TOP@

/* define if we have a stupid (AIX) pthreads implementation that uses
   PTHREAD_CREATE_UNDETACHED instead of the standard
   PTHREAD_CREATE_JOINABLE */
#undef HAVE_PTHREAD_CREATE_UNDETACHED

/* define if we have POSIX threads */
#undef HAVE_PTHREADS

/* define if we have SunOS threads */
#undef HAVE_SUNOS_THREADS

/* define if we have SGI sproc forking */
#undef HAVE_SGI_SPROC

#ifndef API_CONFIG_H /* protect against multiple definitions;
			c.f. src/api-config.h.in */

/* define if we have OpenGL or Mesa */
#undef HAVE_OPENGL

/* define if we have SGI IrisGL */
#undef HAVE_SGI_GL

#endif /*API_CONFIG_H*/

/* define if we have PEX */
#undef HAVE_PEX

/* define if we have Tcl */
#undef HAVE_LIBTCL

/* define if we have NetCDF */
#undef HAVE_LIBNETCDF

/* define if we have Fortran idate function: */
#undef HAVE_IDATE

/* define if we have the McIDAS library */
#undef HAVE_MCIDAS

/* define if we are linking to a newer version of the McIDAS library
   than the one distributed with Vis5d: */
#undef MCIDAS_SIDECAR

/* define to specify if we include <GL/gl.h> or <gl/gl.h>: */
#undef HAVE_GL_gl_H
#undef HAVE_gl_gl_H

/* define if we are single-threading: */
#undef SINGLE_TASK

/* define to installation location of data files (e.g. EARTH.TOPO);
   typically /usr/local/share/vis5d */
#undef DATA_PREFIX

/* define to specify how to mangle identifiers for the Fortran linker: */
#define FORTRANIZE_LOWERCASE 0
#define FORTRANIZE_UPPERCASE 0
#define FORTRANIZE_LOWERCASE_UNDERSCORE 1 /* the default */
#define FORTRANIZE_UPPERCASE_UNDERSCORE 0
#define FORTRANIZE_EXTRA_UNDERSCORE 0

/* Define a macro F77_FUNC(x,X) to convert an identifier x (or its
   uppercase version X) to the format required by the Fortran linker.
   This assumes that x itself does not contain any underscores
   (otherwise, on some systems, an extra underscore needs to be
   appended). */

#if FORTRANIZE_LOWERCASE
#  define F77_FUNC(x,X) x
#elif FORTRANIZE_UPPERCASE
#  define F77_FUNC(x,X) X
#elif FORTRANIZE_LOWERCASE_UNDERSCORE
#  define F77_FUNC(x,X) x##_
#elif FORTRANIZE_UPPERCASE_UNDERSCORE
#  define F77_FUNC(x,X) X##_
#endif

@BOTTOM@
#endif /* CONFIG_H */
