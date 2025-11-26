
#ifndef mtVariable_h
#define mtVariable_h

#include <stdlib.h> // for size_t

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

struct mtObject {
    struct Type type; 
    void* value;
};

struct mtObject* mtCreateVariable(struct Type type);

#endif
