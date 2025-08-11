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

// abstract syntax tree
struct ASTNode {
    struct Token token;

    //pointer to multiple pointers
    struct ASTNode** children;
    size_t childCount;
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

// ___________ Helper functions ______________

//@brief print errors to stderr, uses printf formats
void parserError(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "Error while parsing: \n\t");
    vfprintf(stderr, fmt, args);
}
void unexpectedTokenError(struct Token token)
{
    const char* newlineLiteral = "\\n";
    const char* endOfFileLiteral = "\\0";

    char* str = malloc(token.size * sizeof(char));
    mtGetTokenString(token, str, token.size);

    if (token.string[0] == '\n')
    {
        parserError("Unexpected token: '%s'\n", newlineLiteral);
        return;
    }
    if (token.string[0] == '\0')
    {
        parserError("Unexpected token: '%s'\n", endOfFileLiteral);
    }
    parserError("Unexpected token: '%s'\n", str);
}


struct ASTNode* mtASTAddNode(struct ASTNode* parent)
{
    int index = parent->childCount;
    struct ASTNode* ptr = ( parent->children[index] = mtASTCreateNode() );
    parent->childCount++;

    return ptr;
}   

int mtASTAddChildNode(struct ASTNode* parent, struct ASTNode* child)
{
    int index = parent->childCount;
    
    parent->children[index] = child;
    parent->childCount++;

    return index;
}   

struct ASTNode* mtASTCreateNode()
{

    struct ASTNode* out = malloc( sizeof(struct ASTNode) );

    out->childCount = 0;
    out->children = malloc (sizeof(struct ASTNode**));
    mtCreateToken(&out->token);

    return out;
}
struct ASTNode* mtASTTokenCreateNode(struct Token token)
{

    struct ASTNode* out = malloc( sizeof(struct ASTNode) );

    out->childCount = 0;
    out->children = malloc (sizeof(struct ASTNode**));
    
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

struct ASTNode* parseFactor(struct mtParserState* state)
{
    struct Token token = mtParserGetToken(state);

    if (mtParserCheck(state, TokenType_NumberLiteral))
    {
        mtParserAdvance(state);
        return mtASTTokenCreateNode(token);
    }

    if (mtParserCheck(state, TokenType_LeftParentheses))
    {
        mtParserAdvance(state);
        struct ASTNode* node = parseExpression(state);
  
        if (node == NULL)
        {
            struct Token lastToken = mtParserGetLastToken(state);
            char* str = malloc(sizeof(char) * lastToken.size);

            mtGetTokenString(lastToken, str, lastToken.size);
            parserError("Expected expression after token '%s'\n");

            return NULL;
        }
        if (!mtParserCheck(state, TokenType_RightParentheses))
        {
            struct Token lastToken = mtParserGetLastToken(state);

            char* str = malloc(sizeof(char) * lastToken.size);
            mtGetTokenString(lastToken, str, token.size);

            parserError("Expected rightparentheses after token '%s'\n", str);
        }
        // remove the right-parenteses.
        mtParserAdvance(state);
        return node;
    }


    if (mtParserCheck(state, TokenType_None))
    {
        char* str = malloc(sizeof(char) * token.size);
        mtGetTokenString(token, str, token.size);
        unexpectedTokenError(token);
    }

    
    return NULL;
}
struct ASTNode* parseTerm(struct mtParserState* state)
{
    struct ASTNode* left = parseFactor(state);
    
    if (left == NULL)
        return NULL;

    while (mtParserCheck(state, TokenType_OperatorMultiplication) || mtParserCheck(state, TokenType_OperatorDivision))
    {
        struct Token operator = mtParserGetToken(state);
        mtParserAdvance(state); // remove operator
        
        struct ASTNode* node = mtASTTokenCreateNode(operator);
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
    
    while (mtParserCheck(state, TokenType_OperatorAddition) || mtParserCheck(state, TokenType_OperatorSubtraction))
    {
        struct Token operator = mtParserGetToken(state);
        mtParserAdvance(state); // remove operator
        
        struct ASTNode* node = mtASTTokenCreateNode(operator);
        mtASTAddChildNode(node, left);
        mtASTAddChildNode(node, parseTerm(state));

        //so that we can return it
        left = node;
    }

    if (left == NULL)
        return NULL;

    return left;
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
        rootNode = parseExpression(&state);
    }
    if (rootNode == NULL)
        return NULL;

    return rootNode;
}



#endif


#endif