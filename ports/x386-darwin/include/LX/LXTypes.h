/* -----------------------------------------------------------------------
 * $Id: LXTypes.h 1771 2013-02-17 23:11:26Z viari $
 * -----------------------------------------------------------------------
 * @file: LXTypes.h
 * @desc: LX generic machine independant types
 *
 * @history:
 * @+ <Gloup> : Jul 95 : MWC first draft
 * @+ <Gloup> : Jan 96 : adapted to Pwg
 * @+ <Gloup> : Nov 00 : adapted to Mac_OS_X
 * @+ <Gloup> : Jan 01 : adapted to LX_Ware
 * @+ <JeT>   : Jul 06 : Add Windows targets
 * @+ <JeT>   : Jan 10 : real numbers added
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * This header file should be included in all LX software <br>
 * It defines generic types and values
 *
 * @par Generic types
 * - Int32  : 32 bits signed integer
 * - UInt32 : 32 bits unsigned integer
 * - Int16  : 16 bits signed integer
 * - UInt16 : 16 bits unsigned integer
 * - Int8   :  8 bits signed integer
 * - UInt8  :  8 bits unsigned integer 
 * - Bool   : boolean value
 * - Ptr    : pointer value
 *
 * @par Generic values
 * - TRUE
 * - FALSE
 * - NIL        : NULL
 * - BIG_INT32  : largest 32 bits signed integer
 * - BIG_INT16  : largest 16 bits signed integer
 * - BIG_INT8   : largest  8 bits signed integer
 * - BIG_UINT32 : largest 32 bits unsigned integer
 * - BIG_UINT16 : largest 16 bits unsigned integer
 * - BIG_UINT8  : largest  8 bits unsigned integer
 *
 * @docend:
 */

#ifndef _H_LXTypes

#define _H_LXTypes

/* -------------------------------------------------------------------- */
/* we need LXmach definitions                                           */
/* -------------------------------------------------------------------- */

#ifndef _H_LXMach
#include "LXMach.h"                     /* we need LXmach               */
#endif

/* -------------------------------------------------------------------- */
/* we need NULL to be properly defined                                  */
/* -------------------------------------------------------------------- */

#ifndef NULL
#include <stdio.h>                      /* the official NULL is here    */
#endif

/* -------------------------------------------------------------------- */
/* standard types                                                       */
/* are defined in sys/types for all Unix systems and compilers          */
/* supporting ISO C99 header <inttypes.h>                               */
/* we redefined for portability :                                       */
/* Int32, UInt32, Int16, UInt16, Int8, UInt8                            */
/* -------------------------------------------------------------------- */

#if defined(LX_OS_UNIX)                 /* --- Unix systems ----------- */

#include <sys/types.h>

#ifdef LX_OS_UNIX_BSD                   /* --- BSD based -------------- */
                                        /* --- standard types --------- */
typedef int64_t         Int64;          /* Int64  = 64 bits signe       */
typedef u_int64_t       UInt64;         /* UInt64 = 64 bits ~signe      */
typedef int32_t         Int32;          /* Int32  = 32 bits signe       */
typedef u_int32_t       UInt32;         /* UInt32 = 32 bits ~signe      */
typedef int16_t         Int16;          /* Int16  = 16 bits signe       */
typedef u_int16_t       UInt16;         /* UInt32 = 16 bits ~signe      */
typedef int8_t          Int8;           /* Int8   = 8 bits signe        */
typedef u_int8_t        UInt8;          /* UInt8  = 8 bits ~signe       */
typedef u_int8_t        Byte;           /* Bytes  = 8 bits ~signe       */
typedef float           Float32;        /* 32 bits real number          */
typedef double          Float64;        /* 64 bits real number          */

#else // LX_OS_UNIX_BSD                 /* --- Non BSD ---------------- */
                                        /* --- standard types --------- */
typedef int64_t         Int64;          /* Int64  = 64 bits signe       */
typedef uint64_t        UInt64;         /* UInt64 = 64 bits ~signe      */
typedef int32_t         Int32;          /* Int32  = 32 bits signe       */
typedef uint32_t        UInt32;         /* UInt32 = 32 bits ~signe      */
typedef int16_t         Int16;          /* Int16  = 16 bits signe       */
typedef uint16_t        UInt16;         /* UInt32 = 16 bits ~signe      */
typedef int8_t          Int8;           /* Int8   = 8 bits signe        */
typedef uint8_t         UInt8;          /* UInt8  = 8 bits ~signe       */
typedef uint8_t         Byte;           /* Bytes  = 8 bits ~signe       */
typedef float           Float32;        /* 32 bits real number          */
typedef double          Float64;        /* 64 bits real number          */

#endif // LX_OS_UNIX_BSD

#elif defined(LX_OS_WIN32)              /* --- Windows Win32 ---------- */

#include <stdint.h>
                                        /* --- standard types --------- */
typedef int64_t         Int64;          /* Int64  = 64 bits signe       */
typedef uint64_t        UInt64;         /* UInt64 = 64 bits ~signe      */
typedef int32_t         Int32;          /* Int32  = 32 bits signe       */
typedef uint32_t        UInt32;         /* UInt32 = 32 bits ~signe      */
typedef int16_t         Int16;          /* Int16  = 16 bits signe       */
typedef uint16_t        UInt16;         /* UInt32 = 16 bits ~signe      */
typedef int8_t          Int8;           /* Int8   = 8 bits signe        */
typedef uint8_t         UInt8;          /* UInt8  = 8 bits ~signe       */
typedef uint8_t         Byte;           /* Bytes  = 8 bits ~signe       */
typedef float           Float32;        /* 32 bits real number          */
typedef double          Float64;        /* 64 bits real number          */

#else // LX_OS_UNIX || LX_OS_WIN32

#error standard type not defined for non-unix system

#endif 

/* -------------------------------------------------------------------- */
/* optional types                                                       */
/* Bool, Ptr, String                                                    */
/* note : Ptr and String are not just alias to void* and char*          */
/*        they must refer to a piece of memory allocated thru           */
/*        LXMalloc or LXRealloc (or any corresponding macro in          */
/*        LXMacros.h)                                                   */
/* -------------------------------------------------------------------- */

                                        /* --- optional types --------- */
typedef int             Bool;           /* boolean  (int for ANSI)      */
typedef void            *Ptr;           /* dynamic pointer              */
typedef char            *String;        /* dynamic string               */

/* -------------------------------------------------------------------- */
/* standard values                                                      */
/* -------------------------------------------------------------------- */

#ifndef TRUE
#define TRUE     0x1                    /* bool values = TRUE           */
#endif

#ifndef FALSE
#define FALSE    0x0                    /* bool values = FALSE          */
#endif

#define NIL      NULL                   /* nil pointer                  */

#define BIG_INT32       0x7fffffff      /* plus grand 32 bits signe     */
#define BIG_INT16       0x7fff          /* plus grand 16 bits signe     */
#define BIG_INT8        0x7f            /* plus grand  8 bits signe     */
#define BIG_UINT32      0xffffffff      /* plus grand 32 bits ~signe    */
#define BIG_UINT16      0xffff          /* plus grand 16 bits ~signe    */
#define BIG_UINT8       0xff            /* plus grand  8 bits ~signe    */

#endif // _H_LXTypes
