/*
* The Tokenizer, often called a lexer, turns the source code into small 'tokens'
* which makes it easier for the interpreter to "understand" the code, it tells
* the interpreter that "this part is separate from that part".
*/




#ifndef mtTokenization_h
#define mtTokenization_h

#include "mtUtilities.h"

#include <string.h>
// _______________ Declarations ____________


enum TokenType {

    // the bit-shifting is done so that a type can be 
    // set to multiple types at once via an OR operator, eg. variable1 |= TokenType_whatever
    
    TokenType_Ignore,           // all tokens with this type are ignored by the interpreter. 
    TokenType_NullTerminator,   // exists so that the interpreter knows when there's no more code left.
    TokenType_NumberLiteral,    // any number literal, ex. 5

    TokenType_OperatorAssign,        
    TokenType_OperatorAddition,      
    TokenType_OperatorSubtraction,   
    TokenType_OperatorDivision,      
    TokenType_OperatorMultiplication,
    TokenType_None                 
};

struct Token {
    enum TokenType type;

    // WARNING!! not null-terminated.
    char* string;
    size_t size;
};

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


    const char numbers[10];

    
};


// @brief Creates an empty token, eg. with Token.string at NULL, and Token.size at 0
//
// @param token does not have to be declared in any way.
void mtCreateToken(struct Token* token);

// @brief Sets all Tokens to zero, eg. Token.string to NULL, and Token.size to 0
//
// @param tokens an an arrray of struct Token(s).
// @param tokenCount the length of struct Token* tokens.
void mtCreateTokens(struct Token* tokens, size_t tokenCount);


// @brief Finds the first token after (and including) char* position
//
// @param token a token created with mtCreateToken()
// @param position a pointer to any string
// @param searchLength defines how far forward from char* position we should search.
// @param separators  an array of chars which define what characters should separate tokens.
// @param separatorCount  the number of elements in char* separators.
void mtFindToken(struct Token* token, char* position, size_t searchLength, char* separators, size_t separatorCount);

//@brief Finds all tokens in char* str then writes them to struct Token* tokens.
//
//@param str the text to be tokenized.
//
//@param tokens an array of tokens created with mtCreateTokens()
//@param maxTokens the maximum number of tokens to write to.
//
//@param separators an array of chars which define what characters should separate tokens.
//@param separatorCount the number of elements in the separators array.
void mtFindAllTokens(char* str, struct Token* tokens, size_t maxTokens, char* separators, size_t separatorCount);

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
// @details as an optmization, if the length of one token is not equal to that of the other, then it will always return -1.
// @param t1 a token created with mtCreateToken()
// @param t2 a token created with mtCreateToken()
//
// @return the result of a strcmp() between t1 and t2
int mtTokenCmp(struct Token t1, struct Token t2);

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

// ___________________ IMPLEMENTATION ___________________

#ifdef mtImplementation

void mtGetTokenCountFromString(char* str, size_t *count, char* separators, size_t separatorCount)
{
    size_t tokenCount = 0;
    int i = 0;
    
    while ( i < strlen(str))
    {
        // the current character is a separator
        if (mtAnyOfN(&str[i], 1, separators, separatorCount))
        {
            tokenCount++;
            i++;
        } else {
            //start of a new token
            tokenCount++;
            
            //advance until the end of the token
            while (i < strlen(str) && !mtAnyOfN(&str[i], 1, separators, separatorCount))
                i++;

        }
    }

    //add the null-terminator
    tokenCount++;

    *count = tokenCount;
}

void mtFindToken(struct Token* token, char* position, size_t searchLength, char* separators, size_t separatorCount)
{
        
    token->string = position;

    if (mtAnyOfN(&position[0], 1, separators, separatorCount))
    {
        token->size = 1;
        return;
    }
    
    // this assumes the first token is not a separator, which is guarranteed by the check above.
    for (int i = 0; i < searchLength; i++)
    {
        // check the character after this one
        if (mtAnyOfN(&position[i+1], 1, separators, separatorCount))
        {
            // if it is a separator, stop now so that we don't include it.
            // add 1 because i is an index and not a count.
            token->size = i+1;
            return;
        }
    }

    return;
}


void mtFindAllTokens(char* str, struct Token* tokens, size_t maxTokens, char* separators, size_t separatorCount)
{

    char* ptr = str;
    int remainingLength = strlen(str)+1;

    for (size_t i = 0; i < maxTokens; i++) 
    {
        mtFindToken(&tokens[i], ptr, remainingLength, separators, separatorCount);

        ptr += tokens[i].size;
        remainingLength -= tokens[i].size;
    }
}

int mtTokenCmp(struct Token t1, struct Token t2)
{
    if (t1.size != t2.size)
        return -1;

    // either of the strings being null would be fine,
    // if we didn't manually set the end of the string
    // to \0.
    if (t1.string == NULL && t2.string == NULL)
        return 0;

    if (t1.string == NULL)
        return -1;
    if (t2.string == NULL)
        return 1;

    char strT1[t1.size];
    char strT2[t2.size];

    memcpy(strT1, t1.string, t1.size);
    memcpy(strT2, t2.string, t2.size);
    
    strT1[t1.size] = '\0';
    strT2[t2.size] = '\0';

    return strcmp(&strT1[0], &strT2[0]);
}

size_t mtGetTokenCount(struct Token* tokens, size_t tokenCount)
{
    for (size_t i = 0; i < tokenCount; i++) 
    {
        if (strcmp(tokens->string, "\0") == 0)
        {
            return i+1;
        }
    }
    printf("mtGetTokenCount: Token list does not terminate with \\0 token!\n");
    return tokenCount;
}

void mtCreateToken(struct Token* token)
{
    token->type = 0;
    token->size = 0;
    token->string = NULL;
}
void mtCreateTokens(struct Token* tokens, size_t tokenCount)
{
    for (size_t i = 0; i < tokenCount; i++)
    {
        mtCreateToken(&tokens[i]);
    }
}

void mtFilterTokens(struct Token* unFilteredTokens, size_t unFilteredTokenCount, const struct Token* filterTokens, size_t filterTokenCount)
{
    
    struct Token newTokenList[unFilteredTokenCount];
    mtCreateTokens(&newTokenList[0], unFilteredTokenCount);
    
    int shortenAmount = 0;
    for (size_t i = 0; i < unFilteredTokenCount; i++) 
    {
        bool skip = false;
        for (size_t j = 0; j < filterTokenCount; j++ )
        {
            if (mtTokenCmp(filterTokens[j], unFilteredTokens[i]) == 0)
            {
                skip = true;
            }
        }

        if (!skip)
        {
            newTokenList[i-shortenAmount] = unFilteredTokens[i];
        } else
        {
            shortenAmount++;
        }

        if (unFilteredTokens[i].string[0] == '\0')
            break;
    }

    memcpy(unFilteredTokens, newTokenList, unFilteredTokenCount * sizeof(struct Token));
}

void mtPrintTokenStrings(struct Token* token, size_t tokenCount)
{
    for (size_t i = 0; i < tokenCount; i++)
    {
        mtPrintTokenString(token[i]);

        if (token[i].size <= 0)
            continue;

        if (token[i].string[0] == '\0')
            break;
        printf(",");
    }
}
void mtPrintTokenString(struct Token token)
{
    printf("'");
    for (size_t i = 0; i < token.size; i++) 
    {
        if (token.size <= 0)
            continue;

        if (token.string[i] == '\n')
        {
            printf("\\n");
            continue;
        }
        if (token.string[i] == '\0')
        {
            printf("\\0");
            continue;
        }

        printf("%c", token.string[i]);
    }
    printf("'");
}


struct Token mtCreateStringToken(const char* string)
{
    // token strings are not null terminated!
    struct Token out;
    out.string = (char*)string;
    out.size = strlen(string);

    return out;
}

void mtSetTokenType(struct Token* token, struct TokenTypeRules rules)
{
    bool isEmpty = (token->size == 0) || (token->string == NULL);
    if (isEmpty) // no need to check for anything else if it's empty
    {
        token->type = TokenType_Ignore;
        return;
    }

    bool isSingleCharacter = token->size == 1;
    if (isSingleCharacter)
    {
        bool isEndofFile = token->string[0] == rules.endOfFileChar;
        if (isEndofFile) 
        {
            token->type = TokenType_NullTerminator;
            return;
        }

        //check for the operators.
        char operators[] = {
            rules.additionChar, 
            rules.subtractionChar, 
            rules.multiplicationChar, 
            rules.divisionChar, 
            rules.assignChar
        };
        enum TokenType operatorTypes[] = { // same order as above
            TokenType_OperatorAddition,
            TokenType_OperatorSubtraction,
            TokenType_OperatorMultiplication,
            TokenType_OperatorDivision,
            TokenType_OperatorAssign
        };
        int operatorIndex = mtWhichOf(token->string[0], &operators[0], mtArraySize(operators));
        if (operatorIndex != mtFail)
        {
            token->type = operatorTypes[operatorIndex];
            return;
        }
    }
    
    bool isNumberLiteral = mtOnlyOfN(token->string, token->size, (char*)&rules.numbers[0], 10);
    if (isNumberLiteral)
    {
        token->type = TokenType_NumberLiteral;
        return;
    }
    
    token->type = TokenType_None;
}

void mtSetTokenTypes(struct Token* tokens, size_t tokenCount, struct TokenTypeRules rules)
{
    for (size_t i = 0; i < tokenCount; i++)
    {
        mtSetTokenType(&tokens[i], rules);
    }
}

#endif //mtImplementation

#endif //mtTokenization_h
