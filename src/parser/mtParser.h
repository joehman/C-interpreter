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
#include <tokenizer/mtToken.h>
#include <parser/mtParserState.h>

#include <stdarg.h>
#include <stdlib.h>

// ______________ Declaration _____________



//@returns the root node of the AST.
struct ASTNode* mtASTParseTokens(struct Token* tokens, size_t tokenCount);


//@brief print errors to stderr, uses printf formats
void parserError(struct mtParserState state, const char* fmt, ...);

struct ASTNode* parseExpression(struct mtParserState* state);
struct ASTNode* parseFactor(struct mtParserState* state);
struct ASTNode* parseTerm(struct mtParserState* state);
struct ASTNode* parseBlock(struct mtParserState* state);

struct ASTNode* parseFunctionCall(struct mtParserState* state);

#endif
