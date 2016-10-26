/* -----------------------------------------------------------------------
 * $Id: LXBStream.h 1793 2013-02-24 02:19:06Z viari $
 * -----------------------------------------------------------------------
 * @file: LXBStream.h
 * @desc: file or memory byte stream
 * 
 * @history:
 * @+ <Gloup> : Jan 10 : first version
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * file or memory byte stream<br>
 * read or write to binary file or memory.
 * allow transparent storage of data in memory or external file.
 * for file this is virtually equivalent to fread(3)/fwrite(3)
 * except that endian conversion is performed on the fly.
 *
 * @docend:
 */

#ifndef _H_LXBStream
#define _H_LXBStream

#ifndef _H_LXSys
#include "LXSys.h"
#endif

#include <stdio.h>

/* ---------------------------------------------------- */
/* Constants                                            */
/* ---------------------------------------------------- */

#define LX_BSTREAM_TYPE_FILE        0
#define LX_BSTREAM_TYPE_MEMORY      1

#define LX_BSTREAM_READ             0x01
#define LX_BSTREAM_WRITE            0x02


#define LX_BM_LITTLE_ENDIAN         0
#define LX_BM_BIG_ENDIAN            1

//
// on some systems setvbuf(3) is not POSIX compliant
// change this flag is test do not pass
//

#ifndef LX_SETVBUF_POSIX
#define LX_SETVBUF_POSIX 1
#endif

/* ---------------------------------------------------- */
/* Structures                                           */
/* ---------------------------------------------------- */

/*
 * @doc: Byte Stream 
 * ** for all types **
 * - type : LX_BSTREAM_TYPE_FILE or LX_BSTREAM_TYPE_MEMORY
 * - mode : stream mode flags, combination of LX_BSTREAM_READ and LX_BSTREAM_WRITE
 * - endianReverse : are stream and processor endianness reverse ?
 * - autoConvert: automatic endian conversion
 * - bufferSize : for memory stream : current buffer size
 *                for file stream : optional setvbuf(3) buffer size
 * - buffer     : for memory stream : current buffer
 *                for file stream : optional setvbuf(3) buffer
 * ** for file stream **
 * - stream : input stream
 * ** for memory stream **
 * - currPos : current position in buffer
 */
 
typedef struct {
                                // -- for both types --                 
    int     type;               // stream type LX_BSTREAM_TYPE_FILE/MEMORY
    int     mode;               // stream mode LX_BSTREAM_READ/WRITE
    Bool    reverseEndian;      // reverse stream and processor endianness
    Bool    autoConvert;        // automatic endian conversion
    size_t  bufferSize;         // current buffer size
    char   *buffer;             // current buffer
                                // -- for file stream --
    FILE    *stream;            // binary input stream
                                // -- for memory stream --
    size_t  currPos;            // current position in stream
    size_t  capacity;           // current buffer max capacity
} LXBStream;

/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/*
 * @doc: 
 * free a previously allocated ByteStream.
 * @param bstream LXBStream*
 * @return NULL
 */

LXBStream *LXFreeBStream(LXBStream *bstream);

/*
 * @doc: 
 * create a new FILE ByteStream.
 * note: stream should have been opened in proper mode
 * note: stream is rewound after this call.
 * @param stream FILE* input/output stream
 * @param isLittleEndian Bool input/output stream is little endian
 * @return LXBStream*
 */

LXBStream *LXNewBStreamFile(FILE *stream, Bool isLittleEndian);

/*
 * @doc: 
 * create a new FILE ByteStream from file descriptor fildes
 * note: fildes should have been opened in a mode compatible with 'mode'
 * @param fildes int input/output file descriptor
 * @param mode int flags LX_BSTREAM_READ, LX_BSTREAM_WRITE (may be OR'd)
 * @param isLittleEndian Bool input/output stream is little endian
 * @return LXBStream*
 */

LXBStream *LXNewBStreamDesc(int fildes, int mode, Bool isLittleEndian);

/*
 * @doc: 
 * create a new Memory ByteStream
 * @param mode int flags LX_BSTREAM_READ, LX_BSTREAM_WRITE (may be OR'd)
 * @param initialSize size_t initial size (for write mode)
 * @param isLittleEndian Bool input/output stream is little endian
 *        use LXIsLittleEndian() to ensure same endianness for stream
 *        and actual memory
 * @return LXBStream*
 */

LXBStream *LXNewBStreamMemory(int mode, size_t initialSize, Bool isLittleEndian);

/*
 * @doc: 
 * copy exactly 'size' byte of memory pointed to by 'ptr' into memory stream.
 * the stream may have been opened as LX_BSTREAM_READ or LX_BSTREAM_WRITE
 * or both.
 * Memory data is always copied at the beginning of stream and no endian
 * conversion is performed. Stream buffer size will be increased as needed.
 * note: for a stream openend as LX_BSTREAM_WRITE there are therefore
 * two differences with LXBStreamWrite : data is not appended to stream
 * and no endian conversion is performed.
 * @param bstream LXBStream*
 * @param ptr void* pointer to memory
 * @param size  byte size of memory pointed to by ptr
 * @return LX_NO_ERROR on success.
 */

int LXMemCopyToBStream(LXBStream *bstream, void *ptr, size_t size);

/*
 * @doc: 
 * copy at most 'size' byte from memory stream to memory pointed to by 'ptr'.
 * the stream may have been opened as LX_BSTREAM_READ or LX_BSTREAM_WRITE
 * or both.
 * Data is always copied from the beginning of stream and no endian
 * conversion is performed. 
 * If 0 <= 'size' <= stream->bufferSize then 'size' bytes
 * are copied. 
 * If 'size' > stream->bufferSize then stream->bufferSize are copied.
 * The memory pointed to by 'ptr' should be large enough to hold the
 * copied stuff.
 * note: for a stream openend as LX_BSTREAM_READ there are therefore
 * two differences with LXBStreamData : data is transfered from the
 * beginning of stream and no endian conversion is performed.
 * @param bstream LXBStream*
 * @param ptr void* pointer to memory
 * @param size  byte size of memory to transfer 
 * @return 0 on success -1 on error.
 */

int LXMemCopyFromBStream(LXBStream *bstream, void *ptr, size_t size);

/*
 * @doc:
 * set position into stream.
 * note: for memory in write mode, this may increase the buffer
 *       size up to the requested position.
 * @param bstream LXBStream*
 * @param offset size_t new offset
 * @return 0 if new position has been set, else return -1
 */

int LXBStreamSeek(LXBStream *bstream, size_t offset);


/*
 * @doc:
 * get current position in stream
 * @param bstream LXBStream*
 * @return long offset position or -1 on error
 */
long LXBStreamTell(LXBStream *bstream);

/*
 * @doc:
 * get last position in stream (i.e. current stream size)
 * @param bstream LXBStream*
 * @return long max offset position
 */
long LXBStreamTellEOF(LXBStream *bstream);

/*
 * @doc: 
 * explicit endian conversion. this function is used in very rare cases
 * (when the element size cannot be automatically deciphered),
 * you better have to use the read/write accessors below.
 */
size_t LXBStreamConvert(LXBStream *bstream, void *ptr, size_t sizelt,
                        size_t nitems);

/*
 * @doc: 
 * generic reader (similar to fread(3))
 */
size_t LXBStreamRead(LXBStream *bstream, void *ptr, size_t sizelt,
                     size_t nitems);

#define READ_ACCESSOR(func, type)                                       \
    size_t LXBStreamRead##func##Array(LXBStream *bstream, type *value,  \
                                      size_t nitems);                   \
    size_t LXBStreamRead##func(LXBStream *bstream, type *value);

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

/*
 * @doc: 
 * generic writer (similar to fwrite(3))
 */
size_t LXBStreamWrite(LXBStream *bstream, void *ptr, size_t sizelt, size_t nitems);

#define WRIT_ACCESSOR(func, type)                                           \
    size_t LXBStreamWrite##func##Array(LXBStream *bstream, type *value,     \
                                   size_t nitems);                          \
    size_t LXBStreamWrite##func(LXBStream *bstream, type *value);

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

#endif
