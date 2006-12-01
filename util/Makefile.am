CPROGS = v5dinfo v5dstats v5dedit v5dappend comp_to_v5d topoinfo
FPROGS = igg3d igu3d gg3d gr3d_to_v5d

bin_PROGRAMS = $(CPROGS) @OPT_UTILS@
noinst_PROGRAMS = maketopo makemap
EXTRA_PROGRAMS = $(FPROGS) listfonts fromxwd help newmap outlgrid

AM_CPPFLAGS = -I$(top_srcdir)/src

V5D_LIB = $(top_builddir)/src/libv5d.la

# note that if we don't specify prog_SOURCES, automake assumes prog.c

v5dinfo_LDADD = $(V5D_LIB)
v5dstats_LDADD = $(V5D_LIB)
v5dedit_LDADD = $(V5D_LIB)
v5dappend_LDADD = $(V5D_LIB)

comp_to_v5d_LDADD = $(V5D_LIB)
topoinfo_LDADD = $(V5D_LIB)

listfonts_LDADD = @GLLIBS@
fromxwd_LDADD = -limage

# various programs not normally built:
maketopo_LDADD = $(V5D_LIB)
makemap_LDADD = $(V5D_LIB)
newmap_SOURCES = newmap.c mapfunc.f
newmap_LDADD = $(V5D_LIB) $(FLIBS)
outlgrid_LDADD = $(V5D_LIB)

# McIDAS grid utilities: (only built when we have Fortran)

# All of these utilities share a few common source code files.  Note
# in particular that they all share a common main() routine in main.c.
# Because the main program is in C, we need (for some machines, at
# least) to link using the C linker and not Fortran's, hence the LINK
# specification below.
MAIN_SRC = main.c doubt.f igmk3d.f kludge.f sdest.f topo.f

igg3d_SOURCES = $(MAIN_SRC) igg3d.f
igg3d_LDADD = $(FLIBS)
igg3d_LINK = $(LINK)

igu3d_SOURCES = $(MAIN_SRC) igu3d.f
igu3d_LDADD = $(FLIBS)
igu3d_LINK = $(LINK)

gg3d_SOURCES = $(MAIN_SRC) gg3d.f
gg3d_LDADD = $(FLIBS)
gg3d_LINK = $(LINK)

gr3d_to_v5d_SOURCES = $(MAIN_SRC) gr3d_to_v5d.f get5d.f vis5d.h
gr3d_to_v5d_LDADD = $(V5D_LIB) $(FLIBS)
gr3d_to_v5d_LINK = $(LINK)

EXTRA_DIST = igmk3d.f vis_to_v5d.c
