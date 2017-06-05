#include "../common/ff.h"
#include "../common/iwd.h"

#include "../sys/AppInfo.h"
#include "../common/io.h"
#include "../common/fs.h"

#include "../cvar.h"

#include <vector>
#include <algorithm>

enum PriorityFlags
{
	PATCH =				1 << 0,
	MP =				1 << 1,
	SO =				1 << 2,
	ZM =				1 << 3,

	CODE_PRE_GFX =		1 << 4,
	CODE_POST_GFX =		1 << 5,
	UI =				1 << 6,

	COMMON =			1 << 7,
	TERMINAL =			1 << 8,
	LEVEL =				1 << 9,

	LOCALIZED =			1 << 10
};

// Determine the extraction order priority for a given fastfile
// Lower numbers get higher priority (are extracted first)
int FF_GetPriority(const char* name, const char* path)
{
	int flags = (PriorityFlags)NULL;

	if (strstr(path, AppInfo_FFDir()) != NULL)
		flags |= LOCALIZED;

	if (strstr(name, "_patch") != NULL || strstr(name, "patch") == name)
		flags |= PATCH;

	if (strstr(name, "mp_") != NULL || strstr(name, "_mp") != NULL)
		flags |= MP;

	if (strstr(name, "so_") != NULL)
		flags |= SO;

	if (strstr(name, "zombie"))
		flags |= ZM;

	if (strstr(name, "code_pre_gfx") != NULL)
		flags |= CODE_PRE_GFX;
	else if (strstr(name, "code_post_gfx") != NULL)
		flags |= CODE_POST_GFX;
	else if (strstr(name, "common") != NULL)
		flags |= COMMON;
	else if (strstr(name, "ui") != NULL)
		flags |= UI;
	else if (strstr(name, "terminal") != NULL)
		flags |= TERMINAL;
	else
		flags |= LEVEL;

	return flags;
}

struct FastFileEntry
{
	int priority;
	std::string name;
	std::string path;
};

int Cmd_Extract_FF_f(int argc, char** argv)
{
	if (g_extractAll.ValueBool())
	{
		std::vector<FastFileEntry> entries;
		auto FF_ExtractDefered = [&entries](const char* filePath, const char* fileName) -> int
		{
			FastFileEntry entry;
			entry.priority = FF_GetPriority(fileName, filePath);
			entry.path = filePath;
			entry.name = fileName;

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

		//
		// Sort the fastfiles by priority weight
		//
		std::sort(entries.begin(), entries.end(), [](FastFileEntry& a, FastFileEntry& b)->bool
		{
			return a.priority < b.priority;
		});

		for (auto& entry : entries)
		{
			FF_FFExtract(entry.path.c_str(), entry.name.c_str());
		}
	}
	else
	{
		for (int i = 1; i < argc; i++)
		{
			const char* filename = FS_GetFilenameSubString(argv[i]);
			FF_FFExtract(argv[i], filename);
		}
	}
	return 0;
}

int Cmd_Extract_IWD_f(int argc, char** argv)
{
	FS_FileIterator(AppInfo_IWDDir(), FS_SEARCHPATTERN_IWD, IWD_IWDHandler);
	return 0;
}
