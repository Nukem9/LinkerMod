#include "gdt.h"
#include "fs.h"

GDT::GDT()
{
	this->h = NULL;
}

GDT::~GDT()
{
	for (unsigned int i = 0; i < this->assets.size(); i++)
	{
		delete assets[i];
	}
}

int GDT::WriteFile(const char* qpath, GDT_MODE mode)
{
	this->h = NULL;

	switch (mode)
	{
	case GDT_MODE_DEFAULT:
		if (FS_FileExists(qpath))
			return 1;
		
		fopen_s(&this->h, qpath, "w");
		break;

	case GDT_MODE_APPEND:
		if (FS_FileExists(qpath))
			return 1;

		//GDT_MODE_APPEND is not supported at this time
		_ASSERT(mode != GDT_MODE_APPEND);
		return -1;

	case GDT_MODE_OVERWRITE:
		fopen_s(&this->h, qpath, "w");
		break;
	}

	if (this->h == NULL)
	{
		printf("%s\n", "FS_ERROR");
	}

	fprintf(h, "{\n");
	for (unsigned int i = 0; i < this->assets.size(); i++)
	{
		assets[i]->Write(h);
	}
	fprintf(h, "}\n");

	fclose(h);
	return 0;
}