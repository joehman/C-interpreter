
#ifndef mtInterpreter_h
#define mtInterpreter_h

/*
*   This is the part of the code that actually interprets all the code and does stuff.
*/


#include <parser/mtParser.h>
#include <parser/mtASTTree.h>
#include <tokenizer/mtToken.h>

#include <types/mtGlobalTypes.h>
#include <types/mtType.h>

#include <util/hashmap.h>

void mtInterpret(struct ASTNode* node);
#endif
