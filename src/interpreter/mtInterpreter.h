#ifndef mtInterpreter_h
#define mtInterpreter_h

/*
*   This is the part of the code that actually interprets all the code and does stuff.
*/

#include <util/mtUtilities.h>

#include <parser/mtParser.h>
#include <tokenizer/mtTokenization.h>

#include <types/mtGlobalTypes.h>
#include <types/mtType.h>

#include <util/hashmap.h>

// ___________ Declarations ______________

void mtInterpret(struct ASTNode* node);
#endif
