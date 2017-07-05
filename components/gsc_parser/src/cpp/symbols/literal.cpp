#include "literal.h"

Literal::Literal(void) : value(NULL)
{
	this->type = S_TYPE_LITERAL_UNDEFINED;
}

Literal::Literal(char* val, YYLTYPE loc, SYMBOL_TYPE type_override)
{
	this->type = type_override;
	this->value = val;
	this->location = loc;
}

Literal::~Literal()
{
	free((void*)value);
	value = NULL;
}

void Literal::PrintInfo() const
{
	printf("%s with %d children at %d(%d) - %d(%d), name '%s'\n",
		SYMBOL_TYPE_STRING(type),
		this->children ? this->children->Size() + 1 : 0,
		location.start.line,
		location.start.character,
		location.end.line,
		location.end.character,
		this->value);
}