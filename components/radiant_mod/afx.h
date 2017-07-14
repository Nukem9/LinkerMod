#pragma once
#include "stdafx.h"

typedef HBRUSH(__thiscall * OnCtlColor_t)(void *thisptr, class CDC *pDC, class CWnd *pWnd, UINT nCtlColor);
typedef void(__thiscall * SetTextColor_t)(void *, COLORREF);
typedef COLORREF(__thiscall * SetBkColor_t)(void *, COLORREF);

class CDC
{
private:
	// Do not allow this class to be instanced
	CDC();

public:
	
	void SetTextColor(COLORREF crColor);
	COLORREF SetBkColor(COLORREF crColor);
};

class CWnd
{
private:
	// Do not allow this class to be instanced
	CWnd();

	// pure virtual dummy func
	// force this class to have a vtable
	virtual void vftable() = 0;

	void* baseclass[7];
	HWND m_hWnd;
	BYTE unk[0x30];

public:
	static OnCtlColor_t OnCtlColor_o;
	HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);

	BOOL RedrawWindow(	LPCRECT lpRectUpdate = NULL,
						struct CRgn* prgnUpdate = NULL,
						UINT flags = RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
};
STATIC_ASSERT_SIZE(CWnd, 0x54);