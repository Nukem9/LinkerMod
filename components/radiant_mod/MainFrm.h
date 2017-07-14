#pragma once
#include <Windows.h>
#include "CamWnd.h"
#include "TexWnd.h"
#include "XYWnd.h"
#include "ZWnd.h"

class CMainFrame
{
friend BOOL RadiantMod_Init();
private:
	// Do not allow this class to be instanced
	CMainFrame() = delete;
	~CMainFrame() = delete;

	int gap0;
	BYTE gap4[1976];
	CXYWnd *m_pXYWnd;
	CCamWnd *m_pCamWnd;
	CTexWnd *m_pTexWnd;
	CZWnd *m_pZWnd;
	BYTE gap7CC[76];
	bool m_bCamPreview;
	BYTE gap819[7];
	bool m_bDoLoop;

	static void hk_RoutineProcessing(void);
	static BOOL __stdcall hk_PreCreateWindow(CREATESTRUCTA* cs);
public:
	void RoutineProcessing(void);
	void UpdateWindows(int nBits);

};
