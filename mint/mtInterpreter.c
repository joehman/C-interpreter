#include "mtInterpreter.h"

#include <interpreter/mtBlock.h>


void mtInterpret(struct ASTNode* node)
{
    interpretBlock(node, NULL);
}
