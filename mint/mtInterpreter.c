#include "mtInterpreter.h"

#include "mtBlock.h"

void mtInterpret(struct ASTNode* node)
{
    interpretBlock(node, NULL);
}
