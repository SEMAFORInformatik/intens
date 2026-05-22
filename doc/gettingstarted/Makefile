# SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
# SPDX-License-Identifier: Apache-2.0
#
# export org to html
.PHONY: all clean install-doc

all: gettingstarted.html

gettingstarted.html: public gettingstarted.org
# C
# https://emacs.stackexchange.com/questions/79197/how-can-i-get-the-output-of-a-batch-emacs-run-to-a-custom-path
%.html: %.org
	./docker-org-export.sh org-html-export-to-html $<

public:
	[ -d public ] || mkdir public
	rsync -a styles images public/

clean:
	rm -fr public
