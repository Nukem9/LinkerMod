#pragma once
#include "fs.h"
#include <vector>

#include "assettype\gdtasset.h"

enum GDT_MODE
{
	GDT_MODE_DEFAULT,		// fail if the file exists
	GDT_MODE_APPEND,		// append to the file if it exists, create a new one if it doesnt
	GDT_MODE_OVERWRITE,		// overwrite the file if it exists
};

class GDT
{
private:
	FILE* h;

public:
	std::vector<GDTAsset*> assets;

	GDT();
	~GDT();

	int WriteFile(const char* qpath, GDT_MODE mode = GDT_MODE_DEFAULT);
};