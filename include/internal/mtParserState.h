
#include "mtToken.h" 

struct mtParserState {
    struct Token* tokens;
    size_t currentToken;
    size_t tokenCount;
};

struct Token mtParserGetToken(struct mtParserState* state);
//@brief Gets the token before the current one and returns it.
struct Token mtParserGetLastToken(struct mtParserState* state);
struct Token mtParserAdvance(struct mtParserState* state);
bool mtParserCheck(struct mtParserState* state, enum TokenType type);
