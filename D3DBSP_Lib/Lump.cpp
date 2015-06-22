#include "Lump.h"

Lump& Lump::operator=(Lump& arg)
{
	this->size = arg.size;

	if(arg.size == 0)
	{
		delete[] this->content;
		this->isEmpty = true;
	}
	else
	{
		this->isEmpty = false;
		delete[] this->content;
		this->content = new BYTE[this->size];
		memcpy(this->content,arg.content,this->size);
	}

	return *this;
}

BYTE* Lump::AllocateMemory(size_t size)
{
	this->content = new BYTE[size];
	this->size = size;
	this->isEmpty = false;

	return content;
}

void Lump::FreeMemory(void)
{
	this->size = 0;
	delete[] this->content;
	this->content = nullptr;
	this->isEmpty = true;
}

Lump::Lump(void)
{
	this->content = nullptr;
	this->isEmpty = true;
	this->size = 0;
}

Lump::Lump(const Lump& arg)
{
	this->size = arg.size;
	
	if(this->size == 0)
	{
		this->content = nullptr;
		this->isEmpty = true;
	}
	else
	{
		this->isEmpty = false;
		this->content = new BYTE[this->size];
		memcpy(this->content,arg.content,this->size);
	}
}

Lump::~Lump(void)
{
	delete[] this->content;
}
