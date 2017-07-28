#include "stdafx.h"
#include <algorithm>

const char* g_platform_name[] = { "current", "min_pc", "bad allocation", "bad allocation", "bad allocation", "bad allocation", "bad allocation", "bad allocation" };
const char* imageTypeName[] = { "misc", "debug", "$tex+?", "ui", "lmap", "light", "f/x", "hud", "model", "world" };

void Image_HandleMissingImage(int code, const char *fmt, const char* image)
{
	DB_LogMissingAsset(ASSET_TYPE_IMAGE, image);
	Com_PrintWarning(8, fmt, image);
}

unsigned int Image_GetUsage(int imageFlags, D3DFORMAT imageFormat)
{
	if (imageFlags & 0x20000)
	{
		if (imageFormat != D3DFMT_D24S8
			&& imageFormat != D3DFMT_D24X8
			&& imageFormat != D3DFMT_D16)
			return D3DUSAGE_RENDERTARGET;

		return D3DUSAGE_DEPTHSTENCIL;
	}

	if (imageFlags & 0x10000)
		return D3DUSAGE_DYNAMIC;

	return 0;
}

void Image_Create2DTexture_PC(GfxImage *image, unsigned __int16 width, unsigned __int16 height, int mipmapCount, int imageFlags, D3DFORMAT imageFormat)
{
	ASSERT(image != nullptr);
	ASSERT(!image->texture.basemap);
	ASSERT(Sys_IsRenderThread());

	image->width	= width;
	image->height	= height;
	image->depth	= 1;
	image->mapType	= MAPTYPE_2D;

	D3DPOOL memPool	= D3DPOOL_DEFAULT;
	DWORD usage		= Image_GetUsage(imageFlags, imageFormat);

	if (imageFlags & 0x40000 || imageFlags & 0x100)
		memPool = D3DPOOL_SYSTEMMEM;
	else
		memPool = (usage == 0) ? D3DPOOL_MANAGED : D3DPOOL_DEFAULT;

	// D3D9Ex does not allow D3DPOOL_MANAGED
	if (IsD3D9ExAvailable())
	{
		usage	= (usage == 0) ? D3DUSAGE_DYNAMIC : usage;
		memPool = (memPool == D3DPOOL_MANAGED) ? D3DPOOL_DEFAULT : memPool;
	}

	HRESULT hr = dx_device->CreateTexture(width, height, mipmapCount, usage, imageFormat, memPool, &image->texture.map, nullptr);

	if (FAILED(hr))
	{
		g_disableRendering++;
		Com_Error(ERR_FATAL, "dx.device->CreateTexture(width, height, mipmapCount, usage, imageFormat, memPool, &image->texture.map, nullptr) failed: %s\n", R_ErrorDescription(hr));
	}
}

void Image_Create3DTexture_PC(GfxImage *image, unsigned __int16 width, unsigned __int16 height, unsigned __int16 depth, int mipmapCount, int imageFlags, D3DFORMAT imageFormat)
{
	ASSERT(image != nullptr);
	ASSERT(!image->texture.basemap);
	ASSERT(Sys_IsRenderThread());

	image->width	= width;
	image->height	= height;
	image->depth	= depth;
	image->mapType	= MAPTYPE_3D;

	// D3D9Ex does not allow D3DPOOL_MANAGED
	DWORD usage		= (IsD3D9ExAvailable()) ? D3DUSAGE_DYNAMIC : 0;
	D3DPOOL memPool = (IsD3D9ExAvailable()) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;

	HRESULT hr = dx_device->CreateVolumeTexture(width, height, depth, mipmapCount, usage, imageFormat, memPool, &image->texture.volmap, nullptr);

	if (FAILED(hr))
	{
		g_disableRendering++;
		Com_Error(ERR_FATAL, "dx.device->CreateVolumeTexture(width, height, depth, mipmapCount, usage, imageFormat, memPool, &image->texture.volmap, nullptr) failed: %s\n", R_ErrorDescription(hr));
	}
}

void Image_CreateCubeTexture_PC(GfxImage *image, unsigned __int16 edgeLen, int mipmapCount, D3DFORMAT imageFormat)
{
	ASSERT(image != nullptr);
	ASSERT(!image->texture.basemap);
	ASSERT(Sys_IsRenderThread());

	image->width	= edgeLen;
	image->height	= edgeLen;
	image->depth	= 1;
	image->mapType	= MAPTYPE_CUBE;

	// D3DDeviceCaps support for mipping
	if (!r_supportCubedMipMaps)
		mipmapCount = 1;

	// D3D9Ex does not allow D3DPOOL_MANAGED
	DWORD usage		= (IsD3D9ExAvailable()) ? D3DUSAGE_DYNAMIC : 0;
	D3DPOOL memPool = (IsD3D9ExAvailable()) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;

	HRESULT hr = dx_device->CreateCubeTexture(edgeLen, mipmapCount, usage, imageFormat, memPool, &image->texture.cubemap, nullptr);
	
	if (FAILED(hr))
	{
		g_disableRendering++;
		Com_Error(ERR_FATAL, "dx.device->CreateCubeTexture(edgeLen, mipmapCount, usage, imageFormat, memPool, &image->texture.cubemap, nullptr) failed: %s\n", R_ErrorDescription(hr));
	}
}

void __declspec(naked) hk_Image_Create2DTexture_PC(unsigned __int16 width, int mipmapCount, int imageFormat)
{
	__asm
	{
		push ebp
		mov ebp, esp

		push imageFormat	// a6: imageFormat
		push edx			// a5: imageFlags
		push mipmapCount	// a4: mipmapCount
		movzx ecx, di		//
		push ecx			// a3: height
		movzx ecx, width	//
		push ecx			// a2: width
		push eax			// a1: image
		call Image_Create2DTexture_PC
		add esp, 0x18

		mov esp, ebp
		pop ebp
		ret
	}
}

void __declspec(naked) hk_Image_Create3DTexture_PC(int image, int mipmapCount, int imageFormat)
{
	__asm
	{
		push ebp
		mov ebp, esp

		push imageFormat	// a7: imageFormat
		push 0				// a6: imageFlags
		push mipmapCount	// a5: mipmapCount
		movzx eax, cx		//
		push eax			// a4: depth
		movzx eax, dx		//
		push eax			// a3: height
		movzx eax, si		//
		push eax			// a2: width
		push image			// a1: image
		call Image_Create3DTexture_PC
		add esp, 0x1C

		mov esp, ebp
		pop ebp
		ret
	}
}

void __declspec(naked) hk_Image_CreateCubeTexture_PC(signed int mipmapCount, int imageFormat)
{
	__asm
	{
		push ebp
		mov ebp, esp

		push imageFormat	// a4: imageFormat
		push mipmapCount	// a3: mipmapCount
		movzx edx, cx		//
		push edx			// a2: edgeLen
		push eax			// a1: image
		call Image_CreateCubeTexture_PC
		add esp, 0x10

		mov esp, ebp
		pop ebp
		ret
	}
}

bool __cdecl Image_IsCodeImage(int track)
{
	return track >= 0 && (track <= 1 || track == 4);
}

struct ImageList
{
	unsigned int count;
	GfxImage *image[4096*2]; // make sure there are enough pointers for the modified image limit
};

D3DFORMAT __cdecl R_ImagePixelFormat(GfxImage *image)
{
	_D3DSURFACE_DESC surfaceDesc;
	_D3DVOLUME_DESC volumeDesc;

	switch (image->mapType)
	{
	case MAPTYPE_2D:
		ASSERT(image->texture.map);
		image->texture.map->GetLevelDesc(0, &surfaceDesc);
		return surfaceDesc.Format;
	case MAPTYPE_3D:
		ASSERT(image->texture.volmap);
		image->texture.volmap->GetLevelDesc(0, &volumeDesc);
		return volumeDesc.Format;
	case MAPTYPE_CUBE:
		ASSERT(image->texture.cubemap);
		image->texture.cubemap->GetLevelDesc(0, &surfaceDesc);
		return surfaceDesc.Format;
	default:
		ASSERT_MSG_VA(false, "unhandled case %i for %s", image->mapType, image->name);
		return D3DFMT_UNKNOWN;
	}
	
	return D3DFMT_UNKNOWN;
}

#include <Windows.h>

void __cdecl R_AddImageToList(GfxImage* image, ImageList *list)
{
	ASSERT(list->count < _countof(list->image));
	list->image[list->count++] = image;
}

void __cdecl R_GetImageList(ImageList *imageList)
{
	ASSERT(imageList);
	imageList->count = 0;
	DB_EnumXAssets(ASSET_TYPE_IMAGE, (void(__cdecl *)(XAssetHeader, void *))R_AddImageToList, imageList, 1);
}

void __cdecl R_ImageList_f()
{
	CardMemory total;
	total.platform[0] = 0;
	total.platform[1] = 0;

	CardMemory imageTrack[10];
	memset(imageTrack, 0, sizeof(CardMemory) * 10);

	ImageList imageList;
	R_GetImageList(&imageList);
	
	std::sort<GfxImage**, int(__cdecl *)(GfxImage *, GfxImage *)>(imageList.image, &imageList.image[imageList.count],
		[](GfxImage* image1, GfxImage* image2) -> int
		{
			if (image1->track <= image2->track)
			{
				if (image1->track >= image2->track)
					return image1->cardMemory.platform[0] < image2->cardMemory.platform[0];
				else
					return 1;
			}

			return 0;
		});

	Com_Printf(8, "\n-reqrd w*h-");
	Com_Printf(8, "-fmt-  ");

	for (int platform = 0; platform < 2; ++platform)
	{
		Com_Printf(8, "%s", g_platform_name[platform]);
	}

	Com_Printf(8, "  --name-------\n");

	for (unsigned int i = 0; i < imageList.count; ++i)
	{
		GfxImage* image = imageList.image[i];
		Com_Printf(8, "%4i x %-4i ", image->width, image->height);
		_D3DFORMAT format = R_ImagePixelFormat(image);

		switch (format)
		{
			case D3DFMT_X8R8G8B8:
				Com_Printf(8, "RGB32 ");
				break;
			case D3DFMT_A8R8G8B8:
				Com_Printf(8, "RGBA32");
				break;
			case D3DFMT_R5G6B5:
				Com_Printf(8, "RGB565");
				break;
			case D3DFMT_A8:
				Com_Printf(8, "A8    ");
				break;
			case D3DFMT_G16R16:
				Com_Printf(8, "G16R16");
				break;
			case D3DFMT_L8:
				Com_Printf(8, "L8    ");
				break;
			case D3DFMT_A8L8:
				Com_Printf(8, "AL16  ");
				break;
			case D3DFMT_R32F:
				Com_Printf(8, "R32F  ");
				break;
			case D3DFMT_DXT1:
				Com_Printf(8, "DXT1  ");
				break;
			case D3DFMT_DXT3:
				Com_Printf(8, "DXT3  ");
				break;
			case D3DFMT_DXT5:
				Com_Printf(8, "DXT5  ");
				break;

			default:
				ASSERT_MSG_VA(0, "unhandled case: %d", format);
		}

		Com_Printf(8, "  %s", imageTypeName[image->track]);

		for (int platform = 0; platform < 2; ++platform)
		{
			int cardMemory = image->cardMemory.platform[platform];
			float value = (float)cardMemory / 1024.0f;

			const char* fmt = (value >= 10.0) ? "%7.0fk" : "%7.1fk";
			Com_Printf(8, fmt, value);

			if (!useFastFile->current.enabled)
			{
				imageTrack[image->track].platform[platform] += cardMemory;
				if (Image_IsCodeImage(image->track))
					continue;
			}
			total.platform[platform] += cardMemory;
		}
		Com_Printf(8, "  %-48s  0x%x\n", image->name, image);
	}

	Com_Printf(8, " ---------\n");
	Com_Printf(8, " %i total images\n", imageList.count);

	for (int platform = 0; platform < 2; ++platform)
	{
		Com_Printf(8, " %5.1f MB %s total non-streamed image size\n", (float)((float)total.platform[platform] / 1048576.0), g_platform_name[platform]);
	}
		
	if (!useFastFile->current.enabled)
	{
		Com_Printf(8, "\n");
		Com_Printf(8, "       ");
		for (int platform = 0; platform < 2; ++platform)
		{
			Com_Printf(8, "%s", g_platform_name[platform]);
		}

		Com_Printf(8, "\n");
		
		for (int i = 0; i < 10; ++i)
		{
			Com_Printf(8, "%s:", imageTypeName[i]);
			for (int platform = 0; platform < 2; ++platform)
			{
				Com_Printf(8, "  %5.1f", (float)((float)imageTrack[i].platform[platform] / 1048576.0));
			}

			Com_Printf(8, "  MB\n");
		}
	}

	Com_Printf(8, "Related commands: imagelist, gfx_world, cg_drawfps\n");
}

void Image_Release(GfxImage *image)
{
	ASSERT(image);

	((void(__cdecl *)())0x00709E10)();
}

// /gfx_d3d/r_image.cpp:214
void Image_TrackTotalMemory(GfxImage *image, int platform, int memory)
{
	if (!Image_IsCodeImage(image->track))
		((int *)0x4572D8C)[platform] += memory - image->cardMemory.platform[platform];
}

// /gfx_d3d/r_image.cpp:792
void Image_PicmipForSemantic(char semantic, Picmip *picmip)
{
	int picmipUsed = 0;

	switch (semantic)
	{
	default:
		ASSERT_MSG_VA(0, "Unhandled case: %d", semantic);
	case TS_2D:
	case TS_FUNCTION:
		picmip->platform[PICMIP_PLATFORM_USED] = 0;
		picmip->platform[PICMIP_PLATFORM_MINSPEC] = 0;
		return;

	case TS_NORMAL_MAP:
		picmipUsed = *(int *)0x4572D84;
		break;

	case TS_SPECULAR_MAP:
		picmipUsed = *(int *)0x4572D88;
		break;

	case TS_COLOR_MAP:
	case TS_WATER_MAP:
	case TS_COLOR0_MAP:
	case TS_COLOR1_MAP:
	case TS_COLOR2_MAP:
	case TS_COLOR3_MAP:
	case TS_COLOR4_MAP:
	case TS_COLOR5_MAP:
	case TS_COLOR6_MAP:
	case TS_COLOR7_MAP:
	case TS_COLOR8_MAP:
	case TS_COLOR9_MAP:
	case TS_COLOR10_MAP:
	case TS_COLOR11_MAP:
	case TS_COLOR12_MAP:
	case TS_COLOR13_MAP:
	case TS_COLOR14_MAP:
	case TS_COLOR15_MAP:
	case TS_THROW_MAP:
		picmipUsed = *(int *)0x4572D80;
		break;
	}

	picmip->platform[PICMIP_PLATFORM_MINSPEC] = 2;

	if (picmipUsed >= 0)
	{
		if (picmipUsed > 3)
			picmipUsed = 3;
	}
	else
	{
		picmipUsed = 0;
	}

	picmip->platform[PICMIP_PLATFORM_USED] = picmipUsed;
}

void R_ImageList_Output()
{
}