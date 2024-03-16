#include <stdio.h>

int main()
{
    int x = 10;
    int y = 20;

    asm (
        "addl %[y],%[x];"
        :[x]"+r"(x)
        :[y]"r"(y)
    );

    printf("x + y = %d\n", x);

    return 0;
}
