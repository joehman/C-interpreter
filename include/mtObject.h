
#ifndef mtVariable_h
#define mtVariable_h

#include <stdlib.h> // for size_t
#include <stdbool.h> // for bool

struct Type {
    size_t size;
    
    // set a to b
    void (*set)(void*, void*);

    void* (*add)(void*, void*);
    void* (*sub)(void*, void*);
    void* (*mul)(void*, void*);
    void* (*div)(void*, void*);

    bool (*isEqual)(void*, void*);
    bool (*isGreater)(void*, void*);
    bool (*isLesser)(void*, void*);

    char* (*str)(void*);
};

struct mtObject {
    struct Type type; 
    void* data;
};

struct mtObject* mtCreateObject(struct Type type);


#endif
