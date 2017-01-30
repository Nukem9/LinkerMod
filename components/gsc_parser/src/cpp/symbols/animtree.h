#pragma once
#include "symbol.h"

class Animtree : public Symbol
{
	public:
		Literal* string;
		
		Animtree(void);
		Animtree(Literal* animtree, YYLTYPE loc);
		
		~Animtree(void);
		
		void PrintInfo() const;
};
