#
#  Purpose: Automake makefile for Matlab standalone program
#
EXTRA_DIST=$(wildcard $(srcdir)/*.m)
BUILT_SOURCES=imsc_calc 
sysconf_DATA=$(BUILT_SOURCES)

imsc_calc: $(srcdir)/imsc_calc.m
	mcc -m $(srcdir)/imsc_calc.m $(srcdir)/json/parse_json.m $(srcdir)/json/mat2json.m 
