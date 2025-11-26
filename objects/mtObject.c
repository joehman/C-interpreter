
#include <Mint.h>


struct mtObject* mtCreateVariable(struct Type type)
{
    struct mtObject* var = malloc(sizeof(struct mtObject));
    var->value = malloc(type.size);
    var->type = type;

    memset(var->value, 0, var->type.size);

    return var;
}

