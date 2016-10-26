/* -----------------------------------------------------------------------
 * $Id: LXBase64.h 888 2010-03-12 08:21:35Z viari $
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

#ifndef _H_LXBase64
#define _H_LXBase64

#ifndef _H_LXTypes
#include "LXTypes.h"
#endif

#ifndef _H_LXMacros
#include "LXMacros.h"
#endif

/* ---------------------------------------------------- */
/* Constants                                            */
/* ---------------------------------------------------- */

/*
 * @docstart:
 * @par Error codes
 * - LX_B64_BAD_LENGTH : invalid source length
 * - LX_B64_BAD_CHAR   : invalid base64 character
 * @docend:
 */

#define LX_B64_BAD_LEN     -1
#define LX_B64_BAD_CHAR    -2

/* ---------------------------------------------------- */
/* Prototypes                                           */
/* ---------------------------------------------------- */

/*
 * @doc: 
 * encode array into base64 string.
 *
 * this is the base encoder. 
 *   float floatArray[size];
 *   char  dst[2*size];
 *   lenbytes = b64_encode(floatArray, size * sizeof(float), dst);
 *
 * if endianess has to be changed, then use LX_ENDIAN_SWAP_ADR
 * from the LXSys library before running the encoder. 
 *   LX_ENDIAN_SWAP_ADR(float, floatArray, size);
 *
 *   note: to check machine endianess, use the LXIsLittleEndian() function from
 *         the LXSys library.
 *
 * note: this function also work with dst == src. so you may use as :
 *  lenbytes = b64_encode(floatArray, 10 * sizeof(float), (char *) floatArray);
 *
 * note: dst is NULL terminated.
 *
 * @param src void* source array
 * @param size size_t length (in bytes) of source array
 * @param dst char* destination string of base64 encoded characters (should be of proper size)
 * @return length (in bytes) of dst and a negative value on error (see error codes)
 *
 * note: dst should be at least ceil(size / 3) * 4 + 1 bytes, but it is more safe
 *       to use 2 * size.
 */
 
size_t LXBase64Encode(void *src, size_t size, char *dst);


/*
 * @doc: 
 * decode base64 encoded string to unsigned char array of values.
 *
 * this is the base decoder. 
 * if you don't care about endianess, you may just cast the destination array
 * to the desired type.
 *   float *floatArray = (float*) malloc(size);
 *   lenbytes = b64_decode(src, floatArray);
 * if endianess has to be changed, then further use LX_ENDIAN_SWAP_ADR
 * from the LXSys library.
 *   LX_ENDIAN_SWAP_ADR(float, floatArray, lenbytes/sizeof(float));
 *
 *   note: to check machine endianess, use the LXIsLittleEndian() function from
 *         the LXSys library.
 *
 * note: this function also work with dst == src. so you may use as :
 *  len = b64_decode(src, src);
 *  floatArray = (float *) src;
 *
 * @param src char* base64 encoded characters
 * @param dst void* destination array (should be of proper size)
 * @return length (in bytes) of dst and a negative value on error (see error codes)
 */

size_t LXBase64Decode(char *src, void *dst);

#endif
