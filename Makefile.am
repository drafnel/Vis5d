SUBDIRS = lui5 src util import
EXTRA_DIST = PORTING COPYRIGHT hole.v5d contrib convert scripts userfuncs

# add a target to completely get rid of all the files produced by
# autoheader, automake, autoconf, and aclocal:
super-clean: maintainer-clean
	rm -f configure */Makefile.in Makefile.in INSTALL aclocal.m4 config.guess config.h.in config.sub install-sh ltconfig ltmain.sh missing mkinstalldirs stamp-h.in
