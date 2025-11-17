#include "mtTokenization.h"

void mtGetTokenCountFromString(char* str, size_t *count, char* separators, size_t separatorCount)
{
    size_t tokenCount = 0;
    int i = 0;
    while (i < strlen(str))
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
    int line = 1;

    char* ptr = str;
    int remainingLength = strlen(str)+1;

    for (size_t i = 0; i < maxTokens; i++) 
    {
        mtFindToken(&tokens[i], ptr, remainingLength, separators, separatorCount);

        if (tokens[i].size == 1 && *tokens[i].string == '\n')
        {
            line += 1; 
        }

        tokens[i].line = line;

        ptr += tokens[i].size;
        remainingLength -= tokens[i].size;
    }
}

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

void mtSetTokenType(struct Token* token, struct TokenTypeRules rules)
{
    bool isEmpty = (token->size == 0) || (token->string == NULL);
    if (isEmpty) // no need to check for anything else if it's empty
    {
        token->type = TokenType_Ignore;
        return;
    }
    
    //surely there's some way to automate this?
    bool isSingleCharacter = token->size == 1;
    if (isSingleCharacter)
    {
        char character = token->string[0];

        if (character == rules.endStatementChar)
        {
            token->type = TokenType_EndOfStatement;
            return;
        }

        if (character == rules.endOfFileChar)
        {
            token->type = TokenType_NullTerminator;
            return;
        }
      
        if (character == rules.commaChar)
        {
            token->type = TokenType_Comma;
            return;
        }

        if (character == rules.rightBracket)
        {
            token->type = TokenType_RightBracket;
            return;
        } else if (character == rules.leftBracket)
        {
            token->type = TokenType_LeftBracket;
            return;
        }
    
        if (character == rules.leftParentheses)
        {
            token->type = TokenType_LeftParentheses;
            return;
        } else if (character == rules.rightParentheses)
        {
            token->type = TokenType_RightParentheses;
            return;
        }
        
        //check for the operators.
        char operators[] = {
            rules.additionChar, 
            rules.subtractionChar, 
            rules.multiplicationChar, 
            rules.divisionChar, 
            rules.assignChar,
        };
        enum TokenType operatorTypes[] = { // same order as above
            TokenType_OperatorAddition,
            TokenType_OperatorSubtraction,
            TokenType_OperatorMultiplication,
            TokenType_OperatorDivision,
            TokenType_OperatorAssign,
        };
        int operatorIndex = mtWhichOf(character, &operators[0], mtArraySize(operators));
        if (operatorIndex != mtFail)
        {
            token->type = operatorTypes[operatorIndex];
            return;
        }
    }

    if (mtTokenCmp(*token, mtCreateStringToken(rules.functionKeyword)) == 0)
    {
        token->type = TokenType_FunctionKeyword;
        return;
    }
    if (mtTokenCmp(*token, mtCreateStringToken(rules.endKeyword)) == 0)
    {
        token->type = TokenType_EndKeyword;
        return;
    }

    bool isIntegerLiteral = mtOnlyOfN(token->string, token->size, (char*)&rules.numbers[0], 10);
    if (isIntegerLiteral)
    {
        token->type = TokenType_IntegerLiteral;
        return;
    }

    //There has to be a better way of doing this.
    char decimalChars[11] = {0};
    memcpy(decimalChars, rules.numbers, 10);
    decimalChars[10] = rules.decimalSeparator;

    bool isDecimal = mtAnyOfN(token->string, token->size, decimalChars, mtArraySize(decimalChars));
    if (isDecimal)
    {
        token->type = TokenType_DecimalLiteral;
        return;
    }


    token->type = TokenType_Identifier;
}

void mtSetTokenTypes(struct Token* tokens, size_t tokenCount, struct TokenTypeRules rules)
{
    for (size_t i = 0; i < tokenCount; i++)
    {
        mtSetTokenType(&tokens[i], rules);
    }
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
