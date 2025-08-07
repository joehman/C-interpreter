#ifndef mtTokenization_h
#define mtTokenization_h

#include "mtUtilities.h"

#include <string.h>

struct Token {
    char* string;
    size_t size;
};

// _______________ Declarations ____________

// strcmp for tokens
int mtTokenCmp(struct Token t1, struct Token t2);

// finds the first token following position
void mtFindToken(struct Token* token, char* position, size_t length);

void mtPrintToken(struct Token token);

void mtFindAllTokens(struct Token* tokens, size_t maxTokens, char* position, size_t stringLength);

size_t mtGetTokenCount(struct Token* tokens, size_t tokenCount);

// ___________________ IMPLEMENTATION ___________________
#ifdef mtImplementation
#define mtImplementation

const char TokenSeparators[] = {
    ' '  ,
    '\n' ,
    '\0'
};


const struct Token TokenSeparators_struct[] = {
    { .string = " ",  .size    = 1 },
    { .string = "\n", .size    = 1 }
};

bool isCharTokenSeparator(char character)
{
    for (size_t i = 0; i < sizeof(TokenSeparators) / sizeof(char); i++) 
    {
        if (character == TokenSeparators[i])
            return true;
    }
    return false;
}

void mtFindToken(struct Token* token, char* position, size_t length)
{
    token->string = position;

    if (isCharTokenSeparator(position[0]))
    {
        token->size = 1;
        return;
    }
    
    // this assumes the first token is not a separator, which is guarranteed by the check above.
    for (int i = 0; i < length; i++)
    {
        // check the next character
        if (isCharTokenSeparator(position[i+1]))
        {
            // if it is a separator, stop now so that we don't include it.
            // add 1 because i is an index and not a count.
            token->size = i+1;
            return;
        }
    }
}

void mtFindAllTokens(struct Token* tokens, size_t maxTokens, char* position, size_t stringLength)
{
    char* ptr = position;
    int remainingLength = stringLength;

    for (size_t i = 0; i < maxTokens; i++) 
    {
        mtFindToken(&tokens[i], ptr, remainingLength);
        
        ptr += tokens[i].size;
        remainingLength -= tokens[i].size;
       
        // we want the '\0' to be included as a token.
        if (*(ptr-1) == '\0')
        {
            break;
        }
    }
}

void mtPrintToken(struct Token token)
{
    printf("'");
    for (size_t i = 0; i < token.size; i++) 
    {
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

// strcmp but for tokens
//
// But not really, as an optmization, if the length of one token is not equal to that of the other, then it will always return -1.
int mtTokenCmp(struct Token t1, struct Token t2)
{
    if (t1.size != t2.size)
        return -1;

    char strT1[t1.size];
    char strT2[t1.size];
    
    memcpy(strT1, t1.string, t1.size);
    memcpy(strT2, t2.string, t2.size);

    strT1[t1.size] = '\0';
    strT2[t2.size] = '\0';

    printf("T1:'%s', T2:'%s'\n", strT1, strT2);

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
#endif // mtImplementation

#endif // mtTokenization_h
