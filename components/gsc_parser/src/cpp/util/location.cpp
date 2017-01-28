#include "location.h"

//
// Position
//

Position::Position(void) : line(0), character(0) { };
Position::Position(int line, int character) : line(line), character(character) { };

Position::~Position(void) { };

void Position::Print(void)
{
	printf("%d(%d)", this->line, this->character);
}

const Position& Position::operator=(const Position& arg)
{
	this->line = arg.line;
	this->character = arg.character;
	
	return *this;
}

bool Position::operator==(const Position& arg)
{
	return (this->line == arg.line && this->character == arg.character);
}

bool Position::operator<(const Position& arg)
{
	if(this->line < arg.line)
		return true;
	if(this->line == arg.line)
		return this->character < arg.character;
	return false;
}

bool Position::operator>(const Position& arg)
{
	if(this->line > arg.line)
		return true;
	if(this->line == arg.line)
		return this->character > arg.character;
	return false;
}

bool Position::operator<=(const Position& arg)
{
	if(this->line < arg.line)
		return true;
	if(this->line == arg.line)
		return this->character <= arg.character;
	return false;
}

bool Position::operator>=(const Position& arg)
{
	if(this->line > arg.line)
		return true;
	if(this->line == arg.line)
		return this->character >= arg.character;
	return false;
}

//
// Range
//

Range::Range(void) { };

Range::Range(const YYLTYPE& loc) :
	start(loc.first_line, loc.first_column),
	end(loc.last_line, loc.last_column)
	{ };
	
Range::Range(Position start, Position end) :
	start(start),
	end(end)
	{ };
	
Range::Range(Position start, int endLine, int endChar) :
	start(start),
	end(endLine, endChar)
	{ };
	
Range::Range(int startLine, int startChar, Position end) :
	start(startLine, startChar),
	end(end)
	{ };
	
Range::Range(int startLine, int startChar, int endLine, int endChar) :
	start(startLine, startChar),
	end(endLine, endChar)
	{ };
	
Range::~Range(void) { };

void Range::Print(void)
{
	printf("%d(%d) - %d(%d)\n", this->start.line, this->start.character, this->end.line, this->end.character);
}
