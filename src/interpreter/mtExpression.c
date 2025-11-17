
#include "mtExpression.h"

#include <interpreter/mtFunction.h>
#include <interpreter/mtVariable.h>
#include <interpreter/mtInterpreterError.h>

#include <types/mtGlobalTypes.h>

void interpretStatement(struct ASTNode* node, struct Scope* scope)
{
    struct ASTNode* leftNode = node->children[0];
    struct ASTNode* rightNode = node->children[1];

    struct Variable* left = interpretExpression(leftNode, scope);
    struct Variable* right = interpretExpression(rightNode, scope);

    if (!right)
    {
        interpreterError(node, "Cannot assign with NULL Value!");
        return;
    }

    if (!left)
    {
        char nodeStr[node->token.size];
        mtGetTokenString(leftNode->token, (char*)&nodeStr, mtArraySize(nodeStr));

        left = mtCreateVariable(right->type);
        hashmap_put(scope->variables, nodeStr, left);

        left->type.set(left->value, right->value);
        return;
    }

    left->type.set(left->value, right->value);
}

struct Variable* interpretExpression(struct ASTNode* node, struct Scope* scope)
{
	if (node == NULL)
    {
        return NULL;
	}

    struct Variable* out = NULL;
    if (node->token.type == TokenType_IntegerLiteral)
    {
        out = mtCreateVariable(mtGlobalTypes[NumberTypeIndex]);

        struct mtNumber num;
        num.type = INTEGER;
        num.integer = mtInterpretInteger(&node->token);
       
        out->type.set(out->value, &num);
	    return out;

    } else if (node->token.type == TokenType_DecimalLiteral)
    {
        out = mtCreateVariable(mtGlobalTypes[NumberTypeIndex]);

        struct mtNumber num;
        num.type = DECIMAL;
        num.decimal = mtInterpretDecimal(&node->token);

        out->type.set(out->value, &num);
	    return out;
	} else if (node->type == NodeType_Identifier)
    {
        struct Variable* out = NULL;
        
        char str[node->token.size];
        mtGetTokenString(node->token, (char*)&str, node->token.size); 
        
        out = getVariableFromScope(scope, str); 

        return out;
    } else if (node->type == NodeType_FunctionCall)
    {
        struct Variable* out = NULL;

        out = interpretFunctionCall(node, scope);        

        return out;
    }

    struct Variable* left = interpretExpression(node->children[0], scope);
    struct Variable* right = interpretExpression(node->children[1], scope);

    if (!left)
    {
        interpreterError(node, "Left side of binary operator was NULL!"); 
    }
    if (!right)
    {
        interpreterError(node, "Right side of binary operator was NULL!"); 
    }
    if (!left || !right)
        return NULL;


    out = mtCreateVariable(left->type);
    switch (node->token.type)
    {
        case TokenType_OperatorAddition:
            out->value = left->type.add(left->value, right->value); 
            break;
        case TokenType_OperatorSubtraction:
            out->value = left->type.sub(left->value, right->value);
            break;
        case TokenType_OperatorMultiplication:
            out->value = left->type.mul(left->value, right->value);
            break;
        case TokenType_OperatorDivision:
            out->value = left->type.div(left->value, right->value);
            break;
        default:
            break;
    }

    //free(left);
    //free(right);

    return out;
}
