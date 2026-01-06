#ifndef mtIfStatement_h
#define mtIfStatement_h

#include "mtAST.h"
#include "mtScope.h"
#include "mtUtilities.h"

#define mtWasNotConditional -1


void interpretIfStatement(struct ASTNode* node, struct mtScope* scope);

int interpretConditional(struct ASTNode* node, struct mtScope* scope, bool* result);

#endif
