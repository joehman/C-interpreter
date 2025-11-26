/*
* Mint is an interpreter. Short for 'my interpreter
*/

#include <Mint.h>

#define mtVersion "0.3"

const struct TokenTypeRules rules = {
    .additionChar           = '+',
    .divisionChar           = '/',
    .multiplicationChar     = '*',
    .subtractionChar        = '-',
    .assignChar             = '=',

    .leftParentheses        = '(',
    .rightParentheses       = ')',

    .commaChar              = ',',

    .endOfFileChar          = '\0',
    .endStatementChar       = '\n',
    .separatorChar          = ' ',

    .numbers = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'},
    .functionKeyword = "func",
    .endKeyword = "end"
};

void mtExecute(char* string)
{
    size_t tokenCount = 0; 
    struct Token* tokens = mtTokenize(string, rules, &tokenCount);

    // run the parser, which creates an abstract syntax tree.
    struct ASTNode* rootNode = mtASTParseTokens(tokens, tokenCount);

    if (rootNode != NULL)
    {
        mtInterpret(rootNode);
        mtASTFree(rootNode);
    }
    free(tokens);
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Usage:\n\t Mint [file]\n");
        return -1;
    }

    int result;
    char* path = argv[1];

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
