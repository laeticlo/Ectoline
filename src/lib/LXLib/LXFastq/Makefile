# ---------------------------------------------------------------
# $Id: Makefile 890 2010-03-12 08:49:59Z viari $
# ---------------------------------------------------------------
# @file: Makefile
# @desc: makefile for LXFastq library
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

USER_LIBS = -lLXString -lLXSys

include ../../../../config/makelib.targ

#
# testrun target
#
testrun:
	./test_fastq sanger < sanger.fastq > test.bak
	$(DIFF) test.bak sanger.fastq
	./test_fastq illumina < illumina.fastq > test.bak
	$(DIFF) test.bak sanger.fastq
	./test_fastq solexa < solexa.fastq > test.bak
	$(DIFF) test.bak sanger.fastq
