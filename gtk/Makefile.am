## Process this file with automake to produce Makefile.in
#
#  Currently the vis5d gtk interface is built locally but not installed
#

INCLUDES = -I$(top_srcdir)/gtk/gradients \
	-I$(top_srcdir)/intl \
	@GTK_CFLAGS@

SUBDIRS=gradients

bin_PROGRAMS = vis5dgtk

lib_LTLIBRARIES = libvis5dgtk.la

EXTRA_DIST = vis5dgtk.pc.in vis5d+.glade vargraphicscontrols.glade \
             window3D.glade proceduredialog.glade textplotdialog.glade
	          

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = vis5dgtk.pc

libvis5dgtk_la_SOURCES = window3D.c window3D.h \
                       w3D_interface.c w3D_interface.h \
                       VarGraphicsControls.c  VGC_interface.c \
                       VarGraphicsControls.h  VGC_interface.h \
							  support.c support.h \
	                    support_cb.c support_cb.h \
                       interface.c interface.h \
	                    procedure.c procedure.h \
                       PD_interface.c	PD_interface.h	\
                       ProcedureDialog.c  ProcedureDialog.h \
	                    graph_labels.c  graph_labels.h \
	                    tp_interface.c  tp_interface.h \
	                    textplot.c	   textplot.h 

libvis5dgtk_la_LDFLAGS = -version-info @SHARED_VERSION_INFO@

vis5dgtk_LDADD = libvis5dgtk.la ../src/libvis5d.la ../src/libv5d.la \
	           gradients/libgradients.la \
              @MCIDAS_LIBS@ @V5D_LIBS_AUX@ \
              @GLLIBS@ @XLIBS@ @THREADLIBS@ \
              @GTK_LIBS@ @GTKGL_LIBS@ $(INTLLIBS) 

if MIXKIT
vis5dgtk_SOURCES = main.c dummy.C
else
vis5dgtk_SOURCES = main.c
endif
