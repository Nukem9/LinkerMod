#pragma once

struct DxGlobals
{
	HINSTANCE__ *hinst;
	IDirect3D9 *d3d9;
	IDirect3DDevice9 *device;
	unsigned int adapterIndex;
	unsigned int vendorId;
	bool adapterNativeIsValid;
	int adapterNativeWidth;
	int adapterNativeHeight;
	int adapterFullscreenWidth;
	int adapterFullscreenHeight;
	bool supportsSceneNullRenderTarget;
	bool supportsIntZ;
	bool nvInitialized;
	bool nvStereoActivated;
	void *nvStereoHandle;
	void *nvDepthBufferHandle;
	void *nvFloatZBufferHandle;
	bool resizeWindow;

	// ...
};

struct GfxWindowParms
{
	HWND__ *hwnd;
	int hz;
	bool fullscreen;
	int x;
	int y;
	int sceneWidth;
	int sceneHeight;
	int displayWidth;
	int displayHeight;
	int aaSamples;
};

struct __declspec(align(4)) vidConfig_t
{
	unsigned int sceneWidth;
	unsigned int sceneHeight;
	unsigned int displayWidth;
	unsigned int displayHeight;
	unsigned __int16 outputDisplayWidth;
	unsigned __int16 outputDisplayHeight;
	unsigned int displayFrequency;
	bool isToolMode;
	int isFullscreen;
	float aspectRatioWindow;
	float aspectRatioScenePixel;
	float aspectRatioDisplayPixel;
	unsigned int maxTextureSize;
	unsigned int maxTextureMaps;
	bool deviceSupportsGamma;
};

#define ASPECT_RATIO(W, H) ((float)W) / ((float)H)

enum /*r_aspectRatio*/
{
	ASPECT_RATIO_AUTO,
	ASPECT_RATIO_STANDARD,
	ASPECT_RATIO_16_10,
	ASPECT_RATIO_16_9,
};

VANILLA_VALUE(vidConfig, vidConfig_t, 0x03966148);
VANILLA_VALUE(dx, DxGlobals, 0x03963440);

static IDirect3DDevice9*& dx_device = *(IDirect3DDevice9 **)0x03963448;
static int g_disableRendering;
static bool& r_supportCubedMipMaps = *(bool *)0x0396A4F7;

typedef void(__cdecl *cmd_function)();
static cmd_function R_Cmd_Screenshot = (cmd_function)0x007244D0;
static cmd_function R_Cmd_ScreenshotJpeg = (cmd_function)0x007244E0;
static cmd_function R_Cmd_ApplyPicmip = (cmd_function)0x007244C0;
static cmd_function R_Cmd_ReloadMaterialTextures = (cmd_function)0x006D4420;
static cmd_function R_Cmd_LoadSun = (cmd_function)0x00735DA0;
static cmd_function R_Cmd_SaveSun = (cmd_function)0x00735E90;
static cmd_function R_StaticModelCacheStats_f = (cmd_function)0x0070C240;
static cmd_function R_StaticModelCacheFlush_f = (cmd_function)0x0070C410;

const char *R_ErrorDescription(HRESULT hr);

HWND WINAPI hk_CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
BOOL WINAPI hk_AdjustWindowRectEx(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle);
LONG WINAPI hk_SetWindowLongA(HWND hWnd, int nIndex, LONG dwNewLong);
void hk_R_StoreWindowSettings(void);

void __cdecl R_RegisterCmds();
void __cdecl R_UnregisterCmds();

struct Font_s *__cdecl R_RegisterFont(const char *name, int imageTrack);
void __cdecl R_StoreWindowSettings(GfxWindowParms *wndParms);
