# $Id: Makefile,v 1.19 2008/01/18 09:46:36 bas Exp $

SRCS:=sysadmin.tex config/packages.tex $(wildcard content/*.tex)

DOCS=sysadmin.pdf
FIGS:=$(wildcard xfig/*.fig)
IMGS:=$(wildcard images/*.png)
#EXAMPLES:=$(wildcard examples/navigator/*.png examples/messageQueue/*.png)

.PHONY: all clean

all: $(DOCS)

clean:
	$(RM) diagrams/*
	latexmk -c sysadmin.tex

diagrams/description.tex: eps2tex.py yacc2eps.py addendum.yy ../../src/parser/parseryacc.yy
	./yacc2eps.py ../../src/parser/parseryacc.yy diagrams
	./eps2tex.py diagrams

sysadmin.pdf: $(SRCS) $(IMGS) $(FIGS:.fig=.pdf) diagrams/description.tex
	latexmk -lualatex sysadmin.tex

xfig/%.pdf: xfig/%.fig
	fig2dev -L pdf -p 0 $< >$@
###	fig2dev -L eps $< | epstopdf --filter --outfile=$@


HTMLOPTS=-split +4 -show_section_numbers -long_titles 1 -local_icons -html_version 4.0,frame

sysadmin.html: sysadmin.idx
	perl pl/pre_html_merge-includes.pl
	./pl/latex2html $(HTMLOPTS) sysadmin_html.tex
html: sysadmin.idx
	perl pl/pre_html_merge-includes.pl
	./pl/latex2html $(HTMLOPTS) sysadmin_html.tex

fig-clean:
	$(RM) $(PDFS) $(EPS) $(PICS) $(DIAGRAMME)
