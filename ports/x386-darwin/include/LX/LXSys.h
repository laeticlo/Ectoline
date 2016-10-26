/* -----------------------------------------------------------------------
 * $Id: LXSys.h 1935 2013-07-25 12:38:55Z viari $
 * -----------------------------------------------------------------------
 * @file: LXSys.h
 * @desc: system utilities
 * 
 * @history:
 * @+ <Gloup> : May 99 : first version
 * @+ <Wanou> : Dec 01 : last revision 
 * @+ <Gloup> : Jan 10 : endian conversion added
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * Helix system functs library<br>
 *
 * @docend:
 */

#ifndef _H_LXSys
#define _H_LXSys

#include <stdio.h>
#include <time.h>

#include "LXTypes.h"
#include "LXMacros.h"

/* ---------------------------------------------------- */
/* Constants                                            */
/* ---------------------------------------------------- */

/*
 * ANSI_C __FILE__ and __LINE__
 */

#ifndef __FILE__
#define __FILE__ "unknown file"
#endif

#ifndef __LINE__
#define __LINE__ 0
#endif

/*
 * @doc: separator used in filepath
 */
 
#ifdef LX_OS_WIN
#define LX_FILENAME_SEPARATOR "\\"
#else
#define LX_FILENAME_SEPARATOR  "/"
#endif

/*
 * @doc: arguments of LXSysXtime()
 */

#define LX_TIME_NO_RESET   FALSE
#define LX_TIME_RESET      TRUE

/*
 * @doc: time difference in seconds from January 1601 to January 1970
 * @+    for Windows -> Unix time conversion
 */

#define LX_EPOCH_DIFF       11644473600LLU

/*
 * @docstart: DEVDOC
 * @par LXError() abort levels
 * - LX_WARNING_LEVEL  : abort for warnings (code [1, 9])
 * - LX_FATAL_LEVEL    : abort for fatal errors (code [10, 128])
 * - LX_NO_ABORT_LEVEL : never abort 
 * @docend:
 */

#define LX_WARNING_LEVEL   1
#define LX_FATAL_LEVEL     10
#define LX_NO_ABORT_LEVEL  255

/*
 * @docstart: DEVDOC
 * @par LXDebug() default debug level
 * @docend:
 */

#define LX_DFT_DEBUG_LEVEL  10

/*
 * @docstart:
 * @par Error codes
 * - LX_NO_ERROR    : no error
 * - LX_MEM_ERROR   : memory error
 * - LX_IO_ERROR    : IO error
 * - LX_ARG_ERROR   : argument error
 * - LX_ARCH_ERROR  : architecture error
 * - LX_IMPL_ERROR  : operation not implemented error
 * @docend:
 */

#define LX_EOF                  EOF

#define LX_NO_ERROR             0

#define LX_WARNING_ERROR        1

#define LX_MEM_ERROR           10
#define LX_IO_ERROR            12
#define LX_ARG_ERROR           14
#define LX_ARCH_ERROR          16
#define LX_IMPL_ERROR          18

/*
 * @doc: special char to indicate comment lines in file
 */

#define LX_COMMENT_CHAR          '#'

/* ---------------------------------------------------- */
/* Structures                                           */
/* ---------------------------------------------------- */

/*
 * @doc: structure holding temporary file information
 */

typedef struct {
  char fileName[FILENAME_MAX];
  FILE *stream;
} LXTmpFile;

/* ---------------------------------------------------- */
/* Macros                                               */
/* ---------------------------------------------------- */

/*
 * @doc: swap endianness of a 2 bytes (16 bits) value.
 *       ex : myVal = LX_ENDIAN_SWAP16(myVal)
 *
 * @param: x 2 bytes value
 * @return:  swapped value
 */
#define LX_ENDIAN_SWAP16(x)  (((x) & 0x00FF) << 8) | \
                             (((x) & 0xFF00) >> 8)

/*
 * @doc: swap endianness of a 4 bytes (32 bits) value.
 *       ex : myVal = LX_ENDIAN_SWAP32(myVal)
 *
 * @param: x 4 bytes value
 * @return:  swapped value
 */

#define LX_ENDIAN_SWAP32(x)  (((x) & 0x000000FF) << 24) | \
                             (((x) & 0x0000FF00) << 8)  | \
                             (((x) & 0x00FF0000) >> 8)  | \
                             (((x) & 0xFF000000) >> 24)

/*
 * @doc: swap endianness of a 8 bytes (64 bits) value.
 *       ex : myVal = LX_ENDIAN_SWAP64(myVal)
 *
 * @param: x 8 bytes value
 * @return:  swapped value
 */

#define LX_ENDIAN_SWAP64(x)  ((x)>>56)                           | \
                             (((x)<<40) & 0x00FF000000000000LLU) | \
                             (((x)<<24) & 0x0000FF0000000000LLU) | \
                             (((x)<<8)  & 0x000000FF00000000LLU) | \
                             (((x)>>8)  & 0x00000000FF000000LLU) | \
                             (((x)>>24) & 0x0000000000FF0000LLU) | \
                             (((x)>>40) & 0x000000000000FF00LLU) | \
                             ((x)<<56)

/*
 * @doc: helper macro for calling LXEndianSwapAdr()
 * toggle endian representation of value(s) at base address.
 * base address can be the address of a variable (&x)
 * or an array of given size.
 * ex: LX_ENDIAN_SWAP_ADR(float, &myVar, 1);
 * ex: LX_ENDIAN_SWAP_ADR(float, myArray, 10);
 *
 * @param: type C type of elements to swap
 * @param: base void* base address
 * @param: size_t number of elements to swap
 * @return:  * @return base address or NULL on error
 */
#define LX_ENDIAN_SWAP_ADR(type, base, size) \
        (type *) LXEndianSwapAdr((void *) (base), (size_t) (size), sizeof(type))

/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/*
 * @doc: 
 * setup LX library internal flags.
 * Call of this function is not mandatory but recommanded on
 * some ports (typically win32).
 * Currently this function does nothing, except for port WIN_MINGW
 * where it sets some environment variables to bypass some MSVCRT
 * odd behavior (like "%g" printing exponent with 3 digits).
 * note: maybe called several time without trouble.
 */
void LXSetup (void);

/*
 * @doc: 
 * Debug Notifier LXDebug(level, fmt[, args])
 * @param level int debug level (notify if level >= LXGetDebugLevel())
 * @param fmt char* printf format
 * @param [args] optional arguments list
 */
void LXDebug (int level, ...);

/*
 * @doc:
 * Get current debug level
 * @return current debug level
 */
int LXGetDebugLevel(void);

/*
 * @doc:
 * Get current debug level
 * @param level int current debug level to set
 * @return previous debug level
 */
int LXSetDebugLevel(int level);

/*
 * @doc: 
 * Error Notifier
 * @param filename char* name of the program/function where the error
 * occured - see __FILE__
 * @param lineno int line number of the program/function where the error
 * occured - see __LINE__
 * @param msg char* message to print
 * @param level int error code : if > abortLevel then exits program
 * @return level
 */
int LXError (const char *filename, int lineno, const char *msg, int level);

/*
 * @doc: 
 * File IO Error Notifier
 * @param filename char* name of the program/function where the error
 * occured - see __FILE__
 * @param lineno int line number of the program/function where the error
 * occured - see __LINE__
 * @param msg char* message to print
 * @param level int error code : if > abortLevel then exits program
 * @return level
 */
int LXIOError (const char *filename, int lineno, const char *msg, int level);

/*
 * @doc:
 * Get current abort level
 * @return current abort level
 */
int LXGetAbortLevel(void);

/*
 * @doc:
 * Get current abort level
 * @param level int current abort level to set
 * @return previous abort level
 */
int LXSetAbortLevel(int level);


/*
 * @doc:
 * Allocates size bytes of memory and returns a pointer
 * to the allocated memory or NULL if nor enough memory.
 * This function also keep track of the allocated size.
 * @param size size_t number of bytes to allocate
 * @see: malloc
 */
void *LXMalloc(size_t size);

/*
 * @doc:
 * Tries to change the size of the allocation pointed to by
 * ptr to size, and returns ptr or NULL if nor enough memory.
 * This function also keep track of the allocated size.
 * @param ptr void* current pointer
 * @param size size_t number of bytes to allocate
 */
void *LXRealloc(void *ptr, size_t size);

/*
 * @doc:
 * Make sure the size of the allocation pointed to by
 * ptr is at least size, and call LXRealloc if necessary.
 * This function also keep track of the allocated size.
 * @param ptr void* current pointer
 * @param size size_t number of bytes to allocate
 */
void *LXAssertAlloc(void *ptr, size_t size);

/*
 * @doc:
 * Deallocates the memory allocation pointed to by ptr
 * and allocated thru LXMalloc, LXRealloc or LXAssertAlloc
 * @param ptr void* pointer
 */
void LXFree(void *ptr);

/*
 * @doc:
 * Get the size in bytes of the memory allocation pointed to by ptr
 * and allocated thru LXMalloc, LXRealloc or LXAssertAlloc
 * @param ptr void* pointer
 */
size_t LXAllocSize(void *ptr);

/*
 * @doc: 
 * get currently malloc'ed memory
 * @return currently allocated memory in bytes (0 if no function call does exist)
 */
size_t LXGetUsedMemory(void);

/*
 * @doc:
 * Setup IO error internal filename and location.
 * Usually you don't need to call this function
 * explicitely since it is called internally
 * by LXOpenFile and LXAssignToStdIn/Out
 * @param filename char* name of the current file
 */
void LXSetupIO(const char *filename);

/*
 * @doc: 
 * check access permissions of a file or pathname
 * (wrapper of access(2))
 * @param filename char* name of the file to check
 * @param mode char* the read/write mode ("r"/"w"/"rw")
 * @return LX_NO_ERROR on success else LX_IO_ERROR
 */
int LXAccess(const char *filename, const char *mode);

/*
 * @doc: 
 * open a file in the given mode and return the file stream.
 * (wrapper of fopen(2))
 * @param filename char* name of the file to open
 * @param mode char* the read/write mode ((r|w|a)[+]) see fopen(2)
 * @return the file stream (NULL on error)
 */
FILE *LXOpenFile(const char *filename, const char *mode);

/*
 * @doc: 
 * open a file and assign it to stdin.
 * (wrapper of freopen(2))
 * @param filename char* name of the file to open
 * @return the file stream (stdin)
 */
FILE *LXAssignToStdin(const char *filename);

/*
 * @doc: 
 * open a file and assign it to stdout.
 * (wrapper of freopen(2))
 * @param filename char* name of the file to open
 * @return the file stream (stdin)
 */
FILE *LXAssignToStdout(const char *filename);

/*
 * @doc: 
 * close a file previously opened by LXOpenFile.
 * (wrapper of fclose(2))
 * @param file FILE *
 * @return LX_NO_ERROR on success else LX_IO_ERROR
 */
int LXCloseFile(FILE *file);

/*
 * @doc: 
 * remove file
 * @param filename char* name of the file to remove
 * @return error code LX_NO_ERROR, LX_IO_ERROR
 */
int LXRemoveFile(const char *filename);

/*
 * @doc: 
 * create a temporary file for reading/writing (i.e. mode "w+")
 * @param tmpFile LXTmpFile* will receive a LXTmpFile structure
 * @return error code LX_NO_ERROR, LX_IO_ERROR
 */
int LXOpenTmpFile(LXTmpFile *tmpFile);

/*
 * @doc: 
 * close and remove temporary file
 * @param tmpFile LXTmpFile* previously opened by LXOpenTmpFile
 * @return error code LX_NO_ERROR, LX_IO_ERROR
 */
int LXCloseTmpFile(LXTmpFile *tmpFile);

/*
 * @doc: 
 * read one line in file pointed to by stream, ignoring comment and empty lines.
 * (wrapper of fgets(2))
 * @param buffer char* buffer to read in
 * @param size int size of buffer
 * @param stream FILE* stream to read from
 * @return buffer or NULL if EOF
 * @seealso: LXString:LXReadStr
 */
char * LXGetLine(char *buffer, size_t size, FILE *stream);

/*
 * @doc: 
 * read one line in file pointed to by stream, taking every line.
 * (wrapper of fgets(2))
 * @param buffer char* buffer to read in
 * @param size_t int size of buffer
 * @param stream FILE* stream to read from
 * @return buffer or NULL if EOF
 * @seealso: LXString:LXReadStr
 */
char * LXGetRawLine(char *buffer, size_t size, FILE *stream);

/*
 * @doc: 
 * return the full pathname of the given filename 
 * prepending the value of environment variable prepend
 * if supplied (and found).
 * @param filename char* name of the file 
 * @param prepend char* environment variable to look for prepend 
 * @return a static string (make a copy !) containing 
 * the full name of the file (including path)
 */
char * LXPathName(char *filename, char *prepend);

/*
 * @doc: 
 * return the basename of the given filename 
 * removing directory name (and optionally file extension).
 * @param filename char* name of the file 
 * @param removeExt Bool remove extension too 
 * @return a static string (make a copy !) containing 
 * the basename of the file
 */
char * LXBaseName(char *filename, Bool removeExt);

/*
 * @doc: 
 * get elapsed time since Jan 1, 1970 in seconds with microsecond precision.
 * @return elapsed time (i.e. seconds from January 1970)
 */
double LXCurrentTime(void);

/*
 * @doc: 
 * get current time as string : DDD MMM dd hh:mm:ss:lll:uuu
 * @return current time string (as static buffer, please copy before using)
 */
char *LXStrCurrentTime(void);

/*
 * @doc: 
 * suspends execution of the calling process until (about) msec milliseconds
 * have elapsed 
 * @param msec UInt32 sleep time in milliseconds
 * @return 0 if ok, -1 on error
 */
int LXSleep(UInt32 msec);

/*
 * @doc: 
 * transform Windows time to Unix time
 * @param time long Windows time (i.e. 100-nanosecond intervals from January 1601)
 * @return Unix time (i.e. seconds from January 1970)
 */
time_t LXWinTimeToUnixTime(long long int time);

/*
 * @doc: 
 * Get(/Reset) User Cpu time
 * @param reset Bool parameter (LX_TIME_RESET / LX_TIME_NO_RESET)
 *              additionnaly reset counter if reset == LX_TIME_RESET
 * @return User CPU Time in seconds
 */
float LXUserCpuTime (Bool reset);

/*
 * @doc: 
 * Get(/Reset) System Cpu time
 * @param reset Bool parameter (LX_TIME_RESET / LX_TIME_NO_RESET)
 *              additionnaly reset counter if reset == LX_TIME_RESET
 * @return System CPU Time in seconds
 */
float LXSysCpuTime (Bool reset);

/*
 * @doc: 
 * Get(/Reset) CPU times as a string
 * @param reset Bool parameter (LX_TIME_RESET / LX_TIME_NO_RESET)
 *              additionnaly reset counters if reset == LX_TIME_RESET
 * @return CPU Times String
 */
char *LXStrCpuTime (Bool reset);

/*
 * @doc: 
 * Write Int32 in binary into stream
 * @param stream FILE* file to write in
 * @param i Int32 32 bits signed integer to be written
 * @return 1 if i has been written
 */   
int LXBinWriteInt32 (FILE *stream, Int32 i);

/*
 * @doc: 
 * Read binary Int32 from stream
 * @param stream FILE* file to read in
 * @param i Int32* 32 bits signed integer pointer 
 * @return 1 if value has been read
 */   
int LXBinReadInt32  (FILE *stream, Int32 *i);

/*
 * @doc: 
 * Read binary Int64 from stream
 * @param stream FILE* file to read in
 * @param i Int64* 64 bits signed integer pointer 
 * @return 1 if value has been read
 */   
int LXBinReadInt64  (FILE *stream, Int64 *i);

/*
 * @doc: 
 * Write Int64 in binary into stream
 * @param stream FILE* file to write in
 * @param i Int64 64 bits signed integer to be written
 * @return 1 if i has been written
 */   
int LXBinWriteInt64 (FILE *stream, Int64 i);

/*
 * @doc: 
 * Write float in binary into stream
 * @param stream FILE* file to write in
 * @param f float 32 bits signed float to be written
 * @return 1 if f has been written
 */   
int LXBinWriteFloat (FILE *stream, float f);

/*
 * @doc: 
 * Read binary float from stream
 * @param stream FILE* file to read in
 * @param f float* 32 bits signed float pointer 
 * @return 1 if value has been read
 */   
int LXBinReadFloat  (FILE *stream, float *f);

/*
 * @doc:
 * Write n chars from string into binary file stream
 * @param stream FILE* file to write in
 * @param s char* string to be written
 * @param size size_t number of chars to write
 * @return size if s has been written
 */
size_t LXBinWriteChars (FILE *stream, char *s, size_t size);

/*
 * @doc:
 * Write string s into binary file stream. This means writing 
 * the size of s first, then s (with '\000')
 * @param stream FILE* file to write in
 * @param s char* string to be written
 * @return size if s has been written
 */
size_t LXBinWriteStr (FILE *stream, char *s);

/*
 * @doc:
 * Read n chars into string from binary file stream
 * @param stream FILE* file to read in
 * @param s char* string to be written (should accomodate for at least 'size+1' chars
 * @param size size_t number of chars to read
 * @return size if s has been read
 */
size_t LXBinReadChars (FILE *stream, char *s, size_t size);

/*
 * @doc:
 * Read string s from binary file stream.
 * ! the tring should have been written by LXBinWriteStr !
 * @param stream FILE* file to read in
 * @param s char* string to be written
 * @return size if s has been read
 */
size_t LXBinReadStr (FILE *stream, char *s);

/*
 * @doc:
 * set position into binary file
 * @param stream FILE*
 * @param pos long new position
 * @return pos if new position has been reached, else return -1
 */
long LXSeek (FILE *stream, long pos);


/*
 * @doc:
 * get current position in stream
 * @param stream FILE*
 * @return long offset position or -1 on error
 */
long LXTell (FILE *stream);

/*
 * @doc:
 * get last position in stream
 * @param stream FILE*
 * @return long max offset position
 */
long LXTellEOF (FILE *stream);

/*
 * @doc 
 * End Of File test
 * @param stream FILE*
 * @return TRUE if EOF has been reached, else return FALSE
 */
Bool LXIsEOF (FILE *stream);

/*
 * @doc 
 * test processor endianness
 * Note: the test is dynamic (i.e does not rely on LX_RT_LITTLE_ENDIAN 
 *       or LX_RT_BIG_ENDIAN constants).
 *       if performance is needed, better use the 
 *       LX_RT_LITTLE_ENDIAN or LX_RT_BIG_ENDIAN constants with
 *       #ifdef directive (see LXMacros.h).
 * @return TRUE if processor is little endian
 */
Bool LXIsLittleEndian(void);

/*
 * @doc 
 * Swap endianness (little<->big) of data in array
 * @param base void * base address
 * @param size size_t number of elements in array
 * @param width size_t sizeof element in bytes (should be 2, 4 or 8)
 * @return base address if ok else NULL
 */
void *LXEndianSwapAdr(void *base, size_t size, size_t width);

#endif
