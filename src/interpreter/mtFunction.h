
#ifndef mtFunction_h
#define mtFunction_h

#include <interpreter/mtScope.h>
#include <util/mtUtilities.h>
#include <parser/mtASTTree.h>

struct Parameter {
    const char* identifier;
    struct Type* type; // could be NULL
};

struct Function {
    struct ASTNode* block;

    const char* identifier;
    
    size_t parameterCount; 
    struct Parameter* parameters;
};

struct Function* getFunctionFromScope(struct Scope* scope, const char* key);
struct Variable* interpretFunctionCall(struct ASTNode* node, struct Scope* scope, bool* wasFunc);
void interpretFunctionDef(struct ASTNode* node, struct Scope* scope);

#endif
