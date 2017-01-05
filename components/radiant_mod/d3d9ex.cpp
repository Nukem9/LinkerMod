#include "stdafx.h"

bool r_d3d9ex = false;

bool IsD3D9ExAvailable()
{
	return r_d3d9ex;
}

IDirect3D9 *D3DAPI hk_Direct3DCreate9(UINT SDKVersion)
{
	HMODULE d3d9 = GetModuleHandleA("d3d9.dll");

	if (!d3d9)
		d3d9 = LoadLibraryA("d3d9.dll");

	if (!d3d9)
		return nullptr;

	auto pDirect3DCreate9 = (decltype(&Direct3DCreate9))GetProcAddress(d3d9, "Direct3DCreate9");
	auto pDirect3DCreate9Ex = (decltype(&Direct3DCreate9Ex))GetProcAddress(d3d9, "Direct3DCreate9Ex");

	IDirect3D9 *d3d = nullptr;
	IDirect3D9Ex *d3dex = nullptr;

	// Check for Direct3DCreate9Ex first (user must manually enable dvar, autodetection disabled)
	if (pDirect3DCreate9Ex)
	{
		if (SUCCEEDED(pDirect3DCreate9Ex(SDKVersion, &d3dex)))
		{
			printf("Using Direct3D9Ex interface\n");

			r_d3d9ex = true;
			return d3dex;
		}
	}

	// Otherwise default to the normal Direct3DCreate9
	if (pDirect3DCreate9)
	{
		if (d3d = pDirect3DCreate9(SDKVersion))
		{
			r_d3d9ex = false;
			return d3d;
		}
	}

	r_d3d9ex = false;
	return nullptr;
}

HRESULT D3DAPI hk_CreateDevice(IDirect3D9 *This, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DDevice9 **ppReturnedDeviceInterface)
{
	// Short-circuit to the original call
	if (!IsD3D9ExAvailable())
		return This->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

	// D3D requires a specific structure when in windowed mode
	D3DDISPLAYMODEEX displayMode;
	displayMode.Size = sizeof(D3DDISPLAYMODEEX);
	displayMode.Width = pPresentationParameters->BackBufferWidth;
	displayMode.Height = pPresentationParameters->BackBufferHeight;
	displayMode.RefreshRate = pPresentationParameters->FullScreen_RefreshRateInHz;
	displayMode.Format = pPresentationParameters->BackBufferFormat;
	displayMode.ScanLineOrdering = D3DSCANLINEORDERING_UNKNOWN;

	// This must be null when in windowed mode
	D3DDISPLAYMODEEX *realMode = (pPresentationParameters->Windowed) ? nullptr : &displayMode;

	HRESULT hr = ((IDirect3D9Ex *)This)->CreateDeviceEx(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, realMode, (IDirect3DDevice9Ex **)ppReturnedDeviceInterface);

	if (FAILED(hr))
		return hr;

	printf("Using CreateDeviceEx for device creation\n");
	return D3D_OK;
}

HRESULT D3DAPI hk_GetSwapChain(IDirect3DDevice9 *This, UINT iSwapChain, IDirect3DSwapChain9 **ppSwapChain)
{
	// Get the standard DirectX 9 swapchain
	HRESULT hr = This->GetSwapChain(iSwapChain, ppSwapChain);

	if (FAILED(hr))
		return hr;

	if (!IsD3D9ExAvailable())
		return D3D_OK;

	// Get a handle to the IDirect3DSwapChain9Ex interface via COM GUID
	hr = (*ppSwapChain)->QueryInterface(__uuidof(IDirect3DSwapChain9Ex), (void **)ppSwapChain);

	if (FAILED(hr))
		return hr;

	printf("Using IDirect3DSwapChain9Ex for swapchain %d\n", iSwapChain);
	return D3D_OK;
}

HRESULT D3DAPI hk_CreateAdditionalSwapChain(IDirect3DDevice9 *This, D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DSwapChain9 **ppSwapChain)
{
	// Short-circuit to the original call
	if (!IsD3D9ExAvailable())
		return This->CreateAdditionalSwapChain(pPresentationParameters, ppSwapChain);

	HRESULT hr = ((IDirect3DDevice9Ex *)This)->CreateAdditionalSwapChain(pPresentationParameters, ppSwapChain);

	if (FAILED(hr))
		return hr;

	// Get a handle to the IDirect3DSwapChain9Ex interface via COM GUID
	hr = (*ppSwapChain)->QueryInterface(__uuidof(IDirect3DSwapChain9Ex), (void **)ppSwapChain);

	if (FAILED(hr))
		return hr;

	printf("Using IDirect3DSwapChain9Ex for newly created swapchain\n");
	return D3D_OK;
}