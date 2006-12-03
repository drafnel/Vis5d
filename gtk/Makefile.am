## Process this file with automake to produce Makefile.in
#
#  Currently the vis5d gtk interface is built locally but not installed
#

INCLUDES = -I$(top_srcdir)/gtk/gradients \
	-I$(top_srcdir)/intl \
	@GTK_CFLAGS@

SUBDIRS=gradients

if ENABLE_GTK
bin_PROGRAMS = vis5dgtk
lib_LTLIBRARIES = libvis5dgtk.la
endif

EXTRA_DIST = vis5dgtk.pc.in vis5d+.glade vargraphicscontrols.glade \
             window3D.glade proceduredialog.glade textplotdialog.glade
	          

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = vis5dgtk.pc

BUILT_SOURCES = interface.c interface.h PD_interface.c PD_interface.h	\
support.c support.h tp_interface.c tp_interface.h VGC_interface.c	\
VGC_interface.h w3D_interface.c w3D_interface.h

libvis5dgtk_la_SOURCES = window3D.c window3D.h VarGraphicsControls.c	\
VarGraphicsControls.h support_cb.c support_cb.h procedure.c		\
procedure.h ProcedureDialog.c ProcedureDialog.h graph_labels.c		\
graph_labels.h textplot.c textplot.h $(BUILT_SOURCES)

if MAINTAINER_MODE

w3D_interface.c w3D_interface.h: window3D.glade
	$(GLADE) -w window3D.glade

interface.c interface.h support.c support.h: vis5d+.glade
	$(GLADE) -w vis5d+.glade

PD_interface.c PD_interface.h: proceduredialog.glade
	$(GLADE) -w proceduredialog.glade

tp_interface.c tp_interface.h: textplotdialog.glade
	$(GLADE) -w textplotdialog.glade

VGC_interface.c VGC_interface.h: vargraphicscontrols.glade
	$(GLADE) -w vargraphicscontrols.glade

maintainer-clean-local:
	rm -f $(BUILT_SOURCES)

endif

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
