#ifndef mtUtilities_h
#define mtUtilities_h

// ____ INCLUDES ____

#include <stdio.h>
#include <malloc.h>
#include <stdint.h>
#include <stdbool.h>

// ___________ MACROS ____________

#ifdef linux
    #define mtLinux
#endif

#ifdef _WIN32
    #define mtWin32
#endif

#define mtArraySize(arr) (sizeof(arr)) / sizeof((arr)[0])


// _____________ DECLARATION __________________

FILE* mtOpenFile(char* path);

size_t mtGetFileCharLength(char* path);

void mtLoadTextFromFile(char* path, char* string, size_t stringSize);



// _________________ IMPLEMENTATION _______________


#ifdef mtImplementation

FILE* mtOpenFile(char* path)
{
    FILE *fileptr = NULL;
    size_t fileSize; 

    fileptr = fopen(path, "r");
    
    if (fileptr == NULL)
    {
        printf("Failed to fopen file %s \n", path);
        return NULL;
    }
    return fileptr;
}

size_t mtGetFileCharLength(char* path)
{
    FILE* fileptr = mtOpenFile(path);
    size_t fileSize = 0;


    fseek(fileptr, 0, SEEK_END);
    fileSize = ftell(fileptr);
    rewind(fileptr);
    
    fclose(fileptr);

    return fileSize;
}

void mtLoadTextFromFile(char* path, char* string, size_t stringSize)
{
    FILE *fileptr = mtOpenFile(path);
    
    char character;
    for (int i = 0; (character = fgetc(fileptr)) != EOF && i < stringSize; i++) 
    {
        string[i] = character;
    }
    
    fclose(fileptr);
   
    return;
}

// iterates through an array of tokens, until it finds an end of string token, eg. a token with \0, then returns that number.



#endif // mtImplementation


#endif // mtUtilities_h
