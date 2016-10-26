# ---------------------------------------------------------------
# $Id: Makefile 1791 2013-02-24 02:17:56Z viari $
# ---------------------------------------------------------------
# @file: Makefile
# @desc: makefile for LXList library
#
# @history:
# @+	    <Gloup> : Jan 96 : Created
# @+	    <Gloup> : Jul 93 : revised version
# @+	    <Gloup> : Feb 01 : Adapted to LXware
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

USER_LIBS = -lLXStkp -lLXSys

include ../../../../config/makelib.targ

#
# testrun target
#
testrun:
	./test_list | sed -e 's/0x.*//g' > test.bak
	$(DIFF) test.bak test.ref
