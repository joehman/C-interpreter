
#include "mtExpression.h"
#include "tokenizer/mtToken.h"

#include <interpreter/mtFunction.h>
#include <interpreter/mtVariable.h>
#include <interpreter/mtInterpreterError.h>
#include <interpreter/mtScope.h>

#include <malloc.h>
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
    }

    left->type.set(left->value, right->value);
}

struct Variable* intepretInteger(struct ASTNode* node)
{
    if (node->token.type == TokenType_IntegerLiteral)
    {
        struct Variable* out;
        out = mtCreateVariable(mtGlobalTypes[NumberTypeIndex]);

        struct mtNumber num;
        num.type = INTEGER;
        num.integer = mtInterpretInteger(&node->token);
       
        out->type.set(out->value, &num);
	    return out;
    }
    return NULL;
}
struct Variable* interpretDecimal(struct ASTNode* node)
{
    if (node->token.type == TokenType_DecimalLiteral)    
    {
        struct Variable* out;
        out = mtCreateVariable(mtGlobalTypes[NumberTypeIndex]);

        struct mtNumber num;
        num.type = DECIMAL;
        num.decimal = mtInterpretDecimal(&node->token);

        out->type.set(out->value, &num);
	    return out;
    }
    return NULL;
}

struct Variable* interpretIdentifier(struct ASTNode* node, struct Scope* scope, bool* wasIdentifier)
{
    *wasIdentifier = false;
    if (node->token.type == TokenType_Identifier)
    {
        *wasIdentifier = true; 
        struct Variable* out = NULL;
        
        char str[node->token.size];
        mtGetTokenString(node->token, (char*)&str, node->token.size); 
       
        out = getVariableFromScope(scope, str); 

        return out;
    }
    return NULL;
}

// doesn't interpret identifiers 
struct Variable* interpretLiterals(struct ASTNode* node, struct Scope* scope)
{
#define Check(out, func)        \
        if ( (out = func) )     \
        {                       \
            return out;         \
        }                        
    
    struct Variable* out = NULL;
    Check(out, intepretInteger(node));
    Check(out, interpretDecimal(node));

    return NULL;
}

struct Variable* interpretExpression(struct ASTNode* node, struct Scope* scope)
{
	if (node == NULL)
    {
        return NULL;
	}

    struct Variable* out = NULL;
    if ( (out = interpretLiterals(node, scope)) )
    {
        return out;
    }

    bool wasFound = 0;
    out = interpretFunctionCall(node, scope, &wasFound);
    if (wasFound)
    {
        return out;
    }
    out = interpretIdentifier(node, scope, &wasFound);
    if (wasFound)    
    {
        return out;
    }


    struct Variable* left = interpretExpression(node->children[0], scope);
    struct Variable* right = interpretExpression(node->children[1], scope);

    if (!left)
    {
        interpreterError(node, "Left side of binary operator was NULL!"); 
        return NULL;
    }
    if (!right)
    {
        interpreterError(node, "Right side of binary operator was NULL!"); 
        return NULL;
    }

    out = NULL;
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


    return out;
}
