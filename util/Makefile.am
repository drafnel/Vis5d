CPROGS = v5dinfo v5dstats v5dedit v5dappend comp_to_v5d topoinfo
FPROGS = igg3d igu3d gg3d gr3d_to_v5d

bin_PROGRAMS = $(CPROGS) @OPT_UTILS@
EXTRA_PROGRAMS = $(FPROGS) listfonts fromxwd \
                 help maketopo makemap newmap outlgrid

CPPFLAGS = @CPPFLAGS@ -I$(srcdir)/../src

V5D_LIB = ../src/libvis5d.la

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

noinst_LIBRARIES = @OPT_LIBMAIN@
EXTRA_LIBRARIES = libmain.a
libmain_a_SOURCES = doubt.f igmk3d.f kludge.f main.c sdest.f topo.f

igg3d_SOURCES = igg3d.f
igg3d_LDADD = libmain.a $(FLIBS)

igu3d_SOURCES = igu3d.f
igu3d_LDADD = libmain.a $(FLIBS)

gg3d_SOURCES = gg3d.f
gg3d_LDADD = libmain.a $(FLIBS)

gr3d_to_v5d_SOURCES = gr3d_to_v5d.f get5d.f vis5d.h
gr3d_to_v5d_LDADD = libmain.a $(V5D_LIB) $(FLIBS)

EXTRA_DIST = igmk3d.f vis_to_v5d.c
