# ---------------------------------------------------------------
# $Id: Makefile 1810 2013-02-25 10:04:43Z viari $
# ---------------------------------------------------------------
# @file: Makefile
# @desc: makefile for PM library
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

#
# machine specific configuration file
#

include ../../../config/auto.conf

ROOTLIB = PM

LIBFIX  = PM

LDLIBS += -lLX

include ../../../config/makelib.targ

#
# testrun target
#

testrun:
	./test_aaa < tests/test_aaa.in > test.bak
	$(DIFF) test.bak tests/test_aaa.ref
	./test_enz < tests/test_enz.in > test.bak
	$(DIFF) test.bak tests/test_enz.ref
	./test_tag < tests/test_tag.in > test.bak
	$(DIFF) test.bak tests/test_tag.ref
	./test_sid < tests/test_sid.in > test.bak
	$(DIFF) test.bak tests/test_sid.ref
	./test_hit < tests/test_hit.in > test.bak
	$(DIFF) test.bak tests/test_hit.ref
	$(DIFF) test.asc.bak tests/test_hit.asc.ref

testclean:
	-rm -f test_tag.bin test_tag.asc

#
# final target
#
final:
	-(tar -cf - -C $(INCDIR) --exclude \.svn PM.h PM | tar -xf - -C $(INDDIR))
