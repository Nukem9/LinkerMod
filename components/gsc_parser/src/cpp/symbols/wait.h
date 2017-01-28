#pragma once
#include "symbol.h"

class Wait : public Symbol
{
	public:
		Expression* expr;
		
		Wait(void);
		Wait(Expression* expr, YYLTYPE loc);
		
		virtual ~Wait(void);
		
		void PrintInfo() const;
};