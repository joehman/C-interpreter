#include "mtTokenizer.h"

void mtTokenizerSetTokenType(struct Token* token, struct TokenTypeRules rules)
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

void mtTokenizerSetTokenTypes(struct Token* tokens, size_t tokenCount, struct TokenTypeRules rules)
{
    for (size_t i = 0; i < tokenCount; i++)
    {
        mtTokenizerSetTokenType(&tokens[i], rules);
    }
}

void mtTokenizerFindToken(struct Token* token, char* position, size_t searchLength, char* separators, size_t separatorCount)
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


void mtTokenizerFindAllTokens(char* str, struct Token* tokens, size_t maxTokens, char* separators, size_t separatorCount)
{
    int line = 1;

    char* ptr = str;
    int remainingLength = strlen(str)+1;

    for (size_t i = 0; i < maxTokens; i++) 
    {
        mtTokenizerFindToken(&tokens[i], ptr, remainingLength, separators, separatorCount);

        if (tokens[i].size == 1 && *tokens[i].string == '\n')
        {
            line += 1; 
        }

        tokens[i].line = line;

        ptr += tokens[i].size;
        remainingLength -= tokens[i].size;
    }
}

void mtTokenizerGetTokenCountFromString(char* str, size_t *count, char* separators, size_t separatorCount)
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

