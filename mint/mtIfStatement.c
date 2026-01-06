#include "mtIfStatement.h"
#include "mtAST.h"
#include "mtObject.h"
#include "mtUtilities.h"
#include "mtExpression.h"
#include "mtBlock.h"

#include "mtInterpreterError.h"

void interpretIfStatement(struct ASTNode* node, struct mtScope* scope)
{

    bool conditional = false;

    int result = interpretConditional(node->children[0], scope, &conditional);

    if (result != mtSuccess)
    {
        interpreterError(node, "Could not interpret conditional!");
        return;
    }
    
    if (conditional)
    {
        interpretBlock(node->children[1], scope);
    }
}

int interpretConditional(struct ASTNode* node, struct mtScope* scope, bool* result)
{
    if (node->childCount < 2)
    {
        return mtWasNotConditional;
    }

    struct mtObject* left = interpretExpression(node->children[0], scope);
    struct mtObject* right = interpretExpression(node->children[1], scope);

    if (!left || !right)
    {
        return mtWasNotConditional;
    }

    switch(node->type) 
    {   
        case NodeType_GreaterThan:
            *result =  left->type.isGreater(left->data, right->data);
            break; 
        case NodeType_LesserThan:
            *result = left->type.isLesser(left->data, right->data);
            break; 

        case NodeType_GreaterThanOrEqual:
            *result = left->type.isGreater(left->data, right->data) || left->type.isEqual(left->data, right->data);  // this should probably be its own function
            break; 
        
        case NodeType_LesserThanOrEqual:
            *result = left->type.isLesser(left->data, right->data) || left->type.isEqual(left->data, right->data);  // this should probably be its own function
            break; 
        
        case NodeType_IsEqual:
            *result = left->type.isEqual(left->data, right->data);  
            break; 

        case NodeType_IsNotEqual:
            *result = !left->type.isEqual(left->data, right->data);  
            break; 

        default:
            return mtWasNotConditional;
    }

    return mtSuccess;
}
