# ---------------------------------------------------------------
# $Id: Makefile 1791 2013-02-24 02:17:56Z viari $
# ---------------------------------------------------------------
# @file: Makefile
# @desc: makefile for LXRegexp library
#
# @history:
# @+	    <Gloup> : Jan 96 : Created
# @+	    <Gloup> : Jul 93 : revised version
# @+	    <Gloup> : Feb 01 : Adapted to LXware
# @+	    <Gloup> : Mar 01 : Corrected <if> bug
#
# @note: should be processed with gnu compatible make
# @note: helixware_compatible
#
# @end:
# ---------------------------------------------------------------
#

include ../../../../config/auto.conf

USER_LIBS = -lLXSys

include ../../../../config/makelib.targ

#
# testrun target
#
testrun:
	./test_regexp > test.bak
	$(DIFF) test.ref test.bak

