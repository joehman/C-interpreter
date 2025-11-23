
#ifndef mtVariable_h
#define mtVariable_h

#include <interpreter/mtScope.h>
#include <types/mtType.h>

struct mtObject {
    struct Type type; 
    void* value;
};

struct mtObject* mtCreateVariable(struct Type type);
struct mtObject* getVariableFromScope(struct mtScope* scope, const char* key);

#endif
