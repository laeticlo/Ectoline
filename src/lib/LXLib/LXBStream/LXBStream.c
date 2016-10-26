/* -----------------------------------------------------------------------
 * $Id: LXBStream.c 1782 2013-02-23 00:11:55Z viari $
 * -----------------------------------------------------------------------
 * @file: LXStream.c
 * @desc: file or memory byte stream
 * 
 * @history:
 * @+ <Gloup> : Jan 10 : first version
 * @+ <Gloup> : Mar 10 : growing version removed and replaced by Unix
 * @+                    internal buffered fread
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * file or memory byte stream<br>
 *
 * @docend:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>              // AV: may be removed on Windows

#include "LX/LXBStream.h"

#ifndef BUFSIZ
#define BUFSIZ 1024
#endif

#define INIT_W_SIZE BUFSIZ
#define INIT_R_SIZE 64

/* -------------------------------------------- */
/* internal utilities                           */
/* -------------------------------------------- */

// --------------------------------------------
// tell if we can automatically endian convert element of this sizeltà
//
static Bool sCanConvert(LXBStream *bstream, size_t sizelt) {
    return    bstream->autoConvert
           && bstream->reverseEndian
           && ((sizelt == 2) || (sizelt == 4) || (sizelt == 8));
}

// --------------------------------------------
// get proper f(d)open(3) mode string from mode flags
//
static char *sGetDescMode(int mode) {
    static char sDescMode[4];
    if (mode | LX_BSTREAM_WRITE) {
        (void) strcpy(sDescMode, "w");
        if (mode | LX_BSTREAM_READ)
            (void) strcat(sDescMode, "+");
    }
    else if (mode | LX_BSTREAM_READ) {
        (void) strcpy(sDescMode, "r");
    }
    return sDescMode;
}

// --------------------------------------------
// set buffer size to be at least size
// (double buffer at each recursion)
//

static int sSetBufferSize(LXBStream *bstream, size_t size) {
    if (size <= bstream->capacity) {
        bstream->bufferSize = size;
        return 1;
    }
    size_t capacity = 2 * bstream->capacity;
    void *ptr = REALLOC(bstream->buffer, char, capacity);
    IFF_RETURN(ptr, 0);
    bstream->capacity = capacity;
    bstream->buffer = ptr;
    return sSetBufferSize(bstream, size);
}

// --------------------------------------------
// assert buffer size is at least size
//

static int sAssertBufferSize(LXBStream *bstream, size_t size) {
    if (size <= bstream->bufferSize) {
        return 1;
    }
    if (size <= bstream->capacity) {
        bstream->bufferSize = size;
        return 1;
    }
    return sSetBufferSize(bstream, size);
}

/* -------------------------------------------- */
/* API                                          */
/* -------------------------------------------- */

/* -------------------------------------------- */

LXBStream *LXFreeBStream(LXBStream *bstream) {
    IFF_RETURN(bstream, NULL);
    IFFREE(bstream->buffer);
    FREE(bstream);
    return NULL;
}

/* -------------------------------------------- */

LXBStream *LXNewBStreamFile(FILE *stream, Bool isLittleEndian) {
    IFF_RETURN(stream, NULL);
    
    LXBStream *bstream = NEW(LXBStream);
    IFF_RETURN(bstream, NULL);
    
    ZERO(bstream);
    
    bstream->type = LX_BSTREAM_TYPE_FILE;
    
    bstream->mode = LX_BSTREAM_READ | LX_BSTREAM_WRITE; // we don't care for FILE

    bstream->stream = stream;
    
    bstream->reverseEndian = (LXIsLittleEndian() != isLittleEndian);
    
    bstream->autoConvert = TRUE;

#if LX_SETVBUF_POSIX
    bstream->bufferSize = 0;
    bstream->buffer = NULL;
#else
    bstream->bufferSize = BUFSIZ;
    bstream->buffer = NEWN(char, BUFSIZ);
    IFF_RETURN(bstream->buffer, LXFreeBStream(bstream));
#endif

    (void) setvbuf(stream, bstream->buffer, _IOFBF, bstream->bufferSize);

    rewind(stream);

    return bstream;    
}

/* -------------------------------------------- */

LXBStream *LXNewBStreamDesc(int fildes, int mode, Bool isLittleEndian) {

    FILE *stream = fdopen(fildes, sGetDescMode(mode));
    
    return LXNewBStreamFile(stream, isLittleEndian);
}

/* -------------------------------------------- */

LXBStream *LXNewBStreamMemory(int mode, size_t initialSize, Bool isLittleEndian) {
    
    LXBStream *bstream = NEW(LXBStream);
    IFF_RETURN(bstream, NULL);
    
    ZERO(bstream);
    
    bstream->type = LX_BSTREAM_TYPE_MEMORY;
    
    bstream->mode = mode;

    bstream->reverseEndian = (LXIsLittleEndian() != isLittleEndian);
    
    bstream->autoConvert = TRUE;

    if (initialSize <= 0) {
        initialSize = (mode & LX_BSTREAM_WRITE) ? INIT_W_SIZE : INIT_R_SIZE;
    }
    
    bstream->bufferSize = bstream->capacity = initialSize;
    bstream->buffer = NEWN(char, bstream->capacity);
    IFF_RETURN(bstream->buffer, LXFreeBStream(bstream));

    bstream->currPos = 0;

    return bstream;    
}

/* -------------------------------------------- */

int LXMemCopyToBStream(LXBStream *bstream, void *ptr, size_t size) {
    IFF_RETURN(bstream && ptr, -1);
    IFF_RETURN(bstream->type == LX_BSTREAM_TYPE_MEMORY, -1);

    IFF_RETURN(sSetBufferSize(bstream, size), -1);
    (void) memcpy(bstream->buffer, ptr, size);

    return 0;
}

int LXMemCopyFromBStream(LXBStream *bstream, void *ptr, size_t size) {
    IFF_RETURN(bstream && ptr, -1);
    IFF_RETURN(bstream->type == LX_BSTREAM_TYPE_MEMORY, -1);

    
    size = MIN(size, bstream->bufferSize);
    (void) memcpy(ptr, bstream->buffer, size);

    return 0;
}

/* -------------------------------------------- */

int LXBStreamSeek(LXBStream *bstream, size_t position) {
    IFF_RETURN(bstream, -1);

    if (bstream->type == LX_BSTREAM_TYPE_FILE)
        return LXSeek(bstream->stream, position);

    if (bstream->mode & LX_BSTREAM_WRITE) {
        IFF_RETURN(sAssertBufferSize(bstream, position), -1);
        bstream->currPos = position;
        return 0;
    }
    
    if (position > bstream->bufferSize)
        return -1;

    bstream->currPos = position;
    
    return 0;
}

/* -------------------------------------------- */

long LXBStreamTell(LXBStream *bstream) {
    IFF_RETURN(bstream, -1);
    
    if (bstream->type == LX_BSTREAM_TYPE_FILE)
        return LXTell(bstream->stream);
        
    return bstream->currPos;
}

/* -------------------------------------------- */

long LXBStreamTellEOF(LXBStream *bstream) {
    IFF_RETURN(bstream, -1);
    
    if (bstream->type == LX_BSTREAM_TYPE_FILE)
        return LXTellEOF(bstream->stream);
        
    return bstream->bufferSize;
}


/* -------------------------------------------- */

size_t LXBStreamConvert(LXBStream *bstream, void *ptr, size_t sizelt, size_t nitems) {
    IFF_RETURN(bstream, 0);

    if (sCanConvert(bstream, sizelt))
        (void) LXEndianSwapAdr(ptr, nitems, sizelt);
    return nitems;
}

/* -------------------------------------------- */

size_t LXBStreamRead(LXBStream *bstream, void *ptr, size_t sizelt, size_t nitems) {
    IFF_RETURN(bstream, 0);
    
    size_t nread;
    
    if (bstream->type == LX_BSTREAM_TYPE_FILE) {
        nread = fread(ptr, sizelt, nitems, bstream->stream);
    }
    else {
        IFF_RETURN(bstream->type & LX_BSTREAM_READ, 0);
        
        size_t size = sizelt * nitems;
        while (bstream->currPos + size > bstream->bufferSize)
            size -= sizelt;
    
        (void) memcpy(ptr, bstream->buffer + bstream->currPos, size);
        
        bstream->currPos += size;
        
        nread = size / sizelt;
    }
    
    return LXBStreamConvert(bstream, ptr, sizelt, nread);
}

/* -------------------------------------------- */

#define READ_ACCESSOR(func, type)                                           \
    size_t LXBStreamRead##func##Array(LXBStream *bstream, type *value,   \
                                   size_t nitems) {                         \
        return LXBStreamRead(bstream, value, sizeof(type), nitems);         \
    }                                                                       \
    size_t LXBStreamRead##func(LXBStream *bstream, type *value) {        \
        return LXBStreamRead(bstream, value, sizeof(type), 1);              \
    }

READ_ACCESSOR(Char,  char)
READ_ACCESSOR(Int8,  Int8)
READ_ACCESSOR(Int16, Int16)
READ_ACCESSOR(Int32, Int32)
READ_ACCESSOR(Int64, Int64)

READ_ACCESSOR(UChar,  unsigned char)
READ_ACCESSOR(Byte,   Byte)
READ_ACCESSOR(UInt8,  UInt8)
READ_ACCESSOR(UInt16, UInt16)
READ_ACCESSOR(UInt32, UInt32)
READ_ACCESSOR(UInt64, UInt64)

READ_ACCESSOR(Float32, Float32)
READ_ACCESSOR(Float64, Float64)

#undef READ_ACCESSOR

/* -------------------------------------------- */

size_t LXBStreamWrite(LXBStream *bstream, void *ptr, size_t sizelt,
                      size_t nitems) {
    IFF_RETURN(bstream, 0);
    
    size_t nwrite;

    (void) LXBStreamConvert(bstream, ptr, sizelt, nitems);

    if (bstream->type == LX_BSTREAM_TYPE_FILE) {
        nwrite = fwrite(ptr, sizelt, nitems, bstream->stream);
    }
    else {    
        IFF_RETURN(bstream->mode & LX_BSTREAM_WRITE, 0);
    
        size_t size = sizelt * nitems;
        sAssertBufferSize(bstream, bstream->currPos + size);
    
        (void) memcpy(bstream->buffer + bstream->currPos, ptr, size);

        bstream->currPos += size;
        
        nwrite = nitems;
    }
    
    (void) LXBStreamConvert(bstream, ptr, sizelt, nitems); // restore
    
    return nwrite;
}

#define WRIT_ACCESSOR(func, type)                                           \
    size_t LXBStreamWrite##func##Array(LXBStream *bstream, type *value,  \
                                   size_t nitems) {                         \
        return LXBStreamWrite(bstream, value, sizeof(type), nitems);        \
    }                                                                       \
    size_t LXBStreamWrite##func(LXBStream *bstream, type *value) {       \
        return LXBStreamWrite(bstream, value, sizeof(type), 1);             \
    }

WRIT_ACCESSOR(Char,  char)
WRIT_ACCESSOR(Int8,  Int8)
WRIT_ACCESSOR(Int16, Int16)
WRIT_ACCESSOR(Int32, Int32)
WRIT_ACCESSOR(Int64, Int64)

WRIT_ACCESSOR(UChar,  unsigned char)
WRIT_ACCESSOR(Byte,   Byte)
WRIT_ACCESSOR(UInt8,  UInt8)
WRIT_ACCESSOR(UInt16, UInt16)
WRIT_ACCESSOR(UInt32, UInt32)
WRIT_ACCESSOR(UInt64, UInt64)

WRIT_ACCESSOR(Float32, Float32)
WRIT_ACCESSOR(Float64, Float64)

#undef WRIT_ACCESSOR





