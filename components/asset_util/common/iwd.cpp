#include "iwd.h"
#include "../cvar.h"
#include <Windows.h>
#include "io.h"
#include "../shared/miniz/miniz.h"
#include "str.h"
#include "../sys/AppInfo.h"

#define IWD_DIR_SOUND "sound/"
#define IWD_DIR_IMAGE "images/"

int __cdecl IWD_IWDHandler(const char* iwdPath, const char* iwdName)
{
	if (!g_useLocalized.ValueBool() && stristr(iwdName, "localized"))
	{
		return 0;
	}


	char sub[MAX_PATH];
	if (g_extractAll.ValueBool())
	{
		sprintf_s(sub, "ALL");
	}
	else if (g_extractImages.ValueBool())
	{
		sprintf_s(sub, "IMAGE");

		if (g_extractSounds.ValueBool())
		{
			sprintf_s(sub, "%s and %s", sub, "AUDIO");
		}
	}
	else
	{
		sprintf_s(sub, "AUDIO");
	}

	Con_Print("Extracting %s files from: \"%s\"...\n", sub, iwdName);
	IWD_IWDExtract(iwdPath, iwdName);
	return 0;
}

int __cdecl IWD_IWDExtractFile(mz_zip_archive* iwd, const char* filepath)
{
	char outPath[MAX_PATH];
	sprintf_s(outPath, "%s/%s", AppInfo_OutDir(), filepath);
	FS_SanitizePath(outPath);

	if (fs_overwrite.ValueBool())
	{
		Con_Print_v("Extracting file: \"%s\"... ", filepath);

		if (!mz_zip_reader_extract_file_to_file(iwd, filepath, outPath, 0))
		{
			Con_Print_v("ERROR\n");
			return 1;
		}

		Con_Print_v("SUCCESS\n");
		return 0;
	}

	if (FS_FileExists(outPath))
	{
		Con_Print_v("Skipping file: \"%s\"\n", filepath);
		return 1;
	}
	else
	{
		Con_Print_v("Extracting file: \"%s\"...	", filepath);

		int err = FS_CreatePath(filepath);
		if (err != 0)
		{
			Con_Error_v("DIR ERROR (%d)\n", err);
			return 1;
		}
		
		if (!mz_zip_reader_extract_file_to_file(iwd, filepath, outPath, 0))
		{
			Con_Error_v("ERROR\n");
			return 1;
		}

		Con_Print_v("SUCCESS\n");
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
		Con_Error_v("ERROR: Could not open %s\n", iwdPath);
		return 2;
	}

	int extractedCount = 0;
	for (int f = 0; f < (int)mz_zip_reader_get_num_files(&iwd); f++)
	{
		mz_zip_archive_file_stat file_stat;
		if (!mz_zip_reader_file_stat(&iwd, f, &file_stat))
		{
			Con_Error_v("ERROR: Could Get Info for Compressed File\n");
			mz_zip_reader_end(&iwd);
			return -1;
		}

		if (g_extractAll.ValueBool())
		{
			IWD_IWDExtractFile(&iwd, file_stat.m_filename);
			continue;
		}
		else if (g_extractImages.ValueBool() && _strnicmp(IWD_DIR_IMAGE, file_stat.m_filename, strlen(IWD_DIR_IMAGE)) == 0)
		{
			IWD_IWDExtractFile(&iwd, file_stat.m_filename);
			continue;
		}
		else if (g_extractSounds.ValueBool() && _strnicmp(IWD_DIR_SOUND, file_stat.m_filename, strlen(IWD_DIR_SOUND)) == 0)
		{
			IWD_IWDExtractFile(&iwd, file_stat.m_filename);
			continue;
		}
	}

	mz_zip_reader_end(&iwd);
	return 0;
}