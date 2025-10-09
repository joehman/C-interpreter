#ifndef mtInterpreter_h
#define mtInterpreter_h


/*
*   This is the part of the code that actually interprets all the code and does stuff.
*/

#include "mtUtilities.h" 
#include "mtParser.h"
#include <math.h>

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

enum NumberType {
	DECIMAL,
	INTEGER
};

struct Number {
	enum NumberType type;
	union {
		int integer;
		double decimal;	
	};
};

//@brief print errors to stderr, uses printf formats
static void interpreterError(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "Error while interpreting: \n\t");
    vfprintf(stderr, fmt, args);

    if (fmt[strlen(fmt)] != '\n')
    {
        printf("\n");
    }
}


struct Number numberAdd(struct Number a, struct Number b)
{
	struct Number out;
    
	if (a.type == DECIMAL || b.type == DECIMAL)
	{
		out.type = DECIMAL;
		double x = (a.type == DECIMAL) ? a.decimal : a.integer; 
		double y = (b.type == DECIMAL) ? b.decimal : b.integer;
		out.decimal = x+y;
	} else {
		out.type = INTEGER;
		out.integer = a.integer + b.integer;
	}
	return out;
}	

struct Number numberSubtract(struct Number a, struct Number b)
{
	struct Number out;
    
	if (a.type == DECIMAL || b.type == DECIMAL)
	{
		out.type = DECIMAL;
		double x = (a.type == DECIMAL) ? a.decimal : a.integer; 
		double y = (b.type == DECIMAL) ? b.decimal : b.integer;
		out.decimal = x-y;
	} else {
		out.type = INTEGER;
		out.integer = a.integer - b.integer;
	}
	return out;
}


struct Number numberMultiply(struct Number a, struct Number b)
{
	struct Number out;
    
	if (a.type == DECIMAL || b.type == DECIMAL)
	{
		out.type = DECIMAL;
		double x = (a.type == DECIMAL) ? a.decimal : a.integer; 
		double y = (b.type == DECIMAL) ? b.decimal : b.integer;
		out.decimal = x*y;
	} else {
		out.type = INTEGER;
		out.integer = a.integer * b.integer;
	}
	return out;
}

struct Number numberDivide(struct Number a, struct Number b)
{
	struct Number out;

	double x = (a.type == DECIMAL) ? a.decimal : a.integer;
    double y = (b.type == DECIMAL) ? b.decimal : b.integer;

	double quotient;
	if (y != 0.0)
	{
		quotient = x/y;
	} else {
		interpreterError("Tried to divide by zero!");
		quotient = 0;
	}

	//ints are always truncated when converting, so 1.2 becomes 1.
	//thanks chatgpt for the fabs() check.
	if (fabs(quotient - (int)quotient) == 0 && 
		quotient <= INT_MAX && quotient >= INT_MIN)
	{
		out.type = INTEGER;
		out.integer = (int)quotient;
		return out;
	}

	out.type = DECIMAL;
	out.decimal = quotient;
	return out;
}


float mtInterpretDecimal(struct ASTNode* node)
{
	if (node == NULL)
		return 0;
	float f;

	size_t tokenSize = node->token.size;

	//node->token.string isn't null terminated.
	char* str = malloc(tokenSize * sizeof(char) +1);
	memcpy(str, node->token.string, tokenSize);
	str[tokenSize] = '\0';

	int err = mtStringToFloat(&f, str);	
	free(str);

	if (err == mtSuccess)
	{
		return f;
	}

    char* tokenString = malloc(tokenSize * sizeof(char) + 1);
    mtGetTokenString(node->token, tokenString, tokenSize);

	if (err == mtStringToFloatInconvertible)
	{
		interpreterError("Failed to read token: '%s' as decimal: Inconvertible", tokenString);
	}

	free(tokenString);
	return 0;
}
int mtInterpretInteger(struct ASTNode* node)
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

struct Number interpretBinOp(struct ASTNode* node)
{
	struct Number out;

	if (node == NULL)
    {
		out.type = INTEGER;
		out.integer = 0;
		interpreterError("NULL");
	}

    if (node->token.type == TokenType_IntegerLiteral)
    {
		out.type = INTEGER;
        out.integer = mtInterpretInteger(node);
	    return out;
    } else if (node->token.type == TokenType_DecimalLiteral)
    {
    	out.type = INTEGER;
	    out.decimal = mtInterpretDecimal(node);
	    return out;
	}

    struct Number left = interpretBinOp(node->children[0]);
    struct Number right = interpretBinOp(node->children[1]);

    //no need to break in any of these
    switch (node->token.type)
    {
    case TokenType_OperatorAddition:
		out = numberAdd(left, right);
        break;
    case TokenType_OperatorSubtraction:
        out = numberSubtract(left, right);
        break;
    case TokenType_OperatorMultiplication:
        out = numberMultiply(left, right); 
        break;
    case TokenType_OperatorDivision:
        out = numberDivide(left, right);
        break;
    case TokenType_OperatorAssign:
        interpreterError("Can't assign to number literals!\n");
        break;
    default:
        break;
    }


    return out;
}

void mtInterpreterEvaluate(struct ASTNode* node)
{
    struct Number out = interpretBinOp(node);
    
	if (out.type == INTEGER)
	{
		printf("%d", out.integer);
	} else if(out.type == DECIMAL)
	{
		printf("%f", out.decimal);
	}


    printf("\n");
}


#endif // mtImplementation

#endif // mtInterpreter_h
