#define SYMBOL_TYPE_ENUM_STRING 1
#include "symbol_enum.h"

const char* SYMBOL_TYPE_STRING(SYMBOL_TYPE type)
{
	return SYMBOL_TYPE_STRINGS[type];
}
