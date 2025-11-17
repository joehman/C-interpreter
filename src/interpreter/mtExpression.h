
#ifndef mtExpression_h
#define mtExpression_h

#include <parser/mtASTTree.h>
#include <interpreter/mtScope.h>

void interpretStatement(struct ASTNode* node, struct Scope* scope);
struct Variable* interpretExpression(struct ASTNode* node, struct Scope* scope);

#endif
