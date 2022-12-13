#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main( )
{
    char *str;
    str = (char *)malloc(sizeof(char) * 5);     // 5개의 공간을 만들고 12문자 할당
    strcpy(str, "Hello World\n"); 

    return 0;  
}

