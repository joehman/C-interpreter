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
#define mtImplementation
// ___________ Implementation ___________
#ifdef mtImplementation

struct Result {
	int integer;
	float floating;	
	
	bool isInteger;
	bool isFloat; 	
};

struct Result resultAdd(struct Result r1, struct Result r2)
{
	struct Result out;
    
	if (r1.isFloat || r2.isFloat)
	{
		out.isFloat = true;
		if (r1.isFloat)
		{
			out.floating = r1.floating;
		} else if (r1.isInteger) 
		{
			out.floating = (float)r1.integer;
		}
		if (r2.isFloat)
		{
			out.floating += r2.floating;	
		} else if (r2.isInteger)
		{
			out.floating += (float)r2.integer;	
		}
	} else if (r1.isInteger && r2.isInteger)
	{
		out.isInteger = true;
		out.integer = r1.integer + r2.integer;
	}
	return out;
}	

struct Result resultSubtract(struct Result r1, struct Result r2)
{
	struct Result out;

	if (r1.isFloat || r2.isFloat)
	{
		out.isFloat = true;
		if (r1.isFloat)
		{
			out.floating = r1.floating;
		} else if (r1.isInteger) 
		{
			out.floating = (float)r1.integer;
		}
		if (r2.isFloat)
		{
			out.floating -= r2.floating;	
		} else if (r2.isInteger)
		{
			out.floating -= (float)r2.integer;	
		}
	} else if (r1.isInteger && r2.isInteger)
	{
		if (r1.integer % r2.integer == 0)
		{
			out.isInteger = true;
			out.integer = r1.integer-r2.integer;
		}else {
			out.isInteger = false;
			out.isFloat = true;
			out.floating = (float)r1.integer - (float)r2.integer;
		}
	}

	return out;
}


struct Result resultMultiply(struct Result r1, struct Result r2)
{
	struct Result out;

	if (r1.isFloat || r2.isFloat)
	{
		out.isFloat = true;
		if (r1.isFloat)
		{
			out.floating = r1.floating;
		} else if (r1.isInteger) 
		{
			out.floating = (float)r1.integer;
		}
		if (r2.isFloat)
		{
			out.floating *= r2.floating;	
		} else if (r2.isInteger)
		{
			out.floating *= (float)r2.integer;	
		}
	} else if (r1.isInteger && r2.isInteger)
	{
		out.isInteger = true;
		out.integer = r1.integer * r2.integer;
	}

	return out;
}

struct Result resultDivide(struct Result r1, struct Result r2)
{
	struct Result out;

	if (r1.isFloat || r2.isFloat)
	{
		out.isFloat = true;
		if (r1.isFloat)
		{
			out.floating = r1.floating;
		} else if (r1.isInteger) 
		{
			out.floating = (float)r1.integer;
		}
		if (r2.isFloat)
		{
			out.floating /= r2.floating;	
		} else if (r2.isInteger)
		{
			out.floating /= (float)r2.integer;	
		}
	} else if (r1.isInteger && r2.isInteger)
	{
		if ((float)r1.integer / (float)r2.integer - r1.integer / r2.integer == 0)
		{
			out.isInteger = true;
			out.integer = r1.integer/r2.integer;
		}else {
			out.isInteger = false;
			out.isFloat = true;
			out.floating = (float)r1.integer / (float)r2.integer;
		}
	}

	return out;
}

//@brief print errors to stderr, uses printf formats
static void interpreterError(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "Error while interpreting: \n\t");
    vfprintf(stderr, fmt, args);
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

struct Result interpretBinOp(struct ASTNode* node)
{
	struct Result out;
	out.integer = 0;
	out.floating = 0;	

	if (node == NULL)
        return out;

    if (node->token.type == TokenType_IntegerLiteral)
    {
	    out.isInteger = true;
        out.isFloat = false;
        out.integer = mtInterpretInteger(node);
	    return out;
    } else if (node->token.type == TokenType_DecimalLiteral)
    {
    	out.isFloat = true;
        out.isInteger = false;
	    out.floating = mtInterpretDecimal(node);
	    return out;
    }

    bool hasEnoughChildren = node->childCount >= 2;
    if (!hasEnoughChildren)
    {
   	    out.isInteger = false;
       	out.isFloat = false;
	    return out;	
    }

    struct Result left = interpretBinOp(node->children[0]);
    struct Result right = interpretBinOp(node->children[1]);

    //no need to break in any of these
    switch (node->token.type)
    {
    case TokenType_OperatorAddition:
		out = resultAdd(left, right);
        break;
    case TokenType_OperatorSubtraction:
        out = resultSubtract(left, right);
        break;
    case TokenType_OperatorMultiplication:
        out = resultMultiply(left, right); 
        break;
    case TokenType_OperatorDivision:
        out = resultDivide(left, right);
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
    struct Result out = interpretBinOp(node);
    if (out.isFloat)
    {
        printf("%g", out.floating);
    } else if (out.isInteger)
    {
        printf("%d", out.integer);
    }
    printf("\n");
}


#endif // mtImplementation

#endif // mtInterpreter_h
