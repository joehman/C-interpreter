
#include "mtExpression.h"
#include "mtToken.h"


#include "mtFunction.h"
#include "mtInterpreterError.h"
#include "mtScope.h"

#include "mtNumberObject.h"

#include <Mint.h>

#include "mtUtilities.h"

void interpretStatement(struct ASTNode* node, struct mtScope* scope)
{
    struct ASTNode* leftNode = node->children[0];
    struct ASTNode* rightNode = node->children[1];

    struct mtObject* left = interpretExpression(leftNode, scope);
    struct mtObject* right = interpretExpression(rightNode, scope);

    if (!right)
    {
        interpreterError(node, "Cannot assign with NULL Value!");
        return;
    }

    if (!left)
    {
        char nodeStr[node->token.size];
        mtGetTokenString(leftNode->token, (char*)&nodeStr, mtArraySize(nodeStr));

        left = mtCreateObject(right->type);
        mtHashMapPut(scope->variables, nodeStr, left);
    }

    left->type.set(left->data, right->data);
}

struct mtObject* intepretInteger(struct ASTNode* node)
{
    if (node->token.type == TokenType_IntegerLiteral)
    {
        struct mtObject* out;
        out = mtCreateObject(mtNumberType);

        struct mtNumber num;
        num.type = INTEGER;
        num.integer = mtInterpretInteger(&node->token);
       
        out->type.set(out->data, &num);
	    return out;
    }
    return NULL;
}
struct mtObject* interpretDecimal(struct ASTNode* node)
{
    if (node->token.type == TokenType_DecimalLiteral)    
    {
        struct mtObject* out;
        out = mtCreateObject(mtNumberType);

        struct mtNumber num;
        num.type = DECIMAL;
        num.decimal = mtInterpretDecimal(&node->token);

        out->type.set(out->data, &num);
	    return out;
    }
    return NULL;
}

struct mtObject* interpretIdentifier(struct ASTNode* node, struct mtScope* scope, bool* wasIdentifier)
{
    *wasIdentifier = false;
    if (node->token.type == TokenType_Identifier)
    {
        *wasIdentifier = true; 
        struct mtObject* out = NULL;
        
        char str[node->token.size];
        mtGetTokenString(node->token, (char*)&str, node->token.size); 
       
        out = getObjectFromScope(scope, str); 

        return out;
    }
    return NULL;
}

// doesn't interpret identifiers 
struct mtObject* interpretLiterals(struct ASTNode* node, struct mtScope* scope)
{
#define Check(out, func)        \
        if ( (out = func) )     \
        {                       \
            return out;         \
        }                        
    
    struct mtObject* out = NULL;
    Check(out, intepretInteger(node));
    Check(out, interpretDecimal(node));

    return NULL;
}

struct mtObject* interpretExpression(struct ASTNode* node, struct mtScope* scope)
{
	if (node == NULL)
    {
        return NULL;
	}

    struct mtObject* out = NULL;
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


    struct mtObject* left = interpretExpression(node->children[0], scope);
    struct mtObject* right = interpretExpression(node->children[1], scope);

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
    out = mtCreateObject(left->type);
    switch (node->token.type)
    {
        case TokenType_OperatorAddition:
            out->data = left->type.add(left->data, right->data); 
            break;
        case TokenType_OperatorSubtraction:
            out->data = left->type.sub(left->data, right->data);
            break;
        case TokenType_OperatorMultiplication:
            out->data = left->type.mul(left->data, right->data);
            break;
        case TokenType_OperatorDivision:
            out->data = left->type.div(left->data, right->data);
            break;
        default:
            break;
    }


    return out;
}
