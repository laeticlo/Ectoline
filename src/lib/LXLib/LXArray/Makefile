# ---------------------------------------------------------------
# $Id: Makefile 1791 2013-02-24 02:17:56Z viari $
# ---------------------------------------------------------------
# @file: Makefile
# @desc: makefile for LXArray (dynamic array) library
#
# @history:
#   @+ <Gloup> : Jan 10 : first version
#
# @note: should be processed with gnu compatible make
# @note: helixware_compatible
#
# @end:
# ---------------------------------------------------------------
#

#
# machine specific configuration file
#

include ../../../../config/auto.conf

USER_LIBS = -lLXSys

include ../../../../config/makelib.targ

#
# testrun target
#

testrun:
	./test_dbuf > test.bak
	$(DIFF) test.bak test_dbuf.ref
	./test_2dpt > test.bak
	$(DIFF) test.bak test_2dpt.ref
