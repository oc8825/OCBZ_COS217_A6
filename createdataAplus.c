/* createdataAplus.c
 *
 * Stub-injector for the grader.  Declares everything up front
 * so you don’t mix declarations and code under ISO C90.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "miniassembler.h"

enum { BUFSIZE = 48 };

int main(void) {
    /* --- declarations (must come before any statements under C90) --- */
    FILE               *f;
    int                 i;
    unsigned long       pc;
    unsigned int        instr;
    const unsigned long NAME_ADDR = 0x420058UL;        /* grader’s name[] */
    const unsigned long SKIP_ADDR = 0x4008c4UL;        /* fill in: addr right after their final printf */
    const unsigned int  SVC_INST  = 0xd4000001;        /* encoding for `svc #0` */
    const char         *msg       = "A+ is your grade\nThank you, BZOC\n";
    unsigned            msglen    = (unsigned)strlen(msg);
    int                 pad;
    int                 stub_size;

    /* --- code starts here --- */
    f = fopen("dataAplus", "w");
    if (!f) {
        perror("fopen dataAplus");
        return 1;
    }

    /* build the 48-byte stub at name[]: */
    pc = NAME_ADDR + 28;  /* stub’s runtime start */

    /* 1) write(1, msg, msglen) */
    instr = MiniAssembler_mov(8, 64);                    fwrite(&instr,4,1,f);  pc += 4;
    instr = MiniAssembler_mov(0, 1);                     fwrite(&instr,4,1,f);  pc += 4;
    instr = MiniAssembler_adr(1, NAME_ADDR + BUFSIZE, pc); fwrite(&instr,4,1,f);  pc += 4;
    instr = MiniAssembler_mov(2, msglen);                fwrite(&instr,4,1,f);  pc += 4;
    fwrite(&SVC_INST,          4,1,f);                   pc += 4;

    /* 2) branch past the grader’s own prints */
    instr = MiniAssembler_b(SKIP_ADDR, pc);               fwrite(&instr,4,1,f);  pc += 4;

    /* 3) pad out to 48 bytes */
    stub_size = (int)(pc - (NAME_ADDR + 28));
    pad       = BUFSIZE - stub_size;
    for (i = 0; i < pad; i++) {
        fputc('\0', f);
    }

    /* 4) append the literal message bytes immediately after */
    fwrite(msg, 1, msglen, f);

    fclose(f);
    return 0;
}
