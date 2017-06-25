#include "group.h"

Group::Group(Symbol* childList, YYLTYPE range)
{
	this->type = S_TYPE_GROUP;
	this->children = childList;
	this->location = range;
}

Group::~Group(void)
{
}
