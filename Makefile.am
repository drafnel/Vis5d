#
# gettext support requires that subdir intl be first
#
SUBDIRS = po intl lui5 src util doc @GTK_DIRS@

EXTRA_DIST = PORTING COPYRIGHT hole.v5d contrib convert scripts \
             userfuncs pixmaps vis5d+.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = vis5d+.pc

#
# This target creates lib vis5dapi which contains only those functions 
# nessasary to access vis5d via the api 
#
API: 
	cd src; $(MAKE) API

# add a target to completely get rid of all the files produced by
# autoheader, automake, autoconf, and aclocal:
super-clean: maintainer-clean
	rm -f configure */Makefile.in Makefile.in INSTALL aclocal.m4 config.guess config.h.in config.sub install-sh ltconfig ltmain.sh missing mkinstalldirs stamp-h.in

