#include "iwd.h"
#include "arg.h"
#include <Windows.h>
#include <iostream>
#include "miniz.h"
#include "steam.h"

#define IWD_DIR_SOUND "sound/"
#define IWD_DIR_IMAGE "images/"

int __cdecl IWD_IWDHandler(const char* iwdPath, const char* iwdName)
{
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
		printf("Extracting file: \"%s\"\n", filepath);
		mz_zip_reader_extract_file_to_file(iwd, filepath, outPath, 0);
		return 0;
	}

	if (FILE* h = fopen(outPath, "r"))
	{
		printf("Skipping file: \"%s\"\n", filepath);
		fclose(h);
		return 1;
	}
	else
	{
		printf("Extracting file: \"%s\"...	", filepath);
		if (FS_CreatePath(filepath) != 0)
		{
			printf("DIR ERROR\n");
			return 1;
		}
		
		if (!mz_zip_reader_extract_file_to_file(iwd, filepath, outPath, 0))
		{
			printf("ERROR\n");
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
		printf("ERROR: Could not open %s\n", iwdPath);
		return 2;
	}

	int extractedCount = 0;
	for (int f = 0; f < (int)mz_zip_reader_get_num_files(&iwd); f++)
	{
		mz_zip_archive_file_stat file_stat;
		if (!mz_zip_reader_file_stat(&iwd, f, &file_stat))
		{
			printf("ERROR: Could Get Info for Compressed File\n");
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