#include "stdafx.h"

bool Com_IsBspLoaded()
{
	return comBspGlob->header != nullptr;
}

bool Com_BspError()
{
	if (comBspGlob->header->ident == 'PSBI' &&
		comBspGlob->header->version >= 6 &&
		comBspGlob->header->version <= 45)
		return false;

	return true;
}

void Com_GetBspFilename(char *filename, int size, const char *mapname)
{
	sprintf_s(filename, size, "maps/%s.d3dbsp");
}

void Com_LoadBsp(const char *filename)
{
	ASSERT(filename != nullptr);
	ASSERT(!Com_IsBspLoaded());
	ASSERT(comBspGlob->loadedLumpData == nullptr);

	Com_Printf(0, "\n");
	Com_Printf(0, "Com_LoadBsp(%s)\n", filename);

	//
	// Get a handle to the file
	//
	int bspFileHandle;
	comBspGlob->fileSize = FS_FOpenFile(filename, &bspFileHandle);

	if(!bspFileHandle)
		Com_Error(0, "EXE_ERR_COULDNT_LOAD\x15%s", filename);

	//
	// Allocate space for the BSP header and read it
	//
	comBspGlob->header = (BspHeader *)Z_Malloc(comBspGlob->fileSize);

	if(FS_Read(comBspGlob->header, comBspGlob->fileSize, bspFileHandle) != comBspGlob->fileSize)
		Com_Error(0, "EXE_ERR_COULDNT_LOAD\x15%s", filename);

	//
	// Close the BSP file handle
	//
	FS_FCloseFile(bspFileHandle);

	//
	// Calculate the CRC32 of the BSP data
	//
	comBspGlob->checksum = crc32_calculate((const char *)comBspGlob->header, comBspGlob->fileSize, 0);

	Com_Printf(0, "\tSize:  %d\n", comBspGlob->fileSize);
	Com_Printf(0, "\tCRC32: %X\n", comBspGlob->checksum);

	//
	// Did the BSP load correctly, and is it a valid version?
	//
	if (Com_BspError())
	{
		Z_Free(comBspGlob->header);
		comBspGlob->header = nullptr;

		Com_Printf(0, "EXE_ERR_WRONG_MAP_VERSION_NUM\x15%s", filename);
	}

	//
	// Mod tools assertion checks
	//
	ASSERT(strlen(filename) < ARRAYSIZE(comBspGlob->name));

	strcpy_s(comBspGlob->name, filename);

	ASSERT(Com_IsBspLoaded());

	Com_Printf(0, "Com_LoadBsp: End\n");
	Com_Printf(0, "\n");
}

const void *Com_LoadBspLump(const char *mapname, LumpType type, unsigned int elemSize, unsigned int *count)
{
	ASSERT(comBspGlob->loadedLumpData == nullptr);

	if (Com_IsBspLoaded())
	{
		ASSERT(strstr(comBspGlob->name, mapname) != nullptr);

		comBspGlob->loadedLumpData = Com_GetBspLump(type, elemSize, count);
	}
	else
	{
		//
		// Fall back to a manual disk read if the BSP isn't loaded right now
		// TODO: UNIMPLEMENTED
		//
		char filename[256];
		Com_GetBspFilename(filename, ARRAYSIZE(filename), mapname);

		int fileHandle;
		int fileSize = FS_FOpenFileReadCurrentThread(filename, &fileHandle);

		if (fileSize < 0)
		{
			*count = 0;
			return nullptr;
		}

		Hunk_CheckTempMemoryHighClear();

		ASSERT(false && "Unhandled BSP load type");
		comBspGlob->loadedLumpData = nullptr;//Com_ReadLumpOutOfBsp(fileHandle, type, elemSize, count);

		FS_FCloseFile(fileHandle);
	}

	comBspGlob->loadedLumpType = type;
	return comBspGlob->loadedLumpData;
}

void Com_UnloadBspLump(LumpType type)
{
	ASSERT(comBspGlob->loadedLumpData != nullptr);
	ASSERT(comBspGlob->loadedLumpType == type);

	comBspGlob->loadedLumpData = nullptr;
}