#include "member.h"

Member::Member(void) : object(NULL), expression(NULL)
{
	this->type = S_TYPE_MEMBER_NULL;
}

Member::Member(Expression* obj, Expression* expr, YYLTYPE loc, SYMBOL_TYPE type_override) : object(obj), expression(expr)
{
	this->type = type_override;
	this->location = loc;
	
	if(!obj || !expr)
	{
		delete obj;
		delete expr;
		return;
	}
	
	this->AddChild(obj);	
	this->AddChild(expr);
}

Member::~Member()
{
}

void Member::PrintInfo() const
{
	printf("%s with %d children at %d(%d) - %d(%d)\n",
		SYMBOL_TYPE_STRING(type),
		this->children ? this->children->Size() + 1 : 0,
		location.start.line,
		location.start.character,
		location.end.line,
		location.end.character);
}