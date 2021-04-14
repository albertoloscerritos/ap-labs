#include <stdio.h>

int infof(const char *format)
{
    printf("\x1b[42m INFO \x1b[0m");
    printf("\x1b[32m %s \x1b[0m  \n", format);
    return 1;
}

int warnf(const char *format)
{
    printf("\x1b[30;43m WARNING \x1b[0m");
    printf("\x1b[33m %s \x1b[0m  \n", format);
    return 0;
}

int panicf(const char *format)
{
    printf("\x1b[40m PANIC \x1b[0m");
    printf("\x1b[30m %s \x1b[0m  \n", format);
    return 0;
}

int errorf(const char *format)
{
    printf("\x1b[41m ERROR \x1b[0m");
    printf("\x1b[31m %s \x1b[0m  \n", format);
    return 0;
}