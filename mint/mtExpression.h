
#ifndef mtExpression_h
#define mtExpression_h

#include "mtAST.h"
#include "mtScope.h"

void interpretStatement(struct ASTNode* node, struct mtScope* scope);
struct mtObject* interpretExpression(struct ASTNode* node, struct mtScope* scope);

#endif
