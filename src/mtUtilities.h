#ifndef mtUtilities_h
#define mtUtilities_h

// ____ INCLUDES ____

#include <stdio.h>
#include <malloc.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// ___________ MACROS ____________

#define mtSuccess   0

#define mtFail          -1
#define mtFailOpenFile  -2


#ifdef linux
    #define mtLinux
#endif

#ifdef _WIN32
    #define mtWin32
#endif

#define mtArraySize(arr) (sizeof(arr)) / sizeof((arr)[0])

#define mtMaxHashKeyLength 8
// _____________ DECLARATION __________________

//@brief Opens the file on path with fileptr.
//
//@param path the file's path
//@param fileptr the pointer to open it with.
//
//@returns mtSuccess if it successfully opened the file, mtFailOpenFile if fopen() failed.
int mtOpenFile(char* path, FILE** fileptr);

//@brief Gets the file on path and writes its size to length.
//
//@param path the file to measure
//@param length the variable to write to.
//
//@returns mtSuccess if it succeeded, mtFailOpenFile if it failed to open the file.
int mtGetFileCharLength(char* path, size_t* length);

//@brief Loads all text from the file in path into string up to stringSize.
//
//@param path the path to the file
//
//@param string the string to write text to
//@param stringSize the size of string
//
//@returns mtSuccess if it succeeded, mtFail if it didn't.
int mtLoadTextFromFile(char* path, char* string, size_t stringSize);

//@brief Checks if any of the individual chars in set are in string.
//
//@param string         the string to compare
//
//@param set            the chars to check for
//@param setSize        the length of the set
//
//@returns true if the any of them were present, false if not.
bool mtAnyOf(char* string, char* set, size_t setSize);

//@brief Checks if any of the individual chars in set are in string.
//
//@param string         the string to check
//@param stringSize     the length of the string
//
//@param set            the chars to check for
//@param setSize        the length of the set
//
//@returns true if the any of them were present, false if not.
bool mtAnyOfN(char* string, size_t stringSize, char* set, size_t setSize);

//@brief Checks if any of the individual chars in string are not in set
//
//@param string         the string to check
//@param stringSize     the length of the string
//
//@param set            the chars to check for
//@param setSize        the length of the set
//
//@returns true if all of the chars in string were also present in set, false if not.
bool mtOnlyOfN(char* string, size_t stringSize, char* set, size_t setSize);

//@brief Checks if any of the individual chars in string are not in set
//
//@param string         the string to check
//
//@param set            the chars to check for
//@param setSize        the length of the set
//
//@returns true if all of the chars in string were also present in set, false if not.
bool mtOnlyOf(char* string, char* set, size_t setSize);

//@brief Finds the first char in set that is equal to character. 
//
//@param character the character to check
//@param set the characters to check for
//
//@returns The index of the first char in set which is equal to character.
int mtWhichOf(char character, char* set, size_t setSize);



// _________________ IMPLEMENTATION _______________


#ifdef mtImplementation

int mtOpenFile(char* path, FILE** fileptr)
{
    size_t fileSize; 

    *fileptr = fopen(path, "r");
    
    if (fileptr == NULL)
    {
        printf("Failed to fopen file %s\n", path);
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


#endif // mtImplementation


#endif // mtUtilities_h
