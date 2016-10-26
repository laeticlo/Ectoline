/* -----------------------------------------------------------------------
 * $Id: PMTag.h 888 2010-03-12 08:21:35Z viari $
 * -----------------------------------------------------------------------
 * @file: PMTag.h
 * @desc: tag (PepMap Module) library
 *
 * @history:
 * @+ <Wanou> : Jan 02 : first version
 * @+ <Wanou> : May 02 : Comment Field Added 
 * @+ <Gloup> : Aug 05 : Cleanup Tag formats
 * -----------------------------------------------------------------------
 * @docstart: 
 *
 * tag (PepMap Module) library<br>
 * Defines tags & associated i/o functions 
 *
 * @docend:
 */

#ifndef _H_PMTag
#define _H_PMTag

#include "LX/LXSys.h"

#include "PMSys.h"

#include "PMSpectrum.h"

/* ---------------------------------------------------- */
/* Constants                                            */
/* ---------------------------------------------------- */

/*
 * @doc: maximum length for tag Id name
 */
 
#define PM_TAG_TAGID_LEN  125	/* [AV] JET->1024 ? */

/*
 * @doc: maximum sequence length of a tag (in aa)
 */

#define PM_TAG_MAX_SEQ_LEN 32


/* ---------------------------------------------------- */
/* Data Structures                                      */
/* ---------------------------------------------------- */

/*
 * @doc: Tag structure
 * - id           : char[PM_TAG_TAGID_LEN] - tag id
 * - seq          : char[PM_TAG_MAX_SEQ_LEN] - sequence of amino acids
 * - mN           : float - N terminal Mass (Da)
 * - mC           : float - C terminal Mass (Da)
 * - mParent      : float - Parent ion mass
 * - score        : float - tag score
 * - spectrum     : PMSpectrum * - [optional] parent spectrum
 */

typedef struct _PMTag {                         /* -- Tag Structure --          */

    char    id[PM_TAG_TAGID_LEN + 2],           /* tag id                       */
            seq[PM_TAG_MAX_SEQ_LEN + 2];        /* amino acid sequence          */

    float   mN,                                 /* N terminal Mass              */
            mC,                                 /* C terminal Mass              */
            mParent,                            /* Parent ion Mass              */
            score;                              /* tag score                    */
            
                                                /* ---------------------------- */
                                                /* the following fields are     */
                                                /* optional. They are not       */
                                                /* handled by the library and   */
                                                /* left to your responsability  */
                                                /* ---------------------------- */
												
    PMSpectrum *spectrum;                       /* parent spectrum pointer      */
    
} PMTag, *PMTagPtr;

/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/*
 * @doc: 
 * Allocate a new (empty) PMTag
 * @return PMTag*
 */

PMTag *PMNewTag ( void );

/*
 * @doc: 
 * Free a previously allocated PMTag structure.
 * note: also free subtags if any
 * @param tag PMTag *tag to free
 * @return NULL
 */

PMTag *PMFreeTag ( PMTag *tag );

/*
 * @doc:
 * Read a tag from buffer 
 * @param buffer char* to read in
 * @param tag PMTag* tag to read
 * @return error code depending on read issue
 * LX_NO_ERROR, LX_IO_ERROR
 */

int  PMParseTag ( char *buffer, PMTag *tag );

/*
 * @doc:
 * Read a tag from file 
 * @param streamin FILE* to read in
 * @param tag PMTag* tag to read
 * @return error code depending on read issue
 * LX_NO_ERROR, LX_IO_ERROR, LX_EOF
 */

int  PMReadTag ( FILE *streamin, PMTag *tag );

/*
 * @doc:
 * Write tag into file
 * @param streamou FILE* to write in
 * @param tag PMTag *tag to write
 * @return error code depending on write issue
 * LX_NO_ERROR, LX_IO_ERROR
 */

int  PMWriteTag  ( FILE *streamou, PMTag *tag );

/*
 * @doc: 
 * write binary data structure tag in file streamou
 * @param streamou FILE* binary opened file to write into
 * @param tag PMTag* ptr on tag structure to be dumped 
 * @return result int > 0 if writing operation succeed, 
 * else return null or negative value
 */

int PMBinWriteTag (FILE *streamou, PMTag *tag);

/*
 * @doc: 
 * read binary data structure tag from file streamin
 * @param streamin FILE* binary opened file to read into
 * @param tag PMTag* ptr on tag structure to be loaded 
 * @return result int > 0 if reading operation succeed, 
 * else return null or negative value 
 */

int PMBinReadTag (FILE *streamin, PMTag *tag);

/*
 * @doc:
 * [debug] print PMTag debug information
 * @param streamou FILE* to write into
 * @param tag PMTag* tag to write
 */

void PMDebugTag ( FILE *streamou, PMTag *tag );



#endif








