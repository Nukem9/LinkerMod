#include "iwd.h"
#include "cli/arg.h"
#include <Windows.h>
#include "common/io.h"
#include "../miniz/miniz.h"
#include "common/str.h"
#include "AppInfo.h"

#define IWD_DIR_SOUND "sound/"
#define IWD_DIR_IMAGE "images/"

int __cdecl IWD_IWDHandler(const char* iwdPath, const char* iwdName)
{
	if (!ARG_FLAG_LOCALIZED && stristr(iwdName, "localized"))
	{
		return 0;
	}


	char sub[MAX_PATH];
	if (ARG_FLAG_EVERY)
	{
		sprintf_s(sub, "ALL");
	}
	else if (ARG_FLAG_IMAGE)
	{
		sprintf_s(sub, "IMAGE");

		if (ARG_FLAG_AUDIO)
		{
			sprintf_s(sub, "%s and %s", sub, "AUDIO");
		}
	}
	else
	{
		sprintf_s(sub, "AUDIO");
	}

	printf("Extracting %s files from: \"%s\"...\n", sub, iwdName);
	IWD_IWDExtract(iwdPath, iwdName);
	return 0;
}

int __cdecl IWD_IWDExtractFile(mz_zip_archive* iwd, const char* filepath)
{
	char outPath[MAX_PATH];
	sprintf_s(outPath, "%s/%s", AppInfo_RawDir(), filepath);

	if (ARG_FLAG_OVERWRITE)
	{
		printf_v("Extracting file: \"%s\"\n", filepath);
		mz_zip_reader_extract_file_to_file(iwd, filepath, outPath, 0);
		return 0;
	}

	FILE* h = NULL;
	if (fopen_s(&h, outPath, "r"))
	{
		printf_v("Skipping file: \"%s\"\n", filepath);
		fclose(h);
		return 1;
	}
	else
	{
		printf_v("Extracting file: \"%s\"...	", filepath);
		if (FS_CreatePath(filepath) != 0)
		{
			printf_v("DIR ERROR\n");
			return 1;
		}
		
		if (!mz_zip_reader_extract_file_to_file(iwd, filepath, outPath, 0))
		{
			printf_v("ERROR\n");
			return 1;
		}

		printf("SUCCESS\n");
		return 0;
	}
	
	return 0;
}

int __cdecl IWD_IWDExtract(const char* iwdPath, const char* iwdName)
{
	mz_zip_archive iwd;
	memset(&iwd, 0, sizeof(iwd));

	if (!mz_zip_reader_init_file(&iwd, iwdPath, 0))
	{
		printf_v("ERROR: Could not open %s\n", iwdPath);
		return 2;
	}

	int extractedCount = 0;
	for (int f = 0; f < (int)mz_zip_reader_get_num_files(&iwd); f++)
	{
		mz_zip_archive_file_stat file_stat;
		if (!mz_zip_reader_file_stat(&iwd, f, &file_stat))
		{
			printf_v("ERROR: Could Get Info for Compressed File\n");
			mz_zip_reader_end(&iwd);
			return -1;
		}

		if (ARG_FLAG_EVERY)
		{
			IWD_IWDExtractFile(&iwd, file_stat.m_filename);
			continue;
		}
		else if (ARG_FLAG_IMAGE && _strnicmp(IWD_DIR_IMAGE, file_stat.m_filename, strlen(IWD_DIR_IMAGE)) == 0)
		{
			IWD_IWDExtractFile(&iwd, file_stat.m_filename);
			continue;
		}
		else if (ARG_FLAG_AUDIO && _strnicmp(IWD_DIR_SOUND, file_stat.m_filename, strlen(IWD_DIR_SOUND)) == 0)
		{
			IWD_IWDExtractFile(&iwd, file_stat.m_filename);
			continue;
		}
	}

	mz_zip_reader_end(&iwd);
	return 0;
}