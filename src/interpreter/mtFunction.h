
#ifndef mtFunction_h
#define mtFunction_h

#include <interpreter/mtScope.h>
#include <util/mtUtilities.h>
#include <parser/mtASTTree.h>

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

struct mtFunction* getFunctionFromScope(struct mtScope* scope, const char* key);
struct mtObject* interpretFunctionCall(struct ASTNode* node, struct mtScope* scope, bool* wasFunc);
void interpretFunctionDef(struct ASTNode* node, struct mtScope* scope);

#endif
