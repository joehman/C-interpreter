
#ifndef mtInterpreterError_h
#define mtInterpreterError_h

#include "mtAST.h"

#include <stdarg.h>
#include <stdio.h>

static void interpreterError(struct ASTNode* node, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "Error while interpreting on line %d: \n\t", 
            node->token.line 
    );
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    if (fmt[strlen(fmt)] != '\n')
    {
        printf("\n");
    }
}

#endif
