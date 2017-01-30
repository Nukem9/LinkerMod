#include "identifier.h"

Identifier::Identifier(void) : value(NULL)
{
	this->type = S_TYPE_IDENTIFIER;
}

Identifier::Identifier(char* str) : value(str)
{
	this->type = S_TYPE_IDENTIFIER;
}

Identifier::Identifier(char* str, YYLTYPE loc) : value(str)
{
	this->type = S_TYPE_IDENTIFIER;
	this->location = loc;
}

Identifier::~Identifier()
{
	free((void*)this->value);
}

void Identifier::PrintInfo() const
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

void Identifier::PrintSymbol() const
{
	//
	// type|name|location[|details]
	// By default do not provide type specific info
	//
	printf("%s|%s|%d %d %d %d|%s at line %d, char %d\n",
		SYMBOL_TYPE_STRING(type),
		this->value,
		location.start.line,
		location.start.character,
		location.end.line,
		location.end.character,
		"var",
		location.start.line,
		location.start.character);
}