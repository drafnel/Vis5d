# eventually, build shared libraries too, once I figure out what files
# would go into it.

bin_PROGRAMS = vis5d
pkgdata_DATA = EARTH.TOPO OUTLSUPW OUTLUSAM
pkginclude_HEADERS = api.h api-config.h v5d.h binio.h v5df.h
lib_LTLIBRARIES = libv5d.la libvis5d.la

CPPFLAGS = @CPPFLAGS@ # -I flags

API_SRC = api.c analysis.c anim.c box.c chrono.c compute.c contour.c \
          groupchrono.c globals.c graphics.all.c grid.c image.c imemory.c \
          map.c matrix.c linterp.c memory.c misc.c mwmborder.c pipe.c proj.c \
          queue.c render.c rgb.c record.c save.c socketio.c stream.c \
          sounding.c sync.c tclsave.c textplot.c topo.c traj.c user_data.c \
          volume.c vtmcP.c work.c

SRC_I = analyze_i.c file_i.c grid_i.c \
        main_i.c misc_i.c output_i.c proj_i.c projlist_i.c read_epa_i.c \
        read_gr3d_i.c read_grid_i.c read_grads_i.c read_uwvis_i.c read_v5d_i.c\
        resample_i.c select_i.c tokenize_i.c topo_i.c ui_i.c memory_i.c \
        imain.c igui.c iapi.c file.c irregular_v5d.c

GUI_SRC = cursor.c displaywidget.c gui.c gui_i.c labels.c script.c slice.c \
          soundingGUI.c rgbsliders.c isocolor.c uvwwidget.c

OTHER_API = analysis.o anim.o api.o box.o chrono.o compute.o contour.o \
            groupchrono.o globals.o graphics.all.o grid.o image.o imemory.o \
            map.o matrix.o memory.o misc.o mwmborder.o pipe.o proj.o queue.o \
            record.o render.o rgb.o save.o socketio.o stream.o sounding.o \
            sync.o tclsave.o textplot.o topo.o traj.o user_data.o volume.o \
            vtmcP.o work.o

# when are these not used?
AUXSRC = gl_to_ppm.c graphics.ogl.c graphics.gl.c graphics.pex.c xdump.c

HEADER_SRC = analysis.h analyze_i.h anim.h box.h cb.h chrono.h compute.h contour.h cursor.h displaywidget.h etableP.h file.h file_i.h fsl.h gl_to_ppm.h globals.h graphics.h grid.h grid_i.h groupchrono.h gui.h gui_i.h iapi.h igui.h image.h imain.h imemory.h irregular_api.h irregular_v5d.h isocolor.h labels.h linterp.h main_i.h map.h matrix.h memory.h memory_i.h misc.h misc_i.h model_i.h mwmborder.h netcdf.h netcdf_i.h output_i.h pipe.h proj.h proj_i.h projlist_i.h queue.h read_epa_i.h read_gr3d_i.h read_grads_i.h read_grid_i.h read_uwvis_i.h read_v5d_i.h record.h render.h resample_i.h rgb.h rgbsliders.h save.h script.h select_i.h slice.h socketio.h sounding.h soundingGUI.h stream.h sync.h tclsave.h textplot.h tokenize_i.h topo.h topo_i.h traj.h ui_i.h user_data.h uvwwidget.h vertplot.h vis5d.h volume.h vtmcP.h work.h xdump.h

libv5d_la_SOURCES = v5d.c binio.c v5d.h binio.h v5df.h
libv5d_la_LDFLAGS = -version-info @SHARED_VERSION_INFO@

libvis5d_la_SOURCES = $(API_SRC) $(SRC_I) $(OTHER_API) $(AUXSRC) api.h
libvis5d_la_LDFLAGS = -version-info @SHARED_VERSION_INFO@

vis5d_SOURCES = $(GUI_SRC) main.c $(HEADER_SRC)
EXTRA_vis5d_SOURCES = kltwin.f   # conditionally-included source files
vis5d_LDADD = libvis5d.la ../lui5/liblui.a libv5d.la \
              @MCIDAS_LIBS@ @V5D_LIBS_AUX@ \
              @GLLIBS@ @XLIBS@ @THREADLIBS@

# vis5d should depend on kltwin.o when building with McIDAS.  The
# following is an automake conditional statement to accomplish that
# via a dependency of image.o (which vis5d depends on) (see the
# AM_CONDITIONAL macro in configure.in) (I would have preferred
# something cleaner, but I haven't been able to get automake to grok
# what we need to do in any other way):
if MCIDAS
image.o: kltwin.o
endif

# other stuff...what are these files for?
MISC_SRC = alpha_i.c areal_i.c julian2std_i.c model_i.c mm4_i.c mm4date2std_i.c netcdf2julian_i.c radm_i.c radm_emis_i.c rom_i.c uam_i.c

# Files which aren't involved in the compilation, but we need to make sure
# they are included in the distribution for completeness:
EXTRA_DIST = $(MISC_SRC) g.pex.c extmain.c libmcidas5.a EARTH.TOPO OUTLSUPW OUTLUSAM