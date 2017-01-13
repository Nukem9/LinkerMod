#include "stdafx.h"

CCamWnd *(__stdcall *CCamWnd::ctor_o)(CCamWnd *This);
CCamWnd *CCamWnd::ActiveWindow;

CCamWnd *CCamWnd::ctor(CCamWnd *This)
{
	CCamWnd::ActiveWindow = CCamWnd::ctor_o(This);
	return CCamWnd::ActiveWindow;
}