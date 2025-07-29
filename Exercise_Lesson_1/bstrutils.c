#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#include<errno.h>
#include "strutils.h"

void str_reverse(char *str)
{
    int len = strlen(str);
    for(int i = 0; i < len/2; i++)
    {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}
void str_trim(char *str)
{
    int start = 0;
    while(isspace((unsigned char)str[start]))
    {
        start++;
    }

    int end = strlen(str) - 1;

    while(end >= start && isspace((unsigned char)str[end]))
    {
        end--;
    }

    int i = 0;
    while(start <= end)
    {
        str[i++] = str[start++];
    }
    str[i] = '\0';
}
int str_to_int(const char *str)
{
    char *endptr;
    errno = 0;
    long val = strtol(str, &endptr, 10);

    //check errors
    if(errno != 0 || *endptr != '\0' || endptr == str)
    {
        return 0;
    }
    return (int)val;
}
