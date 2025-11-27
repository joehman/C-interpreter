
#include "internal/mtParserState.h"

struct Token mtParserGetToken(struct mtParserState* state)
{
    return state->tokens[state->currentToken];
}
//@brief Gets the token before the current one and returns it.
struct Token mtParserGetLastToken(struct mtParserState* state)
{
    //returning state->tokens[-1] would be bad.
    if (state->currentToken != 0)
        return state->tokens[state->currentToken-1];
    
    return state->tokens[state->currentToken];
}
struct Token mtParserAdvance(struct mtParserState* state)
{
    return state->tokens[state->currentToken++];
}
bool mtParserCheck(struct mtParserState* state, enum TokenType type)
{
    if (state->tokens[state->currentToken].type == type )
        return true;
    return false;
}
