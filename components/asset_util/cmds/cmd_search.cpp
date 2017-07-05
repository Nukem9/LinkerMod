#include "../sys/AppInfo.h"
#include "cmd_common.h"

#include "../common/fs.h"
#include "../common/ff.h"

#include "zlib\zlib.h"
#include "shared_assert.h"

#include "search/handler.h"

enum class FileType
{
	IWD,
	FF,

	ERR,
};

struct FileEntry
{
	FileType type;

	std::string name;
	std::string path;

	void Clear(void)
	{
		type = FileType::ERR;
		path = "";
		name = "";
	}
};

class CachedFile
{
private:
	FileEntry entry;
	
	BYTE* data;
	size_t size;

private:
	void Clear(void)
	{
		this->entry.Clear();
		delete[] this->data;
		data = NULL;
		size = NULL;
	}

public:
	std::string test;

	CachedFile(void) : data(NULL), size(0)
	{
	}

	CachedFile(CachedFile&& arg)
	{
		this->entry = arg.entry;
		this->data = arg.data;
		this->size = arg.size;
		arg.data = NULL;
		arg.size = 0;
	}

	~CachedFile(void)
	{
		this->Clear();
	}

	bool isValid()
	{
		return data != NULL;
	}

	void Load(FileEntry& _entry)
	{
		Con_Print_v("Load: %s\n", _entry.name.c_str());

		this->entry = _entry;

		FILE* h = nullptr;
		if (fopen_s(&h, entry.path.c_str(), "r+b") != 0)
		{
			Con_Error("ERROR: Fastfile '%s' could not be found\n", entry.path.c_str());
			entry.Clear();
			return;
		}
		rewind(h);

		fseek(h, 0, SEEK_END);
		size_t fileSize = ftell(h);

		if (entry.type == FileType::FF)
		{
			// Get Compressed FileSize and Allocate a Storage Buffer for Compressed Data
			size_t cSize = fileSize - 12;
			BYTE* cBuf = new BYTE[cSize | 0x8000];

			fseek(h, 12, SEEK_SET);
			fread(cBuf, 1, cSize, h);

			this->data = cBuf;
			this->size = cSize;
		}
		else if (entry.type == FileType::IWD)
		{
			this->size = fileSize;
			this->data = new BYTE[size];
			fread(data, 1, size, h);
		}

		fclose(h);
	}

	void Decompress(void)
	{
		Con_Print_v("Decompress: %s\n", entry.name.c_str());

		if (!this->isValid())
			return;

		if (this->entry.type == FileType::FF)
		{
			XFile ffInfo;
			unsigned long dSize = sizeof(XFile);
			uncompress((BYTE*)&ffInfo, &dSize, this->data, 0x8000);

			dSize = ffInfo.size + 36;
			if (dSize >= 1073741824)
			{
				//Any fastfiles that claim they decompress to a file >= 1GB
				//are either corrupt or do not belong to the vanilla game
				Con_Error("ERROR: Skipping %s\n", entry.name.c_str());
				this->Clear();
				return;
			}

			BYTE* dBuf = new BYTE[dSize];
			uncompress(dBuf, &dSize, this->data, this->size);
			
			BYTE* cBuf = this->data;
			this->data = dBuf;
			this->size = dSize;
			delete[] cBuf;
		}
	}

	void Search(char* pattern)
	{
		Con_Print_v("Search: %s\n", this->entry.name.c_str());

		if (!this->isValid())
			return;

		unsigned int pattern_len = strlen(pattern);
		for (unsigned int i = 0; i < this->size - pattern_len + 1; i++)
		{
			if (memcmp(&data[i], pattern, pattern_len) == 0)
			{
				Con_Print("%s\n", entry.name.c_str());
				return;
			}
		}

		return;
	}
};

void FF_Cache(FileEntry& fileEntry, CachedFile& outData)
{
	outData.Load(fileEntry);
}

void FF_Decompress(CachedFile& fileData)
{
	fileData.Decompress();
}

int Cmd_Search_f(int argc, char** argv)
{
	if (argc != 2)
	{
		char* _argv[] = { NULL, "search" };
		Cmd_Help_f(ARRAYSIZE(_argv), _argv);
		return -1;
	}

	const unsigned int MAX_CACHED_FILES = 2;
	const unsigned int THREAD_COUNT = 3;

	std::vector<FileEntry> entries;
	auto FF_ExtractDefered = [&entries](const char* filePath, const char* fileName) -> int
	{
		FileEntry entry = { FileType::FF, fileName, filePath };
		entries.push_back(entry);
		return 0;
	};
	
	// Handle iterating over the fastfiles for each fastfile directory
	// Used to ensure localized fastfiles are retrieved as well
	auto FF_IterateFastFileDirectory = [&FF_ExtractDefered](const char* path) -> int
	{
		return FS_FileIterator(path, FS_SEARCHPATTERN_FF, FF_ExtractDefered);
	};

	//
	// Extract normal fastfiles while deferring patch fastfiles
	//
	if (g_useLocalized.ValueBool())
		FS_DirectoryIterator(AppInfo_ZoneDir(), FF_IterateFastFileDirectory);
	else
		FS_FileIterator(AppInfo_FFDir(), FS_SEARCHPATTERN_FF, FF_ExtractDefered);

	char* pattern = argv[1];
	auto FF_Search = [pattern](CachedFile& fileData)
	{
		fileData.Search(pattern);
	};

	Handler<FileEntry, CachedFile> ffHandler(entries, MAX_CACHED_FILES, FF_Search, FF_Cache, FF_Decompress);

	ffHandler.RunHandler(THREAD_COUNT);

	return 0;
}
