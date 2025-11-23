#include "mtScope.h"

struct mtScope* mtCreateScope()
{
    struct mtScope* scope = malloc(sizeof(struct mtScope));

    scope->parent = NULL;
    scope->variables = mtHashMapCreate(2);
    scope->functions = mtHashMapCreate(2);

    return scope;
}
