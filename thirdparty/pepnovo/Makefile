# ---------------------------------------------------------------
# $Id: $
# ---------------------------------------------------------------
# @file: Makefile
# @desc: makefile for pepnovo
#
# @note: should be processed with gnu compatible make
# @note: helixware_compatible
#
# @end:
# ---------------------------------------------------------------
#

PNDIR = pepnovo.3.1b/src

include ../../config/auto.conf
#
# default targets
#

.PHONY: all port prog clean portclean test

all: port

port: prog
	if test ! -d $(PRTDIR) ; then mkdir $(PRTDIR) ; fi
	if test ! -d $(BINDIR) ; then mkdir $(BINDIR) ; fi
	cp -f $(PNDIR)/PepNovo_bin $(BINDIR)
	
prog:
	cd $(PNDIR) && make

test:
	@echo "no test yet"

clean:
	cd $(PNDIR) && make clean

portclean: clean
	-rm -f $(BINDIR)/PepNovo_bin

# help

include ../../config/helpbase.targ

