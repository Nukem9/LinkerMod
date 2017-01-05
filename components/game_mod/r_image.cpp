#include "stdafx.h"

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
	// ASSERT(Sys_IsRenderThread());

	image->width	= width;
	image->height	= height;
	image->depth	= 1;
	image->mapType	= 3;

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
	// ASSERT(Sys_IsRenderThread());

	image->width	= width;
	image->height	= height;
	image->depth	= depth;
	image->mapType	= 4;

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
	// ASSERT(Sys_IsRenderThread());

	image->width	= edgeLen;
	image->height	= edgeLen;
	image->depth	= 1;
	image->mapType	= 5;

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