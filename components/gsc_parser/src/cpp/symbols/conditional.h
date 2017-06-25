#pragma once

#include "symbol.h"
//#include "operator_enum.cpp"

class Conditional : public Symbol
{
public:
	Expression* expression;
	Symbol* statement;

	Conditional(void);
	Conditional(Expression* expr, Symbol* stmt, YYLTYPE loc, SYMBOL_TYPE type);
	Conditional(Expression* expr0, Expression* expr1, Expression* expr2, Symbol* stmt, YYLTYPE loc, SYMBOL_TYPE type);
	
	~Conditional(void);
	
	void PrintInfo(void) const;
};
