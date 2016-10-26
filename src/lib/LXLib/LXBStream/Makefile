# ---------------------------------------------------------------
# $Id: Makefile 1791 2013-02-24 02:17:56Z viari $
# ---------------------------------------------------------------
# @file: Makefile
# @desc: makefile for LXBStream library
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
	./test_file > test.bak
	$(DIFF) test.bak test_file.ref
	$(DIFF) test_little_endian.bak test_little_endian.in
	$(DIFF) test_big_endian.bak test_big_endian.in
	./test_memy > test.bak
	$(DIFF) test.bak test_memy.ref
