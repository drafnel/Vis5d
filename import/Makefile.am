bin_PROGRAMS = v5dimport

CPPFLAGS = @CPPFLAGS@ -I$(srcdir)/../src -I$(srcdir)/../lui5

V5D_LIB = ../src/libv5d.la

HDRS = analyze.h file.h grid.h gui.h memory.h misc.h model.h output.h proj.h projlist.h read_epa.h read_gr3d.h read_grads.h read_grid.h read_uwvis.h read_v5d.h resample.h select.h tokenize.h topo.h ui.h

v5dimport_SOURCES = analyze.c file.c grid.c gui.c main.c misc.c output.c \
	proj.c projlist.c read_epa.c read_gr3d.c read_grid.c read_grads.c \
	read_uwvis.c read_v5d.c resample.c select.c tokenize.c topo.c ui.c \
	memory.c $(HDRS)
EXTRA_v5dimport_SOURCES = kltwin.f
v5dimport_LDADD = $(V5D_LIB) @MCIDAS_LIBS@ ../lui5/liblui.a @XLIBS@

# hack to include kltwin.o when we have McIDAS (see ../src/Makefile.am):
if MCIDAS
read_grid.o: kltwin.o
endif

# what is all this *_i.[ch] stuff?
MISC_SRC = alpha.c areal.c julian2std.c mm4.c mm4date2std.c model.c netcdf.h netcdf2julian.c outputold.c proj2.c proj2_i.c projlist_i.c radm.c radm_emis.c radm_emis_i.c radm_i.c read_epa_i.c read_gr3d_i.c read_grads_i.c read_grid_i.c read_uwvis_i.c read_v5d_i.c resample_i.c rom.c rom_i.c select_i.c tokenize_i.c topo_i.c uam.c uam_i.c ui_i.c proj_i.h projlist_i.h read_epa_i.h read_gr3d_i.h read_grads_i.h read_grid_i.h read_uwvis_i.h read_v5d_i.h resample_i.h select_i.h tokenize_i.h topo_i.h ui_i.h

EXTRA_DIST = README $(MISC_SRC)
