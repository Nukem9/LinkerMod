#include "stdafx.h"

SRCLINE(268)
const char *R_ErrorDescription(HRESULT hr)
{
	return ((const char *(__cdecl *)(HRESULT))0x0051A970)(hr);
}