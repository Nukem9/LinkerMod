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
