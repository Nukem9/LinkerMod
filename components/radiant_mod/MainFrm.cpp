#include "stdafx.h"
#include "MainFrm.h"
#include <time.h>

#include "../game_mod/vanilla.h"

#define AFX_MAINFRAME_CUSTOM_WNDCLASS 1

VANILLA_VALUE(g_nUpdateBits,	int, 0x027C32F4);
VANILLA_VALUE(g_bScreenUpdates,	bool, 0x0074952F);

VANILLA_VALUE(g_time,		double, 0x02857D20);
VANILLA_VALUE(g_oldtime,	double, 0x02857D18);

// Taken directly from q3radiant
// https://github.com/id-Software/Quake-III-Arena
#define W_CAMERA		0x0001
#define W_XY			0x0002
#define W_XY_OVERLAY	0x0004
#define W_Z				0x0008
#define W_TEXTURE		0x0010
#define W_Z_OVERLAY		0x0020
#define W_CONSOLE		0x0040
#define W_ENTITY		0x0080
#define W_CAMERA_IFON	0x0100
#define W_XZ			0x0200  //--| only used for patch vertex manip stuff
#define W_YZ			0x0400  //--|
#define W_GROUP			0x0800 
#define W_MEDIA			0x1000 
#define W_ALL			0xFFFFFFFF

inline double Sys_DoubleTime(void)
{
    return clock() / 1000.0;
}

void __declspec(naked) CMainFrame::hk_RoutineProcessing(void)
{
	_asm
	{
		push ecx
		mov ecx, eax
		call CMainFrame::RoutineProcessing
		pop ecx
		retn
	}
}

BOOL __stdcall CMainFrame::hk_PreCreateWindow(CREATESTRUCTA *cs)
{
	AfxGetModuleState();
	struct AFX_MODULE_STATE *ms = AfxGetModuleState();
	HICON hIcon = LoadIconA(*((HINSTANCE *)ms + 3), (LPCSTR)0x6F);
	AfxGetModuleState();

	HCURSOR hCursor = LoadCursorA(NULL, (LPCSTR)IDC_ARROW);
	HBRUSH hBackground = CreateSolidBrush(RGB(140, 140, 140));

	const char* classname = "MainFrame";
#if !AFX_MAINFRAME_CUSTOM_WNDCLASS
	classname = AfxRegisterWndClass(CS_VREDRAW | CS_DROPSHADOW | CS_DBLCLKS, hCursor, hBackground, hIcon);
#else
	WNDCLASSA wc;
	if (!GetClassInfoA(*((HINSTANCE *)ms + 3), classname, &wc))
	{
		wc.lpfnWndProc = 0;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = 0;
		wc.hIcon = hIcon;
		wc.hCursor = hCursor;
		wc.hbrBackground = hBackground;
		wc.lpszMenuName = 0;
		wc.style = CS_VREDRAW | CS_DROPSHADOW | CS_DBLCLKS;
		wc.lpszClassName = classname;
		wc.hCursor = hCursor;
		wc.lpfnWndProc = DefWindowProcA;

		if (!AfxRegisterClass(&wc))
			Error("CMainFrame RegisterClass: failed");
	}

	cs->lpszClass = classname;
#endif

	return CFrameWnd::PreCreateWindow(cs);
}

void CMainFrame::RoutineProcessing()
{
	if (!this->m_bDoLoop)
		return;

	if (0.0 == g_time)
		g_time = 0.0;

	if (0.0 == g_oldtime)
		g_oldtime = 0.0;

	double time = Sys_DoubleTime();
	double oldtime = time - g_time;

	g_time = time;

	if (oldtime > 2.0)
		oldtime = 0.1;

	g_oldtime = oldtime;

	if (oldtime > 0.2)
		oldtime = 0.2;

	if (this->m_pCamWnd)
	{
		float delta = (float)oldtime;
		this->m_pCamWnd->Cam_MouseControl(delta);
	}

	if (g_nUpdateBits)
	{
		int nBits = g_nUpdateBits;
		g_nUpdateBits = 0;
		this->UpdateWindows(nBits);
	}
}

void CMainFrame::UpdateWindows(int nBits)
{
	if (!g_bScreenUpdates)
		return;
	
	if (nBits & (W_XY | W_XY_OVERLAY))
	{
		if (this->m_pXYWnd)
			m_pXYWnd->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}

	if (nBits & W_CAMERA || ((nBits & W_CAMERA_IFON) && this->m_bCamPreview))
	{
		if(this->m_pCamWnd)
		{
			// Redraw the camera view
			m_pCamWnd->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

			// Attempt to update the remote camera too
			if (CCamWnd::ActiveWindow)
				RemoteNet_UpdateCamera(CCamWnd::ActiveWindow->cameraOrigin, CCamWnd::ActiveWindow->cameraAngles);
		}
	}

	if (nBits & (W_Z | W_Z_OVERLAY))
	{
		if (this->m_pZWnd)
			m_pZWnd->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}

	if (nBits & W_TEXTURE)
	{
		if (this->m_pTexWnd)
			m_pTexWnd->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}
}
