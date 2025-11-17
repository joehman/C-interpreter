
#ifndef mtScope_h
#define mtScope_h

#include <util/hashmap.h>

struct Scope {
    struct Scope* parent;    
    struct HashMap* variables;
    struct HashMap* functions;
};

struct Scope* mtCreateScope();

#endif
