
#ifndef MT_NUMBER_H
#define MT_NUMBER_H

#include <types/mtType.h>
#include <tokenizer/mtToken.h>

struct mtNumber {
	enum {
        INTEGER,
        DECIMAL
    } type;
	union {
		int integer;
		double decimal;	
	};
};

void numberSet(void* a, void* b);

void* numberAdd(void* a, void* b);
void* numberSub(void* a, void* b);
void* numberDiv(void* a, void* b);
void* numberMul(void* a, void* b);

char* numberStr(void* a);

int mtInterpretInteger(struct Token* token);
float mtInterpretDecimal(struct Token* token);

static const struct Type mtNumberType = {
    .size = sizeof(struct mtNumber),

    .set = &numberSet,

    .add = &numberAdd,
    .sub = &numberSub,
    .mul = &numberMul,
    .div = &numberDiv,

    .str = &numberStr
};

#endif // MT_NUMBER_H
