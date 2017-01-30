#pragma once
#include "symbol.h"

class Include : public Symbol
{
	public:
		Literal* file;
		
		Include(void);
		Include(Literal* filepath, YYLTYPE loc);
		
		~Include(void);
		
		void PrintInfo() const;
};
