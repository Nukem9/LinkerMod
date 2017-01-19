#include "../common/ff.h"
#include "zlib\zlib.h"
#include "../common/io.h"
#include "cmd_common.h"

#include "../cvar.h"

#include <vector>
#include <unordered_map>

enum class ParseEnts_StrType
{
	KEY,
	VALUE,

	INFO, // used for the header - we just drop these for now
};

struct KeyValuePair
{
	std::string key;
	std::string value;
};

struct EntityTable
{
	std::string header;
	std::vector<std::vector<KeyValuePair>> ents;
};

int ParseEnts(const char* ents_str, EntityTable* ents_table)
{
	std::vector<const char*> stack;

	const char* str_start = NULL;
	const char* ent_start = NULL;
	int ent_level = 0; // Its like ESP but for ents!

	ParseEnts_StrType strType = ParseEnts_StrType::INFO;

	std::vector<KeyValuePair> ent;
	KeyValuePair kv;

	for (const char* c = ents_str; *c; c++)
	{
		if (*c == '"')
		{
			if (str_start == NULL)
			{
				str_start = c + 1;
				continue;
			}

			if (strType != ParseEnts_StrType::INFO)
			{
				std::string str(str_start, c);
				if (strType == ParseEnts_StrType::KEY)
				{
					kv.key = str;
					strType = ParseEnts_StrType::VALUE;
				}
				else if (strType == ParseEnts_StrType::VALUE)
				{
					kv.value = str;
					ent.push_back(kv);
					kv.key.clear();
					kv.value.clear();
					strType = ParseEnts_StrType::KEY;
				}
			}
			str_start = NULL;
		}
		else if (str_start == NULL)
		{
			if (*c == '{')
			{
				if (ent_level++ == 0)
				{
					ent_start = c + 1;
					
					// Add the header to the ents table as the first entry
					if (strType == ParseEnts_StrType::INFO)
					{
						ents_table->header = std::string(ents_str, c);
					}

					strType = ParseEnts_StrType::KEY; // The next string we should see is a key
				}
			}
			else if (*c == '}')
			{
				if (--ent_level == 0)
				{
					ents_table->ents.push_back(ent);
					ent.clear();
				}
			}

			if (ent_level < 0)
			{
				return ent_level;
			}
		}
	}

	return ent_level;
}

//
// Attempts to get the value for the given key and place it in 
//
int Ent_GetValueForKey(std::vector<KeyValuePair>* ent, const char* _key, std::string** out)
{
	std::string key = _key;
	for (unsigned int i = 0; i < key.length(); i++)
	{
		key[i] = tolower(key[i]);
	}

	for (unsigned int i = 0; i < ent->size(); i++)
	{
		if ((*ent)[i].key == key)
		{
			*out = &(*ent)[i].value;
			return 1;
		}
	}

	return 0;
}

std::string FormatBrush(float* offset, int size)
{
	char buf[2048];

	int r = size / 2;
	const char* mtl = "trigger";

	std::string out = "";

	if (ents_useLabels.ValueBool())
		out += "// brush 0\n";

	out += "{\n";
	sprintf_s(buf, " (%.2f %.2f %.2f) (%.2f %.2f %.2f) (%.2f %.2f %.2f) %s 64 64 -304 48 0 0 lightmap_gray 16383.969 16384 -304 48 0 0\n", offset[0], offset[1] + r, offset[2] - r, offset[0] - r, offset[1] + r, offset[2] - r, offset[0] - r, offset[1], offset[2] - r, mtl);
	out += buf;
	sprintf_s(buf, " (%.2f %.2f %.2f) (%.2f %.2f %.2f) (%.2f %.2f %.2f) %s 64 64 -304 48 0 0 lightmap_gray 16383.953 16384 -304 48 0 0\n", offset[0] - r, offset[1], offset[2] + r, offset[0] - r, offset[1] + r, offset[2] + r, offset[0], offset[1] + r, offset[2] + r, mtl);
	out += buf;
	sprintf_s(buf, " (%.2f %.2f %.2f) (%.2f %.2f %.2f) (%.2f %.2f %.2f) %s 64 64 -304 0 0 0 lightmap_gray 16383.984 16384 -304 0 0 0\n", offset[0], offset[1] - r, offset[2] + r, offset[0] + r, offset[1] - r, offset[2] + r, offset[0] + r, offset[1] - r, offset[2] - r, mtl);
	out += buf;
	sprintf_s(buf, " (%.2f %.2f %.2f) (%.2f %.2f %.2f) (%.2f %.2f %.2f) %s 64 64 -48 0 0 0 lightmap_gray 16384 16384 -48 0 0 0\n", offset[0] + r, offset[1] - r, offset[2] + r, offset[0] + r, offset[1], offset[2] + r, offset[0] + r, offset[1], offset[2] - r, mtl);
	out += buf;
	sprintf_s(buf, " (%.2f %.2f %.2f) (%.2f %.2f %.2f) (%.2f %.2f %.2f) %s 64 64 -304 0 0 0 lightmap_gray 16384 16384 -304 0 0 0\n", offset[0], offset[1] + r, offset[2] + r, offset[0] - r, offset[1] + r, offset[2] + r, offset[0] - r, offset[1] + r, offset[2] - r, mtl);
	out += buf;
	sprintf_s(buf, " (%.2f %.2f %.2f) (%.2f %.2f %.2f) (%.2f %.2f %.2f) %s 64 64 -48 0 0 0 lightmap_gray 16384 16384 -48 0 0 0\n", offset[0]-r, offset[1] + r, offset[2] + r, offset[0] - r, offset[1], offset[2] + r, offset[0] - r, offset[1], offset[2] - r, mtl);
	out += buf;
	out += "}";
	
	return out;
}

int AddBrushes(EntityTable* ents_table)
{
	std::vector<std::vector<KeyValuePair>>& ents = ents_table->ents;
	for (unsigned int i = 0; i < ents.size(); i++)
	{
		std::vector<KeyValuePair>& ent = ents[i];

		std::string* classname;
		if (!Ent_GetValueForKey(&ent, "classname", &classname))
			continue;

		std::string* model;
		if (!Ent_GetValueForKey(&ent, "model", &model))
			continue;

		// Skip any thing that uses a non-builtin brushmodel
		if ((*model)[0] != '*')
			continue;
		
		std::string* origin;
		if (!Ent_GetValueForKey(&ent, "origin", &origin))
			continue;
		
		float offset[3];
		int r = sscanf_s(origin->c_str(), "%f %f %f", offset, offset + 1, offset + 2);
		if (r != 3)
			continue;
		
		KeyValuePair kv;
		kv.key = "";
		std::string value = FormatBrush(offset, 64);
		
		kv.value = value;
		
		ent.push_back(kv);
	}

	return 0;
}

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

		Con_Print_v("Extracting ents from \"%s\"...\n", filepath);

		FILE* h = nullptr;
		if (fopen_s(&h, filepath, "r+b") != 0)
		{
			Con_Error("ERROR: Fastfile '%s' could not be found\n", filepath);
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

		char* ents_str = FindEntsString((BYTE*)dBuf, dBuf + ffInfo.size + 36);
		if (ents_str == NULL)
		{
			Con_Error("Error: Could not find entity string\n");
			delete[] dBuf;
			return -1;
		}

		EntityTable ents_table;
		int l = ParseEnts(ents_str, &ents_table);
		delete[] dBuf; // Free the fastfile buffer as we don't need it anymore

		if (ents_genBrushes.ValueBool())
			AddBrushes(&ents_table);

		Con_Print_nv(	"iwmap 4\n" 
						"\"000_Global\" flags  active\n"
						"\"The Map\" flags\n" );

		Con_Print("%s", ents_table.header.c_str());

#if 1
		std::vector<std::vector<KeyValuePair>>& ents = ents_table.ents;
		for (unsigned int i = 0; i < ents.size(); i++)
		{
			std::vector<KeyValuePair>& ent = ents[i];

			if (ents_useLabels.ValueBool())
				Con_Print("// Entity %d\n", i);
			Con_Print("{\n");
			for (unsigned int k = 0; k < ents[i].size(); k++)
			{
				KeyValuePair& kv = ent[k];
				if (kv.key.length() > 0)
					Con_Print("\"%s\" \"%s\"\n", kv.key.c_str(), kv.value.c_str());
				else
					Con_Print("%s\n", kv.value.c_str());
			}
			Con_Print("}\n");
		}
#endif
	}

	if (argc < 2)
	{
		char* _argv[] = { NULL, "ents" };
		Cmd_Help_f(ARRAYSIZE(_argv), _argv);
		return -1;
	}

	return 0;
}