#define OPERATOR_TYPE_ENUM_STRING 1
#include "operator_enum.h"

const char* OPERATOR_TYPE_STRING(OPERATOR_TYPE type)
{
	return OPERATOR_TYPE_STRINGS[type];
}
