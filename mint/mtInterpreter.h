
#ifndef mtInterpreter_h
#define mtInterpreter_h

/*
*   This is the part of the code that actually interprets all the code and does stuff.
*/

#include "mtTokenizer.h"
#include "mtHashmap.h"
#include "mtAST.h"

#include <types/mtGlobalTypes.h>

void mtInterpret(struct ASTNode* node);
#endif
