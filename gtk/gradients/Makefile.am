## Process this file with automake to produce Makefile.in

INCLUDES = -I$(top_srcdir)/gtk/gradients -I$(top_srcdir)/gtk \
           -I$(top_srcdir) -I$(top_srcdir)/intl \
	        @GTK_CFLAGS@

if ENABLE_GTK
lib_LTLIBRARIES = libgradients.la
endif

EXTRA_DIST = gradients.glade

libgradients_la_SOURCES = \
	interface.c interface.h \
	gradient_cb.c gradient_cb.h \
	gradient.c gradient.h \
	gradient_select.c gradient_select.h \
	datafiles.c     datafiles.h \
   gradientP.h   gradient_header.h

