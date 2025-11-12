#ifndef mtInterpreter_h
#define mtInterpreter_h

/*
*   This is the part of the code that actually interprets all the code and does stuff.
*/

#include "mtUtilities.h" 
#include "mtParser.h"
#include "mtTokenization.h"

#include <Mint/types/mtGlobalTypes.h>

#include "hashmap.h"
#include <Mint/types/mtType.h>

// ___________ Declarations ______________

void mtInterpret(struct ASTNode* node);

// ___________ Implementation ___________
#ifdef mtImplementation


struct Variable {
    struct Type type; 
    void* value;
};


struct Parameter {
    const char* identifier;
    struct Type* type; // could be NULL
};

struct Function {
    struct ASTNode* block;

    const char* identifier;
    
    size_t parameterCount; 
    struct Parameter* parameters;
};

struct Scope {
    struct Scope* parent;    
    HashMap* variables;
    HashMap* functions;
};

struct Variable* interpretExpression(struct ASTNode* node, struct Scope* scope);
void interpretBlock(struct ASTNode* node, struct Scope* parent);
//@brief print errors to stderr, uses printf formats
static void interpreterError(struct ASTNode* node, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "Error while interpreting on line %d: \n\t", 
            node->token.line 
    );
    vfprintf(stderr, fmt, args);

    if (fmt[strlen(fmt)] != '\n')
    {
        printf("\n");
    }
}

struct Scope* mtCreateScope()
{
    struct Scope* scope = malloc(sizeof(struct Scope));

    scope->parent = NULL;
    scope->variables = hashmap_create(2);
    scope->functions = hashmap_create(2);

    return scope;
}

struct Variable* mtCreateVariable(struct Type type)
{
    struct Variable* var = malloc(sizeof(struct Variable));
    var->value = malloc(type.size);
    var->type = type;

    memset(var->value, 0, var->type.size);

    return var;
}

struct Variable* getVariableFromScope(struct Scope* scope, const char* key)
{
    if (scope == NULL)
    {
        return NULL;
    }

    struct Variable* out = NULL;

    if ( (out = hashmap_get(scope->variables, key)) )
    {
        return out;
    }
    
    struct Scope* currentScope = scope;
    while(currentScope)
    {
        if ( (out = hashmap_get(scope->variables, key)) )
        {
            return out;
        }
        
        // check the scope above
        currentScope = currentScope->parent;
    }

    return NULL;
}

struct Function* getFunctionFromScope(struct Scope* scope, const char* key)
{
    if (scope == NULL)
    {
        return NULL;
    }
    
    struct Function* out = NULL;
    
    struct Scope* currentScope = scope;
    while (currentScope)
    {
        if ((out = hashmap_get(scope->functions, key)))
        {
            return out;
        }

        //check the scope above
        currentScope = scope->parent;
    }

    return NULL;
}

struct Variable* interpretFunctionCall(struct ASTNode* node, struct Scope* scope)
{
    struct Token identifier = node->children[0]->token; 
    struct ASTNode* argumentList = node->children[1];

    char tokenStr[identifier.size];
    mtGetTokenString(identifier, (char*)&tokenStr, identifier.size);
    struct Function* func = getFunctionFromScope(scope, tokenStr);
    if (!func)
    {
        interpreterError(node, "No such function!");
        return NULL;
    }

    if (argumentList->childCount != func->parameterCount)   
    {
        if (argumentList->childCount > func->parameterCount)
        {
            interpreterError(node, "Too many arguments to function, expected %d arguments!", func->parameterCount);
        }
        if (argumentList->childCount < func->parameterCount)
        {
            interpreterError(node, "Too few arguments to function, expected %d arguments!", func->parameterCount);
        }
        return NULL;
    }

    struct Scope* arguments = mtCreateScope();
    arguments->parent = scope; 

    for (size_t i = 0; i < argumentList->childCount; i++)
    {
        struct Variable* argument = interpretExpression(argumentList->children[i], scope);

        if (!argument)
            return NULL;

        hashmap_put(arguments->variables, func->parameters[i].identifier, &argument);   
    }
    
    interpretBlock(func->block, arguments);

    return NULL;
}

void interpretFunctionDef(struct ASTNode* node, struct Scope* scope)
{
    // children of the function_def node:
    // 1st      child: identifier
    // 2nd      child: parameterList 
    // 3rd      child: block
    //
    // children of the parameterList node:
    // 1...nth  child: parameter
    if (node->type != NodeType_FunctionDefinition)
    {
        return;
    }

    struct Function* out = malloc(sizeof(struct Function));

    struct Token identifier = node->children[0]->token;
    if (identifier.type != TokenType_Identifier)
    {
        return;
    }
    
    struct ASTNode* parameterList = node->children[1];

    out->parameterCount = parameterList->childCount;
    out->parameters = malloc( sizeof(struct Parameter) * out->parameterCount);
    
    for (size_t i = 0; i < out->parameterCount; i++)
    {
        struct ASTNode* parameterNode = parameterList->children[i];         
       
        size_t tokenSize = parameterNode->token.size;
        char tokenStr[parameterNode->token.size];

        mtGetTokenString(parameterNode->token, (char*) &tokenStr, tokenSize);
        out->parameters[i].identifier = strndup(tokenStr, tokenSize); 
        out->parameters[i].type = NULL;
        struct ASTNode* block = node->children[2];
        out->block = block; 

        hashmap_put(scope->functions, out->identifier, out);
    }
}

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
       
        if (currentNode->type == NodeType_BinaryOperator || currentNode->type == NodeType_FunctionCall)
        {
            expression = interpretExpression(currentNode, scope); 
        }
        if (currentNode->type == NodeType_Assignment)
        {
            interpretStatement(currentNode, scope); 
        }

        //temporary
        if (expression != NULL)
        {
            printf("%s\n", expression->type.str(expression->value));
        }
    }
}

void mtInterpret(struct ASTNode* node)
{
    interpretBlock(node, NULL);
}


#endif // mtImplementation

#endif // mtInterpreter_h
