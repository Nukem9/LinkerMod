#pragma once
#include "symbol.h"

class Pointer : public Symbol
{
	public:
		Expression* expression;
		
		Pointer(void);
		Pointer(Expression* expression, YYLTYPE loc);
		
		~Pointer(void);
		
		void PrintInfo() const;
};