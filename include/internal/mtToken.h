
#ifndef mtToken_h
#define mtToken_h

#include "mtUtilities.h"

enum TokenType {
    TokenType_Ignore,
    TokenType_NullTerminator,   
    TokenType_EndOfStatement,

    TokenType_Identifier,

    TokenType_IntegerLiteral,   
    TokenType_DecimalLiteral,  

    TokenType_OperatorAssign,        
    TokenType_OperatorAddition,      
    TokenType_OperatorSubtraction,   
    TokenType_OperatorDivision,      
    TokenType_OperatorMultiplication,
    
    TokenType_LeftParentheses,
    TokenType_RightParentheses,
    TokenType_LeftBracket,
    TokenType_RightBracket,
    TokenType_Comma,

    TokenType_FunctionKeyword,
    TokenType_EndKeyword
};

struct Token {
    enum TokenType type;

    // WARNING!! not null-terminated.
    char* string;
    size_t size;

    int line;
};


// @brief Creates an empty token, with Token.string at NULL, and Token.size at 0
//
// @param token does not have to be declared in any way.
void mtCreateToken(struct Token* token);

// @brief Sets all Tokens to zero, eg. Token.string to NULL, and Token.size to 0
//
// @param tokens an an arrray of struct Token(s).
// @param tokenCount the length of struct Token* tokens.
void mtCreateTokens(struct Token* tokens, size_t tokenCount);



// @brief Removes all tokens in filterTokens from unFilteredTokens
//
// @param unFilteredTokens an array of tokens, created with mtCreateTokens()
// @param unFilteredTokenCount the number of elements in the unFilteredTokens array.
//
// @param filterTokens an array of tokens, created with mtCreateTokens(), of filterTokenCount length.
// @param filterTokensCount the number of elements in the filterTokens array.
void mtFilterTokens(struct Token* unFilteredTokens, size_t unFilteredTokenCount, const struct Token* filterTokens, size_t filterTokenCount);

// @brief Prints the string of a token
// 
// @param token a token created with mtCreateToken()
void mtPrintTokenString(struct Token token);

//@brief Prints the string of all tokens in an array
//
//@param tokens an array of tokens created with mtCreateTokens()
//@param tokenCount the number of tokens in the struct Token* tokens array.
void mtPrintTokenStrings(struct Token* tokens, size_t tokenCount);

//@brief Prints the type of a token as a number.
//
//@param token a token created with mtCreateToken()
void mtPrintTokenType(struct Token token);

//@brief Prints all the types of tokens as numbers.
//
//@param tokens An array of tokens created with mtCreateTokens()
void mtPrintTokenTypes(struct Token* tokens, size_t tokenCount);

//@brief Creates a token with a string literal then returns it.
//
//@param string any string literal, no memory that may be freed.
struct Token mtCreateStringToken(const char* string);

// @brief Returns the number of tokens preceeding the null-terminator-token in an array of tokens.
//
// @param tokens is an array of struct Token(s) created with mtCreateTokens().
// @param tokenCount is the size of the struct Token* tokens array.
size_t mtGetTokenCount(struct Token* tokens, size_t tokenCount);

// @brief strcmp but for tokens. 
//
// @param t1 a token created with mtCreateToken()
// @param t2 a token created with mtCreateToken()
//
// @return the result of a strcmp() between t1 and t2
int mtTokenCmp(struct Token t1, struct Token t2);

//@brief Writes the token's string to str including a null terminator, up to stringSize
//
//@param token the token
//@param str a string with the capacity of token.size+1
void mtGetTokenString(struct Token token, char* str, size_t stringSize);

#endif
