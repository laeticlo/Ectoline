/* -----------------------------------------------------------------------
 * $Id: LXXml.h 1793 2013-02-24 02:19:06Z viari $
 * -----------------------------------------------------------------------
 * @file: LXXml.h
 * @desc: XML utilities (expat wrapper)
 * 
 * @history:
 * @+ <Gloup> : Jan 10 : first version
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * XML utilities (expat wrapper)<br>
 * this library requires ThirdParty/Expat
 *
 * @docend:
 */

#ifndef _H_LXXml
#define _H_LXXml

#include <stdio.h>

#include "LXSys.h"
#include "LXStkp.h"
#include "LXString.h"
#include "LXArray.h"
#include "LXBStream.h"

/* ---------------------------------------------------- */
/* Constants                                            */
/* ---------------------------------------------------- */

#define LX_XML_PARSE_UNKNOWN    0
#define LX_XML_PARSE_INITED     1
#define LX_XML_PARSE_PARSING    2
#define LX_XML_PARSE_FINISHED   3
#define LX_XML_PARSE_SUSPENDED  4
#define LX_XML_PARSE_ABORTED    5

#define LX_XML_PARSE_ERROR      20
#define LX_XML_STATE_ERROR      21

#define LX_XML_PARSER_DFT_BUFSIZE   1024

/* ---------------------------------------------------- */
/* Structures                                           */
/* ---------------------------------------------------- */

// forward definition

typedef struct LXXmlParser LXXmlParser;

/*
 * @doc: Attribute
 * - name   : attribute name
 * - value  : attribute value
 */
 
 typedef struct {
    char *name;
    char *value;
} LXXmlAttribute;

/*
 * @doc: Element
 * - name         : element name
 * - nattr        : number of attributes
 * - attr         : array of attributes
 * - value        : dynamic buffer of XML_Char
 *
 * note: by default an XML_Char (defined in expat) is an UTF8 encoded
 *       unsigned char. This may be changed (typically to UTF16 unsigned
 *       short) in expat configuration (you have to recompile expat).
 */

typedef struct {
    char            *name;
    int              nattr;
    LXXmlAttribute **attr; 
    LXArray         *value;
} LXXmlElement;

/*
 * @doc: Element Handler
 * - 
 */

typedef struct {
    void (*startElt)(LXXmlParser *parser, LXXmlElement *elt, void *userarg);
    void (*endElt)(LXXmlParser *parser, LXXmlElement *elt, void *userarg);
} LXXmlHandler;

/*
 * @doc: XML Parser
 * - expat        : internal expat parser 
 * - handler      : user's handler
 * - state        : current state : LX_XML_PARSE_UNKNOWN | LX_XML_PARSE_INITED | LX_XML_PARSE_FINISHED | LX_XML_PARSE_PARSING | LX_XML_PARSE_SUSPENDED | LX_XML_PARSE_ABORTED
 * - userarg      : user's argument to handler
 * - bstream      : input byte stream 
 * - bufsize      : default input buffer size
 * - remblanks    : should remove leading and trailing blanks in element value
 * - stack        : - internal - stack of current LXXmlElements
 * - buffer       : - internal - current input buffer
 */

struct LXXmlParser {
    void         *expat;
    LXXmlHandler  handler;
    int           state;
    void         *userarg;
    LXBStream    *bstream;
    size_t        bufsize;
    Bool          remblanks;
    LXStackp     *stack;
    char         *buffer;
};

/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/*
 * @doc: todo
 * @param
 * @return
 */

LXXmlAttribute *LXFreeXmlAttribute(LXXmlAttribute *att);

/*
 * @doc: todo
 * @param
 * @return
 */

LXXmlAttribute *LXNewXmlAttribute(const char *name, const char *value);

/*
 * @doc: todo
 * @param
 * @return
 */

LXXmlAttribute *LXCopyXmlAttribute(const LXXmlAttribute *att);

/*
 * @doc: todo
 * @param
 * @return
 */

LXXmlElement *LXFreeXmlElement(LXXmlElement *elt);

/*
 * @doc: todo
 * @param
 * @return
 */

LXXmlElement *LXNewXmlElement(const char *name, const char **attr);

/*
 * @doc: todo
 * @param
 * @return
 */

LXXmlElement *LXCopyXmlElement(const LXXmlElement *elt);

/*
 * @doc: todo
 * @param
 * @return
 */

int LXCleanupXmlBuffer(LXArray *buf, int (*isSpace)(int c));

/*
 * @doc: todo
 * @param
 * @return
 */

LXArray *LXGetXmlCharBuffer(LXArray *buf);

/*
 * @doc: todo
 * @param
 * @return
 */

LXXmlParser *LXFreeXmlParser(LXXmlParser *parser);

/*
 * @doc: todo
 * @param
 * @return
 */

LXXmlParser *LXNewXmlParser(LXBStream *bstream, LXXmlHandler handler,
                            void *userarg, size_t bufsize, Bool remblanks);

/*
 * @doc: todo
 * @param
 * @return LX_xxx_ERROR
 */

int LXXmlParse(LXXmlParser *parser);

/*
 * @doc: todo
 * @param
 * @return LX_xxx_ERROR
 * to resume just call LXXmlParse again
 */

int LXXmlSuspend(LXXmlParser *parser);

#endif
