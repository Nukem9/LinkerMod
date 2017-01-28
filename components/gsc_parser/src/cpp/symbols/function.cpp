#include "function.h"

Function::Function(void) : identifier(NULL)
{
	this->type = S_TYPE_FUNCTION_DECL;
}

Function::Function(Identifier* identifier, YYLTYPE loc)
{
	this->type = S_TYPE_FUNCTION_DECL;
	
	this->identifier = identifier;
	this->location = loc;
	
	this->AddChild(identifier);
}

Function::~Function()
{
	//delete this->identifier;
}

void Function::PrintArgs() const
{
	Symbol* args = this->children->NextElem();
	
	for(Symbol* arg = args->Children(); arg; arg = arg->NextElem())
	{
		arg->PrintSymbol();
	}
}

void Function::PrintInfo() const
{
	printf("%s with %d children at %d(%d) - %d(%d), name '%s'\n",
		SYMBOL_TYPE_STRING(type),
		this->children ? this->children->Size() + 1 : 0,
		location.start.line,
		location.start.character,
		location.end.line,
		location.end.character,
		this->identifier->value);
}

//
// Used to provide symbol data to CoD-Sense
//
void Function::PrintSymbol() const
{
	//
	// type|name|location[|details]
	// By default do not provide type specific info
	//
	printf("%s|%s|%d %d %d %d|%s\n",
		SYMBOL_TYPE_STRING(type),
		this->identifier->value,
		location.start.line,
		location.start.character,
		location.end.line,
		location.end.character,
		"...");
}


Call::Call(void) : flags(CALL_FLAG_NULL), identifier(NULL), caller(NULL)
{
	this->type = S_TYPE_FUNCTION_CALL;
}

Call::Call(YYLTYPE loc, int flags) : flags(flags), identifier(NULL), caller(NULL)
{
	this->type = S_TYPE_FUNCTION_CALL;
	this->location = loc;
}

Call::~Call()
{
	//delete this->identifier;
}

void Call::SetCaller(Expression* caller)
{
	if(this->flags & CALL_FLAGS_EXPLICIT_CALLER)
		return;

	this->caller = caller;
	if(caller)
	{
		// Swap the head of the children list with the caller, manually append the old list after that
		for(Symbol* child = this->children; child; )
		{
			Symbol* next = child->NextElem();
			caller->AddToEnd(child);
			child = next;
		}
		
		this->children =  caller;
	}
	this->flags |= CALL_FLAGS_EXPLICIT_CALLER;
}

void Call::PrintInfo() const
{
	printf("%s with %d children at %d(%d) - %d(%d), flags 0x%X\n",
		SYMBOL_TYPE_STRING(type),
		this->children ? this->children->Size() + 1 : 0,
		location.start.line,
		location.start.character,
		location.end.line,
		location.end.character,
		this->flags);
}