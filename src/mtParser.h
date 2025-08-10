/*
* The Parser takes in all the different tokens the tokenizer made,
* and parses them all into an Abstract Syntax Tree(AST), which the
* intepreter then runs.
*/


#ifndef mtParser_h
#define mtParser_h

#include "mtUtilities.h"
#include "mtTokenization.h"
#include <stdlib.h>

// ______________ Declaration _____________

// abstract syntax tree
struct ASTNode {
    struct Token token;

    //pointer to multiple pointers
    struct ASTNode** children;
    size_t childCount;
};

//@returns the root node of the AST.
struct ASTNode* mtASTParseTokens(struct Token* tokens, size_t tokenCount);

//@brief Adds child as a node to parent
//
//@returns Its index.
int mtASTAddChildNode(struct ASTNode* parent, struct ASTNode* child);


struct ASTNode* mtASTAddNode(struct ASTNode* parent);

//@brief Creates an empty node then returns it.
struct ASTNode*  mtASTCreateNode();

//@brief Frees node and all of its children.
void mtASTFree(struct ASTNode* node);

// ________________ Implementation ________________

#ifdef mtImplementation

struct ASTNode* mtASTParseTokens(struct Token* tokens, size_t tokenCount)
{
    
    struct ASTNode* rootNode = mtASTCreateNode();

    struct ASTNode* currentNode = rootNode;

    bool isFirst = true;

    for (size_t i = 0; i < tokenCount; i++)
    {

        if (tokens[i].type == TokenType_Ignore || tokens[i].type == TokenType_None )
        {
            continue;
        }

        if (tokens[i].type == TokenType_NumberLiteral && currentNode->childCount == 0)
        {
            currentNode->token = tokens[i];
            continue;
        }
        if (tokens[i].type == TokenType_NumberLiteral) {
            struct ASTNode* ptr = mtASTAddNode(currentNode);
            ptr->token = tokens[i];
            continue;
        }

        //check for either mul or div
        if (tokens[i].type == TokenType_OperatorDivision || tokens[i].type == TokenType_OperatorMultiplication)
        {
            struct ASTNode* ptr = mtASTCreateNode();
            mtASTAddChildNode(ptr, currentNode);
            ptr->token = tokens[i];
            
            currentNode = ptr;
            continue;
        }
        
        //add or subtract
        if (tokens[i].type == TokenType_OperatorAddition || tokens[i].type == TokenType_OperatorSubtraction)
        {
            struct ASTNode* ptr = mtASTCreateNode();
            mtASTAddChildNode(ptr, currentNode);
            ptr->token = tokens[i];

            currentNode = ptr;
            continue;
        }
    }
    rootNode = currentNode;
    return rootNode;
}

struct ASTNode* mtASTAddNode(struct ASTNode* parent)
{
    int index = parent->childCount;
    struct ASTNode* ptr = ( parent->children[index] = mtASTCreateNode() );
    parent->childCount++;

    return ptr;
}   

int mtASTAddChildNode(struct ASTNode* parent, struct ASTNode* child)
{
    int index = parent->childCount;
    
    parent->children[index] = child;
    parent->childCount++;

    return index;
}   

struct ASTNode* mtASTCreateNode()
{

    struct ASTNode* out = malloc( sizeof(struct ASTNode) );

    out->childCount = 0;
    out->children = malloc (sizeof(struct ASTNode**));
    mtCreateToken(&out->token);

    return out;
}


void mtASTFree(struct ASTNode* node)
{
    // have to free the struct but also the rootNode->children array.
    for (size_t i = 0; i < node->childCount; i++)
    {
        //if it's not a leaf node
        if (node->childCount != 0)
        {
            //call recursively
            mtASTFree(node->children[i]);
        }
    }
    free(node->children);
    free(node);
}

#endif


#endif