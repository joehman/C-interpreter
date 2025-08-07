/*
* Mint is an interpreter. Short for 'my interpreter'.
* Functions will be prefixed with mt, short for Mint
* -------- Syntax -----------
*
* newlines will end statements, like python.
*
* functions are called like this: func(args, moreargs)
* strings start and end with ". 
* 
* types will be: 
* int16, int32, int64
* 
* 
*/

#define mtImplementation

#include "mtUtilities.h"
#include "mtTokenization.h"


#define mtMaxFileSize 1024
#define mtMaxTokenCount 1024

//remove all unneeded tokens
void filterTokens(struct Token* tokens, size_t tokenCount)
{
    const struct Token unneededTokens[] = {
        { .string = " ", .size = 1 },
    };
    
    struct Token newTokenList[tokenCount];
    memset(&newTokenList[0], 0, sizeof(newTokenList));
    

    bool skip = false;
    for (size_t i = 0; i < tokenCount; i++) 
    {
        // look through 
        for (size_t j = 0; j < mtArraySize(unneededTokens); j++ )
        {
            if (mtTokenCmp(unneededTokens[j], tokens[i]) == 0)
            {
                skip = true;
            }
        }

        if (!skip)
            newTokenList[i] = tokens[i];
    }
    
    memcpy(tokens, newTokenList, tokenCount * sizeof(struct Token));
}

int main()
{


    char string[mtMaxFileSize] = "";
    mtLoadTextFromFile("test.txt", &string[0], mtMaxFileSize);
   
    
    const size_t tokenCount = 100;
    struct Token tokens[tokenCount];
    
    mtFindAllTokens(&tokens[0], 100, &string[0], mtMaxFileSize);    

    filterTokens(&tokens[0], tokenCount);

    for (int i = 0; i < tokenCount; i++) {
        mtPrintToken(tokens[i]);
    }

}
