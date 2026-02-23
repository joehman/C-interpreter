/*
* Mint is an interpreter. Short for 'my interpreter
*/

#include <Mint.h>

#include "mtToken.h"
#include "mtTokenizer.h"
#include "mtParser.h"
#include "mtInterpreter.h"
#include "mtUtilities.h"

#define mtVersion "0.4"

bool gPrintASTTree = false;
bool gPrintTokens = false;

const struct TokenTypeRules rules = {
    .additionChar           = '+',
    .divisionChar           = '/',
    .multiplicationChar     = '*',
    .subtractionChar        = '-',
    .assignChar             = '=',
    .greaterThanChar        = '>',
    .lesserThanChar         = '<',

    .leftParentheses        = '(',
    .rightParentheses       = ')',

    .exclamationChar        = '!',

    .commaChar              = ',',

    .endOfFileChar          = '\0',
    .endStatementChar       = '\n',
    .separatorChar          = ' ',

    .numbers = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'},
    .functionKeyword = "func",
    .endKeyword = "end",
    .ifKeyword = "if",
    .importKeyword = "import",
    .whileKeyword = "while"
};

void mtExecute(char* string)
{
    size_t tokenCount = 0; 
    struct Token* tokens = mtTokenize(string, rules, &tokenCount);

    struct ASTNode* rootNode = mtASTParseTokens(tokens, tokenCount);
    
    if (gPrintTokens)
       mtPrintTokens(tokens, tokenCount); 
    if (gPrintASTTree)
        mtPrintASTTree(rootNode);
    
    if (rootNode != NULL)
    {
        mtInterpret(rootNode);
        mtASTFree(rootNode);
    }

    free(tokens);
}

int main(int argc, char* argv[])
{
    printf("Mint version " mtVersion "\n");

    if (argc < 2)
    {
        printf("Usage:\n\t Mint [--args..] [file]\n");
        return -1;
    }

    for (int i = 0; i < argc; i++) 
    {
        if (strcmp(argv[i], "--printAST") == 0) 
            gPrintASTTree = true;
        if (strcmp(argv[i], "--printTokens") == 0)
            gPrintTokens = true;
    }

    int result;
    char* path = argv[argc-1];

    // load the file
    size_t fileSize; 
    result = mtGetFileCharLength(path, &fileSize);

    if (result == mtFailOpenFile)
    {
        printf("Failed to open file %s\n", path);
        return mtFailOpenFile; 
    }

    char *fileString = malloc(fileSize); 
    if (mtLoadTextFromFile(path, fileString, fileSize) != mtSuccess)
    {
        return mtFail;
    }
    
    mtExecute(fileString);

    free(fileString);
}
