
#include <Mint.h>


struct mtObject* mtCreateObject(struct Type type)
{
    struct mtObject* var = malloc(sizeof(struct mtObject));
    var->data = malloc(type.size);
    var->type = type;

    memset(var->data, 0, var->type.size);

    return var;
}

