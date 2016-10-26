/* -----------------------------------------------------------------------
 * $Id: LXBase64.c 1791 2013-02-24 02:17:56Z viari $
 * -----------------------------------------------------------------------
 * @file: LXBase64.c
 * @desc: Base64 codec - RFC4648 (http://tools.ietf.org/html/rfc4648)
 * 
 * @history:
 * @+ <Gloup> : Jan 10 : first version
 * -----------------------------------------------------------------------
 * @docstart:
 *
 * Base64 codec library<br>
 *
 * @docend:
 */

#include <stdlib.h>
#include <string.h>

#include "LX/LXMach.h"
#include "LX/LXSys.h"
#include "LX/LXBase64.h"

/* ==================================================== */
/* internal BASE64 Codes                                */
/* ==================================================== */

static const char *B64_ENCODE_ = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const int  BASE64_DECODE_[] = {
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63, 
   52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, 
   -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 
   15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, 
   -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 
   41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
};

static const unsigned int BASE64_UDECODE_[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62,  0,  0,  0, 63, 
   52, 53, 54, 55, 56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0, 
    0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 
   15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,  0,  0,  0,  0, 
    0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 
   41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,  0,  0,  0,  0,  0, 
};

static char sErrBuff[256];

/* -------------------------------------------- */
/* error messages                               */
/* -------------------------------------------- */

static int sInvalidSourceLength(const char *where, size_t len) {
    (void) sprintf(sErrBuff, "%s : source length (%zu) should be multiple of 4 bytes\n",
                   where, len);
    LX_WARNING(sErrBuff);
    return LX_B64_BAD_LEN;
}

static int sInvalidCharacter(const char *where, char c, size_t pos) {
    (void) sprintf(sErrBuff, "%s : invalid Base64 character 0x%x (%c) at position %zu\n",
                   where, c, c, pos);
    LX_WARNING(sErrBuff);
    return LX_B64_BAD_CHAR;
}

/* ==================================================== */
/* Base64 encoding                                      */
/* ==================================================== */

size_t LXBase64Encode(void *ssrc, size_t size, char *dst)
{
    size_t i, rest, dsize;
    unsigned char *src = (unsigned char *) ssrc;
    
    rest  = size % 3;
    size -= rest;
    dsize = (size / 3) * 4;

    /* ---------------------------------------- */
    /* conversion proceeds upstream             */
    /* to allow dst = src                       */

    src += size - 3;
    dst += dsize - 4;

    /* ---------------------------------------- */
    /* proceed with padding chars first         */
    
    if (rest) {

        UInt32 x = 0;
    
        src += 3;
        dst += 4;

        for (i = 0 ; i < 3 ; i++)
          x = (x << 8) | (i < rest ? src[i] : 0);

        dst[0] = B64_ENCODE_[(x >> 18) & 0x3F];
        dst[1] = B64_ENCODE_[(x >> 12) & 0x3F];
        dst[2] = (rest == 2 ? B64_ENCODE_[(x >> 6) & 0x3F] : '=');
        dst[3] = '=';

        src -= 3;
        dst -= 4;
    }

    /* ---------------------------------------- */
    /* proceed with all other data              */

    for (i = 0 ; i < size ; i += 3) {

        UInt32 x = (src[0] << 16) | (src[1] << 8) | src[2];
        
        dst[0] = B64_ENCODE_[(x >> 18) & 0x3F];
        dst[1] = B64_ENCODE_[(x >> 12) & 0x3F];
        dst[2] = B64_ENCODE_[(x >> 6)  & 0x3F];
        dst[3] = B64_ENCODE_[ x        & 0x3F];
        
        src -= 3;
        dst -= 4;
    }

    dst += 4;

    dsize += (rest ? 4 : 0);
    
    dst[dsize] = '\000';    
   
    return dsize;
}

/* ==================================================== */
/* Base64 decoding                                      */
/* ==================================================== */

size_t LXBase64Decode(char *src, void *ddst)
{
    size_t i, len;
    int pad;
    unsigned char *dst = (unsigned char *) ddst;
    
    if (*src == '\000') /* empty source */
      return 0;

    len = strlen(src);
    
    /* -------------------- */
    /* check input length   */
    
    if ((len % 4) != 0) {
      return sInvalidSourceLength("LXBase64Decode", len);
    }

    pad = 0;
    if (src[len-1] == '=') {
      pad = 1;
      if (src[len-2] == '=')
        pad = 2;
    }

    /* -------------------- */
    /* check input chars    */

    for (i = 0 ; i < (len - pad) ; i++) {
      int s = src[i];
      if ((s < 0) || (BASE64_DECODE_[s] < 0)) {
        return sInvalidCharacter("LXBase64Decode", s, i);
      }
    }

    /* ------------------------ */
    /* go ahead with conversion */

    for (i = 0 ; i < len ; i += 4) {

        UInt32 x =   (BASE64_UDECODE_[((int) src[0])] << 18)
                   | (BASE64_UDECODE_[((int) src[1])] << 12)
                   | (BASE64_UDECODE_[((int) src[2])] << 6)
                   | (BASE64_UDECODE_[((int) src[3])]);

        dst[0] = (x >> 16) & 0xFF;
        dst[1] = (x >> 8)  & 0xFF;
        dst[2] =  x        & 0xFF;
        
        src += 4;
        dst += 3;
    }

    len = (len / 4) * 3 - pad;
   
    return len;
}
