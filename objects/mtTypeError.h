

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static void typeError(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    if (fmt[strlen(fmt)] != '\n')
    {
        printf("\n");
    }
}
