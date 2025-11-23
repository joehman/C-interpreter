
#ifndef mtBlock_h
#define mtBlock_h

#include <parser/mtASTTree.h>
#include <interpreter/mtScope.h>

void interpretBlock(struct ASTNode* node, struct mtScope* parent);

#endif
