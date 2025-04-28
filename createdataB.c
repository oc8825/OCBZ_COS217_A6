#include <stdio.h>

int main(void)
{
    FILE *psFile;
    psFile = fopen("dataB", "w");

    fprintf(psFile, "Owen Clarke \n");

    fclose(psFile);

    return 0;
}