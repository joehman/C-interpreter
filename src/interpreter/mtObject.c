
#include "mtObject.h"

struct mtObject* mtCreateVariable(struct Type type)
{
    struct mtObject* var = malloc(sizeof(struct mtObject));
    var->value = malloc(type.size);
    var->type = type;

    memset(var->value, 0, var->type.size);

    return var;
}

struct mtObject* getVariableFromScope(struct mtScope* scope, const char* key)
{
    if (!scope)
    {
        return NULL;
    }

    struct mtObject* out = NULL;

    if ( (out = mtHashMapGet(scope->variables, key)) )
    {
        return out;
    }
    
    struct mtScope* currentScope = scope;
    while(currentScope)
    {
        if ( (out = mtHashMapGet(currentScope->variables, key)) )
        {
            return out;
        }
        
        // check the scope above
        currentScope = currentScope->parent;
    }

    return NULL;
}
