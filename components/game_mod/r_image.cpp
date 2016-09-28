#include "stdafx.h"

void __cdecl Image_HandleMissingImage(int code, const char *fmt, const char* image)
{
	DB_LogMissingAsset(ASSET_TYPE_IMAGE, image);
	Com_PrintWarning(8, fmt, image);
}