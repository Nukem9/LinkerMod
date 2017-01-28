#include "pointer.h"

Pointer::Pointer(void) : expression(NULL)
{
	this->type = S_TYPE_POINTER;
}

Pointer::Pointer(Expression* expr, YYLTYPE loc) : expression(expr)
{
	this->type = S_TYPE_POINTER;
	
	this->location = loc;
	this->AddChild(expr);
}

Pointer::~Pointer()
{
}

void Pointer::PrintInfo() const
{
	printf("%s with %d children at %d(%d) - %d(%d)\n",
		SYMBOL_TYPE_STRING(type),
		this->children ? this->children->Size() + 1 : 0,
		location.start.line,
		location.start.character,
		location.end.line,
		location.end.character);
}