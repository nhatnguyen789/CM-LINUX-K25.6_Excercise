#include<stdio.h>
#include "strutils.h"

void main()
{
    char str1[20];
    char str2[10];

    printf("Enter a string:");
    fgets(str1, sizeof(str1), stdin);

    str_trim(str1);
    printf("Trimmed string: '%s'\n", str1);

    str_reverse(str1);
    printf("Reversed string: '%s'\n", str1);

    printf("Enter another string:");
    fgets(str2, sizeof(str2), stdin);

    str2[strcspn(str2, "\n")] = '\0';  // Delete newline character 
    int num = str_to_int(str2);
    if(num == 0 && str2[0] != '0')
    {
        printf("Invalid integer string: '%s'\n", str2);
    }
    else
    {
        printf("Converted integer: %d\n", num);
    }
}