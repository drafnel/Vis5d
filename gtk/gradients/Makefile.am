## Process this file with automake to produce Makefile.in

INCLUDES = -I../ -I../../\
	-I$(top_srcdir)/intl \
	@GTK_CFLAGS@

noinst_LTLIBRARIES = libgradients.la

libgradients_la_SOURCES = \
	interface.c interface.h \
	gradient_cb.c gradient_cb.h \
	gradient.c gradient.h \
	gradient_select.c gradient_select.h \
	datafiles.c     datafiles.h

