#include "xmodelalias.h"
#include "../common/fs.h"

#include <Windows.h>
#include <regex>
#include <iostream>

const char* XModelAlias::gdf = "xmodelalias.gdf";

XModelAlias::XModelAlias()
{
	for (int i = 0; i < XMODELALIAS_MODELCOUNT; i++)
	{
		this->model[i] = "";
	}
}

XModelAlias::~XModelAlias()
{
}

int XModelAlias::ExtractFromGSC(const char* qpath)
{
	size_t len = FS_FileSize(qpath);
	if (len == -1)
		return -1;

	std::ifstream t(qpath);
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	
	std::regex ex("a\\[\\d+\\].*?\\\"(\\w+)\\\";");
	std::smatch sm;

	int modelCount = 0;
	while (std::regex_search(str, sm, ex))
	{
		if (sm.size() == 2)
		{
			this->model[modelCount++] = sm[1];
		}
		str = sm.suffix().str();
	}

	const char* start = FS_GetFilenameSubString(qpath);
	const char* stop = FS_GetExtensionSubString(qpath);
	this->_name = std::string(start, stop);

	return 0;
}

int XModelAlias::Write(FILE* f)
{
	//
	// Apparently fprintf automatically converts \n to \r\n
	//
	fprintf(f, "\t\"%s\" ( \"%s\" )\n", this->_name.c_str(), this->gdf);
	fprintf(f, "\t{\n");

	for (int i = 0; i < XMODELALIAS_MODELCOUNT; i++)
	{
		if (this->model == NULL)
			break;

		fprintf(f, "\t\t\"model%d\" \"%s\"\n", i+1, this->model[i].c_str());
	}

	fprintf(f, "\t}\n");

	return 0;
}