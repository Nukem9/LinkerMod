#pragma once

extern bool EnableD3DEx;

IDirect3D9 *D3DAPI hk_Direct3DCreate9(UINT SDKVersion);
HRESULT D3DAPI hk_CreateDevice(IDirect3D9 *This, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DDevice9 **ppReturnedDeviceInterface);
HRESULT D3DAPI hk_GetSwapChain(IDirect3DDevice9 *This, UINT iSwapChain, IDirect3DSwapChain9 **ppSwapChain);