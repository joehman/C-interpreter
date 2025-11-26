
#include "mtParser.h"

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
*   block       = statments | expressions | function_def
*   statement   = identifier {assign} expression 
*   expression  = {add | sub} term {add | sub} term 
*   term        = factor  {mul | div} factor | factor {pow} exponent  
*   factor      = number | identifier | "lparen" expression "rparen" | function_call
*
*   function_def    = "func" identifier "lparen" [params] "rparen" block "end"
*   function_call   = identifier "lparen" [arguments] "rparen"
*
*   params      =  ("comma", identifier) // () = list, comma separates the identifiers
*   arguments   = ("comma", expression) // "comma" separates the expressions 
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

struct ASTNode* parseFunctionCall(struct mtParserState* state)
{
    size_t startToken = state->currentToken;
 
    struct Token identifier;

    if (mtParserCheck(state, TokenType_Identifier))
    {
        state->currentToken = startToken;
        identifier = mtParserGetToken(state);
    } else {
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

    struct ASTNode* functionCall = mtASTCreateNode();
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
    }
    if (rootNode == NULL)
        return NULL;

    return rootNode;
}
