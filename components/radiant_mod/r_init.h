#pragma once

static IDirect3DDevice9*& dx_device = *(IDirect3DDevice9 **)0x13A15A0;
static int& g_disableRendering = *(int *)0x00EE4F80;
static bool& r_supportCubedMipMaps = *(bool *)0x013ACAD6;

const char *R_ErrorDescription(HRESULT hr);