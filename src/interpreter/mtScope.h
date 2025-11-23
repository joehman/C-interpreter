
#ifndef mtScope_h
#define mtScope_h

#include <util/hashmap.h>

struct mtScope {
    struct mtScope* parent;    
    struct mtHashMap* variables;
    struct mtHashMap* functions;
};

struct mtScope* mtCreateScope();

#endif
