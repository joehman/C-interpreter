/*
* The Parser takes in all the different tokens the tokenizer made,
* and parses them all into an Abstract Syntax Tree(AST), which the
* intepreter then runs.
*/

/*
*   expression  = {add | sub} term {add | sub} term .
*   term        = factor {mul | div} factor
*   factor      = number | "lparen" expression "rparen"
*/


#ifndef mtParser_h
#define mtParser_h

#include "mtUtilities.h"
#include "mtTokenization.h"
#include <stdarg.h>
#include <stdlib.h>

// ______________ Declaration _____________
enum NodeType {
    NodeType_None,


    NodeType_Type,
    NodeType_Assignment,
    NodeType_Declaration,
    NodeType_BinaryOperator,
    NodeType_Number,
    NodeType_Identifier
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
*   block       = statment | expression 
*   statement   = type identifier {assign} expression | identifier {assign} expression
*   expression  = {add | sub} term    {add | sub} term 
*   term        = factor  {mul | div} factor | factor {pow} exponent  
*   factor      = number | identifier | "lparen" expression "rparen" 
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

    if (mtParserCheck(state, TokenType_None))
    {
        char* str = malloc(sizeof(char) * (token.size+1));
        mtGetTokenString(token, str, token.size);
        unexpectedTokenError(token);
        free(str);
    }
    return NULL;
}

struct ASTNode* parseTerm(struct mtParserState* state)
{
    struct ASTNode* left = parseFactor(state);
    
    if (left == NULL)
        return NULL;
    
    bool isRightOperator =  mtParserCheck(state, TokenType_OperatorMultiplication)  || 
                            mtParserCheck(state, TokenType_OperatorDivision)        ||
                            mtParserCheck(state, TokenType_OperatorPower)           ;

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
        mtParserAdvance(state); // remove operator
        
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
    //check for declaration
    if (mtParserCheck(state, TokenType_Type))
    {
        struct Token type = mtParserGetToken(state);
        mtParserAdvance(state);
        struct Token identifier = mtParserGetToken(state);
        mtParserAdvance(state);

        struct ASTNode* identifierNode = mtASTTokenCreateNode(identifier);
        identifierNode->type = NodeType_Identifier;
        struct ASTNode* typeNode = mtASTTokenCreateNode(type);
        typeNode->type = NodeType_Type;

        //make the type a child of the identifier
        mtASTAddChildNode(identifierNode, typeNode);
    
        //no token
        //exists mostly to tell the interpreter that we're declaring a variable.
        struct ASTNode* declarationNode = mtASTCreateNode(); 
        declarationNode->type = NodeType_Declaration;
        mtASTAddChildNode(declarationNode, identifierNode);

        //if we're assigning a variable and declaring
        if (mtParserGetToken(state).type == TokenType_OperatorAssign)
        {
            struct Token operator = mtParserGetToken(state);

            struct ASTNode* assignNode = mtASTTokenCreateNode(operator);
            assignNode->type = NodeType_Assignment;

            mtParserAdvance(state);
            struct ASTNode* expression = parseExpression(state);
            
            mtASTAddChildNode(assignNode, expression);
            mtASTAddChildNode(assignNode, declarationNode);
            
            return assignNode;
        }
    }
    //check for just assignment
    else if (mtParserCheck(state, TokenType_Identifier))
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
    
    //didn't declare or assign, must be an expression
    struct ASTNode* expression = parseExpression(state);

    return expression;
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
        rootNode = parseStatement(&state); 
    }
    if (rootNode == NULL)
        return NULL;

    return rootNode;
}

#endif

#endif
