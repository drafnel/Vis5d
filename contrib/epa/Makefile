#
# This is more like the shell of a Makefile
#

OBJECTS = alpha.o areal.o julian2std.o model.o mm4.o mm4date2std.o netcdf2julian.o radm.o radm_emis.o rom.o uam.o

# Do nothing - no executable to build.
default: $(OBJECTS)


clean:
	-rm *.o *~


######################################################################

# How to make a .o file from a .c file:
.c.o:
	$(CC) $(CFLAGS) -I$(V5D) -I../lui5 $<



#### EPA-provided code

alpha.o: alpha.c
	$(CC) $(CFLAGS) alpha.c

areal.o: areal.c
	$(CC) $(CFLAGS) areal.c

julian2std.o: julian2std.c
	$(CC) $(CFLAGS) julian2std.c

model.o: model.c model.h
	$(CC) $(CFLAGS) model.c

mm4.o: mm4.c
	$(CC) $(CFLAGS) mm4.c

mm4date2std.o: mm4date2std.c
	$(CC) $(CFLAGS) mm4date2std.c

netcdf2julian.o: netcdf2julian.c
	$(CC) $(CFLAGS) netcdf2julian.c

radm.o: radm.c
	$(CC) $(CFLAGS) radm.c

radm_emis.o: radm_emis.c
	$(CC) $(CFLAGS) radm_emis.c

rom.o: rom.c
	$(CC) $(CFLAGS) rom.c

uam.o: uam.c
	$(CC) $(CFLAGS) uam.c
