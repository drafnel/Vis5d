## Process this file with automake to produce Makefile.in
#
#  Currently the vis5d gtk interface is built locally but not installed
#

INCLUDES = \
	-I$(top_srcdir)/intl \
	@GTK_CFLAGS@

noinst_PROGRAMS = vis5d

noinst_LTLIBRARIES = libv5dgtk.la

libv5dgtk_la_SOURCES = support.c support.h \
                       interface.c interface.h \
	                    callbacks.h \
                       window3D.c  glarea.c \
                       fileselection.c fontselectiondialog.c \
                       colorselectiondialog.c

libv5dgtk_la_LDFLAGS = -version-info @SHARED_VERSION_INFO@

vis5d_SOURCES = \
	main.c 


vis5d_LDADD = libv5dgtk.la ../src/libvis5d.la ../src/libv5d.la \
              @MCIDAS_LIBS@ @V5D_LIBS_AUX@ \
              @GLLIBS@ @XLIBS@ @THREADLIBS@ \
               @GTK_LIBS@ -lgtkgl $(INTLLIBS) 

