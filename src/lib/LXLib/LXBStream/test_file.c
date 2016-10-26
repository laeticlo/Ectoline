/* test ByteStream FILE */

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
  } block;
  
  FILE *stream;
  LXBStream *bstream;
  
  stream = LXOpenFile("test_little_endian.in", "rb");

  bstream = LXNewBStreamFile(stream, TRUE);

  LXBStreamReadInt32(bstream, &block.i32);
  LXBStreamReadInt16Array(bstream, block.i16, 2);
  LXBStreamReadFloat32(bstream, &block.flt);
  LXBStreamReadCharArray(bstream, block.ch, 4);
  LXBStreamReadFloat64(bstream, &block.dbl);

  (void) printf("-- little endian -- \n");
  (void) printf("i32 %d\n", block.i32);
  (void) printf("i16 %d %d\n", block.i16[0], block.i16[1]);
  (void) printf("flt %.3f\n", block.flt);
  (void) printf("ch  %c %c %c %c\n", block.ch[0], block.ch[1], block.ch[2], block.ch[3]);
  (void) printf("dbl %.3f\n", block.dbl);
  (void) printf("-- -- \n");
  
  fclose(stream);
  
  stream = LXOpenFile("test_little_endian.bak", "wb");
  
  LXBStreamWriteInt32(bstream, &block.i32);
  LXBStreamWriteInt16Array(bstream, block.i16, 2);
  LXBStreamWriteFloat32(bstream, &block.flt);
  LXBStreamWriteCharArray(bstream, block.ch, 4);
  LXBStreamWriteFloat64(bstream, &block.dbl);

  fclose(stream);
  

  (void) LXFreeBStream(bstream);
  
  // -----------------------------------
  
  stream = LXOpenFile("test_big_endian.in", "rb");

  bstream = LXNewBStreamFile(stream, FALSE);

  LXBStreamReadInt32(bstream, &block.i32);
  LXBStreamReadInt16Array(bstream, block.i16, 2);
  LXBStreamReadFloat32(bstream, &block.flt);
  LXBStreamReadCharArray(bstream, block.ch, 4);
  LXBStreamReadFloat64(bstream, &block.dbl);

  (void) printf("-- big endian -- \n");
  (void) printf("i32 %d\n", block.i32);
  (void) printf("i16 %d %d\n", block.i16[0], block.i16[1]);
  (void) printf("flt %.3f\n", block.flt);
  (void) printf("ch  %c %c %c %c\n", block.ch[0], block.ch[1], block.ch[2], block.ch[3]);
  (void) printf("dbl %.3f\n", block.dbl);
  (void) printf("-- -- \n");

  fclose(stream);
  
  stream = LXOpenFile("test_big_endian.bak", "wb");
  
  LXBStreamWriteInt32(bstream, &block.i32);
  LXBStreamWriteInt16Array(bstream, block.i16, 2);
  LXBStreamWriteFloat32(bstream, &block.flt);
  LXBStreamWriteCharArray(bstream, block.ch, 4);
  LXBStreamWriteFloat64(bstream, &block.dbl);

  fclose(stream);

  (void) LXFreeBStream(bstream);
  
  exit(0);  
}
