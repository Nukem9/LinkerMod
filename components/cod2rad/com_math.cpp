#include "stdafx.h"

int ClampByte(int v)
{
	if (v < 0)
		return 0;
	if (v > 255)
		return 255;
	return v;
}

BYTE __cdecl EncodeFloatInByte(float flt)
{
	double f = flt;
	if (f < 0.0)
	{
		return 0;
	}

	if (f > 1.0)
	{
		return 255;
	}

	return (BYTE)(f * 255.0);
}

WORD __cdecl EncodeFloatInWord(float flt)
{
	double f = flt;
	if (f < 0.0)
	{
		return 0;
	}

	return (WORD)(f * 255.0);
}
