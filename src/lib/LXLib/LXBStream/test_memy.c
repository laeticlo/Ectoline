/* test ByteStream MEMORY */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LX/LXBStream.h"

main() {

  struct {
    Int32   i32;
    Int16   i16[2];
    Float32 flt;
    char    ch[4];
    Float64 dbl;
    char    str[20];
  } blick, block;
  
  LXBStream *bstream;

  blick.i32    = -12345678;
  blick.i16[0] = 123;
  blick.i16[1] = 234;
  blick.flt    = -1.234;
  blick.ch[0]  = 'a';
  blick.ch[1]  = 'b';
  blick.ch[2]  = 'c';
  blick.ch[3]  = 'd';
  blick.dbl    = -1.234;
  (void) strcpy(blick.str, "hello you");

  bstream = LXNewBStreamMemory(LX_BSTREAM_READ, 0, LXIsLittleEndian());
  (void) LXMemCopyToBStream(bstream, &blick, sizeof(blick));

  LXBStreamReadInt32(bstream, &block.i32);
  LXBStreamReadInt16Array(bstream, block.i16, 2);
  LXBStreamReadFloat32(bstream, &block.flt);
  LXBStreamReadCharArray(bstream, block.ch, 4);
  LXBStreamReadFloat64(bstream, &block.dbl);

  (void) printf("-- blick->block -- \n");
  (void) printf("i32 %d\n", block.i32);
  (void) printf("i16 %d %d\n", block.i16[0], block.i16[1]);
  (void) printf("flt %.3f\n", block.flt);
  (void) printf("ch  %c %c %c %c\n", block.ch[0], block.ch[1], block.ch[2], block.ch[3]);
  (void) printf("dbl %.3f\n", block.dbl);
  (void) printf("-- -- \n");

  (void) LXFreeBStream(bstream);

  bstream = LXNewBStreamMemory(LX_BSTREAM_WRITE, 0, LXIsLittleEndian());
  
  LXBStreamWriteInt32(bstream, &block.i32);
  LXBStreamWriteInt16Array(bstream, block.i16, 2);
  LXBStreamWriteFloat32(bstream, &block.flt);
  LXBStreamWriteCharArray(bstream, block.ch, 4);
  LXBStreamWriteFloat64(bstream, &block.dbl);

  (void) LXMemCopyFromBStream(bstream, &blick, sizeof(blick));

  (void) printf("-- block->blick -- \n");
  (void) printf("i32 %d\n", blick.i32);
  (void) printf("i16 %d %d\n", blick.i16[0], blick.i16[1]);
  (void) printf("flt %.3f\n", blick.flt);
  (void) printf("ch  %c %c %c %c\n", blick.ch[0], blick.ch[1], blick.ch[2], blick.ch[3]);
  (void) printf("dbl %.3f\n", blick.dbl);
  (void) printf("-- -- \n");

  (void) LXFreeBStream(bstream);
  
  exit(0);  
}
