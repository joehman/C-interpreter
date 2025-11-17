/*
* The Tokenizer, often called a lexer, turns the source code into small 'tokens'
* which makes it easier for the interpreter to "understand" the code, it tells
* the interpreter that "this part is separate from that part".
*
* For example,
* a = 2
* would become
* "a", "=", "2"
*/

#ifndef mtTokenization_h
#define mtTokenization_h

#include <util/mtUtilities.h>
#include <tokenizer/mtToken.h>

#include <string.h>

// _______________ Declarations ____________
struct TokenTypeRules 
{
    const char separatorChar;
    const char endOfFileChar;
    const char endStatementChar;

    //operators
    const char assignChar;
    const char additionChar;
    const char subtractionChar;
    const char divisionChar;
    const char multiplicationChar;
    const char commaChar;

    const char leftParentheses;
    const char rightParentheses;
    const char leftBracket;
    const char rightBracket;

    const char numbers[10];
    const char decimalSeparator; // separates the fractions, ex: in 5.5 the . is the decimalSeparator.
    
    const char* functionKeyword;
    const char* endKeyword;
};

// @brief Returns the number of Tokens in a string
//
// @param str a null terminated string
// @param count a pointer to where the number of tokens will be stored
// @param separators the characters which separate tokens
// @param separatorCount the number of separators
void mtGetTokenCountFromString(char* str, size_t *count, char* separators, size_t separatorCount);

//@brief Sets the token's type based on rules.
//
//@param token a pointer to a token created with mtCreateToken and populated with mtFindToken.
//@param rules the rules to apply when deciding a token's type.
void mtSetTokenType(struct Token* token, struct TokenTypeRules rules);

//@brief Sets an array of tokens' types based on rules
//
//@param tokens a pointer to an array of tokens created with mtCreateTokens and populated with mtFindTokens
//@param tokenCount the number of elements in the tokens array
//
//@param rules the rules to apply when deciding the individual tokens' types.
void mtSetTokenTypes(struct Token* tokens, size_t tokenCount, struct TokenTypeRules rules);
#endif //mtTokenization_h
