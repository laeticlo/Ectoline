# ---------------------------------------------------------------
# $Id: Makefile 1782 2013-02-23 00:11:55Z viari $
# ---------------------------------------------------------------
# @file: Makefile
# @desc: makefile for LXSys library
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

#
# machine specific configuration file
#

include ../../../../config/auto.conf

include ../../../../config/makelib.targ

#
# testrun target
#

testrun:
	./test_bin | grep -v '^@' > test.bak
	$(DIFF) test.bak test_bin.ref
	./test_tim > /dev/null 2>&1
	./test_mem > /dev/null 2>&1
	./test_fil > /dev/null 2>&1

