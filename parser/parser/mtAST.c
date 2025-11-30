#include "internal/mtAST.h"


struct ASTNode* mtASTAddNode(struct ASTNode* parent)
{
    if (parent->childCount >= parent->childCapacity)
    {
        parent->childCapacity *= 2;
        parent->children = realloc(parent->children, sizeof(struct ASTNode*) * parent->childCapacity);
    }

    int index = parent->childCount;
    struct ASTNode* ptr = ( parent->children[index] = mtASTCreateNode() );
    parent->childCount++;

    return ptr;
}   

int mtASTAddChildNode(struct ASTNode* parent, struct ASTNode* child)
{
    if (parent->childCount >= parent->childCapacity)
    {
        parent->childCapacity *= 2;
        parent->children = realloc(parent->children, sizeof(struct ASTNode*) * parent->childCapacity);
    }
    int index = parent->childCount;
    
    parent->children[index] = child;
    parent->childCount++;

    return index;
}   
struct ASTNode* mtASTCreateNode()
{

    struct ASTNode* out = malloc( sizeof(struct ASTNode) );
    out->type = NodeType_None;
    out->childCount = 0;
    out->childCapacity = mtASTInitialChildCapacity;

    out->children = malloc (sizeof(struct ASTNode*) * mtASTInitialChildCapacity);
    mtCreateToken(&out->token);

    return out;
}
struct ASTNode* mtASTTokenCreateNode(struct Token token)
{

    struct ASTNode* out = malloc( sizeof(struct ASTNode) );
    
    out->childCount = 0;
    out->childCapacity = mtASTInitialChildCapacity;
    
    out->children = malloc (sizeof(struct ASTNode*) * mtASTInitialChildCapacity);
    out->token = token;

    return out;
}

void mtASTFree(struct ASTNode* node)
{
    if (node == NULL) return;

    // have to free the struct but also the rootNode->children array.
    for (size_t i = 0; i < node->childCount; i++)
    {
        //call recursively
        mtASTFree(node->children[i]);
    }
    free(node->children);
    free(node);
}
