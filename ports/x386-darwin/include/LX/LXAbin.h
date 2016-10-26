/* -----------------------------------------------------------------------
 * $Id: LXAbin.h 888 2010-03-12 08:21:35Z viari $
 * -----------------------------------------------------------------------
 * @file: LXAbin.h
 * @desc: abin library / include file
 * 
 * @history:
 * @+ <Gloup> : Tue Sep 30 1997 : Created
 * @+ <Wanou> : Dec 01 : adapted to LX_Ware 
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * LXabin library<br>
 * abin files input/output
 *
 * @docend:
 */

#ifndef _H_LXAbin

#define _H_LXAbin

#ifndef _H_LXSys
#include "LXSys.h"
#endif

/* ---------------------------------------------------- */
/* Constants                                            */
/* ---------------------------------------------------- */

/*
 * @doc: LXabin file stamp
 */

#define LX_ABIN_STAMP      0x6162696e

/*
 * @doc: LXabin version
 */

#define LX_ABIN_VERSION    0x01

/*
 * @doc: maximum length for LXabin alphabet, name & comment 
 */

#define LX_ABIN_MAX_STRING_LEN   256

/*
 * @docstart:
 * @par Abin Error codes
 * - LX_ABIN_NO_ERROR           : no error
 * - LX_ABIN_READ_ERROR         : read error
 * - LX_ABIN_WRITE_ERROR        : write error
 * - LX_ABIN_BAD_STAMP_ERROR    : bad stamp
 * - LX_ABIN_BAD_VERSION_ERROR  : bad version number
 * - LX_ABIN_BAD_COMPRESS_ERROR : bad compression factor
 * - LX_ABIN_BAD_LENGTH_ERROR   : bad sequence length
 * - LX_SYS_BAD_RANGE_ERROR     : bad range
 * @docend:
 */

#define LX_ABIN_NO_ERROR          LX_NO_ERROR

#define LX_ABIN_READ_ERROR         21
#define LX_ABIN_WRITE_ERROR        22
#define LX_ABIN_BAD_STAMP_ERROR    23
#define LX_ABIN_BAD_VERSION_ERROR  24
#define LX_ABIN_BAD_COMPRESS_ERROR 25
#define LX_ABIN_BAD_LENGTH_ERROR   26
#define LX_ABIN_BAD_RANGE_ERROR    27

/* ---------------------------------------------------- */
/* Data Structure                                       */
/* ---------------------------------------------------- */

/*
 * @doc: abin header structure
 * - version     : Int32 - abin version
 * - compression : Int32 - abin compression
 * - seqlen      : Int32 - sequence length
 * - seqoff      : Int32 - sequence offset
 * - alphabet    : char[LX_ABIN_MAX_STRING_LEN] - sequence alphabet
 * - name        : char[LX_ABIN_MAX_STRING_LEN] - sequence name
 * - comment     : char[LX_ABIN_MAX_STRING_LEN] - sequence comment
 */

typedef struct {
    Int32   version,
            compression,
            seqlen,
            seqoff;
    char    alphabet[LX_ABIN_MAX_STRING_LEN+2],
            name[LX_ABIN_MAX_STRING_LEN+2],
            comment[LX_ABIN_MAX_STRING_LEN+2];
} LXAbinHeader;

/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/*
 * @doc:
 * Read abin header
 * @param header LXAbinHeader* to read
 * @param stream FILE* to read in
 * @return LX_ABIN_X_ERROR code depending on read issue
 * @sample
 *
 */

int LXAbinReadHeader    (LXAbinHeader *header, FILE *stream);

/*
 * @doc:
 * Write header
 * @param header LXAbinHeader* to write
 * @param stream FILE* to write in
 * @return LX_ABIN_x_ERROR code depending on write issue
 * @sample
 *
 */

int LXAbinWriteHeader   (LXAbinHeader *header, FILE *stream);

/*
 * @doc:
 * Read abin sequence
 * @param seq char* sequence to read
 * @param from Int32 first element to be read
 * @param to Int32 last element to be read
 * @param header LXAbinHeader* sequence information
 * @param stream FILE* to read in
 * @return LX_ABIN_X_ERROR code depending on read issue
 * @sample
 *
 */

int LXAbinReadSequence  (char *seq, Int32 from, Int32 to, 
                         LXAbinHeader *header, FILE *stream);

/*
 * @doc:
 * Write abin sequence
 * @param seq char* sequence to write
 * @param header LXAbinHeader* sequence information
 * @param stream FILE* to write in
 * @return LX_ABIN_x_ERROR code depending on write issue
 * @sample
 *
 */

int LXAbinWriteSequence (char *seq, LXAbinHeader *header, FILE *stream);

#endif
