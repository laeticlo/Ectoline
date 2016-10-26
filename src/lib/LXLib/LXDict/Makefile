# ---------------------------------------------------------------
# $Id: Makefile 1791 2013-02-24 02:17:56Z viari $
# ---------------------------------------------------------------
# @file: Makefile
# @desc: makefile for LXDict library
#
# @history:
# @+	    <Gloup> : Aug 93 : Created
# @+	    <Gloup> : Feb 01 : Adapted to LXware
#
# @end:
# ---------------------------------------------------------------
#

#
# machine specific configuration file
#

include ../../../../config/auto.conf

USER_LIBS = -lLXArray -lLXSys

include ../../../../config/makelib.targ

#
# testrun target
#

testrun:
	./test_dict
	./test_dics > test.bak
	$(DIFF) test.bak test_dics.ref
