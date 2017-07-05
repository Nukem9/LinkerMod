#include "stdafx.h"

// /client/cl_keys.cpp:2219
void CL_KeyEvent(int localClientNum, int key, const int down, const unsigned int time)
{
	((void(__cdecl *)(int, int, const int, const unsigned int))0x00587500)(localClientNum, key, down, time);
}

// /client/cl_keys.cpp:2759
void CL_CharEvent(int localClientNum, int key)
{
	((void(__cdecl *)(int, int))0x004CEC00)(localClientNum, key);
}