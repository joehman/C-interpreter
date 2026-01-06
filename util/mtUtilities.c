#include "mtUtilities.h"

int mtStringToInt(int* out, char* str, int base)
{
    //check for leading spaces
    if (str[0] == '\0' || isspace(str[0]) || str == NULL)
    {
        return mtStringToIntInconvertible;
    }

    char* end = NULL;
    long l = strtol(str, &end, base);

    if (l > INT_MAX)
    {
        return mtStringToIntOverflow;
    }
    if (l < INT_MIN)
    {
        return mtStringToIntUnderflow;
    }
    if (*end != '\0')
    {
        return mtStringToIntInconvertible;
    }

    *out = l;
    return mtSuccess;
}

int mtStringToFloat(float* out, char* str)
{
    //check for leading spaces
    if (str[0] == '\0' || isspace(str[0]) || str == NULL)
    {
	    return mtStringToFloatInconvertible;
    }
    char end = str[strlen(str)];

    float f = strtof(str, NULL);

    if (end != '\0')
    {
	return mtStringToFloatInconvertible;
    }

    *out = f;
    return mtSuccess;
}

int mtOpenFile(char* path, FILE** fileptr)
{
    size_t fileSize; 

    *fileptr = fopen(path, "r");
    
    if (*fileptr == NULL)
    {
        return mtFailOpenFile;
    }
    return mtSuccess;
}

int mtGetFileCharLength(char* path, size_t* length)
{
    FILE* fileptr; 

    int success;
    if ((success = mtOpenFile(path, &fileptr)) != mtSuccess)
    {
        return success;
    }

    fseek(fileptr, 0, SEEK_END);
    *length = ftell(fileptr);
    fclose(fileptr);

    return mtSuccess;
}



int mtLoadTextFromFile(char* path, char* string, size_t stringSize)
{
    FILE *fileptr; 

    int success;
    if ( (success = mtOpenFile(path, &fileptr)) != mtSuccess)
    {
        return success;
    }

    char character;
    for (size_t i = 0; (character = fgetc(fileptr)) != EOF && i < stringSize; i++) 
    {
        string[i] = character;
    }
    
    fclose(fileptr);
   
    return mtSuccess;
}

bool mtAnyOf(char* string, char* set, size_t setSize)
{
    int stringLength = strlen(string);

    for (size_t i = 0; i < stringLength; i++)
    {
        for (size_t j = 0; j < setSize; j++)
        {
            if (string[i] == set[j])
                return true;
        }
    }
    
    return true;
}

bool mtAnyOfN(char* string, size_t stringSize, char* set, size_t setSize)
{
    for (size_t i = 0; i < stringSize; i++)
    {
        for (size_t j = 0; j < setSize; j++)
        {
            if (string[i] == set[j])
                return true;
        }
    }
    return false;
}

bool mtOnlyOf(char* string, char* set, size_t setSize)
{
    int stringLength = strlen(string);

    for (size_t i = 0; i < stringLength; i++)
    {
        if (!mtAnyOfN(&string[i], 1, set ,setSize))
        {
            return false;
        }
    }
    
    return true;
}

bool mtOnlyOfN(char* string, size_t stringSize, char* set, size_t setSize)
{
    for (size_t i = 0; i < stringSize; i++)
    {
        if (!mtAnyOfN(&string[i], 1, set, setSize))
        {
            return false;
        }   
    }
    return true;
}

int mtWhichOf(char character, char* set, size_t setSize)
{   
    for (size_t i = 0; i < setSize; i++)
    {
        if (character == set[i])
            return i;
    }
    return mtFail;
}


bool mtAnyOfStr(char* string, char** set, size_t setSize)
{
    for (size_t i = 0; i < setSize; i++)
    {
        if (strcmp(string, set[i]) == 0)
            return true;
    }
    return false;
}

bool mtAnyOfStrN(char* string, size_t stringSize, char** set, size_t setSize)
{
    char str[stringSize+1];
    memcpy(&str, string, stringSize);
    str[stringSize] = '\0';

    for (size_t i = 0; i < setSize; i++)
    {
        if (strcmp(str, set[i]) == 0)
            return true;
    }

    return false;
}
