#include "animtree.h"

Animtree::Animtree(void) : string(NULL)
{
	this->type = S_TYPE_ANIMTREE;
	//printf("%s\n", SYMBOL_TYPE_STRING(type));
}

Animtree::Animtree(Literal* animtree, YYLTYPE loc): string(animtree)
{
	this->type = S_TYPE_ANIMTREE;
	this->location = loc;
	this->AddChild(animtree);
	//printf("%s animtree: '%s'\n", SYMBOL_TYPE_STRING(type), this->string->value);
}

Animtree::~Animtree()
{
	//delete[] string;
	//printf("~Animtree()\n");
}

void Animtree::PrintInfo() const
{
	printf("%s with %d children at %d(%d) - %d(%d), str '%s'\n",
		SYMBOL_TYPE_STRING(type),
		this->children ? this->children->Size() + 1 : 0,
		location.start.line,
		location.start.character,
		location.end.line,
		location.end.character,
		this->string->value);
}
