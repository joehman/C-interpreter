#ifndef mtInterpreter_h
#define mtInterpreter_h

/*
*   This is the part of the interpreter that actually interprets all the code and does stuff.
*/

#include "mtParser.h"

// ___________ Declarations ______________

/*
*   expression  = {add | sub} term {add | sub} term .
*   term        = factor {mul | div} factor
*   factor      = number | "lparen" expression "rparen" (parentheses are not in the AST)
*/

void mtInterpreterEvaluate(struct ASTNode* node);

int mtInterpretFactor(struct ASTNode* node);

int mtInterpretTerm(struct ASTNode* node);

int mtInterpretExpression(struct ASTNode* node);




// ___________ Implementation ___________
#ifdef mtImplementation

//@brief print errors to stderr, uses printf formats
void interpreterError(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "Error while interpreting: \n\t");
    vfprintf(stderr, fmt, args);
}




//no parentesses
int mtInterpretNumber(struct ASTNode* node)
{
    if (node == NULL)
        return 0;

    size_t tokenSize = node->token.size;

    int num = 0; 
    
    //would use a variable-sized array, but that's not supported everywhere.
    char* str = malloc(tokenSize * sizeof(char) + 1 );

    memcpy(str, node->token.string, tokenSize);
    str[tokenSize] = '\0';

    int err = mtStringToInt(&num, str, 10);

    free(str);

    if (err == mtSuccess)
        return num;
    
    char* tokenString = malloc(tokenSize * sizeof(char) + 1);
    mtGetTokenString(node->token, tokenString, tokenSize);

    if (err == mtStringToIntInconvertible)
    {
        interpreterError("Failed to read token '%s' as number: inconvertible\n", tokenString);
    }

    if (err == mtStringToIntOverflow)
    {
        interpreterError("Failed to read token '%s' as number: integer overflow\n", tokenString);
    }
    if (err == mtStringToIntUnderflow)
    {
        interpreterError("Failed to read token '%s' as number: integer undeflow\n", tokenString);
    }
    free(tokenString);
    return 0;
}

int interpretBinOp(struct ASTNode* node)
{
    if (node == NULL)
        return 0;
    if (node->token.type == TokenType_NumberLiteral)
    {
        return mtInterpretNumber(node);
    }

    bool hasEnoughChildren = node->childCount >= 2;
    if (!hasEnoughChildren)
        return 0;

    int left = interpretBinOp(node->children[0]);
    int right = interpretBinOp(node->children[1]);

    //no need to break in any of these
    switch (node->token.type)
    {
    case TokenType_OperatorAddition:
        return left+right;    
    
    case TokenType_OperatorSubtraction:
        return left-right;
    
    case TokenType_OperatorMultiplication:
        return left*right;
    
    case TokenType_OperatorDivision:
        if (right == 0)
            interpreterError("Can't divide by zero!\n");
            return left;
        return left/right;

    case TokenType_OperatorAssign:
        interpreterError("Can't assign to number literals!\n");
        return left;
    default:
        break;
    }


    return 0;
}

void mtInterpreterEvaluate(struct ASTNode* node)
{
    printf("%d\n", interpretBinOp(node));
}


#endif // mtImplementation

#endif // mtInterpreter_h