/* -----------------------------------------------------------------------
 * $Id: LXMach.h 1793 2013-02-24 02:19:06Z viari $
 * -----------------------------------------------------------------------
 * @file: LXMach.h
 * @desc: LX cpu targets & os specific flags
 * 
 * @history:
 * @+ <Gloup> : Jul 95 : MWC first draft
 * @+ <Gloup> : Jan 96 : adapted to Pwg
 * @+ <Gloup> : Nov 00 : adapted to Mac_OS_X
 * @+ <Gloup> : Jan 01 : adapted to LX_Ware
 * @+ <Wanou> : Dec 01 : last revision 
 * @+ <JeT>   : Jul 06 : Add Windows targets
 * -----------------------------------------------------------------------
 * @docstart: DEVDOC
 *
 * This header file should be included in all LX software <br>
 * It defines several OS and CPU specific flags.
 *
 * @par LX_TARGET
 * is usually defined in MACHINE.conf (included in Makefile)
 * thru the MACHDEF compiler flag.
 * Currently defined are :
 * - LX_TARGET_LINUX     ix86 Linux
 * - LX_TARGET_MACOSX    MacOS_X (PPC)
 * - LX_TARGET_MACINTEL  MacOX_X (Intel)
 * - LX_TARGET_WIN32     Windows 32
 * - LX_TARGET_SUNOS     SunOS (Pre-Solaris)
 * - LX_TARGET_SOLARIS   Sun Solaris
 * - LX_TARGET_SGI       SGI - Irix
 * - LX_TARGET_DEC       DEC - OSF1
 *
 * @par LX_OS
 * is defined from LX_TARGET using following rules :
 * - LX_TARGET_LINUX     UNIX    , LX_OS_UNIX , LX_OS_UNIX_BSD
 * - LX_TARGET_MACOSX    UNIX    , LX_OS_UNIX , LX_OS_UNIX_BSD 
 * - LX_TARGET_MACINTEL  UNIX    , LX_OS_UNIX , LX_OS_UNIX_BSD 
 * - LX_TARGET_WIN32     WINDOWS , LX_OS_WIN  , LX_OS_WIN32 
 * - LX_TARGET_SUNOS     UNIX    , LX_OS_UNIX , LX_OS_UNIX_BSD
 * - LX_TARGET_SOLARIS   UNIX    , LX_OS_UNIX , LX_OS_UNIX_S7
 * - LX_TARGET_SGI       UNIX    , LX_OS_UNIX , LX_OS_UNIX_S7
 * - LX_TARGET_DEC       UNIX    , LX_OS_UNIX , LX_OS_UNIX_OSF
 * <br> note: to override these default settings define LX_OS
 *
 * @par LX_CPU
 * is defined from LX_TARGET using following rules :
 * - LX_TARGET_LINUX     LX_CPU_X86
 * - LX_TARGET_MACOSX    LX_CPU_PPC
 * - LX_TARGET_MACINTEL  LX_CPU_X86
 * - LX_TARGET_WIN32     LX_CPU_X86
 * - LX_TARGET_SUNOS     LX_CPU_SPARC
 * - LX_TARGET_SOLARIS   LX_CPU_SPARC
 * - LX_TARGET_SGI       LX_CPU_MIPS
 * - LX_TARGET_DEC       LX_CPU_ALPHA
 * <br> note: to override these default settings define LX_CPU
 * 
 * @par LX_RT_ENDIAN
 * is defined from LX_CPU using following rules :
 * - LX_CPU_PPC         LX_RT_BIG_ENDIAN
 * - LX_CPU_SPARC       LX_RT_BIG_ENDIAN
 * - LX_CPU_MIPS        LX_RT_BIG_ENDIAN
 * - LX_CPU_ALPHA       LX_RT_LITTLE_ENDIAN
 * - LX_CPU_X86         LX_RT_LITTLE_ENDIAN
 *  <br> note: to override these default settings define LX_RT_ENDIAN
 *
 * @par LX_HAS_H
 * is defined from LX_OS using following rules :
 * - LX_HAS_UNISTD_H  = <unistd.h> if defined LX_OS_UNIX
 * - LX_HAS_GETOPT_H  = <getopt.h> if defined LX_OS_UNIX_S7
 *
 * @docend:
 */

#ifndef _H_LXMach

#define _H_LXMach

/* -------------------------------------------------------------------- */
/* define LX_OS and LX_CPU flags from LX_TARGET                         */
/*                                                                      */
/* exactly one of the LX_TARGET should be defined :                     */
/* (usually using MACHDEF in MACHINE.conf)                              */
/*                                                                      */
/* LX_TARGET           default LX_OS                 default LX_CPU     */
/* --------------      ----------------------------- ------------------ */
/* LX_TARGET_LINUX     LX_OS_UNIX  , LX_OS_UNIX_BSD   LX_CPU_X86        */
/* LX_TARGET_MACOSX    LX_OS_UNIX  , LX_OS_UNIX_BSD   LX_CPU_PPC        */
/* LX_TARGET_MACINTEL  LX_OS_UNIX  , LX_OS_UNIX_BSD   LX_CPU_X86        */
/* LX_TARGET_WIN32     LX_OS_WIN   , LX_OS_WIN32      LX_CPU_X86        */
/* LX_TARGET_SUNOS     LX_OS_UNIX  , LX_OS_UNIX_BSD   LX_CPU_SPARC      */
/* LX_TARGET_SOLARIS   LX_OS_UNIX  , LX_OS_UNIX_S7    LX_CPU_SPARC      */
/* LX_TARGET_SGI       LX_OS_UNIX  , LX_OS_UNIX_S7    LX_CPU_MIPS       */
/* LX_TARGET_DEC       LX_OS_UNIX  , LX_OS_UNIX_OSF   LX_CPU_ALPHA      */
/* -------------------------------------------------------------------- */
/* note: to override these default settings,                            */
/* define LX_OS and LX_CPU                                              */
/* -------------------------------------------------------------------- */

                                                /* PC / Linux           */      
#ifdef LX_TARGET_LINUX
#ifndef LX_OS
#define LX_OS "UNIX"
#define LX_OS_UNIX
#define LX_OS_UNIX_BSD
#endif
#ifndef LX_CPU
#define LX_CPU "X86"
#define LX_CPU_X86
#endif
#endif

                                                /* Macintosh OS-X PPC   */
#ifdef LX_TARGET_MACOSX
#ifndef LX_OS
#define LX_OS "UNIX"
#define LX_OS_UNIX
#define LX_OS_UNIX_BSD
#endif
#ifndef LX_CPU
#define LX_CPU "PPC"
#define LX_CPU_PPC
#endif
#endif

                                                /* Macintosh OS-X/INTEL  */
#ifdef LX_TARGET_MACINTEL
#ifndef LX_OS
#define LX_OS "UNIX"
#define LX_OS_UNIX
#define LX_OS_UNIX_BSD
#endif
#ifndef LX_CPU
#define LX_CPU "X86"
#define LX_CPU_X86
#endif
#endif

                                                /* Windows Win32        */
#ifdef LX_TARGET_WIN32
#ifndef LX_OS
#define LX_OS "WINDOWS"
#define LX_OS_WIN
#define LX_OS_WIN32
#endif
#ifndef LX_CPU
#define LX_CPU "X86"
#define LX_CPU_X86
#endif
#endif

                                                /* SunOS /  pre-Solaris */      
#ifdef LX_TARGET_SUNOS
#ifndef LX_OS
#define LX_OS "UNIX"
#define LX_OS_UNIX
#define LX_OS_UNIX_BSD
#endif
#ifndef LX_CPU
#define LX_CPU "SPARC"
#define LX_CPU_SPARC
#endif
#endif

                                                /* Solaris              */      
#ifdef LX_TARGET_SOLARIS
#ifndef LX_OS
#define LX_OS "UNIX"
#define LX_OS_UNIX
#define LX_OS_UNIX_S7
#endif
#ifndef LX_CPU
#define LX_CPU "SPARC"
#define LX_CPU_SPARC
#endif
#endif

                                                /* SGI / Irix           */      
#ifdef LX_TARGET_SGI
#ifndef LX_OS
#define LX_OS "UNIX"
#define LX_OS_UNIX
#define LX_OS_UNIX_S7
#endif
#ifndef LX_CPU
#define LX_CPU "MIPS"
#define LX_CPU_MIPS
#endif
#endif

                                                /* DEC / OSF            */      
#ifdef LX_TARGET_DEC
#ifndef LX_OS
#define LX_OS "UNIX"
#define LX_OS_UNIX
#define LX_OS_UNIX_OSF
#endif
#ifndef LX_CPU
#define LX_CPU "ALPHA"
#define LX_CPU_ALPHA
#endif
#endif

/* -------------------------------------------------------------------- */
/* check point : LX_OS and LX_CPU                                       */
/* -------------------------------------------------------------------- */

#ifndef LX_OS
#error LX_OS undefined
#endif

#ifndef LX_CPU
#error LX_CPU undefined
#endif

/* -------------------------------------------------------------------- */
/* define Run-Time Endian                                               */
/*                                                                      */
/* LX_CPU          default LX_RT                                        */
/* --------------  ---------------------------------------------------- */
/* LX_CPU_PPC      LX_RT_BIG_ENDIAN                                     */
/* LX_CPU_SPARC    LX_RT_BIG_ENDIAN                                     */
/* LX_CPU_MIPS     LX_RT_BIG_ENDIAN                                     */
/* LX_CPU_ALPHA    LX_RT_LITTLE_ENDIAN                                  */
/* LX_CPU_X86      LX_RT_LITTLE_ENDIAN                                  */
/* -------------------------------------------------------------------- */
/* note: to override these default settings,                            */
/* define LX_RT_ENDIAN                                                  */
/* -------------------------------------------------------------------- */

#ifndef LX_RT_ENDIAN

#ifdef LX_CPU_PPC
#define LX_RT_ENDIAN "BIG"
#define LX_RT_BIG_ENDIAN
#endif

#ifdef LX_CPU_SPARC
#define LX_RT_ENDIAN "BIG"
#define LX_RT_BIG_ENDIAN
#endif

#ifdef LX_CPU_MIPS
#define LX_RT_ENDIAN "BIG"
#define LX_RT_BIG_ENDIAN
#endif

#ifdef LX_CPU_ALPHA
#define LX_RT_ENDIAN "LITTLE"
#define LX_RT_LITTLE_ENDIAN
#endif

#ifdef LX_CPU_X86
#define LX_RT_ENDIAN "LITTLE"
#define LX_RT_LITTLE_ENDIAN
#endif

#endif

/* -------------------------------------------------------------------- */
/* check point : LX_RT_ENDIAN                                           */
/* -------------------------------------------------------------------- */

#ifndef LX_RT_ENDIAN
#error LX_RT_ENDIAN undefined
#endif

/* -------------------------------------------------------------------- */
/* standard Headers                                                     */
/*                                                                      */
/* LX_OS           default LX_HAS_H      value                          */
/* --------------  --------------------- ------------------------------ */
/* LX_OS_UNIX      LX_HAS_UNISTD_H       <unistd.h>                     */
/* LX_OS_UNIX_S7   LX_HAS_GETOPT_H       <getopt.h>                     */
/* -------------------------------------------------------------------- */

#ifdef LX_OS_UNIX
#define LX_HAS_UNISTD_H <unistd.h>
#endif

#ifdef LX_OS_UNIX_S7
#define LX_HAS_GETOPT_H <getopt.h>
#endif

#endif
