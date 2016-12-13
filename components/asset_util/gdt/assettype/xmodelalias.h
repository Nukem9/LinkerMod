#pragma once
#include "gdtasset.h"

#define XMODELALIAS_MODELCOUNT 50

class XModelAlias : public GDTAsset
{
private:
	static const char* gdf;

	std::string model[XMODELALIAS_MODELCOUNT];

public:
	XModelAlias();
	~XModelAlias();

	int ExtractFromGSC(const char* qpath);
	virtual int Write(FILE* f);
};