// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define _CRT_SECURE_NO_WARNINGS 1

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <string>
#include <iostream>
#include <algorithm> 

// Global definitions
#define GITHUB_RELEASE_URL TEXT("https://api.github.com/repos/Nukem9/LinkerMod/releases")

// Additional headers
#include "shared_utility.h"
#include "shared_version.h"

#include "jsoncpp/include/json/json.h"
