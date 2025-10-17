Intens System Administration Manual
===============================================

The following steps build the file sysadmin.pdf:

* create the EBNF diagrams in EPS format:

  ./yacc2eps.py ../../src/parser/parseryacc.yy diagrams

* convert the EPS files into latex figures:

  ./eps2tex.py diagrams

* create the manual in PDF

  latexmk sysadmin

The docker image hub.semafor.ch/semafor/intens/ebnf-latex
includes the required packages:

  docker run --rm -it -v $(pwd)/../..:/work -u $UID:$GID \
     hub.semafor.ch/semafor/intens/ebnf-latex:latest make

(see build.sh)
