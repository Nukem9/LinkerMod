#pragma once
#include "symbol.h"

class Member : public Symbol
{
	public:
		Expression* object;
		Expression* expression;
		
		Member(void);
		Member(Expression* obj, Expression* expr, YYLTYPE loc, SYMBOL_TYPE type_override);
		
		virtual ~Member(void);
		
		void PrintInfo() const;
};