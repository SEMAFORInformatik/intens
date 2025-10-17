.PHONY: all clean install-doc

all: gettingstarted.html

gettingstarted.html: gettingstarted.org init.el

%.html: %.org
	emacs $< --batch -l init.el -f org-html-export-to-html

clean:
	rm *.html
