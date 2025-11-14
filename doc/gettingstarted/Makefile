.PHONY: all clean install-doc

all: gettingstarted.html

gettingstarted.html: public public gettingstarted.org init.el

%.html: %.org
	./docker-org-export.sh org-html-export-to-html $<

public:
	[ -d public ] || mkdir public
	rsync -a styles images public/

clean:
	rm -fr public
