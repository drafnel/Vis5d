## Process this file with automake to produce Makefile.in

SGML_SRC = chap1.sgml  chap3.sgml  chap5.sgml  chap7.sgml  chap9.sgml   \
           chap2.sgml  chap4.sgml  chap6.sgml  chap8.sgml  vis5d.sgml

EXTRA_DIST = vis5d.pdf html $(SGML_SRC)

DB2DVI = @DB2DVI@
DB2HTML = @DB2HTML@
DB2PDF = @DB2PDF@
DB2PS = @DB2PS@
DB2RTF = @DB2RTF@

ps: vis5d.ps

dvi: vis5d.dvi

pdf: vis5d.pdf

rtf: vis5d.rtf

html: $(SGML_SRC)
	rm -rf html
	$(DB2HTML) vis5d.sgml
	mv vis5d html
	cd html && ln -s book1.html index.html

vis5d.dvi: $(SGML_SRC)
	$(DB2DVI) vis5d.sgml

vis5d.ps: $(SGML_SRC)
	$(DB2PS) vis5d.sgml

vis5d.pdf: $(SGML_SRC)
	$(DB2PDF) vis5d.sgml

vis5d.rtf: $(SGML_SRC)
	$(DB2RTF) vis5d.sgml

clean-local:
	rm -f *.aux *.log

maintainer-clean-local:
	rm -f *.dvi *.ps *.pdf *.rtf *.tex
	rm -rf html
