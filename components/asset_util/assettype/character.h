#pragma once
#include "gdtasset.h"

class Character : public GDTAsset
{
private:
	static const char* gdf;

public:
	Character();
	~Character();

	virtual int Write(FILE* f);
};