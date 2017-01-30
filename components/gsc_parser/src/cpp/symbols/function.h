#pragma once
#include "symbol.h"

class Function : public Symbol
{
	public:
		Identifier* identifier;
		
		Function(void);
		Function(Identifier* identifier, YYLTYPE loc);
		
		~Function(void);
		
		void PrintArgs() const;
		void PrintInfo() const;
		void PrintSymbol() const;
};

enum ENUM_CALL_FLAGS
{
	CALL_FLAG_NULL				= 0,
	CALL_FLAG_THREAD			= (1 << 0),
	CALL_FLAG_IDENTIFIER		= (1 << 1),
	CALL_FLAG_POINTER			= (1 << 2),
	CALL_FLAG_REFERENCE			= (1 << 3),
	CALL_FLAGS_EXPLICIT_CALLER	= (1 << 4)
};

class Call : public Symbol
{
	private:
		int flags;
		
	public:
		Identifier* identifier;
		Expression* caller;
		
		Call(void);
		Call(YYLTYPE loc, int flags);
		
		~Call(void);
		
		void SetCaller(Expression* caller);
		
		void PrintInfo() const;
};