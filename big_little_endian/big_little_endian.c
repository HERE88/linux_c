#include <stdio.h>

int main()
{
    union test
    {
        unsigned char a[4];
        unsigned int b;
    }c;

    c.b = 1;

    if(c.a[0] == 0 && c.a[3] == 1)
        printf("big endian!\n");
    else if(c.a[0] == 1 && c.a[3] == 0)
        printf("little endian!\n");
    else
        printf("error!\n");

    return 0;

}