

#include "mtNumberObject.h"
#include "mtUtilities.h"
#include "mtTypeError.h"

#include "internal/mtToken.h"

#include <math.h>
#include <stdio.h>

void numberSet(void* a, void* b)
{
    memcpy(a, b, sizeof(struct mtNumber));
}

void* numberAdd(void* a, void* b)
{
    struct mtNumber* numA = (struct mtNumber*)a;
    struct mtNumber* numB = (struct mtNumber*)b;

	struct mtNumber* out = malloc(sizeof(struct mtNumber));
    memset(out, 0, sizeof(struct mtNumber));

	if (numA->type == DECIMAL || numB->type == DECIMAL)
	{
		out->type = DECIMAL;
		double x = (numA->type == DECIMAL) ? numA->decimal : numA->integer; 
		double y = (numB->type == DECIMAL) ? numB->decimal : numB->integer;
		out->decimal = x+y;
	} else {
		out->type = INTEGER;
		out->integer = numA->integer + numB->integer;
	}
	return (void*)out;
}	

void* numberSub(void* a, void* b)
{
    struct mtNumber* numA = (struct mtNumber*)a;
    struct mtNumber* numB = (struct mtNumber*)b;

	struct mtNumber* out = malloc(sizeof(struct mtNumber));
    
	if (numA->type == DECIMAL || numB->type == DECIMAL)
	{
		out->type = DECIMAL;
		double x = (numA->type == DECIMAL) ? numA->decimal : numA->integer; 
		double y = (numB->type == DECIMAL) ? numB->decimal : numB->integer;
		out->decimal = x-y;
	} else {
		out->type = INTEGER;
		out->integer = numA->integer - numB->integer;
	}
	return (void*)out;
}


void* numberMul(void* a, void* b)
{
	struct mtNumber* numA = (struct mtNumber*)a;
    struct mtNumber* numB = (struct mtNumber*)b;

	struct mtNumber* out = malloc(sizeof(struct mtNumber));
    
	if (numA->type == DECIMAL || numB->type == DECIMAL)
	{
		out->type = DECIMAL;
		double x = (numA->type == DECIMAL) ? numA->decimal : numA->integer; 
		double y = (numB->type == DECIMAL) ? numB->decimal : numB->integer;
		out->decimal = x*y;
	} else {
		out->type = INTEGER;
		out->integer = numA->integer * numB->integer;
	}
	return out;
}

void* numberDiv(void* a, void* b)
{
    struct mtNumber* numA = (struct mtNumber*)a;
    struct mtNumber* numB = (struct mtNumber*)b;

	struct mtNumber* out = malloc(sizeof(struct mtNumber));
    
	double x = (numA->type == DECIMAL) ? numA->decimal : numA->integer;
    double y = (numB->type == DECIMAL) ? numB->decimal : numB->integer;

	double quotient;
	if (y != 0.0)
	{
		quotient = x/y;
	} else {
		typeError("Tried to divide by zero!");
		quotient = 0;
	}

	//ints are always truncated when converting, so 1.2 becomes 1.
	//thanks chatgpt for the fabs() check.
	if (fabs(quotient - (int)quotient) == 0 && 
		quotient <= INT_MAX && quotient >= INT_MIN)
	{
		out->type = INTEGER;
		out->integer = (int)quotient;
		return out;
	}

	out->type = DECIMAL;
	out->decimal = quotient;
	return out;
}

char* numberStr(void* a)
{   
    struct mtNumber* numA = a;

    if (numA->type == INTEGER)
    {
        char* str = malloc(sizeof(char)*12); 

        snprintf(str, 12, "%d", numA->integer);

        return str;
    }

    if (numA->type == DECIMAL)
    {
        char* str = malloc(sizeof(char)*64);

        snprintf(str, 64, "%f", numA->decimal);
      
        return str;
    }

    return NULL;
}

float mtInterpretDecimal(struct Token* token)
{
	if (token == NULL)
		return 0;
	float f;

	size_t tokenSize = token->size;

	//node->token.string isn't null terminated.
	char* str = malloc(tokenSize * sizeof(char) +1);
	memcpy(str, token->string, tokenSize);
	str[tokenSize] = '\0';

	int result = mtStringToFloat(&f, str);	
	free(str);

	if (result == mtSuccess)
	{
		return f;
	}

    char* tokenString = malloc(tokenSize * sizeof(char) + 1);
    mtGetTokenString(*token, tokenString, tokenSize);

	if (result == mtStringToFloatInconvertible)
	{
		typeError("Failed to read token: '%s' as decimal: Inconvertible", tokenString);
	}

	free(tokenString);
	return 0;
}
int mtInterpretInteger(struct Token* token)
{
    if (token == NULL)
        return 0;

    size_t tokenSize = token->size;

    int num = 0; 
    
    //would use a variable-sized array, but that's not supported everywhere.
    char* str = malloc(tokenSize * sizeof(char) + 1 );

    memcpy(str, token->string, tokenSize);
    str[tokenSize] = '\0';

    int err = mtStringToInt(&num, str, 10);

    free(str);

    if (err == mtSuccess)
        return num;
    
    char* tokenString = malloc(tokenSize * sizeof(char) + 1);
    mtGetTokenString(*token, tokenString, tokenSize);

    if (err == mtStringToIntInconvertible)
    {
        typeError("Failed to read token '%s' as number: inconvertible\n", tokenString);
    }
    if (err == mtStringToIntOverflow)
    {
        typeError("Failed to read token '%s' as number: integer overflow\n", tokenString);
    }
    if (err == mtStringToIntUnderflow)
    {
        typeError("Failed to read token '%s' as number: integer undeflow\n", tokenString);
    }
    free(tokenString);
    return 0;
}
