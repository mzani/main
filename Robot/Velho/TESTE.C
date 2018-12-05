#include "8085io.h"
#include "8085int.h"
int flag;
INTERRUPT (32820) int1() {
   if (flag == 0) { 
     out (0xf0, 0x40);
     flag = 1;
   } else {
     out (0xf0, 0x41);
     flag = 0;
   }
}

main () {
  out (0xf0, 0x52);
  out (0xf0, 0x32);
  out (0xf0, 0x50);
  asm {
    mvi A,$1D
    sim
  }
  enable();
  while (in(0xe8) == 0) {
    out (0xf0, 0x22);
    printf("%02x\r\n", in(0xf0));
  }
  disable();
  out (0xf0, 0x51);
  out (0xf0, 0x41);
}
