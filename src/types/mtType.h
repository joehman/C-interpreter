
#ifndef MT_TYPE_H
#define MT_TYPE_H

#include <util/mtUtilities.h>
#include <stdarg.h>

struct Type {
    size_t size;
    
    // set a to b
    void (*set)(void*, void*);

    void* (*add)(void*, void*);
    void* (*sub)(void*, void*);
    void* (*mul)(void*, void*);
    void* (*div)(void*, void*);

    char* (*str)(void*);
};

static void typeError(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "TypeError: \n\t");
    vfprintf(stderr, fmt, args);

    if (fmt[strlen(fmt)] != '\n')
    {
        printf("\n");
    }
}

#endif //MT_TYPE_H
