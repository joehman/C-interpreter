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
* Order for variable declaration will look like this:
* Type, Identifier (optional)= 
*/

#define mtImplementation

#include "mtUtilities.h"
#include "mtTokenization.h"
#include "mtParser.h"

#define mtMaxFileSize 1024
#define mtMaxTokenCount 1024


const struct TokenTypeRules rules = {
    .additionChar           = '+',
    .divisionChar           = '/',
    .multiplicationChar     = '*',
    .subtractionChar        = '-',
    
    .endOfFileChar          = '\0',
    .endStatementChar       = '\n',
    .separatorChar          = ' ',

    .numbers = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'}
};

void mtInterpret(char* string)
{
    //populate the token array
    const char separators[] = {
        rules.additionChar,
        rules.divisionChar,
        rules.multiplicationChar,
        rules.subtractionChar,

        rules.endOfFileChar,
        rules.endStatementChar,
        rules.separatorChar
    };

    //get the number of tokens from fileString
    size_t tokenCount = 0; 
    mtGetTokenCountFromString(string, &tokenCount, (char*) &separators[0], mtArraySize(separators));

    //allocate the array with that size
    struct Token *tokens = malloc(tokenCount * sizeof(struct Token));
    mtCreateTokens(tokens, tokenCount);
    
    //populate the tokens array
    mtFindAllTokens(string, &tokens[0], tokenCount, (char*)&separators[0], mtArraySize(separators));    

    //remove all unneeded tokens, eg. empty space.
    const struct Token unneededTokens[] = {
        mtCreateStringToken(&rules.separatorChar)
    };
    mtFilterTokens(&tokens[0], tokenCount, &unneededTokens[0], mtArraySize(unneededTokens));

    //set all token types, operators, numbers, identifiers etc.
    mtSetTokenTypes(&tokens[0], tokenCount, rules);
    
    // run the parser, which creates an abstract syntax tree.
    struct ASTNode* rootNode = mtASTParseTokens(tokens, tokenCount);
    


    //at the very end!
    mtASTFree(rootNode);
    free(tokens);
}

#define mtVersion "0.1"

int main(int argc, char* argv[])
{

    printf("Mint version: " mtVersion "\n");

    //command-line like environment
    if (argc == 1)
    {
        char string[256];

        while (true)
        {
            printf(">");
            fgets(&string[0], sizeof(string), stdin);
            mtInterpret(string);
        }
        return 0;
    }

    // load the file
    size_t fileSize; 
    mtGetFileCharLength("test.txt", &fileSize);
    char *fileString = malloc(fileSize); //don't free (except at the end)
    if (mtLoadTextFromFile("test.txt", fileString, fileSize) != mtSuccess)
    {
        return mtFail;
    }
    
    mtInterpret(fileString);
    
    free(fileString);
}
