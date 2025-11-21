
#include "mtBlock.h"

#include <types/mtType.h>
#include <interpreter/mtVariable.h>
#include <interpreter/mtExpression.h>
#include <interpreter/mtFunction.h>


void interpretBlock(struct ASTNode* node, struct Scope* parent)
{
    if (node->childCount <= 0)
    {
        return;
    }

    struct Scope* scope = mtCreateScope(); 
    scope->parent = parent;
    for (size_t i = 0; i < node->childCount; i++) 
    {
        struct Variable* expression = NULL;
        
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
