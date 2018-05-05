#include "stdafx.h"

int R_IsRemoteScreenUpdateActive()
{
	return *(int *)0x396A4C0 > 0;
}