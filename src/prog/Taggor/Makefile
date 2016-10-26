# ---------------------------------------------------------------
# $Id: Makefile 1808 2013-02-25 09:59:29Z viari $
# ---------------------------------------------------------------
# @file: Makefile
# @desc: makefile for Taggor program
#
# @history:
# @+	    <Gloup> : Feb 02 : Created
#
# @note: should be processed with gnu compatible make
# @note: helixware_compatible
#
# @end:
# ---------------------------------------------------------------
#

VERSION = 2.2

DATDIR = ../../../data

#
# machine specific configuration file
#

include ../../../config/auto.conf

USER_LIBS =  -lPM -lLX

CFLAGS += -DVERSION=\"$(VERSION)\"

include ../../../config/makeprog.targ

testrun:
	./Taggor -a $(DATDIR)/aa_mono.ref -i test.pkl -o test_tag.bak
	sed -e 's/\.[0-9][0-9]*//g' test_tag.bak > test.bak
	$(DIFF) test.bak test_tag.ref
