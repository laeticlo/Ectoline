# ---------------------------------------------------------------
# $Id: Makefile 1810 2013-02-25 10:04:43Z viari $
# ---------------------------------------------------------------
# @file: Makefile
# @desc: makefile for PMTrans program
#
# @history:
# @+	    <Gloup> : Feb 02 : Created
# @+        <Wanou> : Mar 02 : PMTrans adapted
#
# @note: should be processed with gnu compatible make
# @note: helixware_compatible
#
# @end:
# ---------------------------------------------------------------
#

VERSION = 2.0

#
# machine specific configuration file
#

include ../../../config/auto.conf

USER_LIBS =  -lPM -lLX

CFLAGS += -DVERSION=\"$(VERSION)\"

include ../../../config/makeprog.targ

testrun:
	./PMTrans -c 0 -i test.fst > test.bak
	-$(DIFF) test.bak test_trans.ref
