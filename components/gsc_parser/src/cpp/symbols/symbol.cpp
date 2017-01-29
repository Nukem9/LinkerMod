#include "symbol.h"

Symbol::Symbol(void) : type(S_TYPE_NONE), prev(NULL), next(NULL), children(NULL)
{
	this->SetOwner(this);
	//printf("SYMBOL CTOR %s\n", SYMBOL_TYPE_STRING(type));
}

Symbol::Symbol(YYLTYPE loc) : type(S_TYPE_NONE), prev(NULL), next(NULL), children(NULL), location(loc)
{
	this->SetOwner(this);
	//printf("CTOR %s\n", SYMBOL_TYPE_STRING(type));
}

Symbol::~Symbol()
{
	/*printf("~%s with %d children at %d(%d) - %d(%d)\n",
		SYMBOL_TYPE_STRING(type),
		this->children ? this->children->Size() + 1 : 0,
		location.start.line,
		location.start.character,
		location.end.line,
		location.end.character);*/
	
	delete this->children;
	this->children = NULL;
	delete this->NextElem();
	this->next = NULL;
}

SYMBOL_TYPE Symbol::Type(void) const
{
	return this->type;
}

Range Symbol::Location(void) const
{
	return this->location;
}

void Symbol::AddChild(Symbol* child)
{
	if(!this->children)
	{
		children = child;
		child->parent = this;
	}
	else
	{
		children->AddToEnd(child);
		child->parent = this;
	}
}

void Symbol::FreeChildren(void)
{
	for(Symbol* s = this->children; s; s = s->NextElem())
	{
		s->FreeChildren();
	}
	
	delete this->children;
	this->children = NULL;
}

Symbol* Symbol::Children(void) const
{
	return this->children;
}

void Symbol::PrintInfo() const
{
	printf("%s with %d children at %d(%d) - %d(%d)\n",
		SYMBOL_TYPE_STRING(type),
		this->children ? this->children->Size() + 1 : 0,
		location.start.line,
		location.start.character,
		location.end.line,
		location.end.character);
}

//
// Used to provide symbol data to CoD-Sense
//
void Symbol::PrintSymbol() const
{
	//
	// type|location[|name|details]
	// By default do not provide type specific info
	//
	//printf("%s|%d %d %d %d\n",
	//	SYMBOL_TYPE_STRING(type),
	//	location.start.line,
	//	location.start.character,
	//	location.end.line,
	//	location.end.character);
}

void Symbol::PrintInfoRecursive(int indentLevel) const
{
	this->PrintInfo();
	
	for(Symbol* c = this->children; c; c = c->NextElem())
	{
#ifndef WIN32
		for(int i = 0; i < indentLevel; i++)
		{
			printf("│   ");
		}
		printf("%s", c->NextElem() ? "├── " : "└── ");
#else
		for (int i = 0; i < indentLevel; i++)
		{
			printf("|  ");
		}
		printf("%s", c->NextElem() ? "|-- " : "|__ ");
#endif
		c->PrintInfoRecursive(indentLevel + 1);
	}
}

void Symbol::_debug_override_type(SYMBOL_TYPE type)
{
	this->type = type;
}
