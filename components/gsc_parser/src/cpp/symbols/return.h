#pragma once
#include "symbol.h"

class Return : public Symbol
{
	public:
		Expression* expr;
		
		Return(void);
		Return(Expression* expr, YYLTYPE loc);
		
		virtual ~Return(void);
		
		void PrintInfo() const;
};