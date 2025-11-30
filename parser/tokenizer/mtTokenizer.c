#include "internal/mtToken.h"
#include "internal/mtTokenizer.h"
#include "mtUtilities.h"

#include <inttypes.h>



struct Token* mtTokenize(char* str, struct TokenTypeRules rules, size_t* tokenCount)
{
    char separators[] = {
        rules.additionChar,
        rules.divisionChar,
        rules.multiplicationChar,
        rules.subtractionChar,
        rules.assignChar,

        rules.leftParentheses,
        rules.rightParentheses,
        rules.commaChar,

        rules.endStatementChar,
        rules.separatorChar,
        rules.endOfFileChar
    };

    mtTokenizerGetTokenCountFromString(str, tokenCount, (char*) &separators[0], mtArraySize(separators));

    //allocate the array with that size
    struct Token *tokens = malloc( (*tokenCount) * sizeof(struct Token));
    mtCreateTokens(tokens, *tokenCount);
    
    //populate the tokens array
    mtTokenizerFindAllTokens(str, &tokens[0], *tokenCount, &separators[0], mtArraySize(separators));    

    //remove all unneeded tokens 
    const struct Token unneededTokens[] = {
        mtCreateStringToken(&rules.separatorChar)
    };
    mtFilterTokens(&tokens[0], *tokenCount, &unneededTokens[0], mtArraySize(unneededTokens));

    //set all token types, operators, numbers, identifiers etc.
    mtTokenizerSetTokenTypes(&tokens[0], *tokenCount, rules);
   
    return tokens;
}


void mtTokenizerSetTokenType(struct Token* token, struct TokenTypeRules rules)
{
    if ((token->size == 0) || (token->string == NULL)) 
    {
        token->type = TokenType_Ignore;
        return;
    }
    
    if (token->size == 1)
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

//@brief Advances state past token
void mtTokenizerStateAdvance(struct TokenizerState* state, struct Token* token)
{
    if (!token)
        return;
    if (token->size <= 0)
        return;

    state->position += token->size;
    state->remainingLength -= token->size;
    state->currentToken++;
}

#define advance(state, token)           \
if (token->size == 1) {                 \
    if (token->string[0] == '\n') {     \
        state->line++;                  \
    }                                       \
}                                       \
mtTokenizerStateAdvance(state, token);   

bool checkSingleChar(struct SeparatorChars separators)
{

}

void mtTokenizerFindToken(struct TokenizerState* state, struct SeparatorChars separators)
{

    struct Token* token = &state->tokens[state->currentToken];
    token->string = state->position;

    if (mtAnyOfN(&state->position[0], 1, 
                 separators.singleCharSeparators, 
                 separators.singleCharSeparatorCount))
    {
        token->size = 1;
        advance(state, token);
        return;
    }
    
    // this assumes the first token is not a separator, which is guarranteed by the check above.
    for (int i = 0; i < state->remainingLength; i++)
    {
        // check the character after this one
        if (mtAnyOfN(&state->position[i+1], 1, 
                     separators.singleCharSeparators, 
                     separators.singleCharSeparatorCount))
        {
            // if it is a separator, stop now so that we don't include it.
            // add 1 because i is an index and not a count.
            token->size = i+1;
            token->line = state->line;
            
            advance(state, token);
            return;
        }
    }
}


void mtTokenizerFindAllTokens(char* str, struct Token* tokens, size_t maxTokens, char* separators, size_t separatorCount)
{
    struct TokenizerState state;
    mtCreateTokenizerState(&state, tokens, maxTokens, str);

    for (size_t i = 0; i < maxTokens; i++) 
    {
        mtTokenizerFindToken(&state, separators, separatorCount);
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

void mtCreateTokenizerState(struct TokenizerState* state, struct Token* tokens, size_t tokenCount, char* str)
{
    state->tokens = tokens;
    state->tokenSize = tokenCount;
    state->currentToken = 0;

    state->line = 1;
    state->file = NULL;

    state->remainingLength = strlen(str)+1;
    state->position = str;
}
