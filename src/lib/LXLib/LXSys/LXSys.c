/* -----------------------------------------------------------------------
 * $Id: LXSys.c 1810 2013-02-25 10:04:43Z viari $
 * -----------------------------------------------------------------------
 * @file: LXSys.c
 * @desc: system related utilities
 * 
 * @history:
 * @+ <Gloup> : May 99 : first version
 * @+ <Wanou> : Dec 01 : last revision 
 * @+ <Gloup> : Jan 06 : last revision 
 * @+ <Gloup> : Jan 10 : binary output changed to little endian
 * @+ <Gloup> : Mar 10 : time functions added
 * @+ <Gloup> : Mar 10 : debug notification added
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * system related utilities<br>
 *
 * @docend:
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "LX/LXMach.h"

#ifdef LX_OS_UNIX
#include <sys/resource.h>                                                       
#include <sys/types.h>
#include <sys/time.h>
#include <sys/sysctl.h>
#endif

#if defined (LX_TARGET_MACOSX) || defined(LX_TARGET_MACINTEL)
#include <mach/mach.h>
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif

#include "LX/LXSys.h"

static int  sDebugLevel = LX_DFT_DEBUG_LEVEL;
static int  sAbortLevel = LX_FATAL_LEVEL;

static char sCurrentFileName[FILENAME_MAX] = ""; 
static int  sCurrentLineNo = 0;

/* ==================================================== */
/* should we use dlmalloc or internal libc malloc       */
/* ==================================================== */

#ifdef DLMALLOC
#include "dlmalloc.c"
#define _LXMALLOC  dlmalloc
#define _LXREALLOC dlrealloc
#define _LXFREE    dlfree
#else
#define _LXMALLOC  malloc
#define _LXREALLOC realloc
#define _LXFREE    free
#endif

/* ==================================================== */
/* internal utilities                                   */
/* ==================================================== */

/* -------------------------------------------- */

static int sIsEmptyLine(char *line)
{
  char *s;
  for (s = line ; *s && (*s != '\n') ; s++) {
    if (! isspace(*s))
      return 0;
  }
  return 1;
}

/* -------------------------------------------- */

static int sIsCommentLine(char *line)
{
  return (*line == LX_COMMENT_CHAR);
}

/* -------------------------------------------- */

static int sSkipLine(char *line)
{
  return sIsEmptyLine(line) || sIsCommentLine(line);
}

/* ==================================================== */
/* Library initialization                               */
/* ==================================================== */

/* -------------------------------------------- */
/* initializer for LX posix behavior            */
/* -------------------------------------------- */

void LXSetup(void) {
  static Bool sInited = FALSE;
  if (sInited) return;
  sInited = TRUE;
#ifdef WIN_MINGW
  // setenv does not work in MinGW, you have to use putenv with
  // one separate static buffer per variable
  static char exponentBuf[32];
  
  // this is to prevent MinGW/MSVCRT from printing "%g" with 3 digits
  (void) putenv(strcpy(exponentBuf, "PRINTF_EXPONENT_DIGITS=2"));
  
#endif
}

/* ==================================================== */
/* Debug notifications                                  */
/* ==================================================== */

/* -------------------------------------------- */
/* simple debug notifier                        */
/* -------------------------------------------- */

void LXDebug(int level, ...) {
	va_list	args;
	va_start(args, level);

	if (level < sDebugLevel) return;
	
	char *fmt = va_arg(args, char *);

	(void) fprintf(stderr, "// Debug ");
	(void) vfprintf(stderr, fmt, args);

	va_end(args);
}

/* -------------------------------------------- */
/* get current debug level                      */
/* -------------------------------------------- */

int LXGetDebugLevel() {
    return sDebugLevel;
}

/* -------------------------------------------- */
/* set current debug level                      */
/* -------------------------------------------- */

int LXSetDebugLevel(int level) {
    int previous = sDebugLevel;
    
    sDebugLevel = level;
    
    return previous;
}

/* ==================================================== */
/* Error notifications                                  */
/* ==================================================== */

/* -------------------------------------------- */
/* simple error notifier                        */
/* -------------------------------------------- */

int LXError (const char *filename, int lineno, const char *msg, int level)
{
  (void) fprintf(stderr, "// %s [%d] in file %s at line %d\n",
                 ((level > LX_WARNING_LEVEL) ? "*Error*  " : "*Warning*"),
                 errno, filename, lineno);

  (void) fprintf(stderr, "//           ");

  if (errno != 0) {
    perror(msg);
    errno = 0;
  }
  else {
    (void) fprintf(stderr, "%s: internal error: 0\n", msg);
  }

  if (level >= sAbortLevel) {
    (void) fprintf(stderr,"// *Abort*\n");
    exit(level);
  }

  return level;
}

/* -------------------------------------------- */
/* error in file io                             */
/* -------------------------------------------- */

int LXIOError (const char *filename, int lineno, const char *msg, int level)
{
  char buffer[BUFSIZ];

  (void) sprintf(buffer, "%s in file %s at line %d",
                 msg, sCurrentFileName, sCurrentLineNo);

  return LXError(filename, lineno, buffer, level);
}

/* -------------------------------------------- */
/* get current abort level                      */
/* -------------------------------------------- */

int LXGetAbortLevel()
{
  return sAbortLevel;
}

/* -------------------------------------------- */
/* set current abort level                      */
/* -------------------------------------------- */

int LXSetAbortLevel(int level)
{
  int previous = sAbortLevel;

  sAbortLevel = level;

  return previous;
}

/* ==================================================== */
/* Memory allocator                                     */
/* ==================================================== */

/* -------------------------------------------- */
/* malloc                                       */
/* -------------------------------------------- */

void *LXMalloc(size_t size) {
  char *ptr = _LXMALLOC(size + sizeof(size_t));
  if (ptr) {
    *((size_t *) ptr) = size;
    ptr += sizeof(size_t);
  }
  else {
    MEMORY_ERROR();
  }
  return (void *) ptr;
}

/* -------------------------------------------- */
/* realloc                                      */
/* -------------------------------------------- */

void *LXRealloc(void *aptr, size_t size) {
  char *ptr = (char *) aptr;
  if (ptr) {
    ptr -= sizeof(size_t);
    ptr = _LXREALLOC(ptr, size + sizeof(size_t));
  } else {
    ptr = LXMalloc(size);
  }
  if (ptr) {
    *((size_t *) ptr) = size;
    ptr += sizeof(size_t);
  }
  else {
    MEMORY_ERROR();
  }
  return ptr;
}

/* -------------------------------------------- */
/* assert alloc                                 */
/* -------------------------------------------- */

void *LXAssertAlloc(void *aptr, size_t size) {
  char *ptr = (char *) aptr;
  if (ptr) {
    size_t osize = *((size_t *) (ptr - sizeof(size_t)));
    if (osize < size)
      ptr = LXRealloc(ptr, size);
  }
  return ptr;
}

/* -------------------------------------------- */
/* free                                         */
/* -------------------------------------------- */

void LXFree(void *aptr) {
  char *ptr = (char *) aptr;
  if (ptr) {
    ptr -= sizeof(size_t);
    *((size_t *) ptr) = -1;
    _LXFREE(ptr);
  }
}

/* -------------------------------------------- */
/* get alloc size                               */
/* -------------------------------------------- */

size_t LXAllocSize(void *aptr) {
  char *ptr = (char *) aptr;
  return (ptr ? *((size_t *) (ptr - sizeof(size_t))) : 0);
}

/* ==================================================== */
/* memory statistics                                    */
/* ==================================================== */

size_t LXGetUsedMemory() {

#if defined(DLMALLOC)

  struct mallinfo info = dlmallinfo();
  return info.uordblks;

#elif defined (LX_TARGET_MACOSX) || defined(LX_TARGET_MACINTEL)

  struct mstats mem = mstats();
  return mem.bytes_used;

#else

  struct mallinfo info = mallinfo();
  return info.uordblks;

#endif

}

/* ==================================================== */
/* file manipulations                                   */
/* ==================================================== */

/* -------------------------------------------- */
/* setup ioerror internals                      */
/* -------------------------------------------- */

void LXSetupIO(const char *filename)
{
  (void) strcpy(sCurrentFileName, filename);

  sCurrentLineNo = 0;
}

/* -------------------------------------------- */
/* check file access                            */
/* wrapper to access(2)                         */
/* -------------------------------------------- */

int LXAccess(const char *filename, const char *mode) {

  int imode = 0;
  
  while (*mode) {
    switch (*mode++) {

      case 'r' :
      case 'R' : imode |= R_OK ; break;
      
      case 'w' :
      case 'W' : imode |= W_OK ; break;

      case 'x' :
      case 'X' : imode |= X_OK ; break;
    }
  }

  LXSetupIO(filename);
  
  return (access(filename, imode) == 0 ? LX_NO_ERROR : LX_IO_ERROR);
}

/* -------------------------------------------- */
/* open a file                                  */
/* wrapper to fopen(2)                          */
/* -------------------------------------------- */
 
FILE *LXOpenFile(const char *filename, const char *mode)
{
  LXSetupIO(filename);
  return fopen(filename, mode);
}

/* -------------------------------------------- */
/* open a file and assign it to stdin           */
/* wrapper to freopen(2)                        */
/* -------------------------------------------- */

FILE *LXAssignToStdin(const char *filename)
{
  LXSetupIO(filename);
  return freopen(filename, "r", stdin);
}

/* -------------------------------------------- */
/* open a file and assign it to stdout          */
/* wrapper to freopen(2)                        */
/* -------------------------------------------- */

FILE *LXAssignToStdout(const char *filename)
{
  LXSetupIO(filename);
  return freopen(filename, "w", stdout);
}

/* -------------------------------------------- */
/* close a file                                 */
/* wrapper to fclose(2)                         */
/* -------------------------------------------- */
 
int LXCloseFile(FILE *file)
{
  LXSetupIO("");
  return fclose(file) == 0 ? LX_NO_ERROR : LX_IO_ERROR;
}

/* -------------------------------------------- */
/* remove a file                                */
/* -------------------------------------------- */
int LXRemoveFile(const char *filename) {
  return (unlink(filename) == 0 ? LX_NO_ERROR : LX_IO_ERROR);
}

/* -------------------------------------------- */
/* make a temporary file for reading or writing */
/* -------------------------------------------- */

int LXOpenTmpFile(LXTmpFile *tmpFile)
{

#if defined(WIN_MINGW)    // there is a bug in MinGW
#define LX_TMPDIR ""      //     P_tmpdir == "\\" which is usually unwritable
#elif defined(P_tmpdir)   // in <stdio.h>
#define LX_TMPDIR P_tmpdir
#else
#define LX_TMPDIR ""
#endif

  if (LX_TMPDIR[0] != '\000') {  // LX_TMPDIR is not empty, prepend it
    char *s = strcpy(tmpFile->fileName, LX_TMPDIR);
    if (s[strlen(s)-1] != LX_FILENAME_SEPARATOR[0])
      (void) strcat(s, LX_FILENAME_SEPARATOR);
  }
  else {
    tmpFile->fileName[0] = '\000';
  }

  (void) strcat(tmpFile->fileName, "LX.XXXXXX");

  int desc = mkstemps(tmpFile->fileName, 0);
  
  tmpFile->stream = (desc != -1) ? fdopen(desc, "w+") : NULL;
  
  return (tmpFile->stream ? LX_NO_ERROR : LX_IO_ERROR);
}

/* -------------------------------------------- */
/* close and remove temporary file              */
/* -------------------------------------------- */

int LXCloseTmpFile(LXTmpFile *tmpFile)
{
  if (! tmpFile) return LX_ARG_ERROR;

  int err = LXCloseFile(tmpFile->stream);

  return (err ? err : LXRemoveFile(tmpFile->fileName));
}

/* -------------------------------------------- */
/* read one line in file                        */
/* wrapper to fgets(2)                          */
/* ignore comment and empty lines               */
/* -------------------------------------------- */

char * LXGetLine(char *buffer, size_t size, FILE *stream)
{
  char *line;

  do {
    line = fgets(buffer, size, stream);
    sCurrentLineNo++;
  } while (line && sSkipLine(line));

  return line;
}

/* -------------------------------------------- */
/* read one line in file                        */
/* wrapper to fgets(2)                          */
/* read any line                                */
/* -------------------------------------------- */

char * LXGetRawLine(char *buffer, size_t size, FILE *stream)
{
  char *line;

  line = fgets(buffer, size, stream);
  sCurrentLineNo++;

  return line;
}

/* -------------------------------------------- */
/* get fullpathname                             */
/* ! static buffer !                            */
/* -------------------------------------------- */

char * LXPathName(char *filename, char *prepend)
{
  static char sPath[FILENAME_MAX];

  char *env;

  env = (prepend ? getenv(prepend) : NULL);

  if (env)
    (void) strcat(strcpy(sPath, env), LX_FILENAME_SEPARATOR);
  else
    (void) strcpy(sPath, "");
 
  return strcat(sPath, filename);
}

/* -------------------------------------------- */
/* get basename for file                        */
/* use LX_FILENAME_SEPARATOR                    */
/* ! static buffer !                            */
/* -------------------------------------------- */

char * LXBaseName(char *filename, Bool removeExt)
{
  static char sPath[FILENAME_MAX];

  char *from;
  
  (void) strcpy(sPath, filename);
  
  from = strrchr(sPath, LX_FILENAME_SEPARATOR[0]);

  from = (from ? from + 1 : sPath);

  if (removeExt) {
    char *to;
    if ((to = strrchr(from, '.')) != NULL)
      *to = '\000';
  }
 
  return from;
}

/* ==================================================== */
/* time manipulations                                   */
/* ==================================================== */

/*
 * get elapsed time since Jan 1, 1970 in seconds with microsecond precision
 */

double LXCurrentTime() {
#if defined(LX_OS_UNIX)
    struct timeval clok;
    gettimeofday(&clok, NULL);
    return (double) clok.tv_sec + ((double) clok.tv_usec / 1e6);
#else
    return 0.;
#endif
}

/*
 * get current time as string : DDD MMM dd hh:mm:ss:lll:uuu
 */

char *LXStrCurrentTime() {
#if defined(LX_OS_UNIX)
    static char sBuffer[64]; // keep this as static 
    
    char timbuf[32]; 
    struct timeval clok;
    
    // the unix way
    gettimeofday(&clok, NULL);
    
    // get DDD MMM dd hh:mm:ss part into timbuf
    (void) ctime_r(&clok.tv_sec, timbuf);
    
    // add millisec and microsec part to it
    int msec = (int) (clok.tv_usec / 1000); // millisec
    int usec = (int) (clok.tv_usec - (msec * 1000)); // microsec
    (void) sprintf(sBuffer, "%.19s:%.3d:%.3d", timbuf, msec, usec);

    return sBuffer;
#else
    return "today";
#endif
}

/* -------------------------------------------- */

int LXSleep(UInt32 msec) {
#if defined(LX_OS_UNIX) || defined(WIN_MINGW)
    return usleep(((useconds_t) msec) * 1000L);
#else
    return -1;
#endif
}

/* -------------------------------------------- */

time_t LXWinTimeToUnixTime(long long int tim) {
       tim /= 10000000;      // 100 nanoseconds -> seconds
       tim -= LX_EPOCH_DIFF; // january 1601 -> january 1970 
       return (time_t) tim;
}

/* ==================================================== */
/* CPU time manipulations                               */
/* ==================================================== */

/* -------------------------------------------- */

float LXUserCpuTime (Bool reset)
{
#ifdef LX_OS_UNIX

  static float sLast = 0;

  float     now, ust;

  struct rusage rusage;

  (void) getrusage (RUSAGE_SELF, &rusage);

  now =   (float) rusage.ru_utime.tv_sec
       + ((float) rusage.ru_utime.tv_usec / 1000000.);

  ust = now - sLast;
 
  if (reset)
    sLast = now;
 
  return ust;

#else
  // [JeT] there is no standard package to compute cpu/user usage in MingW
  return 0.;

#endif
} 

/* -------------------------------------------- */

float LXSysCpuTime (Bool reset)
{
#ifdef LX_OS_UNIX

  static float sLast = 0;
 
  float     now, ust;
 
  struct rusage rusage;
 
  (void) getrusage (RUSAGE_SELF, &rusage);
 
  now =   (float) rusage.ru_stime.tv_sec
       + ((float) rusage.ru_stime.tv_usec / 1000000.);

  ust = now - sLast;
 
  if (reset)
    sLast = now;
 
  return ust;

#else
  // [JeT] there is no standard package to compute cpu/user usage in MingW
  return 0.;

#endif
} 

/* -------------------------------------------- */

char *LXStrCpuTime (Bool reset)
{
  static char buffer[256];
 
  float     ust, syt, tot;
 
  ust = LXUserCpuTime (reset);
  syt = LXSysCpuTime (reset);
  tot = ust + syt;
 
  (void) sprintf (buffer, "Cpu time user: %f sys: %f tot: %f sec. ",
                  ust, syt, tot);
 
  return buffer;
}                                                                               

/* ==================================================== */
/* direct access (binary) files                         */
/* ==================================================== */

/* -------------------------------------------- */

int LXBinWriteInt32 (FILE *stream, Int32 i)
{
#ifdef LX_RT_BIG_ENDIAN
  i = LX_ENDIAN_SWAP32(i);
#endif
  return fwrite(&i, sizeof(Int32), 1, stream);
}

/* -------------------------------------------- */

int LXBinReadInt32  (FILE *stream, Int32 *i)
{
  int res = fread(i, sizeof(Int32), 1, stream);
#ifdef LX_RT_BIG_ENDIAN
  *i = LX_ENDIAN_SWAP32(*i);
#endif
  return res;
}

/* -------------------------------------------- */

int LXBinWriteInt64 (FILE *stream, Int64 i)
{
#ifdef LX_RT_BIG_ENDIAN
  i = LX_ENDIAN_SWAP64(i);
#endif
  return fwrite(&i, sizeof(Int64), 1, stream);
}

/* -------------------------------------------- */

int LXBinReadInt64  (FILE *stream, Int64 *i)
{
  int res = fread(i, sizeof(Int64), 1, stream);
#ifdef LX_RT_BIG_ENDIAN
  *i = LX_ENDIAN_SWAP64(*i);
#endif
  return res;
}

/* -------------------------------------------- */

int LXBinWriteFloat (FILE *stream, float f)
{
#ifdef LX_RT_BIG_ENDIAN
  f = LX_ENDIAN_SWAP32(f);
#endif
  return fwrite(&f, sizeof(float), 1, stream);
}

/* -------------------------------------------- */

int LXBinReadFloat  (FILE *stream, float *f)
{
  int res = fread(f, sizeof(float), 1, stream);
#ifdef LX_RT_BIG_ENDIAN
  *f = LX_ENDIAN_SWAP32(*f);
#endif
  return res;
}

/* -------------------------------------------- */

size_t LXBinWriteChars (FILE *stream, char *s, size_t size)
{
  return fwrite(s, sizeof(char), size, stream);
}

/* -------------------------------------------- */

size_t LXBinReadChars (FILE *stream, char *s, size_t size)
{
  return fread(s, sizeof(char), size, stream);
}

/* -------------------------------------------- */

size_t LXBinWriteStr (FILE *stream, char *s)
{
  size_t size, res;
  
  size = strlen(s) + 1;
  
  if ((res = LXBinWriteInt64(stream, size)) != 1)
    return res;
  
  return LXBinWriteChars(stream, s, size);
}

/* -------------------------------------------- */

size_t LXBinReadStr (FILE *stream, char *s)
{
  Int64 size;
  size_t res;
  
  if ((res = LXBinReadInt64(stream, &size)) != 1)
    return res;
    
  return LXBinReadChars(stream, s, size);
}

/* -------------------------------------------- */

long LXSeek (FILE *stream, long pos)
{
  return (fseek(stream, pos, SEEK_SET) == 0 ? pos : -1);
}

/* -------------------------------------------- */

long LXTell (FILE *stream)
{
  return ftell(stream);
}

/* -------------------------------------------- */

long LXTellEOF (FILE *stream)
{
  //
  // [AV] cache to speedup ftell/fseek
  //
  static long currentEOF = -1;
  static FILE *currentStream = NULL;

  if (stream != currentStream) {
    long pos = LXTell(stream);     // keep current position
    fseek(stream, 0, SEEK_END);
    currentEOF = LXTell(stream);
    LXSeek(stream, pos);            // restore position
    currentStream = stream;
  }
    
  return currentEOF;
}

/* -------------------------------------------- */

Bool LXIsEOF (FILE *stream)
{
  return (LXTell(stream) == LXTellEOF(stream));
}

/* ==================================================== */
/* endian manipulations                                 */
/* ==================================================== */

Bool LXIsLittleEndian () {
  static union {
    int integer;
    unsigned char byte[sizeof(int)];
  } pack_ = {1};
  return (pack_.byte[0] != 0);
}

/* -------------------------------------------- */

void *LXEndianSwapAdr(void *base, size_t size, size_t width) {
  size_t i;
  
  switch (width) {
  
    case 2:
      { UInt16 *ptr = base;
        for (i = 0 ; i < size ; i++) {
          ptr[i] = LX_ENDIAN_SWAP16(ptr[i]);
        }
      }
      break;
  
    case 4:
      { UInt32 *ptr = base;
        for (i = 0 ; i < size ; i++) {
          ptr[i] = LX_ENDIAN_SWAP32(ptr[i]);
        }
      }
      break;

    case 8:
      { UInt64 *ptr = base;
        for (i = 0 ; i < size ; i++) {
          ptr[i] = LX_ENDIAN_SWAP64(ptr[i]);
        }
      }
      break;

    default:
        return NULL;
  }
  
  return base;
}

/* LXSys.c */
