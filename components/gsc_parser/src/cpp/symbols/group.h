#pragma once
#include "symbol.h"

//
// A Group consists of multiple child symbols
// it is generally used to define scopes, etc.
//
class Group : public Symbol
{
public:
	Group(Symbol* childList, YYLTYPE range);	
	~Group(void);
};
