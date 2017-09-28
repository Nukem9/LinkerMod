#include "stdafx.h"

CCamWnd *(__stdcall *CCamWnd::ctor_o)(CCamWnd *This);
CCamWnd *CCamWnd::ActiveWindow;

CCamWnd *CCamWnd::ctor(CCamWnd *This)
{
	CCamWnd::ActiveWindow = CCamWnd::ctor_o(This);
	return CCamWnd::ActiveWindow;
}

void CCamWnd::Cam_MouseControl(float dtime)
{
	static DWORD dwCall = 0x00403BB0;

	_asm
	{
		push esi
		mov esi, ecx // esi = this
		push dtime
		call dwCall // Automatically fixes the stack
		pop esi
	}

	return;
}