#pragma once

#if WIN32
	#include <Windows.h>
#else
	#include <unistd.h>
#endif

int Sys_CPUCount(void);
