#include "stdafx.h"

SRCLINE(2210)
void Material_ReloadTextures()
{
	for (int textureIter = 0; textureIter < gfxImageTextureTableSize; textureIter++)
	{
		GfxImage *image = gfxImageTextureTable[textureIter];

		if (image && image->category == IMG_CATEGORY_LOAD_FROM_FILE)
			Image_Reload(image);
	}
}