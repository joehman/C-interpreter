
#include "mtVariable.h"

struct Variable* mtCreateVariable(struct Type type)
{
    struct Variable* var = malloc(sizeof(struct Variable));
    var->value = malloc(type.size);
    var->type = type;

    memset(var->value, 0, var->type.size);

    return var;
}

struct Variable* getVariableFromScope(struct Scope* scope, const char* key)
{
    if (!scope)
    {
        return NULL;
    }

    struct Variable* out = NULL;

    if ( (out = hashmap_get(scope->variables, key)) )
    {
        return out;
    }
    
    struct Scope* currentScope = scope;
    while(currentScope)
    {
        if ( (out = hashmap_get(currentScope->variables, key)) )
        {
            return out;
        }
        
        // check the scope above
        currentScope = currentScope->parent;
    }

    return NULL;
}
