
#ifndef mtVariable_h
#define mtVariable_h

#include <interpreter/mtScope.h>
#include <types/mtType.h>

struct Variable {
    struct Type type; 
    void* value;
};

struct Variable* mtCreateVariable(struct Type type);
struct Variable* getVariableFromScope(struct Scope* scope, const char* key);

#endif
