/* -----------------------------------------------------------------------
 * $Id: LXMacros.h 1807 2013-02-24 23:26:33Z viari $
 * -----------------------------------------------------------------------
 * @file: LXMacros.h
 * @desc: LX standards macros
 *
 * @history:
 * @+ <Gloup> : Jul 95 : MWC first draft
 * @+ <Gloup> : Jan 96 : adapted to Pwg
 * @+ <Gloup> : Nov 00 : adapted to Mac_OS_X
 * @+ <Gloup> : Jan 01 : adapted to LX_Ware 
 * -----------------------------------------------------------------------
 * @docstart: DEVDOC
 *
 * This header file defines LX software standard macros
 *
 * @par Error reporting
 * - ERROR(message, level)
 * - WARNING(message)
 * - MEMORY_ERROR()
 * - IO_ERROR()
 * - FIO_ERROR(message)
 * - ARG_ERROR()
 *
 * @par Memory Allocation/Free
 * - NEW(type)
 * - NEWN(type, dim)
 * - REALLOC(pointer, type, dim)
 * - FREE(ptr)
 * - IFFREE(ptr)
 * - ZERO(ptr)
 * - ZERO_N(ptr, size)
 *
 * @par Math related
 * - ABS(x)
 * - MIN(x, y)
 * - MAX(x, y)
 *
 * @par Misc. utilities
 *
 * - IFF_RETURN(test, value)
 * - IFT_RETURN(test, value)
 *
 * @docend:
 */

#ifndef _H_LXMacros

#define _H_LXMacros

#ifndef _H_LXTypes
#include "LXTypes.h"
#endif

#ifndef _H_LXSys
#include "LXSys.h"
#endif

/* ==================================================== */
/* Error notification macros                            */
/* ==================================================== */

#define LX_ERROR(message, level)                        \
        LXError(__FILE__, __LINE__, message, level)

#define LX_WARNING(message)                             \
        LX_ERROR(message, LX_WARNING_ERROR)

#define MEMORY_ERROR()                                  \
        LX_ERROR("Not enough memory", LX_MEM_ERROR)

#define IO_ERROR()                                      \
        LXIOError(__FILE__, __LINE__, "IO error", LX_IO_ERROR)

#define FIO_ERROR(message)                              \
        LXIOError(__FILE__, __LINE__, message, LX_IO_ERROR)

#define ARG_ERROR()                                     \
        LX_ERROR("Invalid argument", LX_ARG_ERROR)

#define LX_DEBUG(level, fmt, args...)                   \
        LXDebug(level, fmt, ## args)

/* ==================================================== */
/* Memory related macros                                */
/* ==================================================== */

/* 
 * @doc: NEW(typ) allocate sizeof(typ) bytes
 */

#define NEW(typ) (typ*) LXMalloc(sizeof(typ)) 

/* 
 * @doc: NEW(typ) allocate (dim * sizeof(typ)) bytes
 */

#define NEWN(typ, dim) (typ*) LXMalloc((size_t)(dim) * sizeof(typ))

/* 
 * @doc: REALLOC(ptr, typ, dim) reallocate block pointed to
 *       by ptr to size (dim * sizeof(typ)) bytes 
 */

#define REALLOC(ptr, typ, dim)  (typ*) LXRealloc((void *) (ptr), \
                                       (size_t)(dim) * sizeof(typ))

/* 
 * @doc: ALLOCSIZE(ptr) size of allocation pointed to by ptr
 */
#define ALLOCSIZE(ptr) LXAllocSize(ptr)

/* 
 * @doc: FREE(ptr) free block pointed to by ptr
 */

#define FREE(ptr) LXFree(ptr)

/* 
 * @doc: IFFREE(ptr) conditional free block pointed to by ptr
 */

#define IFFREE(ptr) if (ptr) FREE(ptr)

/* 
 * @doc: ZERO(ptr) set block pointed to by ptr to 0
 */

#define ZERO(ptr)  (void) memset((ptr), 0, sizeof(*(ptr)))

/* 
 * @doc: ZERO_N(ptr) set block pointed to by ptr to 0 - size times
 */

#define ZERO_N(ptr, size)  (void) memset((ptr), 0, (size) * sizeof(*(ptr)))


/* ==================================================== */
/* Math related macros                                  */
/* ==================================================== */

/* 
 * @doc: ABS(x) absolute-value of x
 */

#ifndef ABS
#define ABS(x)  (((x) >= 0) ? (x) : -(x))
#endif

/* 
 * @doc: MAX(x,y) maximum of x and y
 */
#ifndef MAX
#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#endif

/* 
 * @doc: MIN(x,y) minimum of x and y
 */

#ifndef MIN
#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif

/* ==================================================== */
/* Misc. macros                                         */
/* ==================================================== */

/* 
 * @doc: IFF_RETURN(test, value) if (! test) return value
 */
 
#define IFF_RETURN(test, value) if (! (test)) return (value)

/* 
 * @doc: IFT_RETURN(test, value) if (test) return value
 */

#define IFT_RETURN(test, value) if (test) return (value)

/* ---------------------------------------------------- */
/* GCC version                                          */
/* ---------------------------------------------------- */

#define GCC_VERSION ((__GNUC__ * 100) + (__GNUC_MINOR__ * 10) + __GNUC_PATCHLEVEL__)


#endif
