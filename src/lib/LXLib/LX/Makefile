# ---------------------------------------------------------------
# $Id: Makefile 1807 2013-02-24 23:26:33Z viari $
# ---------------------------------------------------------------
# @file: Makefile
# @desc: makefile for LX library
#
# @history:
# @+	    <Gloup> : Jan 02 : Created
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

#
# Machine independant flags
#

OLIBS  =            \
      libLXSys.a	\
	  libLXFasta.a	\
	  libLXFastq.a	\
	  libLXRegexp.a \
	  libLXString.a	\
	  libLXArray.a	\
	  libLXStkp.a   \
	  libLXMath.a   \
	  libLXPat.a	\
	  libLXBio.a	\
	  libLXHeap.a   \
	  libLXList.a   \
	  libLXRand.a   \
	  libLXDict.a   \
      libLXUtils.a	\
	  libLXBStream.a   \
	  libLXZlib.a      \
	  libLXBase64.a    \
	  libz.a           \
	  libexpat.a       \
	  libLXXml.a
	 
LOCLIB = libLX.a

#
# Rules
#

all: port

extract:
	-for f in $(OLIBS) ; 			        \
	  do $(AR) $(ARXFLAGS) $(LIBDIR)/$$f ;	\
	done
	
lib: extract
	$(AR) $(ARFLAGS) $(LOCLIB) *.o
	$(RANLIB) $(LOCLIB)

lint:
	@echo "no lint for $(LOCLIB)"

port: lib
	-if test ! -d $(PRTDIR) ; then mkdir $(PRTDIR) ; fi
	-if test ! -d $(LIBDIR) ; then mkdir $(LIBDIR) ; fi
	-if test ! -d $(INDDIR) ; then mkdir $(INDDIR) ; fi
	-cp -f $(LOCLIB) $(LIBDIR)
	-$(RANLIB) $(LIBDIR)/$(LOCLIB)
	-(tar -cf - -C $(INCDIR) --exclude \.svn LX.h LX | tar -xf - -C $(INDDIR))

test:
	@echo "no test for $(LOCLIB)"

clean :
	-rm -f *.o $(LOCLIB) *SYMDEF* *SORTED* *% *.bak so_loc*

portclean: clean
	-rm -f $(LIBDIR)/$(LOCLIB)

superclean: clean portclean
	
rebuild: superclean all
	@echo "rebuild done"
