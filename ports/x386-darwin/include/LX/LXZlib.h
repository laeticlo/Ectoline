/* -----------------------------------------------------------------------
 * $Id: LXZlib.h 1793 2013-02-24 02:19:06Z viari $
 * -----------------------------------------------------------------------
 * @file: LXZlib.h
 * @desc: Zlib compress utilities (zlib wrapper)
 * 
 * @history:
 * @+ <Gloup> : Jan 10 : first version
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * Zlib compress utilities (zlib wrapper)<br>
 * this library requires ThirdParty/Zlib
 *
 * @docend:
 */

#ifndef _H_LXZlib
#define _H_LXZlib

#include <stdio.h>

#include "LXSys.h"
#include "LXStkp.h"
#include "LXString.h"
#include "LXArray.h"
#include "LXBStream.h"

/* ---------------------------------------------------- */
/* Constants                                            */
/* ---------------------------------------------------- */

#define LX_Z_NO_COMPRESSION         0
#define LX_Z_BEST_SPEED             1
#define LX_Z_BEST_COMPRESSION       9
#define LX_Z_DEFAULT_COMPRESSION   -1

#define LX_ZLIB_INIT_ERROR         30
#define LX_ZLIB_COMPRESS_ERROR     31
#define LX_ZLIB_UNCOMPRESS_ERROR   32

#define LX_ZLIB_DFT_BUFSIZE        16384

/* ---------------------------------------------------- */
/* Structures                                           */
/* ---------------------------------------------------- */


/*
 * @doc: LXZlib stream
 * - zstream      : zlib internal z_stream*
 * - instream     : input byte stream 
 * - outstream    : input byte stream 
 * - bufsize      : default input/output buffer size
 * - inbuf        : - internal - current input buffer
 * - outbuf       : - internal - current output buffer
 */

typedef struct {
    void         *zstream;
    LXBStream    *instream;
    LXBStream    *outstream;
    size_t        bufsize;
    char         *inbuf;
    char         *outbuf;
} LXZStream;

/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

LXZStream *LXFreeZStream(LXZStream *zstream);

LXZStream *LXNewZStream(LXBStream *instream, LXBStream *outstream,
                        size_t bufsize);

int LXZCompress(LXZStream *zstream, int level);

int LXZUncompress(LXZStream *zstream);


#endif
