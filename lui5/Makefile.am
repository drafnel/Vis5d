HDRS = browser.h newbrowser.h button.h buttonmatrix.h colorbar.h destroy.h dialog.h field.h label.h layout.h list.h newlist.h newbutton.h newlabel.h newslider.h pixmaps.h popup.h radio.h scrollbar.h lui.h

SRC = browser.c newbrowser.c button.c buttonmatrix.c colorbar.c destroy.c event.c field.c label.c layout.c list.c newlist.c newbutton.c newlabel.c newslider.c pixmaps.c radio.c scrollbar.c lui.c

noinst_HEADERS = $(HDRS)
noinst_LIBRARIES = liblui.a

# when should browser2 not be included?  including it corresponds to
# using liblui2 instead of liblui in the old src/Makefile
liblui_a_SOURCES = $(SRC) browser2.c

# extra stuff to include in the dist but not to compile;
# (link with liblui.a and -lX11 to compile each of these)
EXTRA_DIST = README
