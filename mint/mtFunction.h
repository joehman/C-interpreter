
#ifndef mtFunction_h
#define mtFunction_h

#include <Mint.h>

#include "mtScope.h"
#include "mtAST.h"

struct Parameter {
    const char* identifier;
    struct Type* type; // could be NULL
};

struct mtFunction {
    struct ASTNode* block;

    const char* identifier;
    
    size_t parameterCount; 
    struct Parameter* parameters;
};

struct mtObject* interpretFunctionCall(struct ASTNode* node, struct mtScope* scope, bool* wasFunc);
void interpretFunctionDef(struct ASTNode* node, struct mtScope* scope);

#endif
