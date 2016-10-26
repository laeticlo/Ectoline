# ---------------------------------------------------------------
# $Id: Makefile 1791 2013-02-24 02:17:56Z viari $
# ---------------------------------------------------------------
# @file: Makefile
# @desc: makefile for LXMath library
#
# @history:
# @+	    <Gloup> : Aug 06 : Created
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
	./test_dsta < test_dsta.in > test.bak
	$(DIFF) test.bak test_dsta.ref
	./test_fact > test.bak
	$(DIFF) test.bak test_fact.ref
	./test_mult > test.bak
	$(DIFF) test.bak test_mult.ref
	./test_hypr > test.bak
	$(DIFF) test.bak test_hypr.ref
