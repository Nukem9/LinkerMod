#include "ff.h"
#include "arg.h"
#include "files.h"
#include "io.h"
#include <Windows.h>
#include "AppInfo.h"
#include "zlib/zlib.h"
#include "db_file_load.h"

int FF_FFExtract(const char* filepath, const char* filename)
{
	printf("Extracting rawfiles from \"%s\"...\n", filename);

	if (strstr(filename, "code"))
		return 1;

	HANDLE hFile = CreateFileA(filepath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	TestIt((char *)nullptr, 0, hFile);
	return 1;

	FILE *h;
	if (fopen_s(&h, filepath, "r+b") != 0)
	{
		printf("ERROR: Fastfile %s could not be found\n\n", filepath);
		return 0;
	}

	fseek(h, 0, SEEK_END);
	size_t fileSize = ftell(h);
	fseek(h, 0, SEEK_SET);

	// Validate file magic
	char magic[8];

	if (fread(magic, sizeof(magic), 1, h) != 1)
		return 0;

	if (memcmp(magic, "IWff0100", 8) && memcmp(magic, "IWffu100", 8))
		return 0;

	// Validate version
	int version = 0;

	if (fread(&version, sizeof(int), 1, h) != 1)
		return 0;

	if (version != 473)
		return 0;

	//fileSize -= 12;

	// Allocate a compressed buffer => decompress
	BYTE *compressedBuffer = new BYTE[fileSize];

	fseek(h, 0, SEEK_SET);
	if (fread(compressedBuffer, fileSize, 1, h) != 1)
		return 0;

//	TestIt((char *)compressedBuffer, fileSize);
	return 0;
}