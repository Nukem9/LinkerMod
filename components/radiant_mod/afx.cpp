#include "stdafx.h"

//
// CDC
//

// Do not allow this class to be instanced
CDC::CDC()
{
}

void CDC::SetTextColor(COLORREF crColor)
{
	((SetTextColor_t)0x005A6F7F)(this, crColor);
}

COLORREF CDC::SetBkColor(COLORREF crColor)
{
	return ((SetBkColor_t)0x005A6E98)(this, crColor);
}


//
// CWND
//
OnCtlColor_t CWnd::OnCtlColor_o = 0x0;

// Do not allow this class to be instanced
CWnd::CWnd()
{
}

HBRUSH CWnd::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor)
{
	STATIC_ASSERT_OFFSET(CWnd, m_hWnd, 0x20);

	HBRUSH brush = CWnd::OnCtlColor_o(this, pDC, pWnd, nCtlColor);

	switch (nCtlColor)
	{
	case CTLCOLOR_EDIT:
		pDC->SetTextColor(RADIANT_AFX_CONSOLE_TEXT);
		pDC->SetBkColor(RADIANT_AFX_CONSOLE_BG);
		break;
	}

	return brush;
}

class CGdiObject
{
private:
	DWORD unk;
	DWORD m_hObject;

	// Do not allow this class to be instanced
	CGdiObject() = delete;
	~CGdiObject() = delete;
public:

	inline void* GetSafeHandle(void)
	{
		if (this)
			return (void*)&this->m_hObject;
		else
			return NULL;
	}
};

BOOL CWnd::RedrawWindow(LPCRECT lpRectUpdate, struct CRgn* prgnUpdate, UINT flags)
{
	HRGN hrgnUpdate = (HRGN)reinterpret_cast<CGdiObject*>(prgnUpdate)->GetSafeHandle();
	return ::RedrawWindow(this->m_hWnd, lpRectUpdate, hrgnUpdate, flags);
}
