#pragma once
#include "symbol.h"

class Literal : public Symbol
{
	public:
		const char* value;
		
		Literal(void);
		Literal(char* val, YYLTYPE loc, SYMBOL_TYPE type_override);
		
		virtual ~Literal(void);
		
		void PrintInfo() const;
};