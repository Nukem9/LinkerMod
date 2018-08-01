#pragma once
#include "stdafx.h"
#include "fs.h"

namespace fs
{
	bool FileExists(const std::string& filepath)
	{		
		WIN32_FIND_DATAA findData;
		HANDLE h = FindFirstFileA(filepath.c_str(), &findData);

		if (h == INVALID_HANDLE_VALUE)
			return false;

		FindClose(h);
		
		return true;
	}
};
