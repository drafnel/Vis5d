## Process this file with automake to produce Makefile.in

SGML_SRC = chap1.sgml  chap3.sgml  chap5.sgml  chap7.sgml  chap9.sgml   \
           chap2.sgml  chap4.sgml  chap6.sgml  chap8.sgml  vis5d.sgml

HTML_FILES = t1.html     x1413.html  x2426.html  x2698.html  x396.html \
             x1128.html  x1497.html  x2625.html  x35.html

$(HTML_FILES): html

EXTRA_DIST = vis5d.pdf $(HTML_FILES) $(SGML_SRC)

JADE = @JADE@
JADETEX = @JADETEX@
PDFJADETEX = @PDFJADETEX@
DVIPS = @DVIPS@
NWALSH = /usr/lib/sgml/stylesheet/dsssl/docbook/nwalsh

ps: vis5d.ps

pdf: vis5d.pdf

rtf: vis5d.rtf

vis5d.tex: $(SGML_SRC)
	$(JADE) -t tex -d $(NWALSH)/print/docbook.dsl vis5d.sgml

vis5d.dvi: vis5d.tex
	$(JADETEX) vis5d.tex

vis5d.ps: vis5d.dvi
	$(DVIPS) -Z vis5d.dvi -o vis5d.ps

vis5d.pdf: vis5d.tex
	$(PDFJADETEX) vis5d.tex

html: $(SGML_SRC)
	$(JADE) -t sgml -d $(NWALSH)/html/docbook.dsl vis5d.sgml

vis5d.rtf: $(SGML_SRC)
	$(JADE) -t rtf -d $(NWALSH)/print/docbook.dsl vis5d.sgml

clean-local:
	rm -f *.aux *.log

maintainer-clean-local:
	rm -f *.html *.dvi *.ps *.pdf *.rtf *.tex
