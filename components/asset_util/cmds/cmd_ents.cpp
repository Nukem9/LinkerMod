#include "../common/ff.h"
#include "zlib\zlib.h"
#include "../common/io.h"
#include "cmd_common.h"

char* FindEntsString(BYTE* start, BYTE* end)
{
	const char* pattern = "\"classname\"\x20\"worldspawn\"";
	unsigned int len = strlen(pattern);

	BYTE* c = start;
	while (c < end - (len + 1))
	{
		if (strncmp(pattern, (char*)c, len) == NULL)
		{
			char* p = (char*)c;
			
			//
			// Automatically retry until the real start of the ents string is found
			//
			for (int retryCount = 16; retryCount; retryCount--)
			{
				for (; *p != '{' && p >= (char*)start; p--){}

				// Prevent the automatic retry system from going past the beginning of the ff data
				if (p < (char*)start)
				{
					return nullptr;
				}

				//
				// Immediately before the real ents string, there is an <usually> unsigned integer that contains the length
				// of the string plus the nullchar, ensure that it matches the data that was found
				//
				unsigned int strSize = strlen(p) + 1;
				unsigned int entsSize = *(unsigned int*)(p - 4);

				if (strSize == entsSize)
				{
					return p;
				}

				//
				// so even if the size uint isn't present, if the char before the current one is NULL
				// its probably safe to assume that we found the ents string
				//
				if (p[-1] == NULL)
				{
					return p;
				}

				Con_Print("Trying different offset... (%d attempts remaining)\n", retryCount - 1);
				p--;
			}
		}

		c++;
	}

	return nullptr;
}

//
// Argument format: argv[1...n] are zone filenames
//
int Cmd_Ents_f(int argc, char** argv)
{
	for (int i = 1; i < argc; i++)
	{
		char* filepath = argv[i];

		Con_Print("Extracting ents from \"%s\"...\n", filepath);

		FILE* h = nullptr;
		if (fopen_s(&h, filepath, "r+b") != 0)
		{
			Con_Error("ERROR: Fastfile '%s' could not be found\n\n", filepath);
			return FALSE;
		}
		rewind(h);

		fseek(h, 0, SEEK_END);
		size_t fileSize = ftell(h);

		// Get Compressed FileSize and Allocate a Storage Buffer for Compressed Data
		size_t cSize = fileSize - 12;
		BYTE* cBuf = new BYTE[cSize | 0x8000];

		fseek(h, 12, SEEK_SET);
		fread(cBuf, 1, cSize, h);

		XFile ffInfo;
		unsigned long dSize = sizeof(XFile);
		uncompress((BYTE*)&ffInfo, &dSize, cBuf, 0x8000);

		dSize = ffInfo.size + 36;
		if (dSize >= 1073741824)
		{
			//Any fastfiles that claim they decompress to a file >= 1GB
			//are either corrupt or do not belong to the vanilla game
			Con_Error("ERROR: Skipping %s\n", filepath);
			return 1;
		}

		BYTE* dBuf = new BYTE[dSize];
		uncompress(dBuf, &dSize, cBuf, cSize);
		delete[] cBuf;

		char* result = FindEntsString((BYTE*)dBuf, dBuf + ffInfo.size + 36);
		if (result == NULL)
		{
			Con_Error("Error: Could not find entity string\n");
		}
		else
		{
			Con_Print("%s\n", result);
		}

		delete[] dBuf;
	}

	if (argc < 2)
	{
		char* _argv[] = { NULL, "ents" };
		Cmd_Help_f(ARRAYSIZE(_argv), _argv);
		return -1;
	}

	return 0;
}