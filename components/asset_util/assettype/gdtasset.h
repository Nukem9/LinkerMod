#pragma once
#include <fstream>

class GDTAsset
{
protected:
	static const char* gdf;
	std::string _name;

public:
	GDTAsset();
	~GDTAsset();

	virtual int Write(FILE* f) = 0;
};
