#include "mtScope.h"

struct Scope* mtCreateScope()
{
    struct Scope* scope = malloc(sizeof(struct Scope));

    scope->parent = NULL;
    scope->variables = hashmap_create(2);
    scope->functions = hashmap_create(2);

    return scope;
}
