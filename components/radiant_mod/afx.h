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

public:
	static OnCtlColor_t OnCtlColor_o;
	HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
};
