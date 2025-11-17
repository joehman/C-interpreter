
#include "mtToken.h"


int mtTokenCmp(struct Token t1, struct Token t2)
{
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
    
    mtGetTokenString(t1, (char*)&strT1, t1.size);
    mtGetTokenString(t2, (char*)&strT2, t2.size);

    return strcmp(&strT1[0], &strT2[0]);
}

void mtCreateToken(struct Token* token)
{
    token->type = 0;
    token->size = 0;
    token->string = NULL;
    token->line = -1;
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

void mtPrintTokenType(struct Token token)
{
     printf("%d, ", token.type);
}

void mtPrintTokenTypes(struct Token* tokens, size_t tokenCount)
{
    for (size_t i = 0; i < tokenCount; i++)
    {
        mtPrintTokenType(tokens[i]);
        if (tokens[i].type == TokenType_NullTerminator)
        {
            printf("\n");
            return;
        }
    }
    printf("\n");
}

struct Token mtCreateStringToken(const char* string)
{
    // token strings are not null terminated!
    struct Token out;
    out.string = (char*)string;
    out.size = strlen(string);

    return out;
}

void mtGetTokenString(struct Token token, char* str, size_t stringSize)
{
    if (token.size == 0)
    {
        str = NULL;
        return;
    }

    if (stringSize > token.size)
        stringSize = token.size;
    
    memcpy(str, token.string, stringSize);
    str[stringSize] = '\0';

    for (size_t i = 0; i < strlen(str); i++)
    {
        if (str[i] == '\n')
            str[i] = '\n';
    }
}
