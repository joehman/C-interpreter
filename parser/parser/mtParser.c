
#include "internal/mtParser.h"
#include "internal/mtAST.h"
#include "internal/mtToken.h"
#include <stdlib.h>

// ___________ Helper functions ______________

//@brief prints errors to stderr, uses printf formats
void parserError(struct mtParserState state, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
   
    struct Token token = mtParserGetToken(&state);

    char tokenstr[token.size];
    mtGetTokenString(token, (char*)&tokenstr, token.size);

    fprintf(stderr, "Error while parsing token '%s', on line %d: \n\t", tokenstr, token.line);
    vfprintf(stderr, fmt, args);

    if (fmt[strlen(fmt)-1] != '\n')
        printf("\n");
}
void unexpectedTokenError(struct mtParserState state)
{
    parserError(state, "Unexpected token");
}

/*
*   block       = statments | expressions | function_def | if
*   statement   = identifier {assign} expression 
*   expression  = {add | sub} term {add | sub} term 
*   term        = factor  {mul | div} factor  
*   factor      = number | identifier | "lparen" expression "rparen" | function_call
*
*   function_def    = "func" identifier "lparen" [params] "rparen" block "end"
*   function_call   = identifier "lparen" [arguments] "rparen"
*   
*   if  = "if" {conditional} block "end"
*
*   params      = ("comma", identifier) // () = list, comma separates the identifiers
*   arguments   = ("comma", expression) // "comma" separates the expressions 
*   conditional = true | false | expression {comparison} expression 
*   comparison  = {greaterThan} | {lesserThan} | {greaterThanOrEqual} | {lesserThanOrEqual} | {isEqual} | {isNotEqual}
*/

struct ASTNode* parseFactor(struct mtParserState* state)
{
    struct Token token = mtParserGetToken(state);
    struct ASTNode* node = NULL;

    if (mtParserCheck(state, TokenType_DecimalLiteral) || mtParserCheck(state, TokenType_IntegerLiteral))
    {
        mtParserAdvance(state);
        struct ASTNode* node = mtASTTokenCreateNode(token);
        node->type = NodeType_Number; 

        return node;
    } else if ( (node = parseFunctionCall(state)) )
    {
        return node; 
    }
    if (mtParserCheck(state, TokenType_Identifier)) // check for an identifier
    {
        mtParserAdvance(state);
        struct ASTNode* node = mtASTTokenCreateNode(token);
        node->type = NodeType_Identifier;
        return node;
    }

    if (mtParserCheck(state, TokenType_LeftParentheses))
    {
        mtParserAdvance(state);
        struct ASTNode* node = parseExpression(state);
  
        if (node == NULL)
        {
            struct Token lastToken = mtParserGetLastToken(state);
            char* str = malloc(sizeof(char) * (lastToken.size+1) );

            mtGetTokenString(lastToken, str, lastToken.size);
            parserError(*state, "Expected expression after token '%s'", str);
            
            free(str);

            return NULL;
        }
        if (!mtParserCheck(state, TokenType_RightParentheses))
        {
            struct Token lastToken = mtParserGetLastToken(state);

            char* str = malloc(sizeof(char) * (lastToken.size+1));
            mtGetTokenString(lastToken, str, token.size);

            parserError(*state, "Expected rightparentheses after token '%s'", str);
            free(str);
        }
        // remove the right-parenteses.
        mtParserAdvance(state);
        return node;
    }

    return NULL;
}

struct ASTNode* parseTerm(struct mtParserState* state)
{
    struct ASTNode* left = parseFactor(state);
    
    if (left == NULL)
        return NULL;
    
    bool isRightOperator =  mtParserCheck(state, TokenType_OperatorMultiplication)  || 
                            mtParserCheck(state, TokenType_OperatorDivision);

    if (isRightOperator)
    {
        struct Token operator = mtParserGetToken(state);
        mtParserAdvance(state); // remove operator
        
        struct ASTNode* node = mtASTTokenCreateNode(operator);
        node->type = NodeType_BinaryOperator;  //the interpreter can figure out which operator it is.

        mtASTAddChildNode(node, left);
        mtASTAddChildNode(node, parseFactor(state));

        //so that we can return it
        left = node;
    }

    if (left == NULL)
        return NULL;

    return left;
}

struct ASTNode* parseExpression(struct mtParserState* state)
{
    struct ASTNode* left = parseTerm(state);
    
    if (left == NULL)
        return NULL;

    bool isOperator =   mtParserCheck(state, TokenType_OperatorAddition)    || 
                        mtParserCheck(state, TokenType_OperatorSubtraction) ;
    if (isOperator)
    {
        struct Token operator = mtParserGetToken(state);
        mtParserAdvance(state); 
        
        struct ASTNode* node = mtASTTokenCreateNode(operator);
        node->type = NodeType_BinaryOperator;
        mtASTAddChildNode(node, left);
        mtASTAddChildNode(node, parseTerm(state));

        //so that we can return it
        left = node;
    }

    if (left == NULL)
        return NULL;

    return left;
}

struct ASTNode* parseStatement(struct mtParserState* state)
{
    size_t startToken = state->currentToken;
    
    if (mtParserCheck(state, TokenType_Identifier))
    {
        struct Token identifier = mtParserGetToken(state);
        mtParserAdvance(state);
        
        struct Token operator = mtParserGetToken(state);
        if (operator.type != TokenType_OperatorAssign)
        {
            //handle it as an expression instead
            state->currentToken = startToken;
            return parseExpression(state); 
        }

        mtParserAdvance(state);
        struct ASTNode* right = parseExpression(state);

        struct ASTNode* identifierNode = mtASTTokenCreateNode(identifier); 
        struct ASTNode* operatorNode = mtASTTokenCreateNode(operator);
    
        identifierNode->type = NodeType_Identifier;
        operatorNode->type = NodeType_Assignment;

        mtASTAddChildNode(operatorNode, identifierNode);
        mtASTAddChildNode(operatorNode, right);
    
        return operatorNode;
    }

    if (mtParserCheck(state, TokenType_EndOfStatement))
    {
        mtParserAdvance(state);
        return parseStatement(state);
    }
    return parseExpression(state);
}

struct ASTNode* parseParams(struct mtParserState* state)
{
    if (!mtParserCheck(state, TokenType_LeftParentheses))
    {
        return NULL;
    }
    mtParserAdvance(state);
    
    struct ASTNode* parameters = mtASTCreateNode();
    parameters->type = NodeType_ParameterList;

    while (!mtParserCheck(state, TokenType_RightParentheses))
    {
        if (mtParserCheck(state, TokenType_Comma))
        {
            parserError(*state, "Expected identifier before comma!");
            return NULL;
        }
        
        if (mtParserCheck(state, TokenType_Identifier))
        {
            struct ASTNode* parameter = mtASTTokenCreateNode(mtParserGetToken(state)); 
            mtASTAddChildNode(parameters, parameter);
        }
    
        mtParserAdvance(state);
        if (!mtParserCheck(state, TokenType_Comma) && !mtParserCheck(state, TokenType_RightParentheses))
        {
            parserError(*state, "Commas must separate all parameters!");
            mtASTFree(parameters);
            return NULL;
        }

        if (mtParserCheck(state, TokenType_RightParentheses))
        {
            mtParserAdvance(state);
            return parameters;
        }
        mtParserAdvance(state);
    }

    //advance past the right parentheses.
    mtParserAdvance(state);

    return parameters;
}

struct ASTNode* parseFunctionDef(struct mtParserState* state)
{
    while (mtParserCheck(state, TokenType_EndOfStatement))
    {
        mtParserAdvance(state);
    }
    if (!mtParserCheck(state, TokenType_FunctionKeyword))
    {
        return NULL;
    }
    mtParserAdvance(state);
    
    if (!mtParserCheck(state, TokenType_Identifier))
    {
        parserError(*state, "Function identifier must follow function definition keyword!");
        return NULL;
    }

    struct Token identifier; 
    identifier = mtParserGetToken(state);
    mtParserAdvance(state);

    if (!mtParserCheck(state, TokenType_LeftParentheses))
    {
        parserError(*state, "Parentheses must follow function identifier!");
        return NULL;
    }

    struct ASTNode* functionDef = mtASTCreateNode();
    functionDef->type = NodeType_FunctionDefinition;

    struct ASTNode* identifierNode = mtASTTokenCreateNode(identifier);
    mtASTAddChildNode(functionDef, identifierNode);

    struct ASTNode* parameterList = parseParams(state);
    parameterList->type = NodeType_ParameterList;
    mtASTAddChildNode(functionDef, parameterList);

    struct ASTNode* block = parseBlock(state); 
    if (mtParserCheck(state, TokenType_EndKeyword))
    {
        mtParserAdvance(state);
        mtASTAddChildNode(functionDef, block);
        return functionDef;
    }
    
    parserError(*state, "Functions must end with \'end\' keyword!");
    return NULL;
}

struct ASTNode* parseArguments(struct mtParserState* state)
{
    if (!mtParserCheck(state, TokenType_LeftParentheses))
    {
        return NULL;
    }
    mtParserAdvance(state);
    
    struct ASTNode* arguments = mtASTCreateNode();
    arguments->type = NodeType_ArgumentList;

    while (!mtParserCheck(state, TokenType_RightParentheses))
    {
        if (mtParserCheck(state, TokenType_Comma))
        {
            parserError(*state, "Expected expression before comma!");
            mtASTFree(arguments);
            return NULL;
        }
        
        struct ASTNode* expression = parseExpression(state);
        if (expression) 
        {
            mtASTAddChildNode(arguments, expression);
        }
        
        if (!mtParserCheck(state, TokenType_Comma) && !mtParserCheck(state, TokenType_RightParentheses))
        {
            parserError(*state, "Commas must separate all arguments!");
            return NULL;
        }
        
        if (mtParserCheck(state, TokenType_RightParentheses))
        {
            mtParserAdvance(state);
            return arguments;
        }

        mtParserAdvance(state);
    }

    //advance past the right parentheses.
    mtParserAdvance(state);

    return arguments;
}

//this could probably be less repetetive.
enum NodeType parseConditionalOperator(struct mtParserState* state)
{
    enum NodeType type = NodeType_None;

    if (mtParserCheck(state, TokenType_OperatorGreaterThan))
    {
        mtParserAdvance(state);
        
        if (mtParserCheck(state, TokenType_OperatorAssign))
        {
            type = NodeType_GreaterThanOrEqual;
            mtParserAdvance(state); // advance past the operator
            return type;
        }
        type = NodeType_GreaterThan;
    }

    if (mtParserCheck(state, TokenType_OperatorLesserThan))
    {
        mtParserAdvance(state);
        if (mtParserCheck(state, TokenType_OperatorAssign))
        {
            type = NodeType_LesserThanOrEqual;
            mtParserAdvance(state); // advance past the operator
            return type;
        }

        type = NodeType_LesserThan;
    }

    if (mtParserCheck(state, TokenType_OperatorAssign))
    {
        mtParserAdvance(state);
        if (mtParserCheck(state, TokenType_OperatorAssign))
        {
            type = NodeType_IsEqual;
            mtParserAdvance(state); // advance past the operator
            return type;
        }
    }

    if (mtParserCheck(state, TokenType_ExclamationMark))
    {
        mtParserAdvance(state);
        if (mtParserCheck(state, TokenType_OperatorAssign))
        {
            type = NodeType_IsNotEqual;
            mtParserAdvance(state); // advance past the operator
            return type;
        }
    }


    return type;
} 

// things that can be evaluated as true or false
struct ASTNode* parseCondition(struct mtParserState* state)
{
    size_t stateStart = state->currentToken;

    struct ASTNode* left = parseExpression(state);
    
    if (left == NULL)
        return NULL;

    enum NodeType type = parseConditionalOperator(state);

    if (type == NodeType_None)
    {
        mtASTFree(left);
        state->currentToken = stateStart; // reset the state
        return NULL;
    }
    
    struct ASTNode* right = parseExpression(state);
    if (right == NULL)
        return NULL;

    struct ASTNode* node = mtASTCreateNode();
    node->type = type;
    mtASTAddChildNode(node, left);
    mtASTAddChildNode(node, right);

    return node;
}

struct ASTNode* parseIfStatement(struct mtParserState* state)
{
    while (mtParserCheck(state, TokenType_EndOfStatement))
    {
        mtParserAdvance(state);
    }
    if (!mtParserCheck(state, TokenType_IfKeyword))
    {
        return NULL;
    }
    mtParserAdvance(state); //advance past the if

    struct ASTNode* ifNode = mtASTCreateNode();
    ifNode->type = NodeType_IfStatement;

    struct ASTNode* condition = parseCondition(state);
    struct ASTNode* block = parseBlock(state);

    mtASTAddChildNode(ifNode, condition);
    mtASTAddChildNode(ifNode, block);

    return ifNode;
}

struct ASTNode* parseFunctionCall(struct mtParserState* state)
{
    size_t startToken = state->currentToken;
 
    struct Token identifier;

    if (mtParserCheck(state, TokenType_Identifier))
    {
        identifier = mtParserGetToken(state);
    } else {
        state->currentToken = startToken;
        return NULL;
    }

    mtParserAdvance(state);

    if (!mtParserCheck(state, TokenType_LeftParentheses))
    {
        state->currentToken = startToken;
        return NULL; 
    }
    struct ASTNode* arguments;
    if ( !(arguments = parseArguments(state)) ) 
    {
        return NULL; 
    }

    // the token functionCall stores isn't used by the interpreter
    // it exists mainly for debugging
    struct ASTNode* functionCall = mtASTTokenCreateNode(identifier);
    functionCall->type = NodeType_FunctionCall;
   
    mtASTAddChildNode(functionCall, mtASTTokenCreateNode(identifier));
    mtASTAddChildNode(functionCall, arguments);

    return functionCall;
}

struct ASTNode* parseBlock(struct mtParserState* state)
{
    struct ASTNode* block = mtASTCreateNode();
    block->type = NodeType_Block;

    struct ASTNode* child;
    while (!mtParserCheck(state, TokenType_NullTerminator))
    {
        if ( (child = parseFunctionDef(state)) )
        {
            mtASTAddChildNode(block, child); 
            continue;
        }
        if ( (child = parseIfStatement(state)) )
        {
            mtASTAddChildNode(block, child);
            continue;
        }

        if ( (child = parseStatement(state)) )
        {
            mtASTAddChildNode(block, child); 
            continue;
        }
        if (mtParserCheck(state, TokenType_EndOfStatement))
        {
            continue;
        }
        return block;
    }
    return block;
}

struct ASTNode* mtASTParseTokens(struct Token* tokens, size_t tokenCount)
{
    struct mtParserState state;    
    state.currentToken = 0;
    state.tokenCount = tokenCount;
    state.tokens = tokens;
    
    struct ASTNode* rootNode = NULL;
    
    if (tokenCount > 0)
    {
        rootNode = parseBlock(&state); 
    } else {
        return NULL;
    }
    
    return rootNode;
}
