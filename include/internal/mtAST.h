
#ifndef mtAST_h
#define mtAST_h

#include <Mint.h>
#include "mtToken.h"

#define mtASTInitialChildCapacity 2

enum NodeType {
    NodeType_None,

    NodeType_Assignment,
    NodeType_BinaryOperator,
    NodeType_Number,
    NodeType_Identifier,
    NodeType_Block,
    
    NodeType_GreaterThan,
    NodeType_LesserThan,
    NodeType_GreaterThanOrEqual,
    NodeType_LesserThanOrEqual,

    NodeType_IsEqual,
    NodeType_IsNotEqual,

    NodeType_IfStatement,
    NodeType_FunctionDefinition,
    NodeType_FunctionCall,
    NodeType_ParameterList, // not an actual list
    NodeType_ArgumentList,

};

// abstract syntax tree
struct ASTNode {
    struct Token token;

    enum NodeType type;

    //pointer to multiple pointers
    struct ASTNode** children;
    size_t childCount;
    size_t childCapacity;
};


//@brief Adds child as a node to parent
//
//@returns The child's index.
int mtASTAddChildNode(struct ASTNode* parent, struct ASTNode* child);

//@brief adds an empty node to parent and returns it.
struct ASTNode* mtASTAddNode(struct ASTNode* parent);

//@brief Creates an empty node then returns it.
struct ASTNode* mtASTCreateNode();

//@brief Create a node with a token.
struct ASTNode* mtASTTokenCreateNode(struct Token token);

//@brief Frees node and all of its children.
void mtASTFree(struct ASTNode* node);

#endif
