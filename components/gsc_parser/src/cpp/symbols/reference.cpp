#include "reference.h"

Reference::Reference(void) : file(NULL), identifier(NULL)
{
	this->type = S_TYPE_REFERENCE;
}

Reference::Reference(Literal* filepath, Identifier* identifier, YYLTYPE loc)
{
	this->type = S_TYPE_REFERENCE;
	
	this->file = filepath;
	this->identifier = identifier;
	this->location = loc;
	
	if(filepath) { this->AddChild(filepath); }
	this->AddChild(identifier);
}

Reference::~Reference()
{
}

void Reference::PrintInfo() const
{
	printf("%s with %d children at %d(%d) - %d(%d), file '%s', func '%s'\n",
		SYMBOL_TYPE_STRING(type),
		this->children ? this->children->Size() + 1 : 0,
		location.start.line,
		location.start.character,
		location.end.line,
		location.end.character,
		(this->file) ? this->file->value : "$this",
		this->identifier->value);
}