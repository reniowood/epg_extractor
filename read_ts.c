#include <stdio.h>

int main() {
    FILE *in;
    int ch;

    in = fopen("ch44-PSB1-sudbury.ts", "rb");

    while ((ch = fgetc(in)) != EOF)
        printf("%02X ", ch);
    putchar('\n');

    fclose(in);

    return 0;
}
