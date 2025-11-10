/*
* The Parser takes in all the different tokens the tokenizer made,
* and parses them all into an Abstract Syntax Tree(AST), which the
* intepreter then runs.
*/


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
*   arguments   = ("comma", expression) // "comma" separates the expressios
*/


#ifndef mtParser_h
#define mtParser_h

#include "mtUtilities.h"
#include "mtTokenization.h"
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>

// ______________ Declaration _____________
enum NodeType {
    NodeType_None,

    NodeType_Assignment,
    NodeType_BinaryOperator,
    NodeType_Number,
    NodeType_Identifier,
    NodeType_Block,

    NodeType_FunctionDefinition,
    NodeType_FunctionCall,
    NodeType_ParameterList, // not an actual list
    NodeType_Argument
};

// abstract syntax tree
struct ASTNode {
    struct Token token;

    enum NodeType type;

    //pointer to multiple pointers
    struct ASTNode** children;
    size_t childCount;
    size_t childCapacity;
};

struct mtParserState {
    struct Token* tokens;
    size_t currentToken;
    size_t tokenCount;
};


//@returns the root node of the AST.
struct ASTNode* mtASTParseTokens(struct Token* tokens, size_t tokenCount);

//@brief Adds child as a node to parent
//
//@returns Its index.
int mtASTAddChildNode(struct ASTNode* parent, struct ASTNode* child);

//@brief adds an empty node to parent and returns it.
struct ASTNode* mtASTAddNode(struct ASTNode* parent);

//@brief Creates an empty node then returns it.
struct ASTNode* mtASTCreateNode();

//@brief Create a node with a token.
struct ASTNode* mtASTTokenCreateNode(struct Token token);

//@brief Frees node and all of its children.
void mtASTFree(struct ASTNode* node);

//@brief print errors to stderr, uses printf formats
void parserError(const char* fmt, ...);

struct ASTNode* parseExpression(struct mtParserState* state);
struct ASTNode* parseFactor(struct mtParserState* state);
struct ASTNode* parseTerm(struct mtParserState* state);
struct ASTNode* parseBlock(struct mtParserState* state);

struct ASTNode* parseFunctionCall(struct mtParserState* state);

// ________________ Implementation ________________

#ifdef mtImplementation

#define mtASTInitialChildCapacity 2

// ___________ Helper functions ______________

//@brief prints errors to stderr, uses printf formats
void parserError(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "Error while parsing: \n\t");
    vfprintf(stderr, fmt, args);

    if (fmt[strlen(fmt)-1] != '\n')
        printf("\n");
}
void unexpectedTokenError(struct Token token)
{
    const char* newlineLiteral = "\\n";
    const char* endOfFileLiteral = "\\0";

    char* str = malloc((token.size+1) * sizeof(char));
    mtGetTokenString(token, str, token.size);

    if (token.string[0] == '\n')
    {
        parserError("Unexpected token: '%s'", newlineLiteral);
        return;
    }
    if (token.string[0] == '\0')
    {
        parserError("Unexpected token: '%s'", endOfFileLiteral);
    }
    parserError("Unexpected token: '%s'", str);

    free(str);
}
struct ASTNode* mtASTAddNode(struct ASTNode* parent)
{
    if (parent->childCount >= parent->childCapacity)
    {
        parent->childCapacity *= 2;
        parent->children = realloc(parent->children, sizeof(struct ASTNode*) * parent->childCapacity);
    }

    int index = parent->childCount;
    struct ASTNode* ptr = ( parent->children[index] = mtASTCreateNode() );
    parent->childCount++;

    return ptr;
}   

int mtASTAddChildNode(struct ASTNode* parent, struct ASTNode* child)
{
    if (parent->childCount >= parent->childCapacity)
    {
        parent->childCapacity *= 2;
        parent->children = realloc(parent->children, sizeof(struct ASTNode*) * parent->childCapacity);
    }
    int index = parent->childCount;
    
    parent->children[index] = child;
    parent->childCount++;

    return index;
}   
struct ASTNode* mtASTCreateNode()
{

    struct ASTNode* out = malloc( sizeof(struct ASTNode) );
    out->type = NodeType_None;
    out->childCount = 0;
    out->childCapacity = mtASTInitialChildCapacity;

    out->children = malloc (sizeof(struct ASTNode*) * mtASTInitialChildCapacity);
    mtCreateToken(&out->token);

    return out;
}
struct ASTNode* mtASTTokenCreateNode(struct Token token)
{

    struct ASTNode* out = malloc( sizeof(struct ASTNode) );
    
    out->childCount = 0;
    out->childCapacity = mtASTInitialChildCapacity;
    
    out->children = malloc (sizeof(struct ASTNode*) * mtASTInitialChildCapacity);
    out->token = token;

    return out;
}

void mtASTFree(struct ASTNode* node)
{
    if (node == NULL) return;

    // have to free the struct but also the rootNode->children array.
    for (size_t i = 0; i < node->childCount; i++)
    {
        //call recursively
        mtASTFree(node->children[i]);
    }
    free(node->children);
    free(node);
}

struct Token mtParserGetToken(struct mtParserState* state)
{
    return state->tokens[state->currentToken];
}
//@brief Gets the token before the current one and returns it.
struct Token mtParserGetLastToken(struct mtParserState* state)
{
    //returning state->tokens[-1] would be bad.
    if (state->currentToken != 0)
        return state->tokens[state->currentToken-1];
    
    return state->tokens[state->currentToken];
}
struct Token mtParserAdvance(struct mtParserState* state)
{
    return state->tokens[state->currentToken++];
}
bool mtParserCheck(struct mtParserState* state, enum TokenType type)
{
    if (state->tokens[state->currentToken].type == type )
        return true;
    return false;
}

// ____________________ Actual Parser ____________________


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

    if (mtParserCheck(state, TokenType_DecimalLiteral) || mtParserCheck(state, TokenType_IntegerLiteral))
    {
        mtParserAdvance(state);
        struct ASTNode* node = mtASTTokenCreateNode(token);
        node->type = NodeType_Number; 
        return node;
    } else if (mtParserCheck(state, TokenType_Identifier)) // check for an identifier
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
            parserError("Expected expression after token '%s'");
            
            free(str);

            return NULL;
        }
        if (!mtParserCheck(state, TokenType_RightParentheses))
        {
            struct Token lastToken = mtParserGetLastToken(state);

            char* str = malloc(sizeof(char) * (lastToken.size+1));
            mtGetTokenString(lastToken, str, token.size);

            parserError("Expected rightparentheses after token '%s'", str);
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
    if (mtParserCheck(state, TokenType_Identifier))
    {
        struct Token identifier = mtParserGetToken(state);
        mtParserAdvance(state);
        
        struct Token operator = mtParserGetToken(state);
        if (operator.type != TokenType_OperatorAssign)
        {
            //handle it as an expression instead
            state->currentToken--;
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
        if (mtParserCheck(state, TokenType_Identifier))
        {
            struct ASTNode* parameter = mtASTTokenCreateNode(mtParserGetToken(state)); 
            mtASTAddChildNode(parameters, parameter);
        }
    
        mtParserAdvance(state);
        if (!mtParserCheck(state, TokenType_Comma))
        {
            parserError("Commas must separate all parameters!");
            return NULL;
        }

        mtParserAdvance(state);
    }

    //advance past the right parentheses.
    mtParserAdvance(state);

    return parameters;
}

struct ASTNode* parseFunctionDef(struct mtParserState* state)
{
    struct Token identifier; 

    if (!mtParserCheck(state, TokenType_FunctionKeyword))
    {
        return NULL;
    }
    mtParserAdvance(state);
    
    if (!mtParserCheck(state, TokenType_Identifier))
    {
        parserError("Function identifier must follow function definition keyword!");
        return NULL;
    }

    identifier = mtParserGetToken(state);
    mtParserAdvance(state);

    if (mtParserCheck(state, TokenType_LeftParentheses))
    {
        parserError("Parentheses must follow function identifier!");
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
        return functionDef;
    }
    
    parserError("Functions must end with \"end\" keyword");
    return NULL;
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

    mtParserAdvance(state);
}

struct ASTNode* parseBlock(struct mtParserState* state)
{
    struct ASTNode* block = mtASTCreateNode();
    block->type = NodeType_Block;

    struct ASTNode* child;
    while ( (child = parseStatement(state)) )
    {
        mtASTAddChildNode(block, child); 
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
    
    if (tokenCount > 2)
    {
        rootNode = parseBlock(&state); 
    }
    if (rootNode == NULL)
        return NULL;

    return rootNode;
}

#endif

#endif
