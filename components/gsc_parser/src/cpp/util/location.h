#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <string.h>
#include "../parser/gsc.tab.hpp"

class Position
{
public:
	int line;
	int character;
	
	Position(void);
	Position(int line, int character);
	
	~Position(void);
	
	void Print(void);
	
	const Position& operator=(const Position&);
	bool operator==(const Position&);
	bool operator<(const Position&);
	bool operator>(const Position&);
	bool operator<=(const Position&);
	bool operator>=(const Position&);
};

class Range
{
public:
	Position start;
	Position end;

	Range(void);
	Range(const YYLTYPE& loc);

	Range(Position start, Position end);
	Range(Position start, int endLine, int endChar);
	Range(int startLine, int startChar, Position end);
	Range(int startLine, int startChar, int endLine, int endChar);

	~Range(void);

	void Print(void);
	
	//const Range& operator=(const YYLTYPE& loc);
	/*operator==(const Position&);
	operator<(const Position&);
	operator>(const Position&);
	operator<=(const Position&);
	operator>=(const Position&);*/
	
	/*
	Contains(point/ range)
	Intersects?
	*/
};
