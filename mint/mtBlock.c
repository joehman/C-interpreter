
#include "mtBlock.h"

#include <Mint.h>

#include "mtExpression.h"
#include "mtFunction.h"

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
      
        if (currentNode->type == NodeType_FunctionDefinition)
        {
            interpretFunctionDef(currentNode, scope); 
            continue;
        }

        if (currentNode->type == NodeType_Assignment)
        {
            interpretStatement(currentNode, scope); 
            continue;
        }

        if (currentNode->type == NodeType_BinaryOperator || currentNode->type == NodeType_FunctionCall)
        {
            expression = interpretExpression(currentNode, scope); 
            if (expression)
            {
                printf("%s\n", expression->type.str(expression->value));
            }
            continue;
        }

    }
}
