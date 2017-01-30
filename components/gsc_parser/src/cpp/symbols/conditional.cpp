#include "expression.h"
#include "symbol.h"

Conditional::Conditional(void) : statement(NULL)
{
	this->type = S_TYPE_NONE;
}

Conditional::Conditional(Expression* expr, Symbol* stmt, YYLTYPE loc, SYMBOL_TYPE type) : expression(expr), statement(stmt)
{
	this->type = type;
	this->location = loc;
	
	if(expr) { this->AddChild(expr); }
	if(stmt) { this->AddChild(stmt); }
}

Conditional::Conditional(Expression* expr0, Expression* expr1, Expression* expr2, Symbol* stmt, YYLTYPE loc, SYMBOL_TYPE type) : expression(expr0), statement(stmt)
{
	this->type = type;
	this->location = loc;
	
	if(expr0) { this->AddChild(expr0); }
	if(expr1) { this->AddChild(expr1); }
	if(expr2) { this->AddChild(expr2); }
	if(stmt) { this->AddChild(stmt); }
}

Conditional::~Conditional()
{
}

void Conditional::PrintInfo() const
{
	printf("%s with %d children at %d(%d) - %d(%d)\n",
		SYMBOL_TYPE_STRING(type),
		this->children ? this->children->Size() + 1 : 0,
		location.start.line,
		location.start.character,
		location.end.line,
		location.end.character);
}