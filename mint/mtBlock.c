
#include "mtBlock.h"

#include <Mint.h>

#include "mtAST.h"
#include "mtExpression.h"
#include "mtFunction.h"
#include "mtIfStatement.h"

void interpretBlock(struct ASTNode* node, struct mtScope* parent)
{
    if (node->childCount <= 0)
    {
        return;
    }

    struct mtScope* scope = mtCreateScope(); 
    scope->parent = parent;
    for (size_t i = 0; i < node->childCount; i++) 
    {
        struct mtObject* expression = NULL;
        
        struct ASTNode* currentNode = node->children[i];
      
        switch(currentNode->type)
        {
            case NodeType_IfStatement:
                interpretIfStatement(currentNode, scope);
                break;
            case NodeType_FunctionDefinition:
                interpretFunctionDef(currentNode, scope);
                break;
            case NodeType_Assignment:
                interpretStatement(currentNode, scope);
                break;
            
            case NodeType_BinaryOperator:
            case NodeType_FunctionCall:
                expression = interpretExpression(currentNode, scope); 
                if (expression)
                {
                    printf("%s\n", expression->type.str(expression->data));
                }
                break;
            
            default:
                break;
        }
    }
}
