# ---------------------------------------------------------------
# $Id: Makefile 1810 2013-02-25 10:04:43Z viari $
# ---------------------------------------------------------------
# @file: Makefile
# @desc: makefile for PMMatch program
#
# @history:
# @+	    <Gloup> : Feb 02 : Created
# @+        <Wanou> : Mar 02 : PMMatch adapted
#
# @note: should be processed with gnu compatible make
# @note: helixware_compatible
#
# @end:
# ---------------------------------------------------------------
#

VERSION = 2.1

DATDIR = ../../../data

#
# machine specific configuration file
#

include ../../../config/auto.conf

PRGFIX    =  PM

USER_LIBS =  -lPM -lLX

CFLAGS += -DVERSION=\"$(VERSION)\"

include ../../../config/makeprog.targ

testrun:
	./PMMatch -a $(DATDIR)/aa_mono.ref -e $(DATDIR)/trypsin.ref -f test.fst -i test.pst -o test_match.bak
	sed -e 's/\.[0-9][0-9]*//g' test_match.bak > test.bak
	$(DIFF) test.bak test_match.ref
