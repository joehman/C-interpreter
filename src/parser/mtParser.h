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

#include <util/mtUtilities.h>
#include <tokenizer/mtTokenization.h>
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
    NodeType_ArgumentList,

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
void parserError(struct mtParserState state, const char* fmt, ...);

struct Token mtParserGetToken(struct mtParserState* state);

struct ASTNode* parseExpression(struct mtParserState* state);
struct ASTNode* parseFactor(struct mtParserState* state);
struct ASTNode* parseTerm(struct mtParserState* state);
struct ASTNode* parseBlock(struct mtParserState* state);

struct ASTNode* parseFunctionCall(struct mtParserState* state);

#endif
