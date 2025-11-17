/*
* Mint is an interpreter. Short for 'my interpreter
*/

#include <tokenizer/mtTokenization.h>
#include <parser/mtParser.h>
#include <interpreter/mtInterpreter.h>

#define mtVersion "0.3"

const struct TokenTypeRules rules = {
    .additionChar           = '+',
    .divisionChar           = '/',
    .multiplicationChar     = '*',
    .subtractionChar        = '-',
    .assignChar             = '=',

    .leftParentheses        = '(',
    .rightParentheses       = ')',
    .leftBracket            = '{',
    .rightBracket           = '}',

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
    const char separators[] = {
        rules.additionChar,
        rules.divisionChar,
        rules.multiplicationChar,
        rules.subtractionChar,

        rules.leftParentheses,
        rules.rightParentheses,
        rules.leftBracket,
        rules.rightBracket,
        rules.commaChar,

        rules.endOfFileChar,
        rules.endStatementChar,
        rules.separatorChar
    };

    //TODO : make this token creation block into one function inside mtTokenization.h

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

    if (rootNode != NULL)
        mtInterpret(rootNode);

    if (rootNode != NULL) 
        mtASTFree(rootNode);
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
