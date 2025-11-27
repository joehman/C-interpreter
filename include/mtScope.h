
#ifndef mtScope_h
#define mtScope_h

#include "mtHashmap.h"

// totally arbitrary
#define mtScopeDefaultSize 8

struct mtScope {
    struct mtScope* parent;    
    struct mtHashMap* variables;
    struct mtHashMap* functions;
};

struct mtScope* mtCreateScope();

struct mtObject* getObjectFromScope(struct mtScope* scope, const char* key);
struct mtFunction* getFunctionFromScope(struct mtScope* scope, const char* key);

#endif
